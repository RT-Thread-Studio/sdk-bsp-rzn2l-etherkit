/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_TZC400_H
#define BSP_TZC400_H

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if (1 == BSP_FEATURE_TZC400_SUPPORTED)
 #define BSP_TZC400_GATEKEEPER_FILTER_0    (1 << 0)
 #define BSP_TZC400_GATEKEEPER_FILTER_1    (1 << 1)
 #define BSP_TZC400_GATEKEEPER_FILTER_2    (1 << 2)
 #define BSP_TZC400_GATEKEEPER_FILTER_3    (1 << 3)
 #define BSP_TZC400_ATTR_FILTER_0          (1 << 0)
 #define BSP_TZC400_ATTR_FILTER_1          (1 << 1)
 #define BSP_TZC400_ATTR_FILTER_2          (1 << 2)
 #define BSP_TZC400_ATTR_FILTER_3          (1 << 3)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/** @} (end addtogroup BSP_MCU) */

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/* Public functions defined in bsp.h */
#if BSP_FEATURE_TZC400_SUPPORTED
 #if (1 == _RZN_ORDINAL)

void bsp_tzc_400_cfg(void);            // Used internally by BSP

 #endif
#endif

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
