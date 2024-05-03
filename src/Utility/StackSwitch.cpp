/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

#include "Utility/StackSwitch.h"

#if defined(QBDI_PLATFORM_WINDOWS)
extern "C" QBDI::rword
qbdi_asmStackSwitch(void *switchContext, QBDI::rword newStack,
                    QBDI::rword (*internalHandler)(void *, QBDI::rword));
#else
extern QBDI::rword qbdi_asmStackSwitch(
    void *switchContext, QBDI::rword newStack,
    QBDI::rword (*internalHandler)(void *,
                                   QBDI::rword)) asm("__qbdi_asmStackSwitch");
#endif

namespace QBDI {
namespace {

QBDI::rword stackSwitchInternalHandler(void *switchContext,
                                       QBDI::rword newStackPtr) {
  auto handler =
      static_cast<std::function<QBDI::rword(QBDI::rword)> *>(switchContext);
  return (*handler)(newStackPtr);
}
} // anonymous namespace

rword switchStack(void *newStackPtr, std::function<rword(rword)> handler) {
  auto handler_ = handler;

  return qbdi_asmStackSwitch(&handler_, reinterpret_cast<rword>(newStackPtr),
                             &stackSwitchInternalHandler);
}

} // namespace QBDI
