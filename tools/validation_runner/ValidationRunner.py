#!/usr/bin/env python3
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
#
import sys

from SQLite import SQLiteDBAdapter
from RunConfig import RunConfig, TestConfig
from RunOrchestrator import RunOrchestrator
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-l", "--lib", type=str, default=None, help="Override preload lib location")
    parser.add_argument("-t", "--thread", type=int, default=0, help="Number of thread")
    parser.add_argument("configFile", type=str, default=None)

    args = parser.parse_args()

    run_cfg = RunConfig(args.configFile, args.lib)
    db  = SQLiteDBAdapter(run_cfg.database)
    orchestrator = RunOrchestrator(run_cfg)
    run_result = orchestrator.run()
    run_result.print_stats()
    reg = run_result.compartive_analysis(db)
    run_result.write_to_db(db)
    sys.exit(reg)
