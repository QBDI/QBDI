#!/usr/bin/env python3

# This script will install the latest artifact from 'PyQBDI Linux package' github action

import dateutil.parser
import io
import os
import platform
from pygit2 import Repository
import re
import requests
import subprocess
import sys
import tempfile
import time
import zipfile

base_url = "https://api.github.com/repos/QBDI/QBDI"
default_headers = {"Accept":"application/vnd.github.v3+json",
                   "Authorization":"token {}".format(os.environ.get('GITHUB_TOKEN')) }
workflow_name = "PyQBDI Linux package"
default_per_page = 100

if sys.maxsize > 2**32:
    artifact_name = "PyQBDI_linux_X86_64"
    is_64bits = True
else:
    artifact_name = "PyQBDI_linux_X86"
    is_64bits = False


def do_get_request(path, params={}, headers={}, retry=10, retry_sleep=1, binary=False):

    for i in range(retry):
        r = requests.get(base_url + path, params=params, headers={**default_headers, **headers})
        if r.status_code == 200:
            success = True
            break
        else:
            print('[-] Fail to get {}: received {}'.format(base_url + path, r.status_code))
            success = False
            if i < retry - 1:
                time.sleep(retry_sleep)
    r.raise_for_status()
    if binary:
        return r.content
    return r.json()

def get_workflow_id(name):

    nb_workflows = 1
    workflows = []
    page = 0
    while len(workflows) != nb_workflows:
        page += 1
        d = do_get_request("/actions/workflows", params={"per_page": default_per_page, "page": page})
        nb_workflows = d["total_count"]
        workflows += d["workflows"]

    workflow_id = -1
    for w in workflows:
        if w['name'] == name:
            print(f"[+] Found workflow_id {w['id']} for workflow '{workflow_name}'")
            return w['id']

    assert False, f"Cannot found workflow '{workflow_name}'"

def get_last_workflow_run(ident, branch=None, commit_hash=None):

    nb_runs = 1
    runs = []
    page = 0
    while len(runs) != nb_runs:
        page += 1
        p = {"per_page": default_per_page, "page": page}
        if branch != None:
            p['branch'] = branch
        d = do_get_request(f"/actions/workflows/{ident}/runs", params=p)
        nb_runs = d["total_count"]
        runs += d["workflow_runs"]

    if runs == []:
        print(f"[-] No runs found for workflow id={ident} with branch='{branch}'")
        return None

    print(f"[+] {len(runs)} runs found for workflow id={ident} with branch='{branch}'")

    last_run = None
    for r in runs:
        if r['event'] not in ['push', 'workflow_dispatch']:
            continue
        if commit_hash != None and r['head_sha'] != commit_hash:
            continue
        if r['status'] != 'completed' or r['conclusion'] != 'success':
            continue
        if last_run is None or dateutil.parser.isoparse(last_run['created_at']) < dateutil.parser.isoparse(r['created_at']):
            last_run = r

    if last_run == None:
        print(f"[-] no complete run for branch='{branch}' commit_hash={commit_hash}")
        return None

    print(f"[+] found run {last_run['id']} create at {last_run['created_at']}")
    return last_run

def get_artifact(run_id, artifact_name):
    nb_artifacts = 1
    artifacts = []
    page = 0

    i = 1
    while len(artifacts) != nb_artifacts:
        page += 1
        d = do_get_request(f"/actions/runs/{run_id}/artifacts", params={"per_page": default_per_page, "page": page})
        nb_artifacts = d["total_count"]
        artifacts += d["artifacts"]

    for artifact in artifacts:
        if artifact['name'] == artifact_name:
            print(f"[+] Found artifact {artifact['id']} create at {artifact['created_at']}")
            return artifact

    assert False, f"Cannot found artifact '{artifact_name}' in run {run_id}"

def download_wheel(artifact):

    assert not artifact['expired'], "artifact {artifact['id']} as expired"

    art = do_get_request(f"/actions/artifacts/{artifact['id']}/zip", binary=True)

    name_regex = re.compile("^PyQBDI-.*-cp{}{}-.*\\.whl$".format(sys.version_info.major, sys.version_info.minor))
    with zipfile.ZipFile(io.BytesIO(art)) as zip_archive:
        for f in zip_archive.infolist():
            if bool(name_regex.match(f.filename)):
                print(f'[+] extract {f.filename}')
                with zip_archive.open(f, 'r') as f_wheel:
                    return f.filename, f_wheel.read()

    assert False, f"Cannot found wheel that match {name_regex.pattern}"

def install_wheel(wheelname, wheeldata):

    with tempfile.TemporaryDirectory() as work_dir:
        wheel_path = work_dir + '/' + wheelname
        with open(wheel_path, 'wb') as f:
            f.write(wheeldata)

        print(f'[+] install {wheel_path}')

        subprocess.check_call([sys.executable, "-m", "pip", "install", wheel_path])

def check_installation():
    print(f'[+] test installation')
    subprocess.check_call([sys.executable, "-c", "import pyqbdi; print(pyqbdi.__version__)"])

def install_pyqbdi():
    git_repo = Repository('.')
    current_branch = os.environ.get('READTHEDOCS_VERSION', git_repo.head.shorthand)
    if current_branch in ['latest', 'stable']:
        current_branch = 'master'
    current_hash = git_repo.head.target.hex

    workflow_ID = get_workflow_id(workflow_name)

    # search a run with this hash commit on this branch
    run = get_last_workflow_run(workflow_ID, branch=current_branch, commit_hash=current_hash)
    if run == None:
        # search a run with this hash commit (branch filter isn't not always associated with a run)
        run = get_last_workflow_run(workflow_ID, branch=None, commit_hash=current_hash)
    if run == None:
        # search a run on this branch
        run = get_last_workflow_run(workflow_ID, branch=current_branch)
    if run == None and current_branch != "dev-next":
        # search a run on dev-next
        run = get_last_workflow_run(workflow_ID, "dev-next")

    assert run != None, f"No compatible runs found for workflow {workflow_name} (id={workflow_ID})"

    assert run['status'] == 'completed' and run['conclusion'] == 'success', "Cannot install a fail run"

    artifact = get_artifact(run['id'], artifact_name)
    wheelname, wheeldata = download_wheel(artifact)

    install_wheel(wheelname, wheeldata)

    check_installation()

if __name__ == "__main__":
    install_pyqbdi()

