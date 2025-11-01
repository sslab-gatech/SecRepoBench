import matplotlib as mpl

# Try Libertine (original) and Libertinus (the actively maintained fork)
mpl.rcParams["font.family"] = ["Linux Libertine", "Linux Libertine O", "Libertinus Serif"]
# Optional: if you use sans-serif anywhere
mpl.rcParams["font.sans-serif"] = ["Linux Biolinum", "Libertinus Sans"]

# If you use mathtext and want it to match:
mpl.rcParams["mathtext.fontset"] = "custom"
mpl.rcParams["mathtext.rm"] = "Linux Libertine"
mpl.rcParams["mathtext.it"] = "Linux Libertine:italic"
mpl.rcParams["mathtext.bf"] = "Linux Libertine:bold"


import matplotlib.pyplot as plt

# plt.style.use('seaborn-v0_8-darkgrid')

# Model names and corresponding data
models = [
    "GPT-5", "OpenAI o3", "GPT-4.1", "OpenAI o1", "Claude Sonnet 4 Thinking", "Claude Sonnet 3.7 Thinking", "DeepSeek-R1", "DeepSeek-V3", "Claude Sonnet 3.5", "Qwen3-Coder", "GPT-4o New", "OpenAI o3-mini", "Qwen3 235B"
]

# Updated data points for selected models
sec_repo_bench = [
    39.3, 32.4, 27.7, 23.6, 30.2, 28.0, 23.9, 22.6, 20.1, 23.0, 22.0, 21.4, 16.4
]

sec_code_plt = [
    53.8, 47.7, 41.1, 31.1, 46.9, 39.0, 34.9, 25.0, 34.1, 30.8, 27.4, 33.0, 23.8
]

# Create the scatter plot
fig, ax = plt.subplots(figsize=(10, 7))
scatter = ax.scatter(
    sec_repo_bench, sec_code_plt,
    s=100, facecolors=(0.18, 0.53, 0.79, 0.45), edgecolors="#3498DB", linewidths=2
)

# Add labels manually with offsets
for i, model in enumerate(models):
    if model == "GPT-5":
        # Position above the point and to the left
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, -5), textcoords='offset points',
                   ha='right', va='center', fontsize=16)
    elif model == "OpenAI o3":
        # Position above the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, 5), textcoords='offset points',
                   ha='left', va='bottom', fontsize=16)
    elif model == "GPT-4.1":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, -5), textcoords='offset points',
                   ha='right', va='center', fontsize=16)
    elif model == "OpenAI o1":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, 10), textcoords='offset points',
                   ha='left', va='top', fontsize=16)
    elif model == "Claude Sonnet 4 Thinking":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, -5), textcoords='offset points',
                   ha='right', va='center', fontsize=16)
    elif model == "Claude Sonnet 3.7 Thinking":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, -5), textcoords='offset points',
                   ha='left', va='center', fontsize=16)
    elif model == "DeepSeek-R1":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, 5), textcoords='offset points',
                   ha='left', va='center', fontsize=16)
    elif model == "DeepSeek-V3":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, 0), textcoords='offset points',
                   ha='left', va='center', fontsize=16)
        
    elif model == "Claude Sonnet 3.5":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(5, 15), textcoords='offset points',
                   ha='right', va='center', fontsize=16)

    elif model == "Qwen3-Coder":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, -10), textcoords='offset points',
                   ha='right', va='center', fontsize=16)

    elif model == "GPT-4o New":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, 0), textcoords='offset points',
                   ha='right', va='center', fontsize=16)
    elif model == "OpenAI o3-mini":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(-10, -10), textcoords='offset points',
                   ha='right', va='center', fontsize=16)
    elif model == "Qwen3 235B":
        # Position to the left of the point
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(10, 0), textcoords='offset points',
                   ha='left', va='center', fontsize=16)
    else:
        # Position to the right of points
        ax.annotate(model, (sec_repo_bench[i], sec_code_plt[i]),
                   xytext=(15, 0), textcoords='offset points',
                   ha='right', va='center', fontsize=16)

# Customize axes
ax.set_xlabel("SecRepoBench (%)", fontsize=20, weight='bold', labelpad=12)
ax.set_ylabel("BaxBench (%)", fontsize=20, weight='bold', labelpad=12)
ax.set_title("secure-pass@1", fontsize=24, weight='bold', pad=12)
# ax.set_facecolor('#f8f9fa')

# Format tick labels
ax.set_xticks([15, 20, 25, 30, 35, 40])
ax.set_yticks([25, 30, 35, 40, 45, 50, 55])
ax.set_xticklabels([f"{i}%" for i in [15, 20, 25, 30, 35, 40]])
ax.set_yticklabels([f"{i}%" for i in [25, 30, 35, 40, 45, 50, 55]])
ax.grid(True, linestyle='--', alpha=0.3, color='gray', linewidth=0.8, zorder=1)
ax.set_axisbelow(True)
ax.tick_params(axis='both', which='major', labelsize=16)

for spine in ax.spines.values():
    spine.set_edgecolor('black')
    spine.set_linewidth(1.5)

plt.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig("scatter_plot.pdf", format="pdf", bbox_inches='tight')
plt.show()