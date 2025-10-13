/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_SEMAPHORE_H
#define BSP_SEMAPHORE_H

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

/** The semaphore resource state shared by CPU0 and CPU1 */
typedef enum e_bsp_resource_state
{
    BSP_RESOURCE_STATE_BEING_USED     = 0, ///< Semaphore resource being used.
    BSP_RESOURCE_STATE_NOT_BEING_USED = 1, ///< Semaphore resource not being used.
} bsp_resource_state_t;

/** The semaphore resource number shared by CPU0 and CPU1 */
typedef enum e_bsp_resource_num
{
    BSP_RESOURCE_NUM_0 = 0,            ///< Semaphore resource number 0
    BSP_RESOURCE_NUM_1 = 1,            ///< Semaphore resource number 1
    BSP_RESOURCE_NUM_2 = 2,            ///< Semaphore resource number 2
    BSP_RESOURCE_NUM_3 = 3,            ///< Semaphore resource number 3
    BSP_RESOURCE_NUM_4 = 4,            ///< Semaphore resource number 4
    BSP_RESOURCE_NUM_5 = 5,            ///< Semaphore resource number 5
    BSP_RESOURCE_NUM_6 = 6,            ///< Semaphore resource number 6
    BSP_RESOURCE_NUM_7 = 7,            ///< Semaphore resource number 7
} bsp_resource_num_t;

/** @} (end addtogroup BSP_MCU) */

#if BSP_FEATURE_BSP_SEMAPHORE_SUPPORTED

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Read semaphore resource status. When SYTSEMFEN = 1, reading this status clear the resource status.
 *
 * @param[in]  sem_num Semaphore number to read resource status.
 *
 * @retval     Resource status.
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t R_BSP_SemaphoreStateRead (bsp_resource_num_t sem_num)
{
 #if BSP_FEATURE_SEM_SUPPORTED
    uint32_t sem = R_SEM->SYTSEMF[sem_num];
 #elif BSP_FEATURE_MAILBOX_SEM_SUPPORTED
    uint32_t sem = R_MBXSEM->SEM[sem_num];
 #else
    uint32_t sem = 0;
 #endif

    return sem;
}

#endif

/* Public functions defined in bsp.h */
void bsp_semaphore_init(void);         // Used internally by BSP

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
