/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BSP_RESET_MRCTL_BIT_SHIFT_MASK         (0x0000001FU)
#define BSP_RESET_MRCTL_SELECT_MASK            (0x001F0000U)
#define BSP_RESET_MRCTL_REGION_SELECT_MASK     (0x00400000U)

#define BSP_RESET_RMR_RESET_REQUEST_AARCH64    (0x3)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 *
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Occur the system software reset.
 **********************************************************************************************************************/
void R_BSP_SystemReset (void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    /* System software reset. */
    R_SYSC_S->SWRSYS = BSP_PRV_RESET_KEY;
}

/*******************************************************************************************************************//**
 * Occur the CPU software reset.
 *
 * @param[in] cpu to be reset state.
 *
 * @note With Cortex-A55, you cannot use resets that are not automatically released when a software reset is executed.
 **********************************************************************************************************************/
void R_BSP_CPUReset (bsp_reset_t cpu)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    if (BSP_RESET_CR52_0 == cpu)
    {
        /* CR52_0 software reset. */
        R_SYSC_S->SWRCPU0 = BSP_PRV_RESET_KEY;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

#if defined(BSP_CFG_CORE_CR52) && (0 == BSP_CFG_CORE_CR52)
        __asm volatile ("WFI");
#endif
    }

#if (1U < BSP_FEATURE_BSP_CR52_CORE_NUM)
    else if (BSP_RESET_CR52_1 == cpu)
    {
        /* CR52_1 software reset. */
        R_SYSC_S->SWRCPU1 = BSP_PRV_RESET_KEY;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

 #if defined(BSP_CFG_CORE_CR52) && (1 == BSP_CFG_CORE_CR52)
        __asm volatile ("WFI");
 #endif
    }
#endif
#if (1U < BSP_FEATURE_BSP_CA55_CORE_NUM)
    else if (BSP_RESET_CA55_CLUSTER == cpu)
    {
        /* CA55_cluster software reset. */
        R_SYSC_S->SWR55C = BSP_PRV_RESET_KEY;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
    }
#endif
    else
    {
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
    }
}

/*******************************************************************************************************************//**
 * Occur the CPU software reset. When using this function, the CPU reset state is automatically released
 * after an elapsed time corresponding to the setting value in SCKCR2.DIVSELSUB bit.
 *
 * @param[in] cpu to be reset state.
 **********************************************************************************************************************/
void R_BSP_CPUResetAutoRelease (bsp_reset_t cpu)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    if (BSP_RESET_CR52_0 == cpu)
    {
        /* CR52_0 software reset. */
        R_SYSC_S->SWRCPU0 = BSP_PRV_RESET_KEY_AUTO_RELEASE;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

#if defined(BSP_CFG_CORE_CR52) && (0 == BSP_CFG_CORE_CR52)
        __asm volatile ("WFI");
#endif
    }

#if (1U < BSP_FEATURE_BSP_CR52_CORE_NUM)
    else if (BSP_RESET_CR52_1 == cpu)
    {
        /* CR52_1 software reset. */
        R_SYSC_S->SWRCPU1 = BSP_PRV_RESET_KEY_AUTO_RELEASE;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

 #if defined(BSP_CFG_CORE_CR52) && (1 == BSP_CFG_CORE_CR52)
        __asm volatile ("WFI");
 #endif
    }
#endif
#if (1U < BSP_FEATURE_BSP_CA55_CORE_NUM)
    else if (BSP_RESET_CA55_CLUSTER == cpu)
    {
        /* CA55_cluster software reset. */
        R_SYSC_S->SWR55C = BSP_PRV_RESET_KEY_AUTO_RELEASE;

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
    }
 #if defined(BSP_CFG_CORE_CA55)
    else if (BSP_RESET_CA55_0 == cpu)
    {
        /* CA55_0 software reset. */
        __asm volatile (
            "MOV  x0, %0                                              \n"
            "MSR  RMR_EL3, x0                                         \n"
            ::"r" (BSP_RESET_RMR_RESET_REQUEST_AARCH64) : "memory");

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

  #if (0 == BSP_CFG_CORE_CA55)
        __asm volatile ("WFI");
  #endif
    }
    else if (BSP_RESET_CA55_1 == cpu)
    {
        /* CA55_1 software reset. */
        __asm volatile (
            "MOV  x0, %0                                              \n"
            "MSR  RMR_EL3, x0                                         \n"
            ::"r" (BSP_RESET_RMR_RESET_REQUEST_AARCH64) : "memory");

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

  #if (1 == BSP_CFG_CORE_CA55)
        __asm volatile ("WFI");
  #endif
    }
    else if (BSP_RESET_CA55_2 == cpu)
    {
        /* CA55_2 software reset. */
        __asm volatile (
            "MOV  x0, %0                                              \n"
            "MSR  RMR_EL3, x0                                         \n"
            ::"r" (BSP_RESET_RMR_RESET_REQUEST_AARCH64) : "memory");

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

  #if (2 == BSP_CFG_CORE_CA55)
        __asm volatile ("WFI");
  #endif
    }
    else if (BSP_RESET_CA55_3 == cpu)
    {
        /* CA55_3 software reset. */
        __asm volatile (
            "MOV  x0, %0                                              \n"
            "MSR  RMR_EL3, x0                                         \n"
            ::"r" (BSP_RESET_RMR_RESET_REQUEST_AARCH64) : "memory");

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

  #if (3 == BSP_CFG_CORE_CA55)
        __asm volatile ("WFI");
  #endif
    }
 #endif
#endif
    else
    {
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
    }
}

/*******************************************************************************************************************//**
 * Release the CPU reset state.
 *
 * @param[in] cpu to be release reset state.
 **********************************************************************************************************************/
void R_BSP_CPUResetRelease (bsp_reset_t cpu)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    if (BSP_RESET_CR52_0 == cpu)
    {
        /* Release CR52_0 reset state. */
        R_SYSC_S->SWRCPU0 = BSP_PRV_RESET_RELEASE_KEY;
    }

#if (1U < BSP_FEATURE_BSP_CR52_CORE_NUM)
    else if (BSP_RESET_CR52_1 == cpu)
    {
        /* Release CR52_1 reset state. */
        R_SYSC_S->SWRCPU1 = BSP_PRV_RESET_RELEASE_KEY;
    }
#endif
#if (1U < BSP_FEATURE_BSP_CA55_CORE_NUM)
    else if (BSP_RESET_CA55_CLUSTER == cpu)
    {
        /* Release CA55_cluster software reset. */
        R_SYSC_S->SWR55C = BSP_PRV_RESET_RELEASE_KEY;
    }
    else if (BSP_RESET_CA55_0 == cpu)
    {
        /* Release CA55_0 reset state. */
        R_SYSC_S->SWR550 = BSP_PRV_RESET_RELEASE_KEY;
    }
    else if (BSP_RESET_CA55_1 == cpu)
    {
        /* Release CA55_1 reset state. */
        R_SYSC_S->SWR551 = BSP_PRV_RESET_RELEASE_KEY;
    }
    else if (BSP_RESET_CA55_2 == cpu)
    {
        /* Release CA55_2 reset state. */
        R_SYSC_S->SWR552 = BSP_PRV_RESET_RELEASE_KEY;
    }
    else if (BSP_RESET_CA55_3 == cpu)
    {
        /* Release CA55_3 reset state. */
        R_SYSC_S->SWR553 = BSP_PRV_RESET_RELEASE_KEY;
    }
#endif
    else
    {
        /* Do Nothing. */
    }

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
}

#if (1U < BSP_FEATURE_BSP_CA55_CORE_NUM)

/*******************************************************************************************************************//**
 * Controls whether the core is automatically released from reset when a CA55 cluster reset is performed.
 *
 * @param[in] cpu to be release reset state.
 * @param[in] release Enable/disable automatic reset release.
 **********************************************************************************************************************/
void R_BSP_CPUClusterResetAutoReleaseControl (bsp_reset_t cpu, bsp_cluster_reset_auto_release_t release)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    if (BSP_RESET_CA55_0 == cpu)
    {
        /* Controls the automatic reset release of CA55_0. */
        R_SYSC_S->SWR55ARC_b.ARC550 = release;
    }
    else if (BSP_RESET_CA55_1 == cpu)
    {
        /* Controls the automatic reset release of CA55_1. */
        R_SYSC_S->SWR55ARC_b.ARC551 = release;
    }
    else if (BSP_RESET_CA55_2 == cpu)
    {
        /* Controls the automatic reset release of CA55_2. */
        R_SYSC_S->SWR55ARC_b.ARC552 = release;
    }
    else if (BSP_RESET_CA55_3 == cpu)
    {
        /* Controls the automatic reset release of CA55_3. */
        R_SYSC_S->SWR55ARC_b.ARC553 = release;
    }
    else
    {
        /* Do Nothing. */
    }

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
}

#endif

/*******************************************************************************************************************//**
 *        Enable module reset state.
 *
 * @param[in] module_to_enable Modules to enable module reset state.
 **********************************************************************************************************************/
void R_BSP_ModuleResetEnable (bsp_module_reset_t module_to_enable)
{
    volatile uint32_t mrctl;
    uint32_t        * p_reg;

    /** When MRCTLn register exists in the safety region,
     *  it is necessary to add an offset of safety region. */
    p_reg = (uint32_t *) &R_SYSC_NS->MRCTLA +
            (((module_to_enable & BSP_RESET_MRCTL_SELECT_MASK) >> 16U) +
             (module_to_enable & BSP_RESET_MRCTL_REGION_SELECT_MASK));
    mrctl = 1U << (module_to_enable & BSP_RESET_MRCTL_BIT_SHIFT_MASK);

    /** Enable module reset state using MRCTLE register. */
    *p_reg |= mrctl;

    /** To ensure processing after module reset. */
    mrctl = *(volatile uint32_t *) (p_reg);
}

/*******************************************************************************************************************//**
 * Disable module reset state.
 *
 * @param[in] module_to_disable Modules to disable module reset state.
 *
 * @note      After reset release is performed by the module control register, a dummy read is performed to allow access
 *            to other than the RTC and LCDC. This is done several times according to the RZ microprocessor manual.
 *            However, the dummy read count for the RTC and LCDC may not be met depending on the device used.
 *            In that case, please perform additional dummy read processing after API execution.
 *            For example, in the case of RZN2H, 300 dummy reads are required for RTC and
 *            100 dummy reads are required for LCDC.
 *
 **********************************************************************************************************************/
void R_BSP_ModuleResetDisable (bsp_module_reset_t module_to_disable)
{
    volatile uint32_t mrctl;
    uint32_t        * p_reg;

    /** When MRCTLn register exists in the safety region,
     *  it is necessary to add an offset of safety region. */
    p_reg = (uint32_t *) &R_SYSC_NS->MRCTLA +
            (((module_to_disable & BSP_RESET_MRCTL_SELECT_MASK) >> 16U) +
             (module_to_disable & BSP_RESET_MRCTL_REGION_SELECT_MASK));
    mrctl = 1U << (module_to_disable & BSP_RESET_MRCTL_BIT_SHIFT_MASK);

    /** Disable module stop state using MRCTLn register. */
    *p_reg &= ~mrctl;

    /** In order to secure processing after release from module reset,
     *  dummy read the same register.
     *  Refer to "Notes on Module Reset Control Register Operation" of the RZ microprocessor manual. */
    uint32_t dummy_read_cnt = BSP_FEATURE_BSP_MODULE_RESET_DUMMY_READ_COUNT;

    while (dummy_read_cnt)
    {
        mrctl = *(volatile uint32_t *) (p_reg);
        dummy_read_cnt--;
    }
}

/** @} (end addtogroup BSP_MCU) */
