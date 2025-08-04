/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_clocks.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY                                     (0xA500U)
#define BSP_PRV_PRCR_CGC_UNLOCK                              ((BSP_PRV_PRCR_KEY) | 0x3U)
#define BSP_PRV_PRCR_LOCK                                    ((BSP_PRV_PRCR_KEY) | 0x0U)

/* Key code for writing  PCMD register. */
#define BSP_PRV_PCMD_KEY                                     (0xA5U)

/* Calculate the value to write to SCKCR. */
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS    (BSP_CFG_FSELXSPI0_DIVSELXSPI0 & 0x47U)
 #define BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS    ((BSP_CFG_FSELXSPI1_DIVSELXSPI1 & 0x47U) << 8U)
 #define BSP_PRV_STARTUP_SCKCR_CKIO_BITS                     ((BSP_CFG_CKIO & 7U) << 16U)
 #define BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS                ((BSP_CFG_FSELCANFD & 1U) << 20U)
 #define BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS                   ((BSP_CFG_PHYSEL & 1U) << 21U)
 #define BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS                  ((BSP_CFG_CLMASEL & 1U) << 22U)
 #define BSP_PRV_STARTUP_SCKCR_SPI0ASYNCSEL_BITS             ((BSP_CFG_SPI0ASYNCCLK & 1U) << 24U)
 #define BSP_PRV_STARTUP_SCKCR_SPI1ASYNCSEL_BITS             ((BSP_CFG_SPI1ASYNCCLK & 1U) << 25U)
 #define BSP_PRV_STARTUP_SCKCR_SPI2ASYNCSEL_BITS             ((BSP_CFG_SPI2ASYNCCLK & 1U) << 26U)
 #define BSP_PRV_STARTUP_SCKCR_SCI0ASYNCSEL_BITS             ((BSP_CFG_SCI0ASYNCCLK & 1U) << 27U)
 #define BSP_PRV_STARTUP_SCKCR_SCI1ASYNCSEL_BITS             ((BSP_CFG_SCI1ASYNCCLK & 1U) << 28U)
 #define BSP_PRV_STARTUP_SCKCR_SCI2ASYNCSEL_BITS             ((BSP_CFG_SCI2ASYNCCLK & 1U) << 29U)
 #define BSP_PRV_STARTUP_SCKCR_SCI3ASYNCSEL_BITS             ((BSP_CFG_SCI3ASYNCCLK & 1U) << 30U)
 #define BSP_PRV_STARTUP_SCKCR_SCI4ASYNCSEL_BITS             ((BSP_CFG_SCI4ASYNCCLK & 1U) << 31U)
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS    (BSP_CFG_FSELXSPI0_DIVSELXSPI0 & 0x47U)
 #define BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS    ((BSP_CFG_FSELXSPI1_DIVSELXSPI1 & 0x47U) << 8U)
 #define BSP_PRV_STARTUP_SCKCR_CKIO_BITS                     ((BSP_CFG_CKIO & 7U) << 16U)
 #define BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS                ((BSP_CFG_FSELCANFD & 1U) << 20U)
 #define BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS                   ((BSP_CFG_PHYSEL & 1U) << 21U)
 #define BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS                  ((BSP_CFG_CLMASEL & 1U) << 22U)
#endif

/* Calculate the value to write to SCKCR2. */
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR2_FSELCPU0_BITS                (BSP_CFG_FSELCPU0 & 3U)
 #if (2 == BSP_FEATURE_BSP_CR52_CORE_NUM)
  #define BSP_PRV_STARTUP_SCKCR2_FSELCPU1_BITS               ((BSP_CFG_FSELCPU1 & 3U) << 2U)
 #else
  #define BSP_PRV_STARTUP_SCKCR2_FSELCPU1_BITS               (0U)
 #endif
 #define BSP_PRV_STARTUP_SCKCR2_RESERVED_BIT4_BITS           (1U << 4U) // The write value should be 1.
 #define BSP_PRV_STARTUP_SCKCR2_DIVSELSUB_BITS               ((BSP_CFG_DIVSELSUB & 1U) << 5U)
 #define BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS            ((BSP_CFG_SPI3ASYNCCLK & 1U) << 24U)
 #define BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS            ((BSP_CFG_SCI5ASYNCCLK & 1U) << 25U)
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR2_CR52CPU0_BITS                (BSP_CFG_CR52C0CLK & 3U)
 #define BSP_PRV_STARTUP_SCKCR2_CR52CPU1_BITS                ((BSP_CFG_CR52C1CLK & 3U) << 2U)
 #define BSP_PRV_STARTUP_SCKCR2_CA55CORE0_BITS               ((BSP_CFG_CA55CORE0 & 1U) << 8U)
 #define BSP_PRV_STARTUP_SCKCR2_CA55CORE1_BITS               ((BSP_CFG_CA55CORE1 & 1U) << 9U)
 #define BSP_PRV_STARTUP_SCKCR2_CA55CORE2_BITS               ((BSP_CFG_CA55CORE2 & 1U) << 10U)
 #define BSP_PRV_STARTUP_SCKCR2_CA55CORE3_BITS               ((BSP_CFG_CA55CORE3 & 1U) << 11U)
 #define BSP_PRV_STARTUP_SCKCR2_CA55SCLK_BITS                ((BSP_CFG_CA55SCLK & 1U) << 12U)
 #define BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS            ((BSP_CFG_SPI3ASYNCCLK & 3U) << 16U)
 #define BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS            ((BSP_CFG_SCI5ASYNCCLK & 3U) << 18U)
#endif

/* Calculate the value to write to SCKCR3. */
#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR3_SPI0ASYNCSEL_BITS            (BSP_CFG_SPI0ASYNCCLK & 3U)
 #define BSP_PRV_STARTUP_SCKCR3_SPI1ASYNCSEL_BITS            ((BSP_CFG_SPI1ASYNCCLK & 3U) << 2U)
 #define BSP_PRV_STARTUP_SCKCR3_SPI2ASYNCSEL_BITS            ((BSP_CFG_SPI2ASYNCCLK & 3U) << 4U)
 #define BSP_PRV_STARTUP_SCKCR3_SCI0ASYNCSEL_BITS            ((BSP_CFG_SCI0ASYNCCLK & 3U) << 6U)
 #define BSP_PRV_STARTUP_SCKCR3_SCI1ASYNCSEL_BITS            ((BSP_CFG_SCI1ASYNCCLK & 3U) << 8U)
 #define BSP_PRV_STARTUP_SCKCR3_SCI2ASYNCSEL_BITS            ((BSP_CFG_SCI2ASYNCCLK & 3U) << 10U)
 #define BSP_PRV_STARTUP_SCKCR3_SCI3ASYNCSEL_BITS            ((BSP_CFG_SCI3ASYNCCLK & 3U) << 12U)
 #define BSP_PRV_STARTUP_SCKCR3_SCI4ASYNCSEL_BITS            ((BSP_CFG_SCI4ASYNCCLK & 3U) << 14U)
 #define BSP_PRV_STARTUP_SCKCR3_LCDCDIVSEL_BITS              ((BSP_CFG_LCDCDIVSEL & 15U) << 20U)
#endif

/* Calculate the value to write to SCKCR4. */
#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE0ASYNCSEL_BITS           (BSP_CFG_SCIE0ASYNCCLK & 3U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE1ASYNCSEL_BITS           ((BSP_CFG_SCIE1ASYNCCLK & 3U) << 2U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE2ASYNCSEL_BITS           ((BSP_CFG_SCIE2ASYNCCLK & 3U) << 4U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE3ASYNCSEL_BITS           ((BSP_CFG_SCIE3ASYNCCLK & 3U) << 6U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE4ASYNCSEL_BITS           ((BSP_CFG_SCIE4ASYNCCLK & 3U) << 8U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE5ASYNCSEL_BITS           ((BSP_CFG_SCIE5ASYNCCLK & 3U) << 10U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE6ASYNCSEL_BITS           ((BSP_CFG_SCIE6ASYNCCLK & 3U) << 12U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE7ASYNCSEL_BITS           ((BSP_CFG_SCIE7ASYNCCLK & 3U) << 14U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE8ASYNCSEL_BITS           ((BSP_CFG_SCIE8ASYNCCLK & 3U) << 16U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE9ASYNCSEL_BITS           ((BSP_CFG_SCIE9ASYNCCLK & 3U) << 18U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE10ASYNCSEL_BITS          ((BSP_CFG_SCIE10ASYNCCLK & 3U) << 20U)
 #define BSP_PRV_STARTUP_SCKCR4_SCIE11ASYNCSEL_BITS          ((BSP_CFG_SCIE11ASYNCCLK & 3U) << 22U)
 #define BSP_PRV_STARTUP_SCKCR4_ENCOUTCLK_BITS               ((BSP_CFG_ENCOUTCLK & 1U) << 24U)
#endif
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR                               (BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR_CKIO_BITS |                  \
                                                              BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS |             \
                                                              BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS |                \
                                                              BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS |               \
                                                              BSP_PRV_STARTUP_SCKCR_SPI0ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SPI1ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SPI2ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SCI0ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SCI1ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SCI2ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SCI3ASYNCSEL_BITS |          \
                                                              BSP_PRV_STARTUP_SCKCR_SCI4ASYNCSEL_BITS)

#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR                               (BSP_PRV_STARTUP_SCKCR_FSELXSPI0_DIVSELXSPI0_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR_FSELXSPI1_DIVSELXSPI1_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR_CKIO_BITS |                  \
                                                              BSP_PRV_STARTUP_SCKCR_FSELCANFD_BITS |             \
                                                              BSP_PRV_STARTUP_SCKCR_PHYSEL_BITS |                \
                                                              BSP_PRV_STARTUP_SCKCR_CLMASEL_BITS)
#endif
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR2                              (BSP_PRV_STARTUP_SCKCR2_FSELCPU0_BITS |      \
                                                              BSP_PRV_STARTUP_SCKCR2_FSELCPU1_BITS |      \
                                                              BSP_PRV_STARTUP_SCKCR2_RESERVED_BIT4_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR2_DIVSELSUB_BITS |     \
                                                              BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS)
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR2                              (BSP_PRV_STARTUP_SCKCR2_CR52CPU0_BITS |     \
                                                              BSP_PRV_STARTUP_SCKCR2_CR52CPU1_BITS |     \
                                                              BSP_PRV_STARTUP_SCKCR2_CA55CORE0_BITS |    \
                                                              BSP_PRV_STARTUP_SCKCR2_CA55CORE1_BITS |    \
                                                              BSP_PRV_STARTUP_SCKCR2_CA55CORE2_BITS |    \
                                                              BSP_PRV_STARTUP_SCKCR2_CA55CORE3_BITS |    \
                                                              BSP_PRV_STARTUP_SCKCR2_CA55SCLK_BITS |     \
                                                              BSP_PRV_STARTUP_SCKCR2_SPI3ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR2_SCI5ASYNCSEL_BITS)
#endif

#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR3                              (0U)
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR3                              (BSP_PRV_STARTUP_SCKCR3_SPI0ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SPI1ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SPI2ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SCI0ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SCI1ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SCI2ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SCI3ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_SCI4ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR3_LCDCDIVSEL_BITS)
#endif
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR4                              (0U)
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #define BSP_PRV_STARTUP_SCKCR4                              (BSP_PRV_STARTUP_SCKCR4_SCIE0ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE1ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE2ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE3ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE4ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE5ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE6ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE7ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE8ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE9ASYNCSEL_BITS |  \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE10ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR4_SCIE11ASYNCSEL_BITS | \
                                                              BSP_PRV_STARTUP_SCKCR4_ENCOUTCLK_BITS)
#endif

#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)

 #define BSP_PRV_STARTUP_SCKCR2_FSELCPU0_ICLK_MUL2           (BSP_CLOCKS_FSELCPU0_ICLK_MUL2 << \
                                                              R_SYSC_S_SCKCR2_FSELCPU0_Pos)
 #if (2 == BSP_FEATURE_BSP_CR52_CORE_NUM)
  #define BSP_PRV_STARTUP_SCKCR2_FSELCPU1_ICLK_MUL2          (BSP_CLOCKS_FSELCPU1_ICLK_MUL2 << \
                                                              R_SYSC_S_SCKCR2_FSELCPU1_Pos)
 #endif

#endif

/* Calculate the value to write to HIZCTRLEN. */
#if (4 == BSP_FEATURE_CGC_CLMA_UNIT)
 #define BSP_PRV_STARTUP_HIZCTRLEN       ((BSP_CFG_CLMA1MASK << 2) | (BSP_CFG_CLMA0MASK << 1) | \
                                          BSP_CFG_CLMA3MASK)
#elif (7 == BSP_FEATURE_CGC_CLMA_UNIT)
 #define BSP_PRV_STARTUP_HIZCTRLEN       ((BSP_CFG_CLMA4MASK << 5) | \
                                          (BSP_CFG_CLMA3MASK << 4) | \
                                          (BSP_CFG_CLMA2MASK << 3) | \
                                          (BSP_CFG_CLMA1MASK << 2) | \
                                          (BSP_CFG_CLMA0MASK << 1) | \
                                          (BSP_CFG_CLMA6MASK))
#endif

/* Calculate the value to write to PLL0_SSC_CTR */
#if BSP_FEATURE_CGC_PLL0_STANDBY_STATE_SUPPORTED
 #define BSP_PRV_STARTUP_PLL0_SSC_CTR    ((BSP_CFG_PLL0MFR << 24) | \
                                          (BSP_CFG_PLL0MRR << 16) | \
                                          (BSP_CFG_PLL0SSCEN << 0))
#endif

/* Calculate the value to write to PLL2_SSC_CTR */
#if BSP_FEATURE_CGC_PLL2_STANDBY_STATE_SUPPORTED
 #define BSP_PRV_STARTUP_PLL2_SSC_CTR    ((BSP_CFG_PLL2MFR << 24) | \
                                          (BSP_CFG_PLL2MRR << 16) | \
                                          (BSP_CFG_PLL2SSCEN << 0))

#endif

/* Calculate the value to write to PLL3_VCO_CTR */
#if BSP_FEATURE_CGC_PLL3_STANDBY_STATE_SUPPORTED
 #define BSP_PRV_STARTUP_PLL3_VCO_CTR0         ((BSP_CFG_PLL3P << 16) | \
                                                (BSP_CFG_PLL3M << 0))

 #define BSP_PRV_STARTUP_PLL3_VCO_CTR1         ((BSP_CFG_PLL3K << 16) | \
                                                (BSP_CFG_PLL3S << 0))
#endif

/* Frequencies of clocks. */
#define BSP_PRV_CPU_FREQ_600_MHZ               (600000000U) // CPU frequency is 600 MHz
#define BSP_PRV_CPU_FREQ_500_MHZ               (500000000U) // CPU frequency is 500 MHz
#define BSP_PRV_CPU_FREQ_200_MHZ               (200000000U) // CPU frequency is 200 MHz
#define BSP_PRV_CPU_FREQ_150_MHZ               (150000000U) // CPU frequency is 150 MHz

/* Command sequence for enabling CLMA. */
#define BSP_PRV_CTL0_ENABLE_TARGET_CMD         (0x01)
#define BSP_PRV_CTL0_ENABLE_REVERSED_CMD       (0xFE)
#define BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT    (10U)

#define BSP_PRV_LOCO_STABILIZATION_COUNT       (40000)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
#if !BSP_CFG_SOFT_RESET_SUPPORTED
static void bsp_prv_clock_set_hard_reset(void);

#endif

/*******************************************************************************************************************//**
 * @internal
 * @addtogroup BSP_MCU_PRV Internal BSP Documentation
 * @ingroup RENESAS_INTERNAL
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Update SystemCoreClock variable based on current clock settings.
 **********************************************************************************************************************/
void SystemCoreClockUpdate (void)
{
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #if defined(BSP_CFG_CORE_CR52)
    uint32_t devselsub = R_SYSC_S->SCKCR2_b.DIVSELSUB;
    uint32_t fselcpu   = R_SYSC_S->SCKCR2_b.FSELCPU0;

    if (0U == devselsub)
    {
        SystemCoreClock = BSP_PRV_CPU_FREQ_200_MHZ << fselcpu;
    }
    else
    {
        SystemCoreClock = BSP_PRV_CPU_FREQ_150_MHZ << fselcpu;
    }
 #endif
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
 #if defined(BSP_CFG_CORE_CR52)
  #if (0 == BSP_CFG_CORE_CR52)
    uint32_t cr52cpu = R_SYSC_S->SCKCR2_b.CR52CPU0;
  #elif (1 == BSP_CFG_CORE_CR52)
    uint32_t cr52cpu = R_SYSC_S->SCKCR2_b.CR52CPU1;
  #endif
    SystemCoreClock = BSP_PRV_CPU_FREQ_500_MHZ << cr52cpu;
 #elif defined(BSP_CFG_CORE_CA55)
  #if (0 == BSP_CFG_CORE_CA55)
    uint32_t ca55core = R_SYSC_S->SCKCR2_b.CA55CORE0;
  #elif (1 == BSP_CFG_CORE_CA55)
    uint32_t ca55core = R_SYSC_S->SCKCR2_b.CA55CORE1;
  #elif (2 == BSP_CFG_CORE_CA55)
    uint32_t ca55core = R_SYSC_S->SCKCR2_b.CA55CORE2;
  #elif (3 == BSP_CFG_CORE_CA55)
    uint32_t ca55core = R_SYSC_S->SCKCR2_b.CA55CORE3;
  #endif
    SystemCoreClock = BSP_PRV_CPU_FREQ_600_MHZ << ca55core;
 #endif
#endif
}

/*******************************************************************************************************************//**
 * Applies system core clock source and divider changes.  The MCU is expected to be in high speed mode during this
 * configuration and the CGC registers are expected to be unlocked in PRCR.
 *
 * @param[in] sckcr                  Value to set in SCKCR register
 * @param[in] sckcr2                 Value to set in SCKCR2 register
 * @param[in] sckcr3                 Value to set in SCKCR3 register
 * @param[in] sckcr4                 Value to set in SCKCR4 register
 **********************************************************************************************************************/
void bsp_prv_clock_set (uint32_t sckcr, uint32_t sckcr2, uint32_t sckcr3, uint32_t sckcr4)
{
    volatile uint32_t dummy;
    sckcr  = sckcr & BSP_PRV_SCKCR_MASK;
    sckcr2 = sckcr2 & BSP_PRV_SCKCR2_MASK;
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
    FSP_PARAMETER_NOT_USED(sckcr3);
    FSP_PARAMETER_NOT_USED(sckcr4);
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
    sckcr3 = sckcr3 & BSP_PRV_SCKCR3_MASK;
    sckcr4 = sckcr4 & BSP_PRV_SCKCR4_MASK;
#endif

    /* Set the system source clock */
    R_SYSC_S->SCKCR2 = sckcr2;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;

    R_SYSC_NS->SCKCR = sckcr;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;

#if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
    R_SYSC_NS->SCKCR3 = sckcr3;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;

    R_SYSC_NS->SCKCR4 = sckcr4;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
#endif

    FSP_PARAMETER_NOT_USED(dummy);

    /* Clock is now at requested frequency. */

    /* Update the CMSIS core clock variable so that it reflects the new ICLK frequency. */
    SystemCoreClockUpdate();
}

#if !BSP_CFG_SOFT_RESET_SUPPORTED

static void bsp_prv_clock_set_hard_reset (void)
{
    volatile uint32_t dummy;
    uint32_t          sckcr  = BSP_PRV_STARTUP_SCKCR & BSP_PRV_SCKCR_MASK;
    uint32_t          sckcr2 = BSP_PRV_STARTUP_SCKCR2 & BSP_PRV_SCKCR2_MASK;
 #if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
    uint32_t sckcr3 = BSP_PRV_STARTUP_SCKCR3 & BSP_PRV_SCKCR3_MASK;
    uint32_t sckcr4 = BSP_PRV_STARTUP_SCKCR4 & BSP_PRV_SCKCR4_MASK;
 #endif

    /* Set the system source clock */
    R_SYSC_S->SCKCR2 = sckcr2;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;
    dummy = R_SYSC_S->SCKCR2;

    R_SYSC_NS->SCKCR = sckcr;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;
    dummy = R_SYSC_NS->SCKCR;

 #if (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
    R_SYSC_NS->SCKCR3 = sckcr3;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;
    dummy = R_SYSC_NS->SCKCR3;

    R_SYSC_NS->SCKCR4 = sckcr4;

    /** In order to secure processing after clock frequency is changed,
     *  dummy read the same register at least eight times.
     *  Refer to "Notes on Clock Generation Circuit" in the RZ microprocessor manual. */
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
    dummy = R_SYSC_NS->SCKCR4;
 #endif

    FSP_PARAMETER_NOT_USED(dummy);

    /* Clock is now at requested frequency. */

    /* Update the CMSIS core clock variable so that it reflects the new ICLK frequency. */
    SystemCoreClockUpdate();
}

#endif

/*******************************************************************************************************************//**
 * Initializes system clocks.  Makes no assumptions about current register settings.
 **********************************************************************************************************************/
void bsp_clock_init (void)
{
    volatile uint32_t dummy = 0;

    /* Unlock CGC protection registers. */
    R_RWP_NS->PRCRN = (uint16_t) BSP_PRV_PRCR_CGC_UNLOCK;
    R_RWP_S->PRCRS  = (uint16_t) BSP_PRV_PRCR_CGC_UNLOCK;

    /* The SystemCoreClock needs to be updated before calling R_BSP_SoftwareDelay. */
    SystemCoreClockUpdate();

    /* Set source clock and dividers. */
#if BSP_CFG_SOFT_RESET_SUPPORTED
    bsp_prv_clock_set(BSP_PRV_STARTUP_SCKCR, BSP_PRV_STARTUP_SCKCR2, BSP_PRV_STARTUP_SCKCR3, BSP_PRV_STARTUP_SCKCR4);
#else
    bsp_prv_clock_set_hard_reset();
#endif

#if (BSP_FEATURE_CGC_PLL0_STANDBY_STATE_SUPPORTED)
    if (BSP_CLOCKS_PLL0_NORMAL == BSP_CFG_PLL0)
    {
        if (BSP_CLOCKS_PLL0_STANDBY == R_SYSC_S->PLL0EN_b.PLL0EN)
        {
            /* Configure SSC settings. */
            if (BSP_CFG_PLL0SSCEN == BSP_CLOCKS_PLL0SSCEN_ENABLE)
            {
                R_SYSC_S->PLL0_SSC_CTR = BSP_PRV_STARTUP_PLL0_SSC_CTR;
            }

            /* Release PLL from standby state. */
            R_SYSC_S->PLL0EN = BSP_CFG_PLL0;

            /* Confirm transition to normal mode. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PLL0MON_b.PLL0MON, 1U);

            /* Confirm clock source change of PLL clock domain. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PMSEL_b.PMSEL0MON, 1U);
        }
    }
#endif

#if (BSP_FEATURE_CGC_PLL1_STANDBY_STATE_SUPPORTED)
 #if (BSP_CLOCKS_PLL1_INITIAL != BSP_CFG_PLL1)

    /* Release PLL from standby state. */
    R_SYSC_S->PLL1EN = BSP_CFG_PLL1;
 #endif
#endif

#if (BSP_FEATURE_CGC_PLL2_STANDBY_STATE_SUPPORTED)
    if (BSP_CLOCKS_PLL2_NORMAL == BSP_CFG_PLL2)
    {
        if (BSP_CLOCKS_PLL2_STANDBY == R_SYSC_S->PLL2EN_b.PLL2EN)
        {
            /* Configure SSC settings. */
            if (BSP_CFG_PLL0SSCEN == BSP_CLOCKS_PLL0SSCEN_ENABLE)
            {
                R_SYSC_S->PLL2_SSC_CTR = BSP_PRV_STARTUP_PLL2_SSC_CTR;
            }

            /* Release PLL from standby state. */
            R_SYSC_S->PLL2EN = BSP_CFG_PLL2;

            /* Confirm transition to normal mode. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PLL2MON_b.PLL2MON, 1U);

            /* Confirm clock source change of PLL clock domain. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PMSEL_b.PMSEL2MON, 1U);
        }
    }
#endif

#if (BSP_FEATURE_CGC_PLL3_STANDBY_STATE_SUPPORTED)
    if (BSP_CLOCKS_PLL3_NORMAL == BSP_CFG_PLL3)
    {
        if (BSP_CLOCKS_PLL3_STANDBY == R_SYSC_S->PLL3EN_b.PLL3EN)
        {
            /* Configure PLL3 settings. */
            R_SYSC_S->PLL3_VCO_CTR0 = BSP_PRV_STARTUP_PLL3_VCO_CTR0;
            R_SYSC_S->PLL3_VCO_CTR1 = BSP_PRV_STARTUP_PLL3_VCO_CTR1;

            /* Release PLL from standby state. */
            R_SYSC_S->PLL3EN = BSP_CFG_PLL3;

            /* Confirm transition to normal mode. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PLL3MON_b.PLL3MON, 1U);

            /* Confirm clock source change of PLL clock domain. */
            FSP_HARDWARE_REGISTER_WAIT(R_SYSC_S->PMSEL_b.PMSEL3MON, 1U);
        }
    }
#endif

#if (BSP_CLOCKS_LOCO_ENABLE == BSP_CFG_LOCO_ENABLE)
    R_SYSC_S->LOCOCR = BSP_CLOCKS_LOCO_ENABLE;

    /* Only start using the LOCO clock after
     * the LOCO oscillation stabilization time (tLOCOWT) has elapsed. */
    for (uint16_t i = 0; i < BSP_PRV_LOCO_STABILIZATION_COUNT; i++)
    {
        __asm volatile ("nop");
    }
#endif

    R_SYSC_S->HIZCTRLEN = BSP_PRV_STARTUP_HIZCTRLEN;

#if (BSP_CLOCKS_CLMA0_ENABLE == BSP_CFG_CLMA0_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA0->CMPL = BSP_CFG_CLMA0_CMPL;
    R_CLMA0->CMPH = BSP_CFG_CLMA0_CMPH;

    /* Enabling CLMA0 operation. */
    volatile uint32_t timeout_clma0 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA0 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma0);

        R_CLMA0->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA0->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA0->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA0->PROTSR;
        }

        timeout_clma0--;
    } while (1 == R_CLMA0->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA1_ENABLE == BSP_CFG_CLMA1_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA1->CMPL = BSP_CFG_CLMA1_CMPL;
    R_CLMA1->CMPH = BSP_CFG_CLMA1_CMPH;

    /* Enabling CLMA1 operation. */
    volatile uint32_t timeout_clma1 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA1 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma1);

        R_CLMA1->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA1->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA1->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA1->PROTSR;
        }

        timeout_clma1--;
    } while (1 == R_CLMA1->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA2_ENABLE == BSP_CFG_CLMA2_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA2->CMPL = BSP_CFG_CLMA2_CMPL;
    R_CLMA2->CMPH = BSP_CFG_CLMA2_CMPH;

    /* Enabling CLMA2 operation. */
    volatile uint32_t timeout_clma2 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA2 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma2);

        R_CLMA2->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA2->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA2->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA2->PROTSR;
        }

        timeout_clma2--;
    } while (1 == R_CLMA2->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA3_ENABLE == BSP_CFG_CLMA3_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA3->CMPL = BSP_CFG_CLMA3_CMPL;
    R_CLMA3->CMPH = BSP_CFG_CLMA3_CMPH;

    /* Enabling CLMA3 operation. */
    volatile uint32_t timeout_clma3 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA3 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma3);

        R_CLMA3->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA3->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA3->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA3->PROTSR;
        }

        timeout_clma3--;
    } while (1 == R_CLMA3->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA4_ENABLE == BSP_CFG_CLMA4_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA4->CMPL = BSP_CFG_CLMA4_CMPL;
    R_CLMA4->CMPH = BSP_CFG_CLMA4_CMPH;

    /* Enabling CLMA4 operation. */
    volatile uint32_t timeout_clma4 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA4 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma4);

        R_CLMA4->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA4->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA4->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA4->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA4->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA4->PROTSR;
        }

        timeout_clma4--;
    } while (1 == R_CLMA4->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA5_ENABLE == BSP_CFG_CLMA5_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA5->CMPL = BSP_CFG_CLMA5_CMPL;
    R_CLMA5->CMPH = BSP_CFG_CLMA5_CMPH;

    /* Enabling CLMA5 operation. */
    volatile uint32_t timeout_clma5 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA5 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma5);

        R_CLMA5->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA5->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA5->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA5->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA5->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA5->PROTSR;
        }

        timeout_clma5--;
    } while (1 == R_CLMA5->PROTSR_b.PRERR);
#endif

#if (BSP_CLOCKS_CLMA6_ENABLE == BSP_CFG_CLMA6_ENABLE)

    /* Set the lower and upper limit for comparing frequency domains. */
    R_CLMA6->CMPL = BSP_CFG_CLMA6_CMPL;
    R_CLMA6->CMPH = BSP_CFG_CLMA6_CMPH;

    /* Enabling CLMA6 operation. */
    volatile uint32_t timeout_clma6 = BSP_PRV_CTL0_ENABLE_SETTING_TIMEOUT;

    do
    {
        /* If the setting that enables CLMA6 times out, the assertion will fail. */
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout_clma6);

        R_CLMA6->PCMD = BSP_PRV_PCMD_KEY;

        R_CLMA6->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;
        R_CLMA6->CTL0 = BSP_PRV_CTL0_ENABLE_REVERSED_CMD;
        R_CLMA6->CTL0 = BSP_PRV_CTL0_ENABLE_TARGET_CMD;

        if (1 != R_CLMA6->CTL0)
        {
            /* Check the value of PROTSR register. */
            dummy = R_CLMA6->PROTSR;
        }

        timeout_clma6--;
    } while (1 == R_CLMA6->PROTSR_b.PRERR);
#endif

    /* Lock CGC and LPM protection registers. */
    R_RWP_NS->PRCRN = (uint16_t) BSP_PRV_PRCR_LOCK;
    R_RWP_S->PRCRS  = (uint16_t) BSP_PRV_PRCR_LOCK;

    FSP_PARAMETER_NOT_USED(dummy);
}

/** @} (end addtogroup BSP_MCU_PRV) */
