import sys
import os
import re
import csv
import json
import difflib
import tree_sitter_c as tsc
import tree_sitter_cpp as tscpp
from tree_sitter import Language, Parser
import lizard
from utils import *


def get_c_cpp_file(base_path: str):
    c_path = base_path + '.c'
    cpp_path = base_path + '.cpp'
    if os.path.exists(c_path):
        path = c_path
        ext = 'c'
    elif os.path.exists(cpp_path):
        path = cpp_path
        ext = 'cpp'
    else:
        print(f'This file does not exist with a c or cpp extension: {base_path}')
        return
    with open(path, 'r') as f:
        content = f.read()
    return content, ext


def get_leading_whitespace(text):
    """
    Extract the leading whitespace (spaces or tabs) from a string.
    
    Args:
        text (str): The input string to analyze
        
    Returns:
        str: The leading whitespace characters found
    """
    leading = ''
    for char in text:
        if char in ' \t\n':
            leading += char
        else:
            break
    return leading


def mask_func_desc(id, file_name, mode='base'):
    # add description to the masked function
    # must already ahve description and the masked function separately

    # get description
    desc_path = f'descriptions/{id}/desc.txt'
    with open(desc_path, 'r') as f:
        desc = f.read()

    # get masked function, file
    mask_func, ext = get_c_cpp_file(f'descriptions/{id}/{file_name}_{mode}')
    if '// <MASK>' not in mask_func:
        print(f'ID {id} missing the "// <MASK>" in mask_func')
        return

    # add description to mask

    # find line that has // <MASK>
    mask_func_lines = re.split(r'\n', mask_func)
    for i, ln in enumerate(mask_func_lines):
        if '// <MASK>' in ln:
            break
    
    # get leading spaces
    leading_spaces = get_leading_whitespace(ln)

    # if there's other stuff before or after // <MASK>, add new lines
    ln_segs = ln.split('// <MASK>')
    if ln_segs[0].strip() == '':
        leading_newline = ''
    else:
        leading_newline = '\n' + leading_spaces
    
    if ln_segs[1].strip() == '':
        ending_newline = ''
    else:
        ending_newline = '\n'

    # add spacing to description, mask
    desc_lines = re.split(r'\n', desc)
    desc_lines[1:] = [leading_spaces + ln for ln in desc_lines[1:]]
    desc_lines[0] = leading_newline + desc_lines[0]
    mask_line = f'{leading_spaces}// <MASK>{ending_newline}'
    desc_mask_lines = desc_lines + [mask_line]
    desc_mask = '\n'.join(desc_mask_lines)

    # replace mask with the description + mask
    mask_func = mask_func.replace('// <MASK>', desc_mask)
    with open(f'descriptions/{id}/{file_name}_desc_{mode}.{ext}', 'w') as f:
        f.write(mask_func)


if __name__ == "__main__":
    with open('ids.txt', 'r') as f:
        ids = f.read().splitlines()[1:]

    for id in ids:
        # print(id)
        # mask_func_desc(id, 'mask_sec_func', 'base')
        # mask_func_desc(id, 'mask_sec_func', 'perturbed')
        mask_func_desc(id, 'mask', 'base')
        mask_func_desc(id, 'mask', 'perturbed')
