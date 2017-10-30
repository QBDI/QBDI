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
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#if defined(_MSC_VER)

    #define QBDI_OS_WIN
    #define _QBDI_UNREACHABLE() __assume(0)

    #if defined(_M_X86)
        #define QBDI_ARCH_X86
        #define QBDI_BITS_32
    #elif defined(_M_X64)
        #define QBDI_ARCH_X86_64
        #define QBDI_BITS_64
    #endif

#endif

#if defined(__GNUC__) || defined(__clang__)

    #define _QBDI_UNREACHABLE() __builtin_unreachable()

    #if defined(__ANDROID__)
        #define QBDI_OS_ANDROID
    #elif defined(__APPLE__)
        #define QBDI_OS_DARWIN
        #if (defined(__arm__) || defined(__aarch64__))
            #define QBDI_OS_IOS
        #else
            #define QBDI_OS_MACOS
        #endif
    #elif defined(__linux__)
        #define QBDI_OS_LINUX
    #elif defined(_WIN32)
        #define QBDI_OS_WIN
    #endif

    #if defined(__i386__)
        #define QBDI_ARCH_X86
        #define QBDI_BITS_32
    #elif defined(__x86_64__)
        #define QBDI_ARCH_X86_64
        #define QBDI_BITS_64
    #elif defined(__aarch64__)
        #define QBDI_ARCH_AARCH64
        #define QBDI_BITS_64
    #elif defined(__arm__)
        #define QBDI_ARCH_ARM
        #define QBDI_BITS_32
    #endif

#endif

#if defined(_MSC_VER)
# define QBDI_NOINLINE __declspec(noinline)
# define QBDI_EXPORT __declspec(dllexport)
# define _QBDI_FORCE_USE
#else  // _MSC_VER
# define QBDI_NOINLINE __attribute__((noinline))
# define QBDI_EXPORT __attribute__ ((visibility ("default")))
# define _QBDI_FORCE_USE __attribute__((__used__))
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

#endif // _PLATFORM_H_
