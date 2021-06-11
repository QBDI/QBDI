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
#include "Utility/LogSys.h"

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

static QBDI::Logger logger;

namespace QBDI {

Logger::Logger(void) {
  setDefaultLogger();
  spdlog::set_pattern("%^[%l] (%!) %s:%#%$ %v");
  spdlog::set_level(spdlog::level::info);
}

Logger::~Logger(void) = default;

void Logger::setDefaultLogger() {
#ifdef QBDI_PLATFORM_ANDROID
  sink = spdlog::android_logger_mt("QBDI", "qbdi");
#else
  sink = spdlog::stderr_color_mt("console");
#endif
  spdlog::set_default_logger(sink);
}

void Logger::setConsoleLogger() {
  sink = spdlog::stderr_color_mt("console");
  spdlog::set_default_logger(sink);
}

void Logger::setPriority(LogPriority priority) {
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

void Logger::setFile(const std::string &f, bool truncate) {
  sink = spdlog::basic_logger_mt("QBDI", f, truncate);
  spdlog::set_default_logger(sink);
}

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
