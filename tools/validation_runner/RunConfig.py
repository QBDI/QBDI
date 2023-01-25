#
# This file is part of QBDI.
#
# Copyright 2017 - 2023 Quarkslab
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

import os
import sys
import yaml
import multiprocessing

class TestConfig:
    def __init__(self, conf = None):
        if conf == None:
            return
        if 'command' not in conf:
            print('Invalid test ' + str(conf))
            sys.exit(1)
        self.command = conf['command']
        if 'arguments' in conf:
            self.arguments = conf['arguments']
        else:
            self.arguments = []

    @classmethod
    def from_dict(cls, d):
        self = TestConfig()
        self.command = d['command']
        self.arguments = list(d['arguments'].lstrip("['").rstrip("']").split("', '"))
        return self

    def command_line(self):
        return '{} {}'.format(self.command, ' '.join(self.arguments))

class RunConfig:
    def __init__(self, config_file, lib=None, thread=0):

        with open(config_file, 'r') as f:
            conf = yaml.safe_load(f)

        # Conf validation
        if 'tests' not in conf or len(conf['tests']) == 0:
            print('No tests in configuration, exiting!')
            sys.exit(1)
        if 'validator_path' not in conf and lib is None:
            print('No validator_path in configuration, exiting!')
            sys.exit(1)
        if 'database' not in conf:
            print('No database in configuration, exiting!')
            sys.exit(1)
        # Tests
        self.tests = []
        for test in conf['tests']:
            self.tests.append(TestConfig(test))
        # Thread
        if thread != 0:
            self.thread = thread
            assert self.thread > 0
        elif 'threads' in conf:
            self.thread = int(conf['threads'])
            assert self.thread > 0
        else:
            self.thread = max(1, multiprocessing.cpu_count() // 2)
        # Validator path
        self.validator_path = lib if lib is not None else conf['validator_path']
        # Database
        self.database = conf['database']
