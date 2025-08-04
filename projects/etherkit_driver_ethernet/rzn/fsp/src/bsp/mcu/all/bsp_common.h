/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_COMMON_H
#define BSP_COMMON_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/* C99 includes. */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

/* Different compiler support. */
#include "../../inc/fsp_common_api.h"
#include "bsp_compiler_support.h"
#include "bsp_cfg.h"

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/** Used to signify that an ELC event is not able to be used as an interrupt. */
#define BSP_IRQ_DISABLED                       (0xFFU)

/* Vector Number offset */
#define BSP_VECTOR_NUM_OFFSET                  (32)
#define BSP_INTERRUPT_TYPE_OFFSET              (16U)

#define FSP_CONTEXT_SAVE
#define FSP_CONTEXT_RESTORE

#define BSP_PRV_CPU_FREQ_1000_MHZ              (1000000000U) // CPU frequency is 1000 MHz
#define BSP_PRV_CPU_FREQ_500_MHZ               (500000000U)  // CPU frequency is 500 MHz
#define BSP_PRV_CPU_FREQ_200_MHZ               (200000000U)  // CPU frequency is 200 MHz
#define BSP_PRV_CPU_FREQ_150_MHZ               (150000000U)  // CPU frequency is 150 MHz

#define BSP_PRV_CA55CLK_FREQ_1200_MHZ          (1200000000U) // CA55CLK frequency is 1200 MHz
#define BSP_PRV_CA55CLK_FREQ_600_MHZ           (600000000U)  // CA55CLK frequency is 600 MHz

#define BSP_PRV_CA55SCLK_FREQ_1000_MHZ         (1000000000U) // CA55SCLK frequency is 500 MHz
#define BSP_PRV_CA55SCLK_FREQ_500_MHZ          (500000000U)  // CA55SCLK frequency is 500 MHz

#define BSP_PRV_ICLK_FREQ_200_MHZ              (200000000U)  // ICLK frequency is 200 MHz
#define BSP_PRV_ICLK_FREQ_150_MHZ              (150000000U)  // ICLK frequency is 150 MHz

#define BSP_PRV_PCLKAM_FREQ_200_MHz            (200000000U)  // PCLKAM frequency is 200 MHz

#define BSP_PRV_PCLKH_FREQ_250_MHZ             (250000000U)  // PCLKH frequency is 250 MHz
#define BSP_PRV_PCLKH_FREQ_200_MHZ             (200000000U)  // PCLKH frequency is 200 MHz
#define BSP_PRV_PCLKH_FREQ_150_MHZ             (150000000U)  // PCLKH frequency is 150 MHz

#define BSP_PRV_PCLKM_FREQ_125_MHZ             (125000000U)  // PCLKM frequency is 125 MHz
#define BSP_PRV_PCLKM_FREQ_100_MHZ             (100000000U)  // PCLKM frequency is 100 MHz
#define BSP_PRV_PCLKM_FREQ_75_MHZ              (75000000U)   // PCLKM frequency is 750 MHz

#define BSP_PRV_PCLKL_FREQ_62_5_MHZ            (62500000U)   // PCLKL frequency is 62.5 MHz
#define BSP_PRV_PCLKL_FREQ_50_MHZ              (50000000U)   // PCLKL frequency is 50 MHz
#define BSP_PRV_PCLKL_FREQ_37_5_MHZ            (37500000U)   // PCLKL frequency is 37.5 MHz

#define BSP_PRV_PCLKADC_FREQ_25_MHZ            (25000000U)   // PCLKADC frequency is 25 MHz
#define BSP_PRV_PCLKADC_FREQ_18_75_MHZ         (18750000U)   // PCLKADC frequency is 18.75 MHz

#define BSP_PRV_PCLKGPTL_FREQ_500_MHZ          (500000000U)  // PCLKGPTL frequency is 500 MHz
#define BSP_PRV_PCLKGPTL_FREQ_400_MHZ          (400000000U)  // PCLKGPTL frequency is 400 MHz
#define BSP_PRV_PCLKGPTL_FREQ_300_MHZ          (300000000U)  // PCLKGPTL frequency is 300 MHz

#define BSP_PRV_PCLKSCI_FREQ_75_MHZ            (75000000U)   // PCLKSCI frequency is 75 MHz
#define BSP_PRV_PCLKSCI_FREQ_80_MHZ            (80000000U)   // PCLKSCI frequency is 80 MHz
#define BSP_PRV_PCLKSCI_FREQ_96_MHZ            (96000000U)   // PCLKSCI frequency is 96 MHz
#define BSP_PRV_PCLKSCI_FREQ_100_MHZ           (100000000U)  // PCLKSCI frequency is 100 MHz

#define BSP_PRV_PCLKSPI_FREQ_75_MHZ            (75000000U)   // PCLKSPI frequency is 75 MHz
#define BSP_PRV_PCLKSPI_FREQ_80_MHZ            (80000000U)   // PCLKSPI frequency is 80 MHz
#define BSP_PRV_PCLKSPI_FREQ_96_MHZ            (96000000U)   // PCLKSPI frequency is 96 MHz
#define BSP_PRV_PCLKSPI_FREQ_100_MHZ           (100000000U)  // PCLKSPI frequency is 100 MHz

#define BSP_PRV_PCLKENCO_FREQ_20_MHZ           (20000000U)   // PCLKENCO frequency is 20 MHz
#define BSP_PRV_PCLKENCO_FREQ_80_MHZ           (80000000U)   // PCLKENCO frequency is 80 MHz

#define BSP_PRV_PCLKCAN_FREQ_80_MHZ            (80000000U)   // PCLKCAN frequency is 80 MHz
#define BSP_PRV_PCLKCAN_FREQ_40_MHZ            (40000000U)   // PCLKCAN frequency is 40 MHz

#define BSP_PRV_CKIO_FREQ_125_MHZ              (125000000U)  // CKIO frequency is 125 MHz
#define BSP_PRV_CKIO_FREQ_100_MHZ              (100000000U)  // CKIO frequency is 100 MHz
#define BSP_PRV_CKIO_FREQ_83_3_MHZ             (83333333U)   // CKIO frequency is 83.3 MHz
#define BSP_PRV_CKIO_FREQ_75_MHZ               (75000000U)   // CKIO frequency is 75 MHz
#define BSP_PRV_CKIO_FREQ_66_7_MHZ             (66666666U)   // CKIO frequency is 66.7 MHz
#define BSP_PRV_CKIO_FREQ_62_5_MHZ             (62500000U)   // CKIO frequency is 62.5 MHz
#define BSP_PRV_CKIO_FREQ_50_MHZ               (50000000U)   // CKIO frequency is 50 MHz
#define BSP_PRV_CKIO_FREQ_41_7_MHZ             (41666666U)   // CKIO frequency is 41.6 MHz
#define BSP_PRV_CKIO_FREQ_40_MHZ               (40000000U)   // CKIO frequency is 40 MHz
#define BSP_PRV_CKIO_FREQ_37_5_MHZ             (37500000U)   // CKIO frequency is 37.5 MHz
#define BSP_PRV_CKIO_FREQ_35_7_MHZ             (35714285U)   // CKIO frequency is 125 MHz
#define BSP_PRV_CKIO_FREQ_33_3_MHZ             (33333333U)   // CKIO frequency is 33.3MHz
#define BSP_PRV_CKIO_FREQ_31_25_MHZ            (31250000U)   // CKIO frequency is 31.25 MHz
#define BSP_PRV_CKIO_FREQ_30_MHZ               (30000000U)   // CKIO frequency is 30 MHz
#define BSP_PRV_CKIO_FREQ_28_6_MHZ             (28571428U)   // CKIO frequency is 28.6 MHz
#define BSP_PRV_CKIO_FREQ_25_MHZ               (25000000U)   // CKIO frequency is 25 MHz
#define BSP_PRV_CKIO_FREQ_21_4_MHZ             (21428571U)   // CKIO frequency is 21.4 MHz
#define BSP_PRV_CKIO_FREQ_18_75_MHZ            (18750000U)   // CKIO frequency is 18.75 MHz
#define BSP_PRV_CKIO_FREQ_NOT_SUPPORTED        (0xFFFFFFFFU) // CKIO frequency is not supported

#define BSP_PRV_XSPI_CLK_FREQ_150_MHZ          (150000000U)  // XSPI_CLK frequency is 150.0 MHz
#define BSP_PRV_XSPI_CLK_FREQ_133_3_MHZ        (133333333U)  // XSPI_CLK frequency is 133.3 MHz
#define BSP_PRV_XSPI_CLK_FREQ_100_MHZ          (100000000U)  // XSPI_CLK frequency is 100.0 MHz
#define BSP_PRV_XSPI_CLK_FREQ_75_MHZ           (75000000U)   // XSPI_CLK frequency is 75.0 MHz
#define BSP_PRV_XSPI_CLK_FREQ_50_MHZ           (50000000U)   // XSPI_CLK frequency is 50.0 MHz
#define BSP_PRV_XSPI_CLK_FREQ_37_5_MHZ         (37500000U)   // XSPI_CLK frequency is 37.5 MHz
#define BSP_PRV_XSPI_CLK_FREQ_25_MHZ           (25000000U)   // XSPI_CLK frequency is 25.0 MHz
#define BSP_PRV_XSPI_CLK_FREQ_12_5_MHZ         (12500000U)   // XSPI_CLK frequency is 12.5 MHz
#define BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED    (0xFFFFFFFFU) // XSPI_CLK frequency is not supported

/** Macro to log and return error without an assertion. */
#ifndef FSP_RETURN

 #define FSP_RETURN(err)    FSP_ERROR_LOG((err)); \
    return err;
#endif

/** This function is called before returning an error code. To stop on a runtime error, define fsp_error_log in
 * user code and do required debugging (breakpoints, stack dump, etc) in this function.*/
#if (1 == BSP_CFG_ERROR_LOG)

 #ifndef FSP_ERROR_LOG
  #define FSP_ERROR_LOG(err) \
    fsp_error_log((err), __FILE__, __LINE__);
 #endif
#else

 #define FSP_ERROR_LOG(err)
#endif

/** Default assertion calls ::FSP_ERROR_RETURN if condition "a" is false. Used to identify incorrect use of API's in FSP
 * functions. */
#if (3 == BSP_CFG_ASSERT)
 #define FSP_ASSERT(a)
 #define FSP_ASSERT_NOT_RETURN_VALUE(a)
#elif (2 == BSP_CFG_ASSERT)
 #define FSP_ASSERT(a)                     {assert(a);}
 #define FSP_ASSERT_NOT_RETURN_VALUE(a)    {assert(a);}
#else
 #define FSP_ASSERT(a)                     FSP_ERROR_RETURN((a), FSP_ERR_ASSERTION)
 #define FSP_ASSERT_NOT_RETURN_VALUE(a)    FSP_ERROR_NOT_RETURN_VALUE((a), FSP_ERR_ASSERTION)
#endif                                 // ifndef FSP_ASSERT

/** All FSP error codes are returned using this macro. Calls ::FSP_ERROR_LOG function if condition "a" is false. Used
 * to identify runtime errors in FSP functions. */

#define FSP_ERROR_RETURN(a, err)                        \
    {                                                   \
        if ((a))                                        \
        {                                               \
            (void) 0;                  /* Do nothing */ \
        }                                               \
        else                                            \
        {                                               \
            FSP_ERROR_LOG(err);                         \
            return err;                                 \
        }                                               \
    }

/** This function performs the same operation as ::FSP_ERROR_RETURN, but can be applied to functions that do not return
 *  an error code. */

#define FSP_ERROR_NOT_RETURN_VALUE(a, err)              \
    {                                                   \
        if ((a))                                        \
        {                                               \
            (void) 0;                  /* Do nothing */ \
        }                                               \
        else                                            \
        {                                               \
            FSP_ERROR_LOG(err);                         \
            return;                                     \
        }                                               \
    }

/* Function-like macro used to wait for a condition to be met, most often used to wait for hardware register updates.
 * This macro can be redefined to add a timeout if necessary. */
#ifndef FSP_HARDWARE_REGISTER_WAIT
 #define FSP_HARDWARE_REGISTER_WAIT(reg, required_value)    while (reg != required_value) { /* Wait. */}
#endif

/* Function-like macro used to wait for a condition to be met with timeout,
 * most often used to wait for hardware register updates. */
#define BSP_HARDWARE_REGISTER_WAIT_WTIH_TIMEOUT(reg, required_value, timeout) \
    while ((timeout))                                                         \
    {                                                                         \
        if ((required_value) == (reg))                                        \
        {                                                                     \
            break;                                                            \
        }                                                                     \
        (timeout)--;                                                          \
    }

#ifndef BSP_CFG_IRQ_MASK_LEVEL_FOR_CRITICAL_SECTION
 #define BSP_CFG_IRQ_MASK_LEVEL_FOR_CRITICAL_SECTION    (0U)
#endif

/* This macro defines a variable for saving previous mask value */
#ifndef FSP_CRITICAL_SECTION_DEFINE

 #define FSP_CRITICAL_SECTION_DEFINE              uintptr_t old_mask_level = 0U
#endif

/* These macros abstract methods to save and restore the interrupt state. */
#define FSP_CRITICAL_SECTION_GET_CURRENT_STATE    __get_ICC_PMR
#define FSP_CRITICAL_SECTION_SET_STATE            __set_ICC_PMR
#define FSP_CRITICAL_SECTION_IRQ_MASK_SET         ((uint8_t) (BSP_CFG_IRQ_MASK_LEVEL_FOR_CRITICAL_SECTION << \
                                                              BSP_FEATURE_BSP_IRQ_PRIORITY_POS_BIT))

/** This macro temporarily saves the current interrupt state and disables interrupts. */
#ifndef FSP_CRITICAL_SECTION_ENTER
 #define FSP_CRITICAL_SECTION_ENTER                            \
    old_mask_level = FSP_CRITICAL_SECTION_GET_CURRENT_STATE(); \
    FSP_CRITICAL_SECTION_SET_STATE(FSP_CRITICAL_SECTION_IRQ_MASK_SET)
#endif

/** This macro restores the previously saved interrupt state, reenabling interrupts. */
#ifndef FSP_CRITICAL_SECTION_EXIT
 #define FSP_CRITICAL_SECTION_EXIT              FSP_CRITICAL_SECTION_SET_STATE(old_mask_level)
#endif

/* Number of Cortex processor exceptions. */
#define FSP_PRIV_CORTEX_PROCESSOR_EXCEPTIONS    (32U)

/** Used to signify that the requested IRQ vector is not defined in this system. */
#define FSP_INVALID_VECTOR                      ((IRQn_Type) - 33)

/* This macro Enable or Disable interrupts. */
#if defined(BSP_CFG_CORE_CA55)
 #define BSP_INTERRUPT_ENABLE                   __enable_fiq()

 #define BSP_INTERRUPT_DISABLE                  __disable_fiq()

#elif defined(BSP_CFG_CORE_CR52)
 #define BSP_INTERRUPT_ENABLE                   __asm volatile ("cpsie i"); \
    __asm volatile ("isb");

 #define BSP_INTERRUPT_DISABLE                  __asm volatile ("cpsid i"); \
    __asm volatile ("isb");
#endif

/** In the event of an unrecoverable error the BSP will by default call the __BKPT() intrinsic function which will
 *  alert the user of the error. The user can override this default behavior by defining their own
 *  BSP_CFG_HANDLE_UNRECOVERABLE_ERROR macro.
 */
#if !defined(BSP_CFG_HANDLE_UNRECOVERABLE_ERROR)

 #define BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(x)    __BKPT((x))
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Different warm start entry locations in the BSP. */
typedef enum e_bsp_warm_start_event
{
    BSP_WARM_START_RESET = 0,          ///< Called almost immediately after reset. No C runtime environment, clocks, or IRQs.
    BSP_WARM_START_POST_CLOCK,         ///< Called after clock initialization. No C runtime environment or IRQs.
    BSP_WARM_START_POST_C,             ///< Called after clocks and C runtime environment have been set up
} bsp_warm_start_event_t;

/* Private enum used in R_FSP_SystemClockHzGet. */
typedef enum e_fsp_priv_clock
{
    FSP_PRIV_CLOCK_CPU0,
    FSP_PRIV_CLOCK_CPU1,
    FSP_PRIV_CLOCK_CA55C0,
    FSP_PRIV_CLOCK_CA55C1,
    FSP_PRIV_CLOCK_CA55C2,
    FSP_PRIV_CLOCK_CA55C3,
    FSP_PRIV_CLOCK_CA55SCLK,
    FSP_PRIV_CLOCK_ICLK,
    FSP_PRIV_CLOCK_PCLKH,
    FSP_PRIV_CLOCK_PCLKM,
    FSP_PRIV_CLOCK_PCLKL,
    FSP_PRIV_CLOCK_PCLKADC,
    FSP_PRIV_CLOCK_PCLKGPTL,
    FSP_PRIV_CLOCK_PCLKENCO,
    FSP_PRIV_CLOCK_PCLKSPI0,
    FSP_PRIV_CLOCK_PCLKSPI1,
    FSP_PRIV_CLOCK_PCLKSPI2,
    FSP_PRIV_CLOCK_PCLKSPI3,
    FSP_PRIV_CLOCK_PCLKSCI0,
    FSP_PRIV_CLOCK_PCLKSCI1,
    FSP_PRIV_CLOCK_PCLKSCI2,
    FSP_PRIV_CLOCK_PCLKSCI3,
    FSP_PRIV_CLOCK_PCLKSCI4,
    FSP_PRIV_CLOCK_PCLKSCI5,
    FSP_PRIV_CLOCK_PCLKSCIE0,
    FSP_PRIV_CLOCK_PCLKSCIE1,
    FSP_PRIV_CLOCK_PCLKSCIE2,
    FSP_PRIV_CLOCK_PCLKSCIE3,
    FSP_PRIV_CLOCK_PCLKSCIE4,
    FSP_PRIV_CLOCK_PCLKSCIE5,
    FSP_PRIV_CLOCK_PCLKSCIE6,
    FSP_PRIV_CLOCK_PCLKSCIE7,
    FSP_PRIV_CLOCK_PCLKSCIE8,
    FSP_PRIV_CLOCK_PCLKSCIE9,
    FSP_PRIV_CLOCK_PCLKSCIE10,
    FSP_PRIV_CLOCK_PCLKSCIE11,
    FSP_PRIV_CLOCK_PCLKCAN,
    FSP_PRIV_CLOCK_CKIO,
    FSP_PRIV_CLOCK_XSPI0_CLK,
    FSP_PRIV_CLOCK_XSPI1_CLK,
    FSP_PRIV_CLOCK_PCLKAM
} fsp_priv_clock_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
#if (1 == BSP_FEATURE_CGC_CKIO_CLOCK_FREQ_TYPE)
extern const uint32_t g_bsp_system_clock_select_ckio[][2];
#elif (2 == BSP_FEATURE_CGC_CKIO_CLOCK_FREQ_TYPE)
extern const uint32_t g_bsp_system_clock_select_ckio[];
#endif
extern const uint32_t g_bsp_system_clock_select_xspi_clk[][2];
extern const uint32_t g_bsp_system_clock_select_spi_clk[];
extern const uint32_t g_bsp_system_clock_select_sci_clk[];

extern IRQn_Type g_current_interrupt_num[];
extern uint8_t   g_current_interrupt_pointer;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Inline Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Return active interrupt vector number value
 *
 * @return     Active interrupt vector number value
 **********************************************************************************************************************/
__STATIC_INLINE IRQn_Type R_FSP_CurrentIrqGet (void)
{
    /* Return the current interrupt number. */
    return g_current_interrupt_num[(g_current_interrupt_pointer - 1U)];
}

/*******************************************************************************************************************//**
 * Gets the frequency of a system clock.
 *
 * @return     Frequency of requested clock in Hertz.
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t R_FSP_SystemClockHzGet (fsp_priv_clock_t clock)
{
    uint32_t clock_hz = 0;

#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
    uint32_t fselcpu0  = R_SYSC_S->SCKCR2_b.FSELCPU0;
    uint32_t divselsub = R_SYSC_S->SCKCR2_b.DIVSELSUB;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
    uint32_t cr52cpu0 = R_SYSC_S->SCKCR2_b.CR52CPU0;
    uint32_t cr52cpu1 = R_SYSC_S->SCKCR2_b.CR52CPU1;
#endif
    switch (clock)
    {
        case FSP_PRIV_CLOCK_CPU0:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_CPU_FREQ_200_MHZ : BSP_PRV_CPU_FREQ_150_MHZ;
            clock_hz = clock_hz << fselcpu0;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == cr52cpu0) ? BSP_PRV_CPU_FREQ_500_MHZ : BSP_PRV_CPU_FREQ_1000_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_CPU1:
        {
#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == cr52cpu1) ? BSP_PRV_CPU_FREQ_500_MHZ : BSP_PRV_CPU_FREQ_1000_MHZ;
#endif
            break;
        }

#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
        case FSP_PRIV_CLOCK_CA55C0:
        {
            uint32_t ca55core0 = R_SYSC_S->SCKCR2_b.CA55CORE0;
            clock_hz = (0 == ca55core0) ? BSP_PRV_CA55CLK_FREQ_600_MHZ : BSP_PRV_CA55CLK_FREQ_1200_MHZ;
            break;
        }

        case FSP_PRIV_CLOCK_CA55C1:
        {
            uint32_t ca55core1 = R_SYSC_S->SCKCR2_b.CA55CORE1;
            clock_hz = (0 == ca55core1) ? BSP_PRV_CA55CLK_FREQ_600_MHZ : BSP_PRV_CA55CLK_FREQ_1200_MHZ;
            break;
        }

        case FSP_PRIV_CLOCK_CA55C2:
        {
            uint32_t ca55core2 = R_SYSC_S->SCKCR2_b.CA55CORE2;
            clock_hz = (0 == ca55core2) ? BSP_PRV_CA55CLK_FREQ_600_MHZ : BSP_PRV_CA55CLK_FREQ_1200_MHZ;
            break;
        }

        case FSP_PRIV_CLOCK_CA55C3:
        {
            uint32_t ca55core3 = R_SYSC_S->SCKCR2_b.CA55CORE3;
            clock_hz = (0 == ca55core3) ? BSP_PRV_CA55CLK_FREQ_600_MHZ : BSP_PRV_CA55CLK_FREQ_1200_MHZ;
            break;
        }

        case FSP_PRIV_CLOCK_CA55SCLK:
        {
            uint32_t ca55sclk = R_SYSC_S->SCKCR2_b.CA55SCLK;
            clock_hz = (0 == ca55sclk) ? BSP_PRV_CA55SCLK_FREQ_500_MHZ : BSP_PRV_CA55SCLK_FREQ_1000_MHZ;
            break;
        }
#endif

        case FSP_PRIV_CLOCK_ICLK:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_ICLK_FREQ_200_MHZ : BSP_PRV_ICLK_FREQ_150_MHZ;
#endif
            break;
        }

#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
        case FSP_PRIV_CLOCK_PCLKAM:
        {
            clock_hz = BSP_PRV_PCLKAM_FREQ_200_MHz;
            break;
        }
#endif

        case FSP_PRIV_CLOCK_PCLKH:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_PCLKH_FREQ_200_MHZ : BSP_PRV_PCLKH_FREQ_150_MHZ;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKH_FREQ_250_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKM:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_PCLKM_FREQ_100_MHZ : BSP_PRV_PCLKM_FREQ_75_MHZ;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKM_FREQ_125_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKL:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_PCLKL_FREQ_50_MHZ : BSP_PRV_PCLKL_FREQ_37_5_MHZ;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKL_FREQ_62_5_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKADC:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_PCLKADC_FREQ_25_MHZ : BSP_PRV_PCLKADC_FREQ_18_75_MHZ;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKADC_FREQ_25_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKGPTL:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = (0 == divselsub) ? BSP_PRV_PCLKGPTL_FREQ_400_MHZ : BSP_PRV_PCLKGPTL_FREQ_300_MHZ;
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKGPTL_FREQ_500_MHZ;
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSPI0:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR_b.SPI0ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR3_b.SPI0ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSPI1:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR_b.SPI1ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR3_b.SPI1ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSPI2:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR_b.SPI2ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_NS->SCKCR3_b.SPI2ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSPI3:
        {
            clock_hz = g_bsp_system_clock_select_spi_clk[R_SYSC_S->SCKCR2_b.SPI3ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI0:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR_b.SCI0ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR3_b.SCI0ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI1:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR_b.SCI1ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR3_b.SCI1ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI2:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR_b.SCI2ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR3_b.SCI2ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI3:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR_b.SCI3ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR3_b.SCI3ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI4:
        {
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR_b.SCI4ASYNCSEL];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR3_b.SCI4ASYNCSEL];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCI5:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_S->SCKCR2_b.SCI5ASYNCSEL];
            break;
        }

#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
        case FSP_PRIV_CLOCK_PCLKSCIE0:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE0ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE1:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE1ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE2:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE2ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE3:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE3ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE4:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE4ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE5:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE5ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE6:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE6ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE7:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE7ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE8:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE8ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE9:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE9ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE10:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE10ASYNCSEL];
            break;
        }

        case FSP_PRIV_CLOCK_PCLKSCIE11:
        {
            clock_hz = g_bsp_system_clock_select_sci_clk[R_SYSC_NS->SCKCR4_b.SCIE11ASYNCSEL];
            break;
        }
#endif

        case FSP_PRIV_CLOCK_PCLKCAN:
        {
            uint32_t fselcanfd = R_SYSC_NS->SCKCR_b.FSELCANFD;
            clock_hz = (0 == fselcanfd) ? BSP_PRV_PCLKCAN_FREQ_80_MHZ : BSP_PRV_PCLKCAN_FREQ_40_MHZ;
            break;
        }

        case FSP_PRIV_CLOCK_CKIO:
        {
            uint32_t ckio = R_SYSC_NS->SCKCR_b.CKIO;
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_ckio[ckio][divselsub];
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = g_bsp_system_clock_select_ckio[ckio];
#endif
            break;
        }

        case FSP_PRIV_CLOCK_XSPI0_CLK:
        {
            uint32_t fselxspi0 = R_SYSC_NS->SCKCR_b.FSELXSPI0;
            clock_hz = g_bsp_system_clock_select_xspi_clk[fselxspi0][R_SYSC_NS->SCKCR_b.DIVSELXSPI0];
            break;
        }

        case FSP_PRIV_CLOCK_XSPI1_CLK:
        {
            uint32_t fselxspi1 = R_SYSC_NS->SCKCR_b.FSELXSPI1;
            clock_hz = g_bsp_system_clock_select_xspi_clk[fselxspi1][R_SYSC_NS->SCKCR_b.DIVSELXSPI1];
            break;
        }

#if (BSP_FEATURE_BSP_ENCOUT_SUPPORTED)
        case FSP_PRIV_CLOCK_PCLKENCO:
        {
 #if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
            clock_hz = BSP_PRV_PCLKENCO_FREQ_20_MHZ;
 #elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
            uint32_t encoutclk = R_SYSC_NS->SCKCR4_b.ENCOUTCLK;
            clock_hz = (0 == encoutclk) ? BSP_PRV_PCLKENCO_FREQ_20_MHZ : BSP_PRV_PCLKENCO_FREQ_80_MHZ;
 #endif
            break;
        }
#endif

        default:
        {
            break;
        }
    }

    return clock_hz;
}

#if ((1 == BSP_CFG_ERROR_LOG) || (1 == BSP_CFG_ASSERT))

/** Prototype of default function called before errors are returned in FSP code if BSP_CFG_LOG_ERRORS is set to 1. */
void fsp_error_log(fsp_err_t err, const char * file, int32_t line);

#endif

/** @} (end addtogroup BSP_MCU) */

void * bsp_prv_malloc(size_t size);
void   bsp_prv_free(void * ptr);

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
