#
# This file is part of QBDI.
#
# Copyright 2017 Quarkslab
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import os
import time
import multiprocessing
import subprocess

from TestResult import TestResult
from RunResult import RunResult

def run_test(test, env, idx):
    print('[{}] Validating {}'.format(idx, test.command_line()))
    # Setup files
    result = open('.{}_result'.format(idx), 'w')
    output = open('.{}_output'.format(idx), 'w')
    env['VALIDATOR_COVERAGE'] = '.{}_coverage'.format(idx)
    # Execute
    process = subprocess.Popen([test.command] + test.arguments, stdout=output, stderr=result, env=env)
    retcode = process.wait()
    output.close()
    result.close()
    if retcode == 0:
        print('[{}] Validated {}'.format(idx, test.command_line()))
    else:
        print('[{}] Failed validation {}'.format(idx, test.command_line()))
    # Parse test results and remove test files
    with open('.{}_result'.format(idx), 'r') as f:
        result = f.read()
        f.close()
    with open('.{}_coverage'.format(idx), 'r') as f:
        coverage = f.read()
        f.close()
    test_result = TestResult(test, retcode, result, coverage)
    os.remove('.{}_result'.format(idx))
    os.remove('.{}_coverage'.format(idx))
    os.remove('.{}_output'.format(idx))
    return test_result

class RunOrchestrator:
    def __init__(self, run_cfg):
        self.run_cfg = run_cfg

    def run(self):
        pool = multiprocessing.Pool(processes=self.run_cfg.thread)
        tests = self.run_cfg.tests
        async_res = []
        # Schedule validation on multiple process
        for idx in range(len(tests)):
            env = dict(os.environ, LD_PRELOAD=self.run_cfg.validator_path, VALIDATOR_VERBOSITY='Detail')
            async_res.append(pool.apply_async(run_test, (tests[idx], env, idx)))
        test_results = []
        for idx in range(len(tests)):
            test_results.append(async_res[idx].get())
        run_result = RunResult(test_results)
        return run_result
