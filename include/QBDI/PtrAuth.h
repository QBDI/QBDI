/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#ifndef QBDI_PTRAUTH_H_
#define QBDI_PTRAUTH_H_

#include "QBDI/Config.h"
#include "QBDI/State.h"

namespace QBDI {
struct real_addr_t {
  explicit real_addr_t() = default;
};

} // namespace QBDI

#ifdef QBDI_PTRAUTH

#ifndef QBDI_ARCH_AARCH64
#error "PTRAUTH is only implemented for AARCH64 architecture"
#elif defined(QBDI_PLATFORM_OSX) || defined(QBDI_PLATFORM_IOS)

#include <ptrauth.h>
namespace QBDI {
struct auth_addr_t {
  explicit auth_addr_t() = default;
};

template <typename T>
inline T strip_ptrauth(T addr) {
  static_assert(sizeof(T) == sizeof(void *), "Invalid type");
  return reinterpret_cast<T>(strip_ptrauth(reinterpret_cast<void *>(addr)));
}

template <typename T>
inline T sign_code_ptrauth(T addr) {
  static_assert(sizeof(T) == sizeof(void *), "Invalid type");
  return reinterpret_cast<T>(sign_code_ptrauth(reinterpret_cast<void *>(addr)));
}

template <>
inline void *strip_ptrauth<void *>(void *addr) {
  return ptrauth_strip(addr, ptrauth_key_asia);
}

template <>
inline void *sign_code_ptrauth<void *>(void *addr) {
  return ptrauth_sign_unauthenticated(strip_ptrauth(addr), ptrauth_key_asia, 0);
}

} // namespace QBDI
#else
#error "PTRAUTH is only implemented for OSX and IOS"
#endif

#else // QBDI_PTRAUTH

namespace QBDI {
using auth_addr_t = real_addr_t;

template <typename T>
inline T strip_ptrauth(T addr) {
  static_assert(sizeof(T) == sizeof(void *), "Invalid type");
  return addr;
}

template <typename T>
inline T sign_code_ptrauth(T addr) {
  static_assert(sizeof(T) == sizeof(void *), "Invalid type");
  return addr;
}

} // namespace QBDI
#endif

#endif // QBDI_PTRAUTH_H_
