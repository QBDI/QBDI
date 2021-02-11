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
#ifndef QBDI_PLATFORM_H_
#define QBDI_PLATFORM_H_

#include "Config.h"

#ifdef QBDI_PLATFORM_WINDOWS
# define QBDI_ALIGNED(n) __declspec(align(n))
# define QBDI_EXPORT __declspec(dllexport)
# define QBDI_NOINLINE __declspec(noinline)
# define QBDI_NOSTACKPROTECTOR
# define _QBDI_FORCE_USE
# define _QBDI_UNREACHABLE() __assume(0)
#else
# define QBDI_ALIGNED(n) __attribute__((aligned(n)))
# define QBDI_EXPORT __attribute__ ((visibility ("default")))
# define QBDI_NOINLINE __attribute__((noinline))
# define QBDI_NOSTACKPROTECTOR __attribute__((no_stack_protector))
# define _QBDI_FORCE_USE __attribute__((__used__))
# define _QBDI_UNREACHABLE() __builtin_unreachable()
#endif

#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
#define _QBDI_ASAN_ENABLED_
#  endif
#endif

#ifdef __cplusplus
#define _QBDI_EI(X) X
#else
#define _QBDI_EI(X) QBDI_##X
#endif

#endif // QBDI_PLATFORM_H_
