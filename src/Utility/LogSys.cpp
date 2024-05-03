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
void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const Patch &patch) {
  dump_inst(level, message, loc, *(patch.llvmcpu), patch.metadata.inst,
            patch.metadata.address, patch.metadata.endAddress());
}

void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const LLVMCPU &llvmcpu,
               const InstMetadata &metadata) {
  dump_inst(level, message, loc, llvmcpu, metadata.inst, metadata.address,
            metadata.endAddress());
}

void dump_inst(spdlog::level::level_enum level,
               const spdlog::memory_buf_t &message,
               const spdlog::source_loc &loc, const LLVMCPU &llvmcpu,
               const llvm::MCInst &inst, rword address, rword endAddress) {

  std::string disass = llvmcpu.showInst(inst, address);
  unsigned opcode = inst.getOpcode();
  const char *opcodeName = llvmcpu.getInstOpcodeName(opcode);
  spdlog::default_logger_raw()->log(
      loc, level, "{} 0x{:x}: {} | {} ({}) | ({:n}){}",
      spdlog::string_view_t(message.data(), message.size()), address, disass,
      opcodeName, opcode,
      spdlog::to_hex(reinterpret_cast<uint8_t *>(address),
                     reinterpret_cast<uint8_t *>(endAddress)),
#if defined(QBDI_ARCH_ARM)
      (llvmcpu == CPUMode::ARM) ? " (ARM mode)" : " (Thumb mode)"
#else
      ""
#endif
  );
}

// ====================================================

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

} // namespace QBDI
