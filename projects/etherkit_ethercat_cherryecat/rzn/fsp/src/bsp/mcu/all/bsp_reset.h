/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_RESET_H
#define BSP_RESET_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Key code for writing reset register. */
#define BSP_PRV_RESET_KEY                 (0x4321A501U)
#define BSP_PRV_RESET_KEY_AUTO_RELEASE    (0x4321A502U)
#define BSP_PRV_RESET_RELEASE_KEY         (0x00000000U)

/* MRCTL register selection. Bits 16-20 assign values in order for the module control registers (A=0, E=4).
 * Bit 24 indicates whether MRCTLn register is in the safety region. */
#define BSP_RESET_MRCTLA_SELECT           (0x00000000U)
#define BSP_RESET_MRCTLE_SELECT           (0x00040000U)
#define BSP_RESET_MRCTLI_SELECT           (0x00480000U)
#define BSP_RESET_MRCTLM_SELECT           (0x000C0000U)

/* MRCTL register bit number.  */
#define BSP_RESET_MRCTL_BIT0_SHIFT        (0x00000000U)
#define BSP_RESET_MRCTL_BIT1_SHIFT        (0x00000001U)
#define BSP_RESET_MRCTL_BIT2_SHIFT        (0x00000002U)
#define BSP_RESET_MRCTL_BIT3_SHIFT        (0x00000003U)
#define BSP_RESET_MRCTL_BIT4_SHIFT        (0x00000004U)
#define BSP_RESET_MRCTL_BIT5_SHIFT        (0x00000005U)
#define BSP_RESET_MRCTL_BIT6_SHIFT        (0x00000006U)
#define BSP_RESET_MRCTL_BIT7_SHIFT        (0x00000007U)
#define BSP_RESET_MRCTL_BIT8_SHIFT        (0x00000008U)
#define BSP_RESET_MRCTL_BIT9_SHIFT        (0x00000009U)
#define BSP_RESET_MRCTL_BIT10_SHIFT       (0x0000000AU)
#define BSP_RESET_MRCTL_BIT11_SHIFT       (0x0000000BU)
#define BSP_RESET_MRCTL_BIT12_SHIFT       (0x0000000CU)
#define BSP_RESET_MRCTL_BIT13_SHIFT       (0x0000000DU)
#define BSP_RESET_MRCTL_BIT14_SHIFT       (0x0000000EU)
#define BSP_RESET_MRCTL_BIT15_SHIFT       (0x0000000FU)
#define BSP_RESET_MRCTL_BIT16_SHIFT       (0x00000010U)
#define BSP_RESET_MRCTL_BIT17_SHIFT       (0x00000011U)
#define BSP_RESET_MRCTL_BIT18_SHIFT       (0x00000012U)
#define BSP_RESET_MRCTL_BIT19_SHIFT       (0x00000013U)
#define BSP_RESET_MRCTL_BIT20_SHIFT       (0x00000014U)
#define BSP_RESET_MRCTL_BIT21_SHIFT       (0x00000015U)
#define BSP_RESET_MRCTL_BIT22_SHIFT       (0x00000016U)
#define BSP_RESET_MRCTL_BIT23_SHIFT       (0x00000017U)
#define BSP_RESET_MRCTL_BIT24_SHIFT       (0x00000018U)
#define BSP_RESET_MRCTL_BIT25_SHIFT       (0x00000019U)
#define BSP_RESET_MRCTL_BIT26_SHIFT       (0x0000001AU)
#define BSP_RESET_MRCTL_BIT27_SHIFT       (0x0000001BU)
#define BSP_RESET_MRCTL_BIT28_SHIFT       (0x0000001CU)
#define BSP_RESET_MRCTL_BIT29_SHIFT       (0x0000001DU)
#define BSP_RESET_MRCTL_BIT30_SHIFT       (0x0000001EU)
#define BSP_RESET_MRCTL_BIT31_SHIFT       (0x0000001FU)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/** CPU to be reset target.*/
typedef enum e_bsp_reset
{
    BSP_RESET_CR52_0       = 0,        ///< Software reset for CR52_0
    BSP_RESET_CR52_1       = 1,        ///< Software reset for CR52_1
    BSP_RESET_CA55_CLUSTER = 2,        ///< Software reset for CA55_CLUSTER
    BSP_RESET_CA55_0       = 3,        ///< Software reset for CA55_0
    BSP_RESET_CA55_1       = 4,        ///< Software reset for CA55_1
    BSP_RESET_CA55_2       = 5,        ///< Software reset for CA55_2
    BSP_RESET_CA55_3       = 6,        ///< Software reset for CA55_3
} bsp_reset_t;

/** CA55 cluster reset auto reset release status. */
typedef enum e_bsp_cluster_reset_auto_release
{
    BSP_CLUSTER_RESET_AUTO_RELEASE_DISABLE = 0,
    BSP_CLUSTER_RESET_AUTO_RELEASE_ENABLE  = 1,
} bsp_cluster_reset_auto_release_t;

/** The different types of registers that can control the reset of peripheral modules related to Ethernet. */
typedef enum e_bsp_module_reset
{
    /** Enables writing to the registers related to xSPI Unit 0 reset control. */
    BSP_MODULE_RESET_XSPI0 = (BSP_RESET_MRCTLA_SELECT | BSP_RESET_MRCTL_BIT4_SHIFT),

    /** Enables writing to the registers related to xSPI Unit 1 reset control. */
    BSP_MODULE_RESET_XSPI1 = (BSP_RESET_MRCTLA_SELECT | BSP_RESET_MRCTL_BIT5_SHIFT),

    /** Enables writing to the registers related to GMAC (PCLKH clock domain) reset control. */
    BSP_MODULE_RESET_GMAC0_PCLKH = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT0_SHIFT),

    /** Enables writing to the registers related to GMAC (PCLKM clock domain) reset control. */
    BSP_MODULE_RESET_GMAC0_PCLKM = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT1_SHIFT),

    /** Enables writing to the registers related to ETHSW reset control. */
    BSP_MODULE_RESET_ETHSW = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT2_SHIFT),

    /** Enables writing to the registers related to ESC (Bus clock domain) reset control. */
    BSP_MODULE_RESET_ESC_BUS = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT3_SHIFT),

    /** Enables writing to the registers related to ESC (IP clock domain) reset control. */
    BSP_MODULE_RESET_ESC_IP = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT4_SHIFT),

    /** Enables writing to the registers related to Ethernet subsystem register reset control. */
    BSP_MODULE_RESET_ESC_ETH_SUBSYSTEM = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT5_SHIFT),

    /** Enables writing to the registers related to MII converter reset control. */
    BSP_MODULE_RESET_MII = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT6_SHIFT),

    /** Enables writing to the registers related to GMAC Unit 1 (PCLKAH clock domain) reset control. */
    BSP_MODULE_RESET_GMAC1_ACLK = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT16_SHIFT),

    /** Enables writing to the registers related to GMAC Unit 1 (PCLKAM clock domain) reset control. */
    BSP_MODULE_RESET_GMAC1_HCLK = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT17_SHIFT),

    /** Enables writing to the registers related to GMAC Unit 2 (PCLKAH clock domain) reset control. */
    BSP_MODULE_RESET_GMAC2_ACLK = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT18_SHIFT),

    /** Enables writing to the registers related to GMAC Unit 2 (PCLKAM clock domain) reset control. */
    BSP_MODULE_RESET_GMAC2_HCLK = (BSP_RESET_MRCTLE_SELECT | BSP_RESET_MRCTL_BIT19_SHIFT),

    /** Enables writing to the registers related to PHOSTIF reset control. */
    BSP_MODULE_RESET_PHOSTIF = (BSP_RESET_MRCTLI_SELECT | BSP_RESET_MRCTL_BIT0_SHIFT),

    /** Enables writing to the registers related to SHOSTIF (Master bus clock domain) reset control. */
    BSP_MODULE_RESET_SHOSTIF_MASTER_BUS_CLOCK = (BSP_RESET_MRCTLI_SELECT | BSP_RESET_MRCTL_BIT1_SHIFT),

    /** Enables writing to the registers related to SHOSTIF (Slave bus clock domain) reset control. */
    BSP_MODULE_RESET_SHOSTIF_SLAVE_BUS_CLOCK = (BSP_RESET_MRCTLI_SELECT | BSP_RESET_MRCTL_BIT2_SHIFT),

    /** Enables writing to the registers related to SHOSTIF (IP clock domain) reset control. */
    BSP_MODULE_RESET_SHOSTIF_IP_CLOCK = (BSP_RESET_MRCTLI_SELECT | BSP_RESET_MRCTL_BIT3_SHIFT),

    /** Enables writing to the registers related to PCIE reset control. */
    BSP_MODULE_RESET_PCIE = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT8_SHIFT),

    /** Enables writing to the registers related to DDRSS rst_n reset control. */
    BSP_MODULE_RESET_DDRSS_RST_N = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT16_SHIFT),

    /** Enables writing to the registers related to DDRSS PwrOkln reset control. */
    BSP_MODULE_RESET_DDRSS_RST_PWROKLN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT17_SHIFT),

    /** Enables writing to the registers related to DDRSS Reset reset control. */
    BSP_MODULE_RESET_DDRSS_RESET = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT18_SHIFT),

    /** Enables writing to the registers related to DDRSS axi0_ARESETn reset control. */
    BSP_MODULE_RESET_DDRSS_AXI0_ARESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT19_SHIFT),

    /** Enables writing to the registers related to DDRSS axi1_ARESETn reset control. */
    BSP_MODULE_RESET_DDRSS_AXI1_ARESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT20_SHIFT),

    /** Enables writing to the registers related to DDRSS axi2_ARESETn reset control. */
    BSP_MODULE_RESET_DDRSS_AXI2_ARESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT21_SHIFT),

    /** Enables writing to the registers related to DDRSS axi3_ARESETn reset control. */
    BSP_MODULE_RESET_DDRSS_AXI3_ARESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT22_SHIFT),

    /** Enables writing to the registers related to DDRSS axi4_ARESETn reset control. */
    BSP_MODULE_RESET_DDRSS_AXI4_ARESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT23_SHIFT),

    /** Enables writing to the registers related to DDRSS MC_PRESETn reset control. */
    BSP_MODULE_RESET_DDRSS_MC_PRESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT24_SHIFT),

    /** Enables writing to the registers related to DDRSS PHY_PRESETn reset control. */
    BSP_MODULE_RESET_DDRSS_PHY_PRESETN = (BSP_RESET_MRCTLM_SELECT | BSP_RESET_MRCTL_BIT25_SHIFT),
} bsp_module_reset_t;

/** @} (end addtogroup BSP_MCU) */

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
