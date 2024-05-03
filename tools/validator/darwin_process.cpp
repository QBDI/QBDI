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
#include "darwin_process.h"
#include "validator.h"
#include "QBDIPreload.h"

#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/mach_traps.h>
#include <mach/mach_vm.h>
#include <mach/thread_act.h>
#include <mach/vm_prot.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "Utility/LogSys.h"

pthread_mutex_t STATUS_LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t STATUS_COND = PTHREAD_COND_INITIALIZER;

enum Status { Running, Stopped, Crashed, Exited } STATUS;

kern_return_t onBreakpoint(mach_port_t exception_port, mach_port_t thread,
                           mach_port_t task, exception_type_t exception,
                           mach_exception_data_t code,
                           mach_msg_type_number_t codeCnt) {
  kern_return_t kr;
  mach_msg_type_number_t count;
  THREAD_STATE threadState;

  pthread_mutex_lock(&STATUS_LOCK);

  if (exception == EXC_BREAKPOINT) {
    // Getting thread state
    count = THREAD_STATE_COUNT;
    kr = thread_get_state(thread, THREAD_STATE_ID, (thread_state_t)&threadState,
                          &count);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR("Failed to get thread state\n");
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }

#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
    // x86 breakpoint quirk
    threadState.THREAD_STATE_PC -= 1;
#endif

    // Setting thread state
    count = THREAD_STATE_COUNT;
    kr = thread_set_state(thread, THREAD_STATE_ID, (thread_state_t)&threadState,
                          count);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR("Failed to set thread state\n");
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }

    // Suspending thread
    task_suspend(task);
    // Set status
    STATUS = Status::Stopped;
  } else if (exception == EXC_CRASH) {
    // Set status
    STATUS = Status::Crashed;
  }

  // Signal new status
  pthread_mutex_unlock(&STATUS_LOCK);
  pthread_cond_signal(&STATUS_COND);

  return KERN_SUCCESS;
}

DarwinProcess::DarwinProcess(pid_t process)
    : pid(process), brk_address(nullptr), brk_value(0), suspended(true),
      prot_page(0), prot_rx(true) {
  kern_return_t kr;
  thread_act_array_t threads;
  mach_msg_type_number_t count;
  struct kevent ke;

  this->pageSize = getpagesize();

  // Get task port
  kr = task_for_pid(mach_task_self(), process, &this->task);
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to get task for pid {}, are you running as root?",
               (unsigned)process);
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  // Get thread ports
  kr = task_threads(this->task, &threads, &count);
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to get task for pid {}, are you running as root?",
               (unsigned)process);
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  // Keep only the main thread (the preload doesn't support multithreading
  // anyway)
  this->mainThread = threads[0];
  for (unsigned i = 1; i < count; i++) {
    kr = mach_port_deallocate(mach_task_self(), threads[i]);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR("Failed to deallocate thread port");
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
  }
  kr = mach_vm_deallocate(mach_task_self(), (mach_vm_address_t)threads,
                          count * sizeof(thread_act_t));
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to deallocate thread list");
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }

  // Setting up kernel queue to receive exit notification
  this->kq = kqueue();
  EV_SET(&ke, process, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
  if (kevent(this->kq, &ke, 1, NULL, 0, NULL) == -1) {
    QBDI_ERROR("Failed to setup kqueue");
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }

  // Setting up pthread synchronization primitive
  STATUS = Status::Running;

  // Starting the exception handler
  QBDI::qbdipreload_setup_exception_handler(
      this->task, EXC_MASK_BREAKPOINT | EXC_MASK_CRASH, (void *)&onBreakpoint);
}

void DarwinProcess::suspend() {
  kern_return_t kr;

  if (this->suspended == false) {
    kr = task_suspend(this->task);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR("Failed to suspend process: {}", mach_error_string(kr));
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    this->suspended = true;
  }
}

void DarwinProcess::resume() {
  kern_return_t kr;

  if (this->suspended == true) {
    kr = task_resume(this->task);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR("Failed to resume process: {}", mach_error_string(kr));
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    this->suspended = false;
  }
}

void DarwinProcess::makeRW(void *address) {
  kern_return_t kr;
  QBDI::rword page =
      (QBDI::rword)address - ((QBDI::rword)address % this->pageSize);

  // We are setting another page as RW, RX the previous one
  if (page != this->prot_page) {
    makeRX();
    // Switch to this new page
    this->prot_page = page;
  }
  // The same page but it is in RX
  if (this->prot_rx == true) {
    kr = mach_vm_protect(this->task, (mach_vm_address_t)page, this->pageSize,
                         false, VM_PROT_READ | VM_PROT_WRITE);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR(
          "Failed to change memory protection to RW of remote process: {}",
          mach_error_string(kr));
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    this->prot_rx = false;
  }
}

void DarwinProcess::makeRX() {
  kern_return_t kr;

  if (this->prot_rx == false) {
    kr = mach_vm_protect(this->task, (mach_vm_address_t)this->prot_page,
                         this->pageSize, false, VM_PROT_READ | VM_PROT_EXECUTE);
    if (kr != KERN_SUCCESS) {
      QBDI_ERROR(
          "Failed to change memory protection to RX of remote process: {}",
          mach_error_string(kr));
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    this->prot_rx = true;
  }
}

void DarwinProcess::setBreakpoint(void *address) {
  mach_vm_size_t readSize = 0;
  kern_return_t kr;

  suspend();
  makeRW(address);
  kr = mach_vm_read_overwrite(this->task, (mach_vm_address_t)address,
                              sizeof(uint8_t),
                              (mach_vm_address_t) & this->brk_value, &readSize);
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to read remote process memory");
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  kr = mach_vm_write(this->task, (mach_vm_address_t)address,
                     (mach_vm_offset_t)&BRK_INS, sizeof(uint8_t));
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to write remote process memory");
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  this->brk_address = address;
}

void DarwinProcess::unsetBreakpoint() {
  kern_return_t kr;

  suspend();
  makeRW(this->brk_address);
  kr = mach_vm_write(this->task, (mach_vm_address_t)this->brk_address,
                     (mach_vm_offset_t) & this->brk_value, sizeof(uint8_t));
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to write remote process memory");
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
}

void DarwinProcess::continueExecution() {
  makeRX();
  resume();
}

int DarwinProcess::waitForStatus() {
  struct kevent ke;
  timespec ts, zero = timespec{0, 0};

  pthread_mutex_lock(&STATUS_LOCK);
  // Wait for new status
  while (STATUS == Status::Running) {
    // Check for exit
    if (kevent(this->kq, nullptr, 0, &ke, 1, &zero) == -1) {
      QBDI_ERROR("Failed to poll the kqueue");
      exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    if (ke.fflags & NOTE_EXIT) {
      pthread_mutex_unlock(&STATUS_LOCK);
      return (int)Status::Exited;
    }
    // Else wait on the status lock
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 200;
    pthread_cond_timedwait(&STATUS_COND, &STATUS_LOCK, &ts);
  }
  suspended = true;
  Status ret = STATUS;
  STATUS = Status::Running;
  pthread_mutex_unlock(&STATUS_LOCK);
  return (int)ret;
}

void DarwinProcess::getProcessGPR(QBDI::GPRState *gprState) {
  kern_return_t kr;
  THREAD_STATE threadState;
  mach_msg_type_number_t count;

  count = THREAD_STATE_COUNT;
  kr = thread_get_state(this->mainThread, THREAD_STATE_ID,
                        (thread_state_t)&threadState, &count);
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to get GPR thread state: {}", mach_error_string(kr));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }

  threadStateToGPRState(&threadState, gprState);
}

void DarwinProcess::getProcessFPR(QBDI::FPRState *fprState) {
  kern_return_t kr;
  THREAD_STATE_FP floatState;
  mach_msg_type_number_t count;

  count = THREAD_STATE_FP_COUNT;
  kr = thread_get_state(this->mainThread, THREAD_STATE_FP_ID,
                        (thread_state_t)&floatState, &count);
  if (kr != KERN_SUCCESS) {
    QBDI_ERROR("Failed to get FPR thread state: {}", mach_error_string(kr));
    exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
  }

  floatStateToFPRState(&floatState, fprState);
}

bool hasExited(int status) { return status == Status::Exited; }

bool hasStopped(int status) { return status == Status::Stopped; }

bool hasCrashed(int status) { return status == Status::Crashed; }

void threadStateToGPRState(THREAD_STATE *ts, QBDI::GPRState *gprState) {
  return qbdipreload_threadCtxToGPRState((void *)ts, gprState);
}

void floatStateToFPRState(THREAD_STATE_FP *fs, QBDI::FPRState *fprState) {
  return qbdipreload_floatCtxToFPRState((void *)fs, fprState);
}
