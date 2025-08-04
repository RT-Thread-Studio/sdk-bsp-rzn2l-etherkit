/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "bsp_address_expander.h"

#if (1 == BSP_FEATURE_ADDRESS_EXPANDER_SUPPORTED)
 #if (1 == _RZN_ORDINAL)

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Private definition to set address expander values. */
  #define BSP_PRV_ADX_DDRMIR1_BITS     (24)
  #define BSP_PRV_ADX_DDRMIR0_BITS     (16)
  #define BSP_PRV_ADX_PCIE1MIR_BITS    (8)
  #define BSP_PRV_ADX_PCIE0MIR_BITS    (0)

  #define BSP_PRV_STARTUP_ADX(index)                                               \
    (BSP_CFG_ADX ## index ## _DDR_MIRROR1_ADDRESS << BSP_PRV_ADX_DDRMIR1_BITS) |   \
    (BSP_CFG_ADX ## index ## _DDR_MIRROR0_ADDRESS << BSP_PRV_ADX_DDRMIR0_BITS) |   \
    (BSP_CFG_ADX ## index ## _PCIE1_MIRROR_ADDRESS << BSP_PRV_ADX_PCIE1MIR_BITS) | \
    (BSP_CFG_ADX ## index ## _PCIE0_MIRROR_ADDRESS << BSP_PRV_ADX_PCIE0MIR_BITS)

  #define BSP_PRV_STARTUP_ADX_NO_PCIE(index)                                     \
    (BSP_CFG_ADX ## index ## _DDR_MIRROR1_ADDRESS << BSP_PRV_ADX_DDRMIR1_BITS) | \
    (BSP_CFG_ADX ## index ## _DDR_MIRROR0_ADDRESS << BSP_PRV_ADX_DDRMIR0_BITS)

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
 * Configure Address Expander.
 * @param[in]    None
 * @param[out]   None
 * @note         This function supports configurable the registers of Address Expander.
 * @retval       None
 **********************************************************************************************************************/
void bsp_address_expander_init (void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_SYSTEM);

    R_ADXC0->ADXCTL0  = BSP_PRV_STARTUP_ADX(0);
    R_ADXC0->ADXCTL1  = BSP_PRV_STARTUP_ADX(1);
    R_ADXC0->ADXCTL2  = BSP_PRV_STARTUP_ADX(2);
    R_ADXC0->ADXCTL3  = BSP_PRV_STARTUP_ADX(3);
    R_ADXC0->ADXCTL4  = BSP_PRV_STARTUP_ADX(4);
    R_ADXC0->ADXCTL5  = BSP_PRV_STARTUP_ADX_NO_PCIE(5);
    R_ADXC1->ADXCTL6  = BSP_PRV_STARTUP_ADX_NO_PCIE(6);
    R_ADXC1->ADXCTL7  = BSP_PRV_STARTUP_ADX_NO_PCIE(7);
    R_ADXC1->ADXCTL8  = BSP_PRV_STARTUP_ADX_NO_PCIE(8);
    R_ADXC1->ADXCTL9  = BSP_PRV_STARTUP_ADX_NO_PCIE(9);
    R_ADXC1->ADXCTL10 = BSP_PRV_STARTUP_ADX_NO_PCIE(10);
    R_ADXC1->ADXCTL11 = BSP_PRV_STARTUP_ADX_NO_PCIE(11);
    R_ADXC1->ADXCTL12 = BSP_PRV_STARTUP_ADX_NO_PCIE(12);

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_SYSTEM);
}

 #endif
#endif
