/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#ifndef LOGSYS_H
#define LOGSYS_H

#include <memory>
#include <string>

#include "QBDI/Config.h"
#include "QBDI/Logs.h"
#include "QBDI/State.h"
#include "spdlog/spdlog.h"

namespace llvm {
class MCInst;
}

namespace QBDI {
class LLVMCPU;
class InstMetadata;
class Patch;

namespace {

template <typename... Args>
spdlog::memory_buf_t logger_early_format_(spdlog::format_string_t<Args...> fmt,
                                          Args &&...args) {
#ifdef SPDLOG_USE_STD_FORMAT
  return std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
#else
  spdlog::memory_buf_t buf;
  fmt::detail::vformat_to(buf, spdlog::string_view_t(fmt),
                          fmt::make_format_args(std::forward<Args>(args)...));
  return buf;
#endif
}

} // namespace

#define QBDI_CURRENT_POS \
  spdlog::source_loc { __FILE__, __LINE__, SPDLOG_FUNCTION }

void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const Patch &patch);

void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const LLVMCPU &llvmcpu,
               const InstMetadata &metadata);

void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const LLVMCPU &llvmcpu,
               const llvm::MCInst &inst, rword address, rword endAddress);

#define QBDI_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define QBDI_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define QBDI_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define QBDI_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#if defined(QBDI_LOG_DEBUG)
#define QBDI_DEBUG_BLOCK(block) block
#define QBDI_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define QBDI_DUMP_PATCH_DEBUG(patch, ...)                            \
  dump_inst(spdlog::level::debug, logger_early_format_(__VA_ARGS__), \
            QBDI_CURRENT_POS, (patch))
#else
#define QBDI_DEBUG_BLOCK(block) (void)0
#define QBDI_DEBUG(...) (void)0
#define QBDI_DUMP_PATCH_DEBUG(...) (void)0
#endif

#define QBDI_REQUIRE(req)                      \
  if (!(req)) {                                \
    QBDI_ERROR("Assertion Failed : {}", #req); \
  }
#define QBDI_REQUIRE_ACTION(req, ac)           \
  if (!(req)) {                                \
    QBDI_ERROR("Assertion Failed : {}", #req); \
    ac;                                        \
  }
#define QBDI_REQUIRE_ABORT(req, ...) \
  if (!(req)) {                      \
    QBDI_CRITICAL(__VA_ARGS__);      \
    spdlog::shutdown();              \
    abort();                         \
  }
#define QBDI_REQUIRE_ABORT_PATCH(req, patch, ...)                         \
  if (!(req)) {                                                           \
    dump_inst(spdlog::level::critical, logger_early_format_(__VA_ARGS__), \
              QBDI_CURRENT_POS, (patch));                                 \
    spdlog::shutdown();                                                   \
    abort();                                                              \
  }
#define QBDI_ABORT(...)         \
  do {                          \
    QBDI_CRITICAL(__VA_ARGS__); \
    spdlog::shutdown();         \
    abort();                    \
  } while (0);
#define QBDI_ABORT_PATCH(patch, ...)                                      \
  do {                                                                    \
    dump_inst(spdlog::level::critical, logger_early_format_(__VA_ARGS__), \
              QBDI_CURRENT_POS, (patch));                                 \
    spdlog::shutdown();                                                   \
    abort();                                                              \
  } while (0);

} // namespace QBDI

#endif
