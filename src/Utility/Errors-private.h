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
#ifndef __ERRORS_H__
#define __ERRORS_H__

enum {
     VMNoErr                     = 0    ,    /* No Error detected                                  */
     VMErrParams                 = 10000,    /* Error with parameters provided to API(s)           */
     VMErrFile                   = 10001,    /* Error opening/creating file                        */
     VMErrMem                    = 10002,    /* Error with memory access                           */
     VMErrSystem                 = 10003,    /* Error triggered by system API                      */
     VMErrOperationDenied        = 10100,    /* Error performing denied operation                  */
     VMErrNotInitialized         = 10200,    /* Error with unproper initialization done            */
     VMErrNotFound               = 10201,    /* Error the specified element has not been found     */
     VMErrNotSupported           = 10202,    /* Error calling non-supported feature                */
     VMErrNotImplementedYet      = 20000,    /* Error calling non-implemented feature              */
};

#endif // __ERRORS_H__
