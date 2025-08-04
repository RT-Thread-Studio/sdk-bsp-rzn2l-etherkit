/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/* Ensure Renesas MCU variation definitions are included to ensure MCU
 * specific register variations are handled correctly. */
#ifndef BSP_FEATURE_H
 #error "INTERNAL ERROR: bsp_feature.h must be included before renesas.h."
#endif

/** @addtogroup Renesas Electronics Corporation
 * @{
 */

/** @addtogroup RZN2
 * @{
 */

#ifndef RZN2_H
 #define RZN2_H

 #ifdef __cplusplus
extern "C" {
 #endif

 #include "../../../../mcu/all/bsp_compiler_support.h"

/* Define compiler macros for CPU architecture, used in CMSIS 5. */
 #if defined(__ICCARM__)
  #if __ARM_ARCH_6M__ || __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ || __ARM_ARCH_8M_BASE__ || __ARM_ARCH_8M_MAIN__

/* Macros already defined */
  #else
   #if defined(__ARM8M_MAINLINE__) || defined(__ARM8EM_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__    1
   #elif defined(__ARM8M_BASELINE__)
    #define __ARM_ARCH_8M_BASE__    1
   #elif defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M'
    #if __ARM_ARCH == 6
     #define __ARM_ARCH_6M__        1
    #elif __ARM_ARCH == 7
     #if __ARM_FEATURE_DSP
      #define __ARM_ARCH_7EM__      1
     #else
      #define __ARM_ARCH_7M__       1
     #endif
    #endif                             /* __ARM_ARCH */
   #endif                              /* __ARM_ARCH_PROFILE == 'M' */
  #endif

/* Alternative core deduction for older ICCARM's */
  #if !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) && !defined(__ARM_ARCH_7EM__) && \
    !defined(__ARM_ARCH_8M_BASE__) && !defined(__ARM_ARCH_8M_MAIN__)
   #if defined(__ARM6M__) && (__CORE__ == __ARM6M__)
    #define __ARM_ARCH_6M__         1
   #elif defined(__ARM7M__) && (__CORE__ == __ARM7M__)
    #define __ARM_ARCH_7M__         1
   #elif defined(__ARM7EM__) && (__CORE__ == __ARM7EM__)
    #define __ARM_ARCH_7EM__        1
   #elif defined(__ARM8M_BASELINE__) && (__CORE == __ARM8M_BASELINE__)
    #define __ARM_ARCH_8M_BASE__    1
   #elif defined(__ARM8M_MAINLINE__) && (__CORE == __ARM8M_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__    1
   #elif defined(__ARM8EM_MAINLINE__) && (__CORE == __ARM8EM_MAINLINE__)
    #define __ARM_ARCH_8M_MAIN__    1
   #elif defined(__ARM8R__) && (__CORE__ == __ARM8R__)
    #define __ARM_ARCH_8R__         1
   #elif defined(__ARM8A__) && (__CORE__ == __ARM8A__)
    #define __ARM_ARCH_8A__         1
   #else
    #error "Unknown target."
   #endif
  #endif
 #endif

/** @addtogroup Configuration_of_CMSIS
 * @{
 */

/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */
/* IRQn_Type is generated as part of an FSP project. It can be found in vector_data.h. */

/** @} */ /* End of group Configuration_of_CMSIS */

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

 #if   __ARM_ARCH_7EM__
  #define RENESAS_CORTEX_M4
 #elif __ARM_ARCH_6M__
  #define RENESAS_CORTEX_M0PLUS
 #elif __ARM_ARCH_8M_BASE__
  #define RENESAS_CORTEX_M23
 #elif __ARM_ARCH_8M_MAIN__
  #define RENESAS_CORTEX_M33
 #elif __ARM_ARCH_8R__
  #define RENESAS_CORTEX_R52
 #elif __ARM_ARCH_8A__ || __ARM_ARCH_8A
  #define RENESAS_CORTEX_A55
 #else
  #warning Unsupported Architecture
 #endif

 #if BSP_MCU_GROUP_RZN2L
  #include "R9A07G084.h"
 #elif BSP_MCU_GROUP_RZN2H
  #include "R9A09G087.h"
 #else
  #warning Unsupported MCU
 #endif

 #ifdef __cplusplus
}
 #endif

#endif                                 /* RZN2_H */

/** @} */ /* End of group RZN2 */

/** @} */ /* End of group Renesas Electronics Corporation */
