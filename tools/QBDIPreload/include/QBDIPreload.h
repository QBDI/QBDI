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
#ifndef QBDIPRELOAD_H
#define QBDIPRELOAD_H

#include "QBDI.h"

#ifdef __cplusplus
namespace QBDI {
extern "C" {
#endif


/** @brief No error */
#define QBDIPRELOAD_NO_ERROR 0
/** @brief Startup step not handled by callback */
#define QBDIPRELOAD_NOT_HANDLED 1
/** @brief Error in the startup (preload) process */
#define QBDIPRELOAD_ERR_STARTUP_FAILED 2


/**
 * A C pre-processor macro declaring a constructor.
 *
 * @warning `QBDIPRELOAD_INIT` must be used once in any project using QBDIPreload.
 * It declares a constructor, so it must be placed like a function declaration on a single line.
 */
#ifdef __cplusplus
#define QBDIPRELOAD_INIT __attribute__((constructor)) void init() { QBDI::qbdipreload_hook_init(); }
#else
#define QBDIPRELOAD_INIT __attribute__((constructor)) void init() { qbdipreload_hook_init(); }
#endif

/*
 * Public APIs
 */

/** Convert a QBDIPreload GPR context (platform dependent) to a QBDI GPR state.
 * @param[in] gprCtx     Platform GPRState pointer
 * @param[in] gprState   QBDI GPRState pointer
 */
QBDI_EXPORT void qbdipreload_threadCtxToGPRState(const void* gprCtx, GPRState* gprState);

/** Convert a QBDIPreload FPR context (platform dependent) to a QBDI FPR state.
 * @param[in] fprCtx     Platform FPRState pointer
 * @param[in] fprState   QBDI FPRState pointer
 */
QBDI_EXPORT void qbdipreload_floatCtxToFPRState(const void* fprCtx, FPRState* fprState);

/** Enable QBDIPreload hook on the main function (using its address)
 *
 * @warning It MUST be used in `qbdipreload_on_start` if you want to handle this step.
 *          The assumed `main` address is provided as a callback argument.
 *
 * @param[in] main       Pointer to the main function 
 */
QBDI_EXPORT int qbdipreload_hook_main(void *main);

/*
 * QBDIPreload callbacks
 *
 * The following functions are callbacks that MUST be defined
 * in any project based on libboostrap.
 *
 * All of them are a unique step in the preload process.
 * If no custom actions are required in a given step,
 * QBDIPRELOAD_NOT_HANDLED can simply be returned and
 * default behavior will be used.
 */
 
/*! Function called when preload is on a program entry point
 * (interposed start or an early constructor).
 * It provides the main function address, that can be used
 * to place a hook using the `qbdipreload_hook_main` API.
 * @param[in]   main    Address of the main function
 * @return      int     QBDIPreload state
 */
extern int qbdipreload_on_start(void *main);

/*! Function called when preload hook on main function is triggered.
 * It provides original (and platforms dependent) GPR and FPR contexts.
 * They can be converted to QBDI states, using `qbdipreload_threadCtxToGPRState`
 * and `qbdipreload_floatCtxToFPRState` APIs.
 * @param[in]   gprCtx  Original GPR context
 * @param[in]   fpuCtx  Original FPU context
 * @return      int     QBDIPreload state
*/
extern int qbdipreload_on_premain(void *gprCtx, void *fpuCtx);

/*! Function called when preload has successfully hijacked
 * the main thread and we are in place of the original main
 * function (with the same thread state).
 * @param[in]   argc    Original argc
 * @param[in]   argv    Original argv
 * @return      int     QBDIPreload state
 */
extern int qbdipreload_on_main(int argc, char** argv);

/*! Function called when preload is done and we have a valid
 * QBDI VM object on which we can call run (after some last initializations).
  * @param[in]  vm          VM instance.
  * @param[in]  start       Start address of the range (included).
  * @param[in]  stop        End address of the range (excluded).
  * @return     int         QBDIPreload state
 */
extern int qbdipreload_on_run(VMInstanceRef vm, rword start, rword stop);

/*! Function called when process is exiting (using `_exit` or `exit`).
  * @param[in]  status  exit status
  * @return     int     QBDIPreload state
*/
extern int qbdipreload_on_exit(int status);

/*
 * Private API
 */

QBDI_EXPORT int qbdipreload_hook_init();

QBDI_EXPORT void* qbdipreload_setup_exception_handler(uint32_t target, uint32_t mask, void* handler);

#ifdef __cplusplus
} // "C"
}
#endif

#endif // QBDIPRELOAD_H
