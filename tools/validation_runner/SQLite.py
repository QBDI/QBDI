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
import time
import sqlite3

from RunResult import RunResult
from TestResult import TestResult

class SQLiteDBAdapter:
    def __init__(self, db_path):
       self.connection = sqlite3.connect(db_path)
       self.connection.row_factory = sqlite3.Row
       self.setup_db()

    def setup_db(self):
        cursor = self.connection.cursor()
        cursor.execute('''CREATE TABLE IF NOT EXISTS Runs (
                            run_id INTEGER PRIMARY KEY AUTOINCREMENT, 
                            branch TEXT, 
                            "commit" TEXT,
                            timestamp INTEGER,
                            total_tests INTEGER,
                            passed_tests INTEGER,
                            total_instr INTEGER,
                            unique_instr INTEGER,
                            errors INTEGER,
                            no_impact_err INTEGER,
                            non_critical_err INTEGER,
                            critical_err INTEGER,
                            cascades INTEGER,
                            no_impact_casc INTEGER,
                            non_critical_casc INTEGER,
                            critical_casc INTEGER,
                            coverage_log TEXT);''')
        cursor.execute('''CREATE TABLE IF NOT EXISTS Tests (
                            test_id INTEGER PRIMARY KEY AUTOINCREMENT, 
                            run_id INTEGER,
                            command TEXT,
                            arguments TEXT,
                            binary_hash TEXT,
                            retcode INTEGER,
                            total_instr INTEGER,
                            unique_instr INTEGER,
                            diff_map INTEGER,
                            errors INTEGER,
                            no_impact_err INTEGER,
                            non_critical_err INTEGER,
                            critical_err INTEGER,
                            cascades INTEGER,
                            no_impact_casc INTEGER,
                            non_critical_casc INTEGER,
                            critical_casc INTEGER,
                            cascades_log TEXT,
                            coverage_log TEXT);''')
        cursor.execute('''CREATE INDEX IF NOT EXISTS RunIdx on Tests (run_id);''')
        self.connection.commit()

    def insert_run_result(self, run_result):
        cursor = self.connection.cursor()
        cursor.execute('''INSERT INTO Runs (branch, "commit", timestamp, total_tests, passed_tests, 
                          total_instr, unique_instr, errors, no_impact_err, non_critical_err, 
                          critical_err, cascades, no_impact_casc, non_critical_casc, critical_casc, 
                          coverage_log) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);''', 
                          (run_result.branch, run_result.commit, int(time.time()), run_result.total_tests,
                          run_result.passed_tests, run_result.total_instr, run_result.unique_instr,
                          run_result.errors, run_result.no_impact_err, run_result.non_critical_err,
                          run_result.critical_err, run_result.cascades, run_result.no_impact_casc,
                          run_result.non_critical_casc, run_result.critical_casc, run_result.coverage_log))
        run_id = cursor.lastrowid
        self.connection.commit()
        return run_id

    def insert_test_result(self, run_id, test_result):
        cursor = self.connection.cursor()
        cursor.execute('''INSERT INTO Tests (run_id, command, arguments, binary_hash, retcode,
                          total_instr, unique_instr, diff_map, errors, no_impact_err, non_critical_err, 
                          critical_err, cascades, no_impact_casc, non_critical_casc, critical_casc, 
                          cascades_log, coverage_log) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);''', 
                          (run_id, test_result.cfg.command, str(test_result.cfg.arguments), 
                          test_result.binary_hash, test_result.retcode, test_result.total_instr, 
                          test_result.unique_instr, test_result.diff_map, test_result.errors, 
                          test_result.no_impact_err, test_result.non_critical_err, test_result.critical_err, 
                          test_result.cascades, test_result.no_impact_casc, test_result.non_critical_casc, 
                          test_result.critical_casc, test_result.cascades_log, test_result.coverage_log))
        run_id = cursor.lastrowid
        self.connection.commit()
        return run_id

    def get_last_run(self, branch):
        cursor = self.connection.cursor()
        # Find run result
        cursor.execute('select * from Runs where branch=? order by timestamp desc;', (branch,))
        row = cursor.fetchone()
        if row == None:
            return None
        # Rebuild RunResult object Run row
        run_result = RunResult.from_dict({k: row[k] for k in row.keys()})
        # Find test results
        for row in cursor.execute('select * from Tests where run_id=?;', (row['run_id'],)):
            # Rebuild TestResult object Test row
            run_result.test_results.append(TestResult.from_dict({k: row[k] for k in row.keys()}))
        return run_result
