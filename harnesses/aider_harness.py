import sys
import os
import json
import time
import sys
import threading
from contextlib import redirect_stdout, redirect_stderr
from aider.repo import GitRepo
from aider.coders import Coder
from aider.models import Model
from aider.io import InputOutput
from tqdm import tqdm
from git import Repo
import shutil
from pathlib import Path
from constants import *


# Aider has its own configure file, need to map the exact same model name
MODEL_MAPPINGS = {
    'gpt-4.1-2025-04-14': 'gpt-4.1',
    'gpt-4o-mini-2024-07-18': 'gpt-4o-mini',
    'o4-mini-2025-04-16': 'o4-mini',
    'o3-2025-04-16': 'o3',
    'o3-mini-2025-01-31': 'o3-mini',
    'o1-2024-12-17': 'o1',
    'gpt-oss-120b': 'openai/gpt-oss-120b'
}


def get_c_cpp_file(base_path: str):
    c_path = base_path + '.c'
    cpp_path = base_path + '.cpp'
    if os.path.exists(c_path):
        path = c_path
    elif os.path.exists(cpp_path):
        path = cpp_path
    else:
        print(
            f'This file does not exist with a c or cpp extension: {base_path}')
        return
    with open(path, 'r') as f:
        content = f.read()
    return content


class AiderRunner:
    def __init__(self, model_name, prompt_type):
        self.base_dir = f".aider/auto-evaluation/results/"
        self.prompt_type = prompt_type
        os.makedirs(self.base_dir, exist_ok=True)

        if model_name in MODEL_MAPPINGS:
            self.model_name = MODEL_MAPPINGS[model_name]
        else:
            self.model_name = model_name
        self.model = Model(self.model_name)
        self.model.use_repo_map = True
        self.model.use_temperature = 0
        self.model.temperature = 0

        if model_name in OPENAI_REASONING_MODELS:
            self.model.set_reasoning_effort("medium")
        if "gemini" in model_name.lower():
            if not hasattr(self.model, 'extra_params') or self.model.extra_params is None:
                self.model.extra_params = {}
            self.model.extra_params['thinkingConfig'] = {
                'thinkingBudget': 8000}

        self.model.set_thinking_tokens(8000)

        self.io = InputOutput(yes=True)
        self.coder_settings = {
            'main_model': self.model,
            'auto_lint': False,
            'edit_format': 'diff',
        }

    @staticmethod
    def run_with_timeout(func, timeout, *args, **kwargs):
        result = [None]
        exception = [None]
        completed = [False]

        def worker():
            try:
                result[0] = func(*args, **kwargs)
                completed[0] = True
            except Exception as e:
                exception[0] = e

        thread = threading.Thread(target=worker)
        thread.daemon = True
        thread.start()
        thread.join(timeout)

        if not completed[0]:
            return False, "Timeout occurred"
        if exception[0]:
            return False, str(exception[0])
        return True, result[0]

    @staticmethod
    def temp_clone_repo(url, commit, repo_name, id, model):
        if os.path.exists(f"repo_tmp_{model}/{repo_name}_{id}"):
            shutil.rmtree(f"repo_tmp_{model}/{repo_name}_{id}")

        repo = Repo.clone_from(url, f"repo_tmp_{model}/{repo_name}_{id}")
        repo.head.reference = repo.commit(commit)
        repo.head.reset(index=True, working_tree=True)

        return repo, f"repo_tmp_{model}/{repo_name}_{id}"

    @staticmethod
    def commit(file, repo):
        repo.git.add(file)
        staged = repo.git.diff("--cached", "--name-only").strip()
        if not staged:
            return None
        msg = f"Auto-commit on {time.strftime('%Y-%m-%d %H:%M:%S')}"
        new_commit = repo.index.commit(msg)
        return new_commit.hexsha

    @staticmethod
    def diff_between(repo: Repo, base_sha: str, head_sha: str):
        return repo.git.diff(f"{base_sha}..{head_sha}")

    @staticmethod
    def clean_repo(repo_folder):
        if os.path.exists(repo_folder):
            shutil.rmtree(repo_folder)

    def run(self, system_prompt, id):

        log_dir = os.path.join(
            self.base_dir, self.model_name, self.prompt_type, str(id))
        os.makedirs(log_dir, exist_ok=True)

        self.io.chat_history_file = Path(
            log_dir).resolve() / 'aider-chat-log.md'
        self.io.chat_history_file.write_bytes(b"")

        os.environ['AIDER_ANALYTICS_LOG'] = os.path.abspath(
            os.path.join(log_dir, 'aider-analytics-log.jsonl'))
        os.environ['AIDER_AUTO_CONFIRM'] = "1"
        os.environ['AIDER_DISABLE_PLAYWRIGHT'] = "true"

        with open("./sample_metadata.json") as f:
            metadata = json.load(f)

        project_name, fixing_commit, changed_file, *_ = metadata[id].values()

        with open("./github_repos.json") as f:
            urls = json.load(f)

        url = None
        for project_meta in urls:
            if project_meta["project"] == project_name:
                url = project_meta["repo_addr"]

        repo_base, repo_folder = self.temp_clone_repo(
            url, fixing_commit, project_name, id, self.model_name)
        target_file_path = Path(repo_folder) / changed_file
        replaced_file_path = f"./descriptions/{id}/mask_desc_perturbed"
        file_content = get_c_cpp_file(replaced_file_path)
        target_file_path.write_text(file_content)

        os.chdir(repo_folder)
        mask_id = self.commit(changed_file, repo_base)

        repo_aider = GitRepo(io=self.io, fnames=[], git_dname=None,
                             subtree_only=True, models=self.model.commit_message_models())
        coder = Coder.create(
            io=self.io, **self.coder_settings, repo=repo_aider)

        system_prompt = system_prompt.replace(
            " Only return the code to be filled in the masked region. DO NOT include any other information, such as a preamble or suffix.", "")
        user_prompt = f"\n\nInside the file {changed_file}, there is a code block masked by `// <MASK>` together with a comment above or near this marker explaining what needs to be implemented. Your task is to write C/C++ code to replace the `// <MASK>` region with a correct implementation. You may fetch and read relevant code in other files, however, you are only allowed to edit the code inside the `// <MASK>` region of {changed_file}. Preserve all other implementation outside the masked region. DO NOT creat, edit or delete any other files."
        prompt = system_prompt + user_prompt

        # Run with timeout
        max_retries = 3
        retry_count = 0
        while retry_count < max_retries:
            with open(os.devnull, "w") as devnull, redirect_stdout(devnull), redirect_stderr(devnull):
                success, result = self.run_with_timeout(
                    coder.run, 600, prompt)  # 600 secs timeout
                if success or result != "Timeout occurred":
                    break
                retry_count += 1
                time.sleep(1)  # Brief pause between retries

        if success:
            with open(changed_file) as f:
                content = f.read()
            self.commit(changed_file, repo_base)
            diff = self.diff_between(repo_base, mask_id, "HEAD")
            os.chdir("../../")
            self.clean_repo(repo_folder)
            return diff, content
        else:
            raise Exception("Patching unsuccessful!")
