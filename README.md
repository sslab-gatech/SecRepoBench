# 🛡️ SecRepoBench

## ⚙️ Configuration

### **1. Install `uv`**

Please check [installation methods](https://docs.astral.sh/uv/getting-started/installation/) to install `uv` on your platform.

### **2. Install dependencies**
Run the following command to install dependencies required by SecRepoBench:

```bash
cd SecRepoBench
uv sync
```
To install dependencies for agent framework `Aider` and `OpenHands`, please run the following commands:

```bash
uv pip install aider-chat
git clone https://github.com/OpenHands/software-agent-sdk.git
uv pip install -e ./software-agent-sdk/openhands-sdk
uv pip install -e ./software-agent-sdk/openhands-tools
```

Upzip useful metadata.

```bash
gunzip -k report.json.gz sample_metadata.json.gz
```

## 🚀 Running Inference

To run inference using SecRepoBench:

```bash
uv run run_inference.py \
  --agents [YOUR_AGENT_NAMES] \
  --model_names [YOUR_MODEL_NAMES] \
  --prompt_types [YOUR_PROMPT_TYPES] \
  --context_types [YOUR_CONTEXT_TYPES] \
  [--rerun]
```

- **Agent names**:
  - `none` (Without using agent framework)
  - `aider`
  - `openhands`
  - `claudecode`
- **Model names**: Defined in `constants.py`
- **Prompt types**:
  - `no-security-reminder`
  - `sec-generic`
  - `sec-specific`
  - `security-policy`
- **Context types**:
  - `BM25`
  - `dense-file`
  - `in-file`

📁 *Code completions are saved in the `completions/` directory.*

---

## 📊 Running Evaluation

To evaluate the model completions:

```bash
uv run run_eval.py \
  --agents [YOUR_AGENT_NAMES] \
  --model_names [YOUR_MODEL_NAMES] \
  --prompt_types [YOUR_PROMPT_TYPES] \
  --context_types [YOUR_CONTEXT_TYPES] \
  [--rerun]
```

📁 *Evaluation results are saved in the `eval_results/` directory.*
