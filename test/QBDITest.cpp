/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <time.h>
#include <gtest/gtest.h>

#include "ExecBlock/ExecBlockTest.h"
#include "ExecBlock/ExecBlockManagerTest.h"
#include "Utility/LogSys.h"

int main(int argc, char** argv) {

    srand(time(nullptr));

    // This allows the user to override the flag on the command line.
    ::testing::InitGoogleTest(&argc, argv);
#ifdef _QBDI_DEBUG
    QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::DEBUG);
#endif

    return RUN_ALL_TESTS();
}
