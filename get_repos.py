import json
import os
import subprocess


BASE_DIR = 'repos'

def clone_repository(project_name, repo_url):
    target_dir = os.path.join(BASE_DIR, project_name)
    
    # Skip if already cloned
    if os.path.exists(target_dir):
        print(f"Repository {project_name} already exists, skipping...")
        return True
    
    try:
        subprocess.run(
            ['git', 'clone', repo_url, target_dir],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        return True
    except subprocess.CalledProcessError as e:
        print(f"Failed to clone {project_name}: {e}")
        return False


with open('final_ids.txt', 'r') as f:
    ids = f.read().splitlines()[1:]

projects = []
urls = []

for id in ids:
    with open(f'/home/cdilgren/project_benchmark/ARVO-Meta/meta/{id}.json', 'r') as f:
        meta = json.load(f)
    project = meta['project']
    if project not in projects:
        url = meta['repo_addr']
        projects.append(project)
        urls.append(url)
        clone_repository(project, url)

# print(len(projects))