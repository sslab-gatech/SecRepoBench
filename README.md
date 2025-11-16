# 🛡️ SecRepoBench
### **Links:**
- Paper: [SecRepoBench: Benchmarking Code Agents for Secure Code Completion in Real-World Repositories](https://arxiv.org/abs/2504.21205)
- Website & Leaderboard: Under Construction

## 📝 Overview

SecRepoBench is a benchmark that evaluates LLMs and code agents on secure code completion in real-world repositories. SecRepoBench has 318 code completion tasks in 27 C/C++ repositories, covering 15 CWEs. We evaluate 28 standalone LLMs and 13 code agents across 3 state-of-the-art agent frameworks using our benchmark. We find that state-of-the-art LLMs struggle with generating correct and secure code completions. However, code agents significantly outperform standalone LLMs. We show that SecRepoBench is more difficult than the prior state-of-the-art benchmark. Finally, our comprehensive analysis provides insights into potential directions for enhancing the ability of code agents to write correct and secure code in real-world repositories.

## ⚙️ Configuration

### **1. Install `uv`**

Please check [installation methods](https://docs.astral.sh/uv/getting-started/installation/) to install `uv` on your platform.

### **2. Install dependencies**

Run the following command to install dependencies required by SecRepoBench:

```bash
cd SecRepoBench
uv sync
```

To install dependencies for agent framework `ClaudeCode`, please run the following commands:

```bash
curl -fsSL https://claude.ai/install.sh | bash
```

### **3. Set Environment Variables**

SecRepoBench requires API keys for the language models you plan to use. Please set the following environment variables:

```bash
export OPENAI_API_KEY=<YOUR_API_KEY>
export ANTHROPIC_API_KEY=<YOUR_API_KEY>
export GEMINI_API_KEY=<YOUR_API_KEY>
export TOGETHER_API_KEY=<YOUR_API_KEY>
```


### **4. Extract metadata files**
Upzip metadata that would be used during inference or evalutaion:

```bash
gunzip -k report.json.gz sample_metadata.json.gz
```

## 🚀 Running Inference

To run inference using SecRepoBench:

```bash
uv run run_inference.py \
  --agents [YOUR_AGENT_NAMES] \
  --model-names [YOUR_MODEL_NAMES] \
  --prompt-types [YOUR_PROMPT_TYPES] \
  --context-types [YOUR_CONTEXT_TYPES] \
  [--rerun]
```

- **Agent names**:
  - `none` (Without using agent framework)
  - `aider`
  - `openhands`
  - `claudecode`
- **Model names**: Defined in `assets/constants.py`
- **Prompt types**:
  - `no-security-reminder`
  - `sec-generic`
  - `sec-specific`
  - `security-policy`
- **Context types**: (This option is disabled while using agent framework)
  - `BM25`
  - `dense-file`
  - `in-file`

📁 *Code completions are saved in the `completions/` directory.*


## 📊 Running Evaluation

To evaluate the model completions:

```bash
uv run run_eval.py \
  --agents [YOUR_AGENT_NAMES] \
  --model-names [YOUR_MODEL_NAMES] \
  --prompt-types [YOUR_PROMPT_TYPES] \
  --context-types [YOUR_CONTEXT_TYPES] \
  [--rerun]
```

📁 *Evaluation results are saved in the `eval_results/` directory.*

## 📖 Citation
```latex
@article{shen2025secrepobench,
  title={SecRepoBench: Benchmarking Code Agents for Secure Code Completion in Real-World Repositories},
  author={Shen, Chihao and Dilgren, Connor and Chiniya, Purva and Griffith, Luke and Ding, Yu and Chen, Yizheng},
  journal={arXiv preprint arXiv:2504.21205},
  year={2025}
}
```