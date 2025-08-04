/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_REGISTER_PROTECTION_H
#define BSP_REGISTER_PROTECTION_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/** The different types of registers that can be protected. */
typedef enum e_bsp_reg_protect
{
    /** Enables writing to the registers related to the clock generation circuit. */
    BSP_REG_PROTECT_CGC = 0,

    /** Enables writing to the registers related to low power consumption and reset. */
    BSP_REG_PROTECT_LPC_RESET,

    /** Enables writing to the registers related to GPIO. */
    BSP_REG_PROTECT_GPIO,

    /** Enables writing to the registers related to Non-Safety reg. */
    BSP_REG_PROTECT_SYSTEM,
} bsp_reg_protect_t;

/** @} (end addtogroup BSP_MCU) */

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/* Public functions defined in bsp.h */
void bsp_register_protect_open(void);  // Used internally by BSP

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
