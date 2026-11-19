"""Identify the installed Tcl runtime shared by test and package jobs."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


def output(*args):
    return subprocess.check_output(args, text=True).strip()


toolchain = sys.argv[1]
root = Path(__file__).resolve().parents[2]
inputs = {
    'source': output('git', '-C', str(root / '_ci/tcl'), 'rev-parse', 'HEAD'),
    'arch': os.environ['RUNNER_ARCH'],
    'os': os.environ['ImageOS'],
    'prefix': str(root / '_ci/runtime'),
}
if toolchain == 'windows-msvc':
    recipe = 'build-msvc.ps1'
    inputs['compiler'] = os.environ['VCToolsVersion']
    inputs['sdk'] = os.environ['WindowsSDKVersion']
else:
    recipe = 'build-tea.sh'
    inputs['compiler'] = output('gcc', '--version')
    inputs['target'] = output('gcc', '-dumpmachine')
    if toolchain == 'windows-ucrt64':
        packages = ('gcc', 'binutils', 'crt', 'headers', 'libgcc', 'libwinpthread')
        inputs['packages'] = output('pacman', '-Q', 'make', 'bash',
                                    *('mingw-w64-ucrt-x86_64-' + p for p in packages))
    else:
        major = output('gcc', '-dumpversion').split('.')[0]
        inputs['packages'] = output('dpkg-query', '-W', 'make', 'binutils', f'gcc-{major}',
                                    f'libgcc-{major}-dev', 'libc6-dev', 'linux-libc-dev',
                                    'zlib1g-dev')
inputs['recipe'] = hashlib.sha256(Path(__file__).with_name(recipe).read_bytes()).hexdigest()
digest = hashlib.sha256(json.dumps(inputs, sort_keys=True).encode()).hexdigest()
with open(os.environ['GITHUB_OUTPUT'], 'a', encoding='utf-8') as stream:
    print(f'runtime=argparse-tcl-v1-{toolchain}-{digest}', file=stream)
