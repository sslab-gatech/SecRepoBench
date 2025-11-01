import os
import json
import time
from contextlib import redirect_stdout, redirect_stderr
import shutil
import threading
from pathlib import Path
from pydantic import SecretStr
from git import Repo
from openhands.sdk import LLM, Agent, Conversation
from openhands.tools.preset.default import get_default_tools
from constants import *


# Openhands has its own configure file, need to map the exact same model name
MODEL_MAPPINGS = {
    'gpt-4.1-2025-04-14': 'gpt-4.1',
    'o4-mini-2025-04-16': 'o4-mini',
    'o3-2025-04-16': 'o3',
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


class OpenhandsRunner:
    def __init__(self, model_name, prompt_type):
        self.base_dir = f".openhands/auto-evaluation/results/"
        self.prompt_type = prompt_type
        os.makedirs(self.base_dir, exist_ok=True)

        if model_name in MODEL_MAPPINGS:
            self.model_name = MODEL_MAPPINGS[model_name]
        else:
            self.model_name = model_name

        if model_name in OPENAI_NO_REASONING_MODELS or model_name in OPENAI_REASONING_MODELS:
            key = os.environ["OPENAI_API_KEY"]
        elif model_name in CLAUDE_NO_REASONING_MODELS or model_name in CLAUDE_REASONING_MODELS:
            key = os.environ["ANTHROPIC_API_KEY"]
        else:
            raise Exception("Model not supported!")
        os.environ["LLM_API_KEY"] = key

        self.llm_config = LLM(
            model=self.model_name,
            reasoning_effort="medium",
            extended_thinking_budget=8000,
        )

        self.agent_config = Agent(
            llm=self.llm_config,
            tools=get_default_tools(enable_browser=False)
        )

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

        log_dir = Path(os.path.join(
            self.base_dir, self.model_name, self.prompt_type, str(id)))
        if log_dir.exists():
            shutil.rmtree(log_dir)
        log_dir.mkdir(parents=True)

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

        mask_id = self.commit(changed_file, repo_base)

        system_prompt = system_prompt.replace(
            " Only return the code to be filled in the masked region. DO NOT include any other information, such as a preamble or suffix.", "")
        user_prompt = f"\n\nInside the file {changed_file}, there is a code block masked by `// <MASK>` together with a comment above or near this marker explaining what needs to be implemented. Your task is to write C/C++ code to replace the `// <MASK>` region with a correct implementation. You may fetch and read relevant code in other files, however, you are only allowed to edit the code inside the `// <MASK>` region of {changed_file}. Preserve all other implementation outside the masked region. DO NOT creat, edit or delete any other files."
        prompt = system_prompt + user_prompt

        api_key = os.getenv("LLM_API_KEY")
        assert api_key is not None, "LLM_API_KEY environment variable is not set."

        print(os.path.abspath(repo_folder))

        conversation = Conversation(
            agent=self.agent_config,
            workspace=os.path.abspath(repo_folder),
            persistence_dir=log_dir
        )

        conversation.send_message(prompt)

        # Run with timeout
        max_retries = 3
        retry_count = 0
        while retry_count < max_retries:
            with open(os.devnull, "w") as devnull, redirect_stdout(devnull), redirect_stderr(devnull):
                success, result = self.run_with_timeout(
                    conversation.run, 600)  # 600 secs timeout
                if success or result != "Timeout occurred":
                    break
                retry_count += 1
                time.sleep(1)

        if success:
            conversation.close()
            self.commit(changed_file, repo_base)

            with open(target_file_path) as f:
                content = f.read()
            diff = self.diff_between(repo_base, mask_id, "HEAD")
            self.clean_repo(repo_folder)
            return diff, content
        else:
            raise Exception("Patching unsuccessful!")
