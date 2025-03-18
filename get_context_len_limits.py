import tiktoken
import os
from tqdm import tqdm
import json

from constants import *
from cwe_map import *

from mistral_common.tokens.tokenizers.mistral import MistralTokenizer
from mistral_common.protocol.instruct.messages import UserMessage
from mistral_common.protocol.instruct.request import ChatCompletionRequest


def get_c_cpp_file(base_path: str):
    c_path = base_path + '.c'
    cpp_path = base_path + '.cpp'
    if os.path.exists(c_path):
        path = c_path
    elif os.path.exists(cpp_path):
        path = cpp_path
    else:
        print(f'This file does not exist with a c or cpp extension: {base_path}')
        return
    with open(path, 'r') as f:
        content = f.read()
    return content


def get_constant_prompt_token_len():

    tokenizer = MistralTokenizer.v3()

    system_prompts = [SYSTEM_PROMPT, SEC_GENERIC_PROMPT]
    prompts = [INFILE_PROMPT.format(context=""),
               CROSS_FILE_PROMPT.format(context1="", context2=""),
               FUNC_PROMPT.format(context="")]

    # add cwe id, desc to SEC_SPECIFIC_PROMPT
    for CWE_name, CWE_ID in crash_type_to_cwe.items():
        CWE_description = cwe_id_to_desc[CWE_ID]
        system_prompt = SEC_SPECIFIC_PROMPT.format(CWE_ID=CWE_ID, CWE_description=CWE_description)
        system_prompts.append(system_prompt)

    system_prompt_token_lens = []
    for system_prompt in system_prompts:
        completion_request = ChatCompletionRequest(messages=[UserMessage(content=system_prompt)])    
        tokens = tokenizer.encode_chat_completion(completion_request).tokens
        system_prompt_token_lens.append(len(tokens))

    prompts_token_lens = []
    for prompt in prompts:
        completion_request = ChatCompletionRequest(messages=[UserMessage(content=prompt)])    
        tokens = tokenizer.encode_chat_completion(completion_request).tokens
        prompts_token_lens.append(len(tokens))
    
    max_system_prompt_token_lens = max(system_prompt_token_lens)
    max_prompts_token_lens = max(prompts_token_lens)
    # print(f"Max system prompt: {max_system_prompt_token_lens}")
    # print(f"Max prompt: {max_prompts_token_lens}")

    return max_system_prompt_token_lens + max_prompts_token_lens


def get_context_len_limit(id, max_prompt_len):
    tokenizer = MistralTokenizer.v3()

    # get max token len of sec, vul code blocks
    sec_block = get_c_cpp_file(f"descriptions/{id}/sec_code_block_perturbed")
    vul_block = get_c_cpp_file(f"descriptions/{id}/vul_code_block_perturbed")

    sec_completion_request = ChatCompletionRequest(messages=[UserMessage(content=sec_block)])    
    sec_tokens = tokenizer.encode_chat_completion(sec_completion_request).tokens

    vul_completion_request = ChatCompletionRequest(messages=[UserMessage(content=vul_block)])    
    vul_tokens = tokenizer.encode_chat_completion(vul_completion_request).tokens

    max_block_token_len = max(len(sec_tokens), len(vul_tokens))

    # get token len of masked_func + desc
    mask_func_desc_perturbed = get_c_cpp_file(f"descriptions/{id}/mask_func_desc_perturbed")

    completion_request = ChatCompletionRequest(messages=[UserMessage(content=mask_func_desc_perturbed)])    
    mask_func_desc_perturbed_token_len = len(tokenizer.encode_chat_completion(completion_request).tokens)

    # get context len limit, codestral has smallest context window of 32k
    context_len_limit = 32000 - max_prompt_len - 2 * max_block_token_len - mask_func_desc_perturbed_token_len

    return context_len_limit


def main():
    input_dir = 'descriptions'

    max_prompt_len = get_constant_prompt_token_len()

    with open('ids.txt', 'r') as f:
        ids = f.read().splitlines()[1:]

    context_len_limits = {}
    for id in tqdm(ids):
        len_limit = get_context_len_limit(id, max_prompt_len)
        context_len_limits[id] = len_limit
    
    context_len_limits = dict(sorted(context_len_limits.items(), key=lambda item: item[1]))
    with open("context_len_limits.json", 'w') as f:
        json.dump(context_len_limits, f, indent=4)


if __name__ == '__main__':
    main()