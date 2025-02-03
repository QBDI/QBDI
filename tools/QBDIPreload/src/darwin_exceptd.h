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
#include <mach/task.h>
#include <pthread.h>

typedef kern_return_t (*exception_handler_func)(mach_port_t, mach_port_t,
                                                mach_port_t, exception_type_t,
                                                mach_exception_data_t,
                                                mach_msg_type_number_t);

struct ExceptionHandler;

struct ExceptionHandler *setupExceptionHandler(task_t target,
                                               exception_mask_t exceptionMask,
                                               exception_handler_func handler);

void stopExceptionHandler(struct ExceptionHandler *exceptionHandler);
