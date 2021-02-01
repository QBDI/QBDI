#ifndef QBDI_OPTION_H_
#define QBDI_OPTION_H_

#include "stdint.h"

#include "Bitmask.h"
#include "Platform.h"

#ifdef __cplusplus
namespace QBDI {
#endif

typedef enum {
    _QBDI_EI(NO_OPT)                          = 0,          /*!< Default value */
    // general options between 0 and 23
    _QBDI_EI(OPT_DISABLE_FPR)                 = 1<<0,       /*!< Disable all operation on FPU (SSE, AVX, SIMD).
                                                                 May break the execution if the target use the FPU */
    _QBDI_EI(OPT_DISABLE_OPTIONAL_FPR)        = 1<<1,       /*!< Disable context switch optimisation when the target
                                                                 execblock doesn't used FPR */

    // architecture specific option between 24 and 31
    #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    _QBDI_EI(OPT_ATT_SYNTAX)                  = 1<<24,      /*!< Used the AT&T syntax for instruction disassembly */
    #endif
} Options;

_QBDI_ENABLE_BITMASK_OPERATORS(Options)

#ifdef __cplusplus
}
#endif

#endif /* QBDI_OPTION_H_ */
