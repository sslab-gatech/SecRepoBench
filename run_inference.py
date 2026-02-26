import argparse
from dotenv import load_dotenv
import sys
from tools.preprocessor import run_inferences, docker_setup

load_dotenv()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--agents", nargs='+', help="List of agents to evaluate")
    parser.add_argument("--model-names", nargs='+', help="List of models to evaluate")
    parser.add_argument("--prompt-types", nargs='+', help="List of prompt types to evaluate")
    parser.add_argument("--context-types", nargs='+', help="List of context retrieval methods to evaluate")
    parser.add_argument("--rerun", action="store_true", help="With the rerun flag, it will rerun a task even if it is in report_eval.json. Otherwise, it will not.")
    parser.add_argument("--ids", nargs='+', default=None, help="Specific sample IDs to run. If not provided, runs all IDs from assets/ids.txt.")

    args = parser.parse_args()

    # modes refers to the code mutation strategy to mitigate memorization.
    # We only have the local var perturbation ('perturbed' mode).
    modes = ['perturbed']

    # consider exposing num_workers
    num_workers = 1

    if args.ids:
        ids = args.ids
    else:
        with open('assets/ids.txt', 'r') as f:
            ids = f.read().splitlines()[1:]

    for agent in args.agents:
        if agent != "none" and agent != "claudecode":
            docker_setup(ids, num_workers, args.rerun)

    run_inferences(ids, args.agents, args.model_names, args.prompt_types,
                   args.context_types, args.rerun, modes, num_workers)


if __name__ == "__main__":
    main()
