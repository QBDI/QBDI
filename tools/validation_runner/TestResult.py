#
# This file is part of QBDI.
#
# Copyright 2017 - 2025 Quarkslab
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
import re
import hashlib
from operator import itemgetter

from RunConfig import TestConfig

class Scan_Pattern_Exception(Exception):
    pass

def scan_for_pattern(data, pattern):
    m = re.search(pattern, data)
    if m != None:
        return m.groups()
    raise Scan_Pattern_Exception("Not found {} in :\n{}".format(pattern, data))

def scan_for_multipattern(data, pattern):
    r = {}
    for m in re.finditer(pattern, data):
        r[m.groups()[0]] = r.get(m.groups()[0], 0) + 1
    return r

def coverage_to_log(coverage):
    coverage = list(coverage)
    coverage.sort(key=itemgetter(1), reverse=True)
    return '\n'.join(map(lambda x: '{}: {}'.format(x[0], x[1]), coverage))

class TestResult:
    def __init__(self, cfg = None, retcode = None, result = None, coverage = None, error=False):
        if cfg == None or retcode == None or result == None or coverage == None:
            return
        self.cfg = cfg
        self.retcode = retcode
        self.exec_error = error
        self.binary_hash = self.get_binary_hash()
        # Process coverage file, rebuilding a dictionnary from it
        self.coverage = {}
        self.memaccess_unique = {}
        if not error:
            for line in coverage.split('\n'):
                if ':' in line:
                    inst, count = line.split(':')
                    self.coverage[inst] = int(count)

            try:
                # Process result file, getting statistics
                self.total_instr = int(scan_for_pattern(result, 'Executed (\d+) total instructions')[0])
                self.unique_instr = int(scan_for_pattern(result, 'Executed (\d+) unique instructions')[0])
                self.diff_map = int(scan_for_pattern(result, 'Encountered (\d+) difference mappings')[0])
                self.errors = int(scan_for_pattern(result, 'Encountered (\d+) errors')[0])
                self.no_impact_err = int(scan_for_pattern(result, 'No impact errors: (\d+)')[0])
                self.non_critical_err = int(scan_for_pattern(result, 'Non critical errors: (\d+)')[0])
                self.critical_err = int(scan_for_pattern(result, 'Critical errors: (\d+)')[0])
                self.cascades = int(scan_for_pattern(result, 'Encountered (\d+) error cascades')[0])
                self.no_impact_casc = int(scan_for_pattern(result, 'No impact cascades: (\d+)')[0])
                self.non_critical_casc = int(scan_for_pattern(result, 'Non critical cascades: (\d+)')[0])
                self.critical_casc = int(scan_for_pattern(result, 'Critical cascades: (\d+)')[0])
                self.memaccess_error = int(scan_for_pattern(result, 'Encountered (\d+) memoryAccess errors')[0])
                self.memaccess_unique_error = int(scan_for_pattern(result, 'Encountered (\d+) memoryAccess unique errors')[0])
                self.output_len_dbg = int(scan_for_pattern(result, 'SizeOutput: (\d+) (\d+)')[0])
                self.output_len_dbi = int(scan_for_pattern(result, 'SizeOutput: (\d+) (\d+)')[1])
                self.same_output = 1 if 'SameOutput: True' in result else 0
            except Scan_Pattern_Exception as e:
                print("[!] {}".format(e))
                error = True
                self.retcode = 255
            else:
                # Storing logs
                memAccess_start = result.find('Error MemoryAccess:')
                cascade_start = result.find('Error cascades:')
                self.memaccess_log = result[memAccess_start:cascade_start]
                self.cascades_log = result[cascade_start:]
                self.coverage_log = coverage_to_log(self.coverage.items())

                self.memaccess_unique = scan_for_multipattern(self.memaccess_log, "MemoryAccess Error \(mnemonic : ([^\)]+)\)")
                self.memaccess_unique_log = coverage_to_log(self.memaccess_unique.items())
        if error:
            # Process result file, getting statistics
            self.total_instr = 0
            self.unique_instr = 0
            self.diff_map = 0
            self.errors = 0
            self.no_impact_err = 0
            self.non_critical_err = 0
            self.critical_err = 0
            self.cascades = 0
            self.no_impact_casc = 0
            self.non_critical_casc = 0
            self.critical_casc = 0
            self.memaccess_error = 0
            self.memaccess_unique_error = 0
            self.output_len_dbg = 0
            self.output_len_dbi = 0
            self.same_output = 0

            # Storing logs
            self.memaccess_log = ""
            self.cascades_log = ""
            self.coverage_log = ""
            self.memaccess_unique_log = ""

    @classmethod
    def from_dict(cls, d):
        self = TestResult()
        self.binary_hash = d['binary_hash']
        self.retcode = d['retcode']
        self.total_instr = d['total_instr']
        self.unique_instr = d['unique_instr']
        self.diff_map = d['diff_map']
        self.errors = d['errors']
        self.no_impact_err = d['no_impact_err']
        self.non_critical_err = d['non_critical_err']
        self.critical_err = d['critical_err']
        self.cascades = d['cascades']
        self.no_impact_casc = d['no_impact_casc']
        self.non_critical_casc = d['non_critical_casc']
        self.critical_casc = d['critical_casc']
        self.memaccess_error = d['memaccess_error']
        self.memaccess_unique_error = d['memaccess_unique_error']
        self.memaccess_log = d['memaccess_log']
        self.cascades_log = d['cascades_log']
        self.coverage_log = d['coverage_log']
        self.memaccess_unique_log = d['memaccess_unique_log']
        self.output_len_dbg = d['output_len_dbg']
        self.output_len_dbi = d['output_len_dbi']
        self.same_output = d['same_output']
        #Rebuild coverage
        self.coverage = {}
        self.memaccess_unique = {}
        for line in self.coverage_log.split('\n'):
            if ':' in line:
                inst, count = line.split(':')
                self.coverage[inst] = int(count)
        for line in self.memaccess_unique_log.split('\n'):
            if ':' in line:
                inst, count = line.split(':')
                self.memaccess_unique[inst] = int(count)
        #Rebuild config
        self.cfg = TestConfig.from_dict(d)
        return self

    def get_binary_hash(self):
        for path in os.environ["PATH"].split(os.pathsep):
            realpath = os.path.join(path.strip('"'), self.cfg.command)
            if os.path.isfile(realpath):
                h = hashlib.sha256()
                with open(realpath, 'rb') as f:
                    h.update(f.read())
                return h.hexdigest()
        return 'UNKNOWN'
