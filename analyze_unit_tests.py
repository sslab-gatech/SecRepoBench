import json
import subprocess
import re


with open('relevant_unittests.json', 'r') as f:
    relevant_unittests = json.load(f)

with open('filter_logs/cases.json', 'r') as f:
    cases = json.load(f)

def execute_command(command, sample_id):
    return subprocess.run(['docker', 'run', '--rm', '-it', f'n132/arvo:{sample_id}-fix', 'sh', '-c', command], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()

def get_github_base(sample_id):
    with open(f'ARVO-Meta/meta/{sample_id}.json', 'r') as f:
        meta = json.load(f)
    return meta['repo_addr']

def find_github_url(test_name, sample_id):
    project_name = cases[sample_id]['project_name']
    commit_hash = cases[sample_id]['fixing_commit']
    github_base = get_github_base(sample_id)
    line_number = 0
    FIND_COMMAND = 'find . -wholename "*{test_name}*"'


    if project_name == 'ffmpeg':
        # Actual testing code located in unknown file
        command = FIND_COMMAND
        file_name = execute_command(command, sample_id).split('\n')[0]    
    elif project_name == 'hunspell':
        # Tests based on dictionary files if words are spelled correctly
        file_name = f'tests/{test_name}.dic'
    elif project_name == 'c-blosc2':
        if test_name.endswith('0_1') or test_name.endswith('1_1') or test_name.endswith('2_1'):
            test_name = re.sub(r'_?\d+', '', test_name)
        if test_name.startswith('test_example'):
            test_name = test_name[13:]
            file_name = f'examples/{test_name}.c'
        elif test_name.startswith('test_compat'):
            test_name = test_name[11:]
            file_name = f'compat/{test_name}'
        else:
            file_name = f"tests/{test_name}.c"
    elif project_name == 'gpac':
        command = f'grep -rI "{test_name}" testsuite'
        output = execute_command(command, sample_id)
        if len(output) == 0:
            new_test_name = test_name.split('-')[0] + '-\\\\$'
            command = f'grep -rI "{new_test_name}" testsuite'
            output = execute_command(command, sample_id)
        file_name = output.split(':')[0][10:]
        if test_name.startswith('dash_exotic'):
            file_name = 'scripts/dash_exotic.sh'
        elif test_name.startswith('counter_cues'):
            file_name = 'scripts/dash_cues.sh'
        elif test_name.startswith('raw-aud'):
            file_name = 'scripts/raw-audio.sh'
        elif test_name.startswith('raw-vid'):
            file_name = 'scripts/raw-video.sh'
    elif project_name == 'lcms':
        command = f'grep -r "\\"{test_name}\\"" .'
        output = execute_command(command, sample_id)
        file_name = output.split(':')[0]
        function_name = output.split(',')[1][:-2].strip()
        command = f'grep -n "{function_name}(" {file_name}'
        output = execute_command(command, sample_id)
        if len(output) == 0:
            command = f'grep -n "{function_name}(" testbed/testplugin.c'
            file_name = 'testbed/testplugin.c'
            output = execute_command(command, sample_id)
        line_number = output.split(':')[0]
    elif project_name == 'assimp':
        file_base, new_test_name = test_name.split('.')
        command = f'grep --include=*.cpp  -r "{file_base}" .'
        file_name = execute_command(command, sample_id).split(':')[0]
        command = f'grep -n "{new_test_name}" {file_name}'
        output = execute_command(command, sample_id)
        line_number = output.split(':')[0]
    elif project_name == 'harfbuzz':
        command = f'find . -wholename "*{test_name}.*"'
        file_name = execute_command(command, sample_id)
        if file_name == '':
            command = FIND_COMMAND
            file_name = execute_command(command, sample_id)
    elif project_name == 'fluent-bit':
        command = f'grep -r "\\"{test_name}\\"" .'
        output = execute_command(command, sample_id)
        file_name = output.split(':')[0]
        test_function = output.split(',')[1][:-1]
        command = f'grep -n "{test_function}" {file_name}'
        output = execute_command(command, sample_id)
        line_number = output.split(':')[0]
    elif project_name == 'file':
        # Actual testing code located in test.c
        file_name = test_name[1:]
    elif project_name == 'libplist':
        file_name = f'test/{test_name}'
    elif project_name == 'libdwarf':
        # Testing done by the same file, different arguments
        # add_test(NAME selfdwarfdumpelf COMMAND ${elfshdir}/dwarfdumptest.py Elf cmake
        file_name = 'test/dwarfdumptest.py'
    elif project_name == 'yara':
        file_name = f'tests/{test_name}.c'
    elif project_name == 'libxml2':
        command = f'grep -r "{test_name}" . -A 1 --include=*.c' 
        output = execute_command(command, sample_id).split('\n')[1]
        file_name = output.split('-')[0]
        function_name = output.split('-')[1].split(',')[0].strip()
        command = f'grep -n "{function_name}(" {file_name}'
        output = execute_command(command, sample_id)
        line_number = output.split(':')[0]
    elif project_name == 'php-src':
        command = f"find . -wholename '*/{test_name}.phpt'"
        file_name = execute_command(command, sample_id)
        
    # if file_name starts with project_name, remove it
    if file_name.startswith('./'):
        file_name = file_name[2:]
    if file_name.startswith(f'{project_name}/'):
        file_name = file_name[(len(project_name))+1:]
    
    if project_name == 'ffmpeg':
        return f'https://git.ffmpeg.org/gitweb/ffmpeg.git/blob/{commit_hash}:/{file_name}'
    else:
        if github_base.endswith('.git'):
            github_base = github_base[:-4]
        if project_name == 'gpac':
            commit_hash = execute_command('cd testsuite && git rev-parse HEAD', sample_id)
            github_base = github_base[:-4] + 'testsuite'
        return f'{github_base}/blob/{commit_hash}/{file_name}#L{line_number}'

for sample_id in relevant_unittests:
    tests = relevant_unittests[sample_id]['relevant_unittests']
    
    print("Sample ID:", sample_id)
    #  docker run --rm -it n132/arvo:13736-vul
    for test_name in tests:
        # output = subprocess.run(['docker', 'run', '--rm', '-it', f'n132/arvo:{sample_id}-fix', 'sh', '-c', commands[]], stdout=subprocess.PIPE).stdout.decode('utf-8')
        print(find_github_url(test_name, sample_id))
    exit