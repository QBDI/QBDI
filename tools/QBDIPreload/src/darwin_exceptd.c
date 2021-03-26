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
#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "darwin_exceptd.h"
#include "mach_exc.h"

struct ExceptionHandler {
  task_t target;
  mach_port_t port;
  exception_mask_t mask;
  exception_handler_func handler;
  bool running;
};

static pthread_mutex_t REGISTERED_HANDLERS_LOCK = PTHREAD_MUTEX_INITIALIZER;

static struct ExceptionHandler **REGISTERED_HANDLERS = NULL;
static size_t REGISTERED_HANDLERS_COUNT = 0;
static size_t REGISTERED_HANDLERS_CAPACITY = 0;

kern_return_t catch_mach_exception_raise(mach_port_t exception_port,
                                         mach_port_t thread, mach_port_t task,
                                         exception_type_t exception,
                                         mach_exception_data_t code,
                                         mach_msg_type_number_t codeCnt) {
  exception_handler_func forward = NULL;
  pthread_mutex_lock(&REGISTERED_HANDLERS_LOCK);
  for (size_t i = 0; i < REGISTERED_HANDLERS_COUNT; i++) {
    if (exception_port == REGISTERED_HANDLERS[i]->port) {
      forward = REGISTERED_HANDLERS[i]->handler;
      break;
    }
  }
  pthread_mutex_unlock(&REGISTERED_HANDLERS_LOCK);

  if (forward != NULL) {
    return forward(exception_port, thread, task, exception, code, codeCnt);
  }

  fprintf(stderr, "Exception handler not found!\n");
  return KERN_FAILURE;
}

// Should never be called but must exist
kern_return_t catch_mach_exception_raise_state(
    mach_port_t exception_port, exception_type_t exception,
    const mach_exception_data_t code, mach_msg_type_number_t codeCnt,
    int *flavor, const thread_state_t old_state,
    mach_msg_type_number_t old_stateCnt, thread_state_t new_state,
    mach_msg_type_number_t *new_stateCnt) {
  fprintf(stderr, "catch_mach_exception_raise_state called!");
  return KERN_FAILURE;
}

// Should never be called but must exist
kern_return_t catch_mach_exception_raise_state_identity(
    mach_port_t exception_port, mach_port_t thread, mach_port_t task,
    exception_type_t exception, mach_exception_data_t code,
    mach_msg_type_number_t codeCnt, int *flavor, thread_state_t old_state,
    mach_msg_type_number_t old_stateCnt, thread_state_t new_state,
    mach_msg_type_number_t *new_stateCnt) {
  fprintf(stderr, "catch_mach_exception_raise_state called!");
  return KERN_FAILURE;
}

// Undocumented function implemented in libsystem_kernel.dylib
boolean_t mach_exc_server(mach_msg_header_t *msg, mach_msg_header_t *reply);

static void *exception_server(void *data) {
  kern_return_t kr;
  mach_msg_return_t rt;
  struct ExceptionHandler *exceptionHandler = (struct ExceptionHandler *)data;
  mach_msg_header_t *msg = (mach_msg_header_t *)calloc(
      1, sizeof(__Request__mach_exception_raise_state_identity_t));
  mach_msg_header_t *reply = (mach_msg_header_t *)calloc(
      1, sizeof(__Request__mach_exception_raise_state_identity_t));

  while (exceptionHandler->running) {
    rt = mach_msg(msg, MACH_RCV_MSG, 0,
                  sizeof(__Request__mach_exception_raise_state_identity_t),
                  exceptionHandler->port, 200, MACH_PORT_NULL);
    if (rt == MACH_MSG_SUCCESS) {
      mach_exc_server(msg, reply);
      rt = mach_msg(reply, MACH_SEND_MSG, reply->msgh_size, 0, MACH_PORT_NULL,
                    500, MACH_PORT_NULL);
      if (rt != MACH_MSG_SUCCESS) {
        fprintf(stderr, "Failed to send reply on exception port: %s\n",
                mach_error_string(rt));
        break;
      }
    } else if (rt != MACH_RCV_TIMED_OUT) {
      fprintf(stderr, "Failed to receive message on exception port: %s\n",
              mach_error_string(rt));
      break;
    }
  }

  kr = task_set_exception_ports(exceptionHandler->target,
                                exceptionHandler->mask, MACH_PORT_NULL, 0, 0);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to deregister our exception handler\n");
  }
  kr = mach_port_deallocate(exceptionHandler->target, exceptionHandler->port);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to deallocate target exception port right\n");
  }
  kr = mach_port_deallocate(mach_task_self(), exceptionHandler->port);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to deallocate self exception port right\n");
  }

  if (reply)
    free(reply);
  if (msg)
    free(msg);
  if (exceptionHandler)
    free(exceptionHandler);
  return NULL;
}

struct ExceptionHandler *setupExceptionHandler(task_t target,
                                               exception_mask_t exceptionMask,
                                               exception_handler_func handler) {
  pthread_t tid;
  pthread_attr_t attr;
  kern_return_t kr;
  struct ExceptionHandler *exceptionHandler =
      calloc(1, sizeof(struct ExceptionHandler));

  exceptionHandler->target = target;
  exceptionHandler->port = 0;
  exceptionHandler->mask = exceptionMask;
  exceptionHandler->handler = handler;
  exceptionHandler->running = true;

  if (REGISTERED_HANDLERS == NULL) {
    REGISTERED_HANDLERS_CAPACITY = 1;
    REGISTERED_HANDLERS_COUNT = 0;
    REGISTERED_HANDLERS = (struct ExceptionHandler **)calloc(
        REGISTERED_HANDLERS_CAPACITY, sizeof(struct ExceptionHandler *));
  }

  kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
                          &exceptionHandler->port);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to allocate a port\n");
    free(exceptionHandler);
    return NULL;
  }

  kr = mach_port_insert_right(mach_task_self(), exceptionHandler->port,
                              exceptionHandler->port, MACH_MSG_TYPE_MAKE_SEND);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to insert port right into target\n");
    free(exceptionHandler);
    return NULL;
  }

  kr = task_set_exception_ports(
      target, exceptionHandler->mask, exceptionHandler->port,
      EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES, THREAD_STATE_NONE);
  if (kr != KERN_SUCCESS) {
    fprintf(stderr, "Failed to set exception port: %s\n",
            mach_error_string(kr));
    free(exceptionHandler);
    return NULL;
  }

  pthread_mutex_lock(&REGISTERED_HANDLERS_LOCK);
  if (REGISTERED_HANDLERS_COUNT == REGISTERED_HANDLERS_CAPACITY) {
    // Realloc
    struct ExceptionHandler **newlist = (struct ExceptionHandler **)calloc(
        REGISTERED_HANDLERS_CAPACITY * 2, sizeof(struct ExceptionHandler *));
    // Copy old list
    memcpy(newlist, REGISTERED_HANDLERS,
           REGISTERED_HANDLERS_COUNT * sizeof(struct ExceptionHandler));
    // Free and replace
    free(REGISTERED_HANDLERS);
    REGISTERED_HANDLERS = newlist;
    // Update capacity
    REGISTERED_HANDLERS_CAPACITY *= 2;
  }
  REGISTERED_HANDLERS[REGISTERED_HANDLERS_COUNT] = exceptionHandler;
  REGISTERED_HANDLERS_COUNT += 1;
  pthread_mutex_unlock(&REGISTERED_HANDLERS_LOCK);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&tid, &attr, &exception_server, exceptionHandler);

  return exceptionHandler;
}

void stopExceptionHandler(struct ExceptionHandler *exceptionHandler) {
  pthread_mutex_lock(&REGISTERED_HANDLERS_LOCK);
  for (size_t i = 0; i < REGISTERED_HANDLERS_COUNT; i++) {
    if (exceptionHandler == REGISTERED_HANDLERS[i]) {
      // Remove and update list
      for (size_t j = i; j < REGISTERED_HANDLERS_COUNT - 1; j++) {
        REGISTERED_HANDLERS[j] = REGISTERED_HANDLERS[j + 1];
      }
      REGISTERED_HANDLERS_COUNT -= 1;
      // Set running flag to false
      exceptionHandler->running = false;
      pthread_mutex_unlock(&REGISTERED_HANDLERS_LOCK);
      return;
    }
  }
  pthread_mutex_unlock(&REGISTERED_HANDLERS_LOCK);
  fprintf(stderr, "Exception handler not found in the registered handler\n");
}
