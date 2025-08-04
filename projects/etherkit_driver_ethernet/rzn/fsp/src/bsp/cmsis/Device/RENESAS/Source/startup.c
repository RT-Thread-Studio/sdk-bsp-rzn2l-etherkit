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

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
#if (1 == _RZN_ORDINAL)
extern void bsp_master_mpu_init(void);
extern void bsp_global_system_counter_init(void);

#endif

#if defined(BSP_CFG_CORE_CR52)
 #if (BSP_FEATURE_TFU_SUPPORTED & BSP_CFG_USE_TFU_MATHLIB)
extern void bsp_tfu_init(void);

 #endif
#endif

#if BSP_CFG_C_RUNTIME_INIT
extern void bsp_static_constructor_init(void);
extern void bsp_loader_data_init(void);
extern void bsp_loader_bss_init(void);

#endif

#if !BSP_CFG_PORT_PROTECT
extern void bsp_release_port_protect(void);

#endif

#if !(BSP_CFG_RAM_EXECUTION)

extern void bsp_copy_to_ram(void);

extern void bsp_application_bss_init(void);

#endif

extern void bsp_memory_protect_setting(void);
extern void bsp_irq_cfg_common(void);

extern void R_BSP_WarmStart(bsp_warm_start_event_t event);

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
int32_t main(void);

#if BSP_CFG_EARLY_INIT
static void bsp_init_uninitialized_vars(void);

#endif

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initialize the MCU and the runtime environment.
 **********************************************************************************************************************/
void SystemInit (void)
{
#if BSP_CFG_EARLY_INIT

    /* Initialize uninitialized BSP variables early for use in R_BSP_WarmStart. */
    bsp_init_uninitialized_vars();
#endif

    /* Call before initializing clock and variables. */
    R_BSP_WarmStart(BSP_WARM_START_RESET);

#if (1 == _RZN_ORDINAL)

    /* Configure system clocks. */
    bsp_clock_init();
#endif

    /* Call post clock initialization hook. */
    R_BSP_WarmStart(BSP_WARM_START_POST_CLOCK);

#if BSP_CFG_C_RUNTIME_INIT

    /* Copy the primary core loader data from external Flash to internal RAM. */
    bsp_loader_data_init();

    /* Clear loader bss section in internal RAM. */
    bsp_loader_bss_init();
#endif

#if BSP_FEATURE_ADDRESS_EXPANDER_SUPPORTED
 #if (1 == _RZN_ORDINAL)

    /* Initialize the Address Expander settings. */
    bsp_address_expander_init();
 #endif
#endif

#if BSP_FEATURE_TZC400_SUPPORTED
 #if (1 == _RZN_ORDINAL)

    /* Initialize the TZC-400 settings. */
    bsp_tzc_400_cfg();
 #endif
#endif

    /* Initialize SystemCoreClock variable. */
    SystemCoreClockUpdate();

    /* Set memory attributes, etc. */
    bsp_memory_protect_setting();

#if !(BSP_CFG_RAM_EXECUTION)

    /* Copy the application program from external Flash to internal RAM.
     * In the case of multi-core operation, copies each section (vector, loader(program/data), user(program/data)) of
     * the secondary core (or later). */
    bsp_copy_to_ram();

    /* Clear bss section in internal RAM. */
    bsp_application_bss_init();
#endif

#if BSP_CFG_C_RUNTIME_INIT

    /* Initialize static constructors */
    bsp_static_constructor_init();
#endif

#if !BSP_CFG_PORT_PROTECT

    /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
     * disable writes. */
    bsp_release_port_protect();
#endif

    /* Call Post C runtime initialization hook. */
    R_BSP_WarmStart(BSP_WARM_START_POST_C);

#if (1 == _RZN_ORDINAL)
 #if BSP_CFG_SEMAPHORE_ENABLE

    /* Initialize semaphores required for synchronization and exclusive control between CPUs. */
    bsp_semaphore_init();
 #endif

    /* Initialize the Master-MPU settings. */
    bsp_master_mpu_init();

    /* Initialize global system counter. The counter is enabled and is incrementing. */
    bsp_global_system_counter_init();
#endif

    /* GIC initialization */
    bsp_irq_cfg_common();

    /* Initialize GIC interrupts. */
    bsp_irq_cfg();

#if defined(BSP_CFG_CORE_CR52)
 #if (BSP_FEATURE_TFU_SUPPORTED & BSP_CFG_USE_TFU_MATHLIB)

    /* Initialize the TFU settings. */
    bsp_tfu_init();
 #endif
#endif

#if defined(__GNUC__)
    extern void entry(void);
    entry();
#elif defined(__ICCARM__)
    extern void __low_level_init(void);
    __low_level_init();
#else
    /* Jump to main. */
    main();
#endif
}

/** @} (end addtogroup BSP_MCU) */

#if BSP_CFG_EARLY_INIT

/*******************************************************************************************************************//**
 * Initialize BSP variables not handled by C runtime startup.
 **********************************************************************************************************************/
static void bsp_init_uninitialized_vars (void)
{
    g_protect_port_counter = 0;

    extern volatile uint16_t g_protect_counters[];
    for (uint32_t i = 0; i < 4; i++)
    {
        g_protect_counters[i] = 0;
    }

    SystemCoreClockUpdate();
}

#endif
