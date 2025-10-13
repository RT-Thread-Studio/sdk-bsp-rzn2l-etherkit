/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_CACHE_H
#define BSP_CACHE_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#if defined(BSP_CFG_CORE_CR52)
 #include "cr/bsp_cache_core.h"
#elif defined(BSP_CFG_CORE_CA55)
 #include "ca/bsp_cache_core.h"
#endif

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

void R_BSP_CacheEnableInst(void);
void R_BSP_CacheEnableData(void);
void R_BSP_CacheEnableMemoryProtect(void);
void R_BSP_CacheDisableInst(void);
void R_BSP_CacheDisableData(void);
void R_BSP_CacheDisableMemoryProtect(void);
void R_BSP_CacheCleanAll(void);
void R_BSP_CacheInvalidateAll(void);
void R_BSP_CacheCleanInvalidateAll(void);
void R_BSP_CacheCleanRange(uintptr_t base_address, uintptr_t length);
void R_BSP_CacheInvalidateRange(uintptr_t base_address, uintptr_t length);
void R_BSP_CacheCleanInvalidateRange(uintptr_t base_address, uintptr_t length);
void R_BSP_CacheL3PowerCtrl(void);

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
