from pathlib import Path
import json
import requests
import itertools
from requests.exceptions import RequestException
from base64 import b64encode
from time import sleep
import lizard
from unidiff import PatchSet
from unidiff.errors import UnidiffParseError
from alive_progress import alive_bar
from git import Repo
from pydriller import Repository, Git, ModificationType
import tempfile

def is_cxx_src(path):
    path = path.lower()
    for f in ["fuzz", "test"]:
        if f in path:
            return False
    if True in [ext == path.split(".")[-1] for ext in ["c", "cc", "cpp", "cxx", "h", "hh", "hpp"]]:
        return True
    return False

def filter_case(path):
    try:
        local_id = path.stem
        patch = PatchSet.from_filename(path)
        meta = json.load(Path(f"./ARVO-Meta/meta/{local_id}.json").open())
        
        # connor: remove this requirement, fixing commit is in 'fix' field
        if isinstance(meta["fix_commit"], list): # only a single fixing commit is allowed
            return ">1 fixing commits"
        
        # remove commits that change multiple files
        files = [f for f in patch if is_cxx_src(f.path)] # only the non testing files are included
        if len(files) != 1: # only one file must be modified
            return "0 or >1 modified C/C++ file"
        file = files[0]

        #if file.added < 1 or file.removed < 1: # file must include additions and deletions
        #    return "0 additions or 0 deletions"
        #analysis = lizard.analyze_file.analyze_source_code(file.path, str(file))
        #if len(analysis.function_list) != 1:
        #    return False
        #if len(list(file)) > 1: # there must only be one hunk
        #    return False
        
        return local_id, file, patch, meta
    except (UnidiffParseError, UnicodeDecodeError, TypeError, FileNotFoundError) as e:
        return "exception " + str(e)

def filter_commit(commit):
    files = [file for file in commit.modified_files if file.change_type == ModificationType.MODIFY and not ("test" in file.old_path.lower() or "fuzz" in file.old_path.lower())]
    if len(files) != 1: # only one file may be modified
        return "0 or >1 modified non test/fuzz file"
    file = files[0]
    #if file.added_lines == 0 or file.deleted_lines == 0: # must include additions and deletions
    #    return "0 additions or 0 deletions"
    if len(file.changed_methods) != 1: # only a single function modified
        return "0 or >1 functions modified"
    return file.source_code, file.source_code_before, file.changed_methods[0].name
    
    
def get_diff_dict(file):
    diff_dict = {"added": [], "deleted": []}
    for hunk in file:
        for line in hunk:
            if line.is_added:
                diff_dict["added"].append([line.target_line_no, line.value.strip()])
            elif line.is_removed:
                diff_dict["deleted"].append([line.source_line_no, line.value.strip()])
    return diff_dict     

def main():

    if not Path("ARVO-Meta").is_dir():
        Repo.clone_from("https://github.com/n132/ARVO-Meta.git", "ARVO-Meta")

    commits = {}
    cases = {}
    stats = {}
    
    # track how many samples we have at each filtering step
    samples_each_step = []

    # get starting samples
    samples = {}
    patch_paths = list(Path("./").glob("ARVO-Meta/patches/*.diff"))
    samples = {patch_path.stem: {'patch_path': patch_path}
                for patch_path in patch_paths}
    meta_paths = list(Path("./").glob("ARVO-Meta/meta/*.json"))
    for meta_path in meta_paths:
        if meta_path.stem in samples:
            samples[meta_path.stem]['meta_path'] = meta_path
    samples_each_step.append({"start": len(patch_paths)})

    # filter out samples that aren't in both meta and patches folders
    remove = [stem for stem in samples if 'meta_path' not in samples[stem]]
    for rm in remove: del samples[rm]
    samples_each_step.append({"samples in both meta and patches folders": len(samples)})

    # filter out duplicates | keep first instance of commit
    fixes = set()
    remove = []
    for stem in samples:
        meta = json.load(Path(f"./ARVO-Meta/meta/{stem}.json").open())
        samples[stem]['meta'] = meta
        fix = meta['fix']
        if fix in fixes:
            remove.append(stem)
        fixes.add(fix)
    for rm in remove: del samples[rm]
    samples_each_step.append({"samples with unique fixing commits": len(samples)})

    # filter out non-top 40 projects
    with open("top_40.json", "r") as f:
        top_40 = json.load(f)
    remove = []
    for stem in samples:
        if samples[stem]['meta']["project"].lower() not in top_40:
            remove.append(stem)
    for rm in remove: del samples[rm]
    samples_each_step.append({"samples in top 40 projects": len(samples)})

    # filter out patch files with UnicodeDecodeError
    remove = []
    for stem in samples:
        try:
            patch = PatchSet.from_filename(f"./ARVO-Meta/patches/{stem}.diff")
            samples[stem]['patch'] = patch
        except (UnicodeDecodeError, UnidiffParseError):
            remove.append(stem)
    for rm in remove: del samples[rm]
    samples_each_step.append({"samples without UnicodeDecodeError or UnidiffParseError in diff file": len(samples)})

    # filter out patches that change 0 or >1 code files
    remove = []
    for stem in samples:
        patch = samples[stem]['patch']
        changed_files = []
        for f in patch:
            if f.source_file == None or f.target_file == None:
                continue
            if is_cxx_src(f.path):
                changed_files.append(f)
        if len(changed_files) != 1:
            remove.append(stem)
    for rm in remove: del samples[rm]
    samples_each_step.append({"samples with one changed cxx file": len(samples)})

    # filter out non-git repositories (e.g., svn- see 38424)
    remove = []
    for stem in samples:
        url = samples[stem]['meta']["repo_addr"].rstrip("/")
        if 'git' not in url:
            remove.append(stem)
            # print(url)
    for rm in remove: del samples[rm]
    samples_each_step.append({"removed non-git repos": len(samples)})

    # filter out patches that can't be read, change != file, or change 0 or >1 functions
    rm_bad_read = []
    rm_not1_commit_parent = []
    rm_not1_file = []
    rm_mult_funcs = []
    with alive_bar(len(samples)) as bar:
        for stem in samples:

            url = samples[stem]['meta']["repo_addr"].rstrip("/")

            # get commit hash
            if isinstance(samples[stem]['meta']['fix_commit'], str):
                commit_hash = samples[stem]['meta']['fix_commit']
            else:
                for fix_can in samples[stem]['meta']['fix_commit']:
                    if fix_can in samples[stem]['meta']['fix']:
                        commit_hash = fix_can
                        break

            with tempfile.TemporaryDirectory() as temp_dir:
                try:
                    commits = list(Repository(url, single=commit_hash, clone_repo_to=temp_dir).traverse_commits())
                except:
                    rm_bad_read.append(stem)
                    continue
                commit = commits[0]

                # filter out merge commits (have >1 parent)
                if len(commit.parents) != 1:
                    rm_not1_commit_parent.append(stem)
                    continue

                # the single cxx file must have ModificationType.MODIFY
                files = []
                for file in commit.modified_files:
                    if file.change_type == ModificationType.MODIFY and is_cxx_src(file.old_path.lower()):
                        files.append(file)
                if len(files) != 1: # only one file may be modified
                    rm_not1_file.append(stem)
                    continue
                file = files[0]

                # only a single function modified
                if len(file.changed_methods) != 1:
                    rm_mult_funcs.append(stem)
                    continue

                # store the case
                cases[stem] = {
                    "project_name": samples[stem]['meta']["project"].lower(),
                    "fixing_commit": commit.hash, 
                    "changed_file": file.new_path, 
                    "changed_function": file.changed_methods[0].name,
                    "diff": file.diff_parsed,
                    "source_code_before": file.source_code_before,
                    "source_code": file.source_code
                }
            bar()

    for rm in rm_bad_read: del samples[rm]
    samples_each_step.append({"samples that could be read with Repository()": len(samples)})

    for rm in rm_not1_commit_parent: del samples[rm]
    samples_each_step.append({"samples that are not merge commits": len(samples)})

    for rm in rm_not1_file: del samples[rm]
    samples_each_step.append({"samples whose cxx file is a ModificationType.MODIFY": len(samples)})

    for rm in rm_mult_funcs: del samples[rm]
    samples_each_step.append({"samples with 0 or >1 changed functions": len(samples)})

    # save remaining cases
    with open("cases.json", 'w') as f:
        json.dump(cases, f, indent=4)

    with open("samples_each_step.json", 'w') as f:
        json.dump(samples_each_step, f, indent=4)

    # with alive_bar(len(patch_paths)) as bar:
    #     for diff_path in patch_paths:
    #         try:
    #             local_id = diff_path.stem
    #             res = filter_case(diff_path)
    #             if isinstance(res, tuple):
    #                 local_id, file, patch, meta = res
    #                 url = meta["repo_addr"].rstrip("/")
    #                 for traversed_case in cases.values():
    #                     print(meta["project"].lower())
    #                     print(traversed_case["project_name"])
    #                     print(meta["fix_commit"])
    #                     print(traversed_case["fixing_commit"])
    #                     if meta["project"].lower() == traversed_case["project_name"] and meta["fix_commit"] == traversed_case["fixing_commit"]:
    #                         print("!!!")
    #                         res = "duplicate commit"
    #                         break
    #                 if not isinstance(res, str):  
    #                     for commit in Repository(url, single=meta["fix_commit"]).traverse_commits():
    #                         res = filter_commit(commit)
    #                         if isinstance(res, tuple):
    #                             sec, vul, method = res
    #                             #print(commit.hash)
    #                             #print(commit.hash in commits)
    #                             cases[local_id] = {
    #                                 "project_name":meta["project"].lower(),
    #                                 "fixing_commit":commit.hash, 
    #                                 "changed_file":file.path, 
    #                                 "changed_function": method,
    #                                 "diff":get_diff_dict(file),
    #                                 "source_code_before":vul,
    #                                 "source_code":sec
    #                             }
    #                             res = "success"
    #         except Exception as e:
    #             res = "exception " + str(e)
    #         if res in stats:
    #             stats[res].append(diff_path.stem)
    #         else:
    #             stats[res] = [diff_path.stem] 
    #         #print(stats)
    #         bar()
    
    # json.dump(stats, open("stats.json", "w"), indent=4)
    # json.dump(cases, open("cases.json", "w"), indent=4)


if __name__ == "__main__":
    main()