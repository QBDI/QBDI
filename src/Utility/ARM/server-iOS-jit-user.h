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
#ifndef _frida_jit_user_
#define _frida_jit_user_

/* Module frida_jit */

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/ndr.h>
#include <mach/notify.h>
#include <mach/port.h>
#include <string.h>

/* BEGIN VOUCHER CODE */

#ifndef KERNEL
#if defined(__has_include)
#if __has_include(<mach/mig_voucher_support.h>)
#ifndef USING_VOUCHERS
#define USING_VOUCHERS
#endif
#ifndef __VOUCHER_FORWARD_TYPE_DECLS__
#define __VOUCHER_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
extern boolean_t voucher_mach_msg_set(mach_msg_header_t *msg)
    __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif // __VOUCHER_FORWARD_TYPE_DECLS__
#endif // __has_include(<mach/mach_voucher_types.h>)
#endif // __has_include
#endif // !KERNEL

/* END VOUCHER CODE */

/* BEGIN MIG_STRNCPY_ZEROFILL CODE */

#if defined(__has_include)
#if __has_include(<mach/mig_strncpy_zerofill_support.h>)
#ifndef USING_MIG_STRNCPY_ZEROFILL
#define USING_MIG_STRNCPY_ZEROFILL
#endif
#ifndef __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#define __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
extern int mig_strncpy_zerofill(char *dest, const char *src, int len)
    __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif /* __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__ */
#endif /* __has_include(<mach/mig_strncpy_zerofill_support.h>) */
#endif /* __has_include */

/* END MIG_STRNCPY_ZEROFILL CODE */

#ifdef AUTOTEST
#ifndef FUNCTION_PTR_T
#define FUNCTION_PTR_T
typedef void (*function_ptr_t)(mach_port_t, char *, mach_msg_type_number_t);
typedef struct {
  char *name;
  function_ptr_t function;
} function_table_entry;
typedef function_table_entry *function_table_t;
#endif /* FUNCTION_PTR_T */
#endif /* AUTOTEST */

#ifndef frida_jit_MSG_COUNT
#define frida_jit_MSG_COUNT 1
#endif /* frida_jit_MSG_COUNT */

#include <mach/mach_types.h>
#include <mach/mig.h>
#include <mach/std_types.h>

#ifdef __BeforeMigUserHeader
__BeforeMigUserHeader
#endif /* __BeforeMigUserHeader */

#include <sys/cdefs.h>
    __BEGIN_DECLS

/* Routine frida_jit_alloc */
#ifdef mig_external
        mig_external
#else
extern
#endif /* mig_external */
            kern_return_t
            frida_jit_alloc(mach_port_t server, vm_map_t task,
                            mach_vm_address_t *address, mach_vm_size_t size,
                            int flags);

__END_DECLS

/********************** Caution **************************/
/* The following data types should be used to calculate  */
/* maximum message sizes only. The actual message may be */
/* smaller, and the position of the arguments within the */
/* message layout may vary from what is presented here.  */
/* For example, if any of the arguments are variable-    */
/* sized, and less than the maximum is sent, the data    */
/* will be packed tight in the actual message to reduce  */
/* the presence of holes.                                */
/********************** Caution **************************/

/* typedefs for all requests */

#ifndef __Request__frida_jit_subsystem__defined
#define __Request__frida_jit_subsystem__defined

#ifdef __MigPackStructs
#pragma pack(4)
#endif
typedef struct {
  mach_msg_header_t Head;
  /* start of the kernel processed data */
  mach_msg_body_t msgh_body;
  mach_msg_port_descriptor_t task;
  /* end of the kernel processed data */
  NDR_record_t NDR;
  mach_vm_address_t address;
  mach_vm_size_t size;
  int flags;
} __Request__frida_jit_alloc_t __attribute__((unused));
#ifdef __MigPackStructs
#pragma pack()
#endif
#endif /* !__Request__frida_jit_subsystem__defined */

/* union of all requests */

#ifndef __RequestUnion__frida_jit_subsystem__defined
#define __RequestUnion__frida_jit_subsystem__defined
union __RequestUnion__frida_jit_subsystem {
  __Request__frida_jit_alloc_t Request_frida_jit_alloc;
};
#endif /* !__RequestUnion__frida_jit_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__frida_jit_subsystem__defined
#define __Reply__frida_jit_subsystem__defined

#ifdef __MigPackStructs
#pragma pack(4)
#endif
typedef struct {
  mach_msg_header_t Head;
  NDR_record_t NDR;
  kern_return_t RetCode;
  mach_vm_address_t address;
} __Reply__frida_jit_alloc_t __attribute__((unused));
#ifdef __MigPackStructs
#pragma pack()
#endif
#endif /* !__Reply__frida_jit_subsystem__defined */

/* union of all replies */

#ifndef __ReplyUnion__frida_jit_subsystem__defined
#define __ReplyUnion__frida_jit_subsystem__defined
union __ReplyUnion__frida_jit_subsystem {
  __Reply__frida_jit_alloc_t Reply_frida_jit_alloc;
};
#endif /* !__RequestUnion__frida_jit_subsystem__defined */

#ifndef subsystem_to_name_map_frida_jit
#define subsystem_to_name_map_frida_jit \
  { "frida_jit_alloc", 421337 }
#endif

#ifdef __AfterMigUserHeader
__AfterMigUserHeader
#endif /* __AfterMigUserHeader */

#endif /* _frida_jit_user_ */
