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
#ifndef QBDI_ERRORS_H_
#define QBDI_ERRORS_H_

#include "QBDI/Platform.h"

#ifdef __cplusplus
namespace QBDI {
#endif

/*! QBDI Error values
 */
typedef enum {
  _QBDI_EI(INVALID_EVENTID) = 0xffffffff, /*!< Mark a returned event id as
                                           * invalid
                                           */
} VMError;

#ifdef __cplusplus
}
#endif

#endif // QBDI_ERRORS_H_
