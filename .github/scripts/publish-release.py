#!/usr/bin/env python3
"""Publish the three tested argparse packages to the rolling 'latest' release."""
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile


def run(*args):
    subprocess.run(args, check=True)


def api(path, *args):
    return json.loads(subprocess.check_output(['gh', 'api', path, *args], text=True))


def latest_release(repository):
    # Drafts can have an untagged-* REST tag_name until publication. GraphQL
    # resolves the pending tag as well, allowing a failed publication to resume.
    owner, name = repository.split('/')
    result = api('graphql', '-f', '''query=query($owner:String!, $name:String!) {
        repository(owner:$owner, name:$name) {
            release(tagName:"latest") { databaseId }
        }
    }''', '-f', f'owner={owner}', '-f', f'name={name}')
    release = result['data']['repository']['release']
    return api(f'repos/{repository}/releases/{release["databaseId"]}') if release else None


def packages(directory):
    """Require one matching distribution for each of the three build jobs."""
    packages = sorted(path for path in directory.iterdir() if path.name != 'SHA256SUMS.txt')
    suffixes = ('-linux-gcc.tar.gz', '-windows-ucrt64.zip', '-windows-msvc.zip')
    match = re.fullmatch(r'argparse([0-9]+(?:\.[0-9]+)+)-linux-gcc\.tar\.gz',
                         next((p.name for p in packages if p.name.endswith(suffixes[0])), ''))
    if not match:
        raise ValueError('Missing Linux argparse distribution')
    version = match.group(1)
    expected = {f'argparse{version}{suffix}' for suffix in suffixes}
    if {p.name for p in packages} != expected or not all(p.is_file() for p in packages):
        raise ValueError(f'Expected matching Linux, UCRT64 and MSVC packages for {version}')
    return packages, version


def main():
    repository = os.environ['GITHUB_REPOSITORY']
    sha = os.environ['GITHUB_SHA']
    directory = Path(sys.argv[1]).resolve()
    archives, version = packages(directory)
    release = latest_release(repository)
    if release and release.get('immutable'):
        raise RuntimeError('The latest release is immutable; rolling releases must remain mutable')
    checksums = directory / 'SHA256SUMS.txt'
    checksums.write_text(''.join(
        f'{hashlib.sha256(path.read_bytes()).hexdigest()}  {path.name}\n'
        for path in archives), encoding='utf-8')
    server = os.environ.get('GITHUB_SERVER_URL', 'https://github.com')
    run_url = f'{server}/{repository}/actions/runs/{os.environ["GITHUB_RUN_ID"]}'
    notes = (
        f'Rolling stable build of argparse {version} for Tcl 9.1.0. All three jobs tested both C and Tcl implementations.\n\n'
        f'Commit: [{sha}]({server}/{repository}/commit/{sha})\n\n'
        f'Built by: [Build packages]({run_url})\n\n'
        f'Published: {datetime.now(timezone.utc).isoformat(timespec="seconds")}\n\n'
        'This release and the `latest` tag are replaced on subsequent successful manual builds.\n')
    with tempfile.TemporaryDirectory() as temporary:
        notes_file = Path(temporary) / 'notes.md'
        notes_file.write_text(notes, encoding='utf-8')
        if release is None:
            # Retain the draft ID directly from the creation response.
            release = api(f'repos/{repository}/releases', '--method', 'POST',
                          '-f', 'tag_name=latest', '-f', f'target_commitish={sha}',
                          '-F', 'draft=true', '-f', 'name=Latest stable',
                          '-f', f'body={notes}')
        assets = [*archives, checksums]
        run('gh', 'release', 'upload', 'latest', '--repo', repository,
            '--clobber', *(str(path) for path in assets))
        # Only remove old assets after every replacement has uploaded.
        release = api(f'repos/{repository}/releases/{release["id"]}')
        keep = {path.name for path in assets}
        for asset in release['assets']:
            if asset['name'] not in keep:
                run('gh', 'api', '--method', 'DELETE',
                    f'repos/{repository}/releases/assets/{asset["id"]}')
        run('git', 'push', 'origin', f'{sha}:refs/tags/latest', '--force')
        run('gh', 'release', 'edit', 'latest', '--repo', repository,
            '--draft=false', '--prerelease=false', '--latest', '--title', 'Latest stable',
            '--notes-file', str(notes_file))
    print(f'Published {server}/{repository}/releases/tag/latest')


if __name__ == '__main__':
    main()
