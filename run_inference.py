import argparse
from evaler import APIEvaler, ChatEvaler, AiderEvaler, ClaudeCodeEvaler, OpenhandsEvaler
from tqdm import tqdm
from dotenv import load_dotenv
from pathlib import Path
import sys
from constants import *
from concurrent.futures import ProcessPoolExecutor, as_completed

load_dotenv()


def process_id(id, agent, model_name, context_type, prompt_type, mode, rerun):
    if agent != "none":
        if agent == "aider":
            evaler = AiderEvaler(model_name, context_type, prompt_type, mode)
        elif agent == "claudecode":
            evaler = ClaudeCodeEvaler(model_name, context_type, prompt_type, mode)
        elif agent == "openhands":
            evaler = OpenhandsEvaler(model_name, context_type, prompt_type, mode)

    else: 
        if 'gpt-' in model_name or 'claude-' in model_name or 'gemini-' in model_name or 'qwen-' in model_name or model_name in API_MODEL_NAMES:
            evaler = APIEvaler(model_name, context_type, prompt_type, mode)
        else:
            evaler = ChatEvaler(model_name, context_type, prompt_type, mode)    

    save_path = Path('completions') / id
    save_path.mkdir(parents=True, exist_ok=True)

    response, prompt, system_prompt = evaler.get_response(id, mode, rerun)

    if agent != "none":
        with open(save_path / f'{agent}-{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_code_completion.txt', 'w') as file:
            file.write(response)
        with open(save_path / f'{agent}-{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_prompt.txt', 'w') as file:
            file.write(prompt)
        with open(save_path / f'{agent}-{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_system_prompt.txt', 'w') as file:
            file.write(system_prompt)
    else:
        with open(save_path / f'{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_code_completion.txt', 'w') as file:
            file.write(response)
        with open(save_path / f'{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_prompt.txt', 'w') as file:
            file.write(prompt)
        with open(save_path / f'{model_name}-filled-code-{context_type}-{prompt_type}-{mode}_system_prompt.txt', 'w') as file:
            file.write(system_prompt)
    evaler.save_cache()


def run_inference(agent, model_name, context_type, prompt_type, mode, rerun, max_workers):
    with open('ids.txt', 'r') as f:
        ids = f.read().splitlines()[1:]
        ids = ['910']
        
    if not rerun:
        print('Using cache where possible')

    with ProcessPoolExecutor(max_workers=max_workers) as executor:
        futures = {
            executor.submit(process_id, id, agent, model_name, context_type, prompt_type, mode, rerun): id
            for id in ids
        }

        for future in tqdm(as_completed(futures), total=len(ids), miniters=1, mininterval=0, desc="Processing"):
            try:
                result = future.result()
            except Exception as e:
                print(f'Error processing {futures[future]}: {e}')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--agents", nargs='+', help="List of agents to evaluate")
    parser.add_argument('--model_names', nargs='+', type=str)
    parser.add_argument('--context_types', nargs='+', type=str)
    parser.add_argument('--prompt_types', nargs='+', type=str)
    parser.add_argument('--rerun', action="store_true",
                        help="With the rerun flag, it will run inference for a task even if it is in the cache. Otherwise, it will not.")
    args = parser.parse_args()

    max_workers = 10  # Adjust based on API limits
    for agent in args.agents:
        for model_name in args.model_names:
            for context_type in args.context_types:
                for prompt_type in args.prompt_types:
                    for mode in ['perturbed']:
                        print(
                            f"Running inference for {agent}, {model_name}, {context_type}, {prompt_type}, {mode}")
                        
                        run_inference(agent, model_name, context_type,
                                    prompt_type, mode, args.rerun, max_workers)


if __name__ == "__main__":
    main()
