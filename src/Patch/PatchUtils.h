/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#ifndef PATCHUTILS_H
#define PATCHUTILS_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/LLVMCPU.h"
#include "Patch/Types.h"

namespace llvm {
class MCInst;
class MCInstrInfo;
class MCRegisterInfo;
} // namespace llvm

namespace QBDI {

class RelocatableInst;
class PatchGenerator;

// Helper template

// helper to create a object as a unique_ptr
template <typename T, typename U>
class AutoUnique : public T {
public:
  template <typename... Args>
  AutoUnique(Args &&...args) : T(std::forward<Args>(args)...) {}

  template <typename... Args>
  inline static std::unique_ptr<T> unique(Args &&...args) {
    return std::make_unique<U>(std::forward<Args>(args)...);
  };
};

// helper to clone object, when the copy is possible
template <typename T, typename U>
class AutoClone : public AutoUnique<T, U> {
public:
  template <typename... Args>
  AutoClone(Args &&...args) : AutoUnique<T, U>(std::forward<Args>(args)...) {}

  inline std::unique_ptr<T> clone() const override {
    return std::make_unique<U>(*static_cast<const U *>(this));
  };
};

template <class T>
inline void _conv_unique(std::vector<std::unique_ptr<T>> &vec,
                         std::unique_ptr<T> &&u) {
  vec.push_back(std::forward<std::unique_ptr<T>>(u));
}

template <class T, class... Args>
inline void _conv_unique(std::vector<std::unique_ptr<T>> &vec,
                         std::unique_ptr<T> &&u, Args... args) {
  vec.push_back(std::forward<std::unique_ptr<T>>(u));
  _conv_unique<T>(vec, std::forward<Args>(args)...);
}

template <class T, class... Args>
inline std::vector<std::unique_ptr<T>> conv_unique(Args... args) {
  std::vector<std::unique_ptr<T>> vec;
  _conv_unique<T>(vec, std::forward<Args>(args)...);
  return vec;
}

template <class T>
inline std::vector<std::unique_ptr<T>>
cloneVec(const std::vector<std::unique_ptr<T>> &u) {
  std::vector<std::unique_ptr<T>> v;
  std::transform(u.cbegin(), u.cend(), std::back_inserter(v),
                 [](const std::unique_ptr<T> &c) {
                   if (c)
                     return c->clone();
                   return std::unique_ptr<T>();
                 });
  return v;
}

template <class T>
inline void append(std::vector<std::unique_ptr<T>> &u,
                   std::vector<std::unique_ptr<T>> v) {
  std::move(v.begin(), v.end(), std::back_inserter(u));
}

template <class T>
inline void prepend(std::vector<std::unique_ptr<T>> &u,
                    std::vector<std::unique_ptr<T>> v) {
  std::move(u.begin(), u.end(), std::back_inserter(v));
  u.swap(v);
}

// Helper classes

class TempManager {

  std::vector<std::pair<unsigned int, unsigned int>> temps;
  const llvm::MCInst &inst;
  bool allowInstRegister;

public:
  const llvm::MCInstrInfo &MCII;
  const llvm::MCRegisterInfo &MRI;

  TempManager(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
              bool allowInstRegister = false)
      : inst(inst), allowInstRegister(allowInstRegister),
        MCII(llvmcpu.getMCII()), MRI(llvmcpu.getMRI()) {}

  Reg getRegForTemp(unsigned int id);

  Reg::Vec getUsedRegisters() const;

  size_t getUsedRegisterNumber() const;

  unsigned getSizedSubReg(unsigned reg, unsigned size) const;
};

} // namespace QBDI

#endif
