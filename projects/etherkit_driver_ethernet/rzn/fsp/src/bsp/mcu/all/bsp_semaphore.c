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

/* Private definition to set enumeration values. */
#define BSP_PRV_SYTSEMFEN_ENABLE      (0x00000001U)
#define BSP_PRV_RESOURCE_NOT_USED     (0x00000001U)
#define BSP_SEMAPHORE_RESOURCE_MAX    (8U)
#define BSP_PRV_SEMRCEN_ENABLE        (0xFFU)

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
 * Semaphore initialization.
 **********************************************************************************************************************/
void bsp_semaphore_init (void)
{
#if BSP_FEATURE_BSP_SEMAPHORE_SUPPORTED
 #if BSP_FEATURE_SEM_SUPPORTED
    uint32_t sem_num;

    /* Disable register protection for semaphore related registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_SYSTEM);

    /* Enable read clear function. */
    R_SEM->SYTSEMFEN = BSP_PRV_SYTSEMFEN_ENABLE;

    for (sem_num = 0; sem_num < BSP_SEMAPHORE_RESOURCE_MAX; sem_num++)
    {
        /* Set the semaphore state not being used. */
        R_SEM->SYTSEMF[sem_num] = BSP_PRV_RESOURCE_NOT_USED;
    }

    /* Enable register protection for semaphore related registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_SYSTEM);
 #endif

 #if BSP_FEATURE_MAILBOX_SEM_SUPPORTED
    uint32_t sem_num;

    /* Disable register protection for semaphore related registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_SYSTEM);

    /* Enable read clear function for External Host and Internal CPUs. */
    R_MBXSEM->SEMRCEN = BSP_PRV_SEMRCEN_ENABLE;

    /* Enable read clear function for Internal CPUs. */
    R_MBXSEM->SEMRCENAR = BSP_PRV_SEMRCEN_ENABLE;

    for (sem_num = 0; sem_num < BSP_SEMAPHORE_RESOURCE_MAX; sem_num++)
    {
        /* Set the semaphore state not being used for External Host and Internal CPUs. */
        R_MBXSEM->SEM[sem_num] = BSP_PRV_RESOURCE_NOT_USED;

        /* Set the semaphore state not being used for Internal CPUs. */
        R_MBXSEM->SEMAR[sem_num] = BSP_PRV_RESOURCE_NOT_USED;
    }

    /* Enable register protection for semaphore related registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_SYSTEM);
 #endif
#endif
}
