/*
 * This file is part of pyQBDI (python binding for QBDI).
 *
 * Copyright 2017 - 2022 Quarkslab
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

#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "pyqbdi.hpp"
#include "QBDIPreload.h"

namespace py = pybind11;

/* Init the QBDIPreload */
QBDIPRELOAD_INIT;

int QBDI::qbdipreload_on_start(void *main) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
  return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_main(int argc, char **argv) {
  const char *fileTool = std::getenv("PYQBDI_TOOL");
  std::vector<std::string> args;

  if (fileTool == nullptr) {
    std::cerr << "QBDI::qbdipreload_on_run(): PYQBDI_TOOL not found !"
              << std::endl;
    exit(1);
  }

  for (int i = 0; i < argc; i++) {
    args.push_back(std::string(argv[i]));
  }

  py::initialize_interpreter();
  {
    // need to initialize sys module after initialize_interpreter()
    py::module_ sys = py::module_::import("sys");
    sys.attr("argv") = args;
    // remove LD_PRELOAD to avoid reuse it in subprocess.popen
    py::module_ os = py::module_::import("os");
#if defined(QBDI_PLATFORM_OSX)
    os.attr("environ").attr("__delitem__")("DYLD_INSERT_LIBRARIES");
#elif defined(QBDI_PLATFORM_LINUX) || defined(QBDI_PLATFORM_ANDROID)
    os.attr("environ").attr("__delitem__")("LD_PRELOAD");
#endif
    py::module_ pyqbdi = py::module_::import("pyqbdi");
    pyqbdi.attr("__preload__") = true;

    // load file before create VM object
    try {
      py::module_ main = py::module_::import("__main__");
      py::object scope = main.attr("__dict__");
      py::eval_file(fileTool, scope);
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      exit(1);
    }
  }

  return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start,
                             QBDI::rword stop) {
  try {
    py::module_ main = py::module_::import("__main__");
    main.attr("pyqbdipreload_on_run")(vm, start, stop);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
  return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_exit(int status) {
  // need to destroy atexit module before call finalize_interpreter.
  {
    py::module_ atexit = py::module_::import("atexit");
    atexit.attr("_run_exitfuncs")();
  }
  py::finalize_interpreter();

  return QBDIPRELOAD_NO_ERROR;
}
