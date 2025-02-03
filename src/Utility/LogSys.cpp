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
#include <utility>

#include "Engine/LLVMCPU.h"
#include "Patch/Patch.h"
#include "Utility/LogSys.h"

#include "spdlog/common.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef QBDI_PLATFORM_ANDROID
#include "spdlog/sinks/android_sink.h"
#endif

// windows seem to define a macro on ERROR
#ifdef QBDI_PLATFORM_WINDOWS
#ifdef ERROR
#undef ERROR
#endif
#endif

namespace QBDI {
namespace {

class Logger {
public:
  Logger(void) {
    setDefaultLogger();
    spdlog::set_pattern("%^[%l] (%!) %s:%#%$ %v");
    spdlog::set_level(spdlog::level::info);
  }

  Logger(const Logger &) = delete;

  Logger &operator=(const Logger &) = delete;

  void setPriority(LogPriority priority) {
    switch (priority) {
      case LogPriority::DEBUG:
        spdlog::set_level(spdlog::level::debug);
        break;
      default:
      case LogPriority::INFO:
        spdlog::set_level(spdlog::level::info);
        break;
      case LogPriority::WARNING:
        spdlog::set_level(spdlog::level::warn);
        break;
      case LogPriority::ERROR:
        spdlog::set_level(spdlog::level::err);
        break;
      case LogPriority::DISABLE:
        spdlog::set_level(spdlog::level::off);
        break;
    }
  }

  void setFile(const std::string &f, bool truncate) {
    sink = spdlog::basic_logger_mt("QBDI", f, truncate);
    spdlog::set_default_logger(sink);
  }

  void setDefaultLogger() {
#ifdef QBDI_PLATFORM_ANDROID
    sink = spdlog::android_logger_mt("QBDI", "qbdi");
#else
    sink = spdlog::stderr_color_mt("console");
#endif
    spdlog::set_default_logger(sink);
  }

  void setConsoleLogger() {
    sink = spdlog::stderr_color_mt("console");
    spdlog::set_default_logger(sink);
  }

  ~Logger(void) = default;

private:
  std::shared_ptr<spdlog::logger> sink;
};

static Logger logger;

} // namespace

// =========================

void qbdi_setLogFile(const char *filename, bool truncate) {
  logger.setFile(std::string(filename), truncate);
}

void setLogFile(const std::string &filename, bool truncate) {
  logger.setFile(filename, truncate);
}

void qbdi_setLogPriority(LogPriority priority) { logger.setPriority(priority); }

void qbdi_setLogConsole() { logger.setConsoleLogger(); }

void qbdi_setLogDefault() { logger.setDefaultLogger(); }

// ====================================================

std::string format_as(const QBDI::Patch &patch) {

  std::string disass =
      patch.llvmcpu->showInst(patch.metadata.inst, patch.metadata.address);
  unsigned opcode = patch.metadata.inst.getOpcode();
  const char *opcodeName = patch.llvmcpu->getInstOpcodeName(opcode);

#ifdef SPDLOG_USE_STD_FORMAT
  std::string message = std::vformat(
#else
  std::string message = fmt::format(
#endif
      "0x{:x}: {} | {} ({}) | ({:n}){}", patch.metadata.address, disass,
      opcodeName, opcode,
      spdlog::to_hex(reinterpret_cast<uint8_t *>(patch.metadata.address),
                     reinterpret_cast<uint8_t *>(patch.metadata.endAddress())),
#if defined(QBDI_ARCH_ARM)
      (patch.llvmcpu->getCPUMode() == QBDI::CPUMode::ARM) ? " (ARM mode)"
                                                          : " (Thumb mode)"
#else
      ""
#endif
  );
  return message;
}

rword format_as(Options opt) { return fmt::underlying(opt); }
unsigned format_as(CPUMode mode) { return fmt::underlying(mode); }
unsigned format_as(InstPosition pos) { return fmt::underlying(pos); }
unsigned format_as(AnalysisType t) { return fmt::underlying(t); }
unsigned format_as(const RegLLVM &r) { return r.getValue(); }
unsigned format_as(const Reg &r) { return r.getID(); }
rword format_as(const Shadow &s) { return s.getTag(); }
uint16_t format_as(ShadowReservedTag tag) { return tag; }
rword format_as(const Constant &cst) { return cst; }
int64_t format_as(const Offset &off) { return off; }
unsigned format_as(const Temp &t) { return t; }
unsigned format_as(const Operand &op) { return op; }
unsigned format_as(RelocatableInstTag tag) { return tag; }

} // namespace QBDI
