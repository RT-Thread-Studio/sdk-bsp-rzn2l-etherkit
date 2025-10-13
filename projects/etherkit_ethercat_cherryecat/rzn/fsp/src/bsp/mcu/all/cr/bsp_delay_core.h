/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_DELAY_CORE_H
#define BSP_DELAY_CORE_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* The number of cycles required per software delay loop. */
#ifndef BSP_DELAY_LOOP_CYCLES
 #define BSP_DELAY_LOOP_CYCLES    (4)
#endif

/* Calculates the number of delay loops to pass to r_bsp_software_delay_loop to achieve at least the requested cycle
 * count delay. This is 1 loop longer than optimal if cycles is a multiple of BSP_DELAY_LOOP_CYCLES, but it ensures
 * the requested number of loops is at least 1 since r_bsp_software_delay_loop cannot be called with a loop count
 * of 0. */
#define BSP_DELAY_LOOPS_CALCULATE(cycles)    (((cycles) / BSP_DELAY_LOOP_CYCLES) + 1U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

BSP_ATTRIBUTE_STACKLESS void r_bsp_software_delay_loop(uint32_t loop_cnt);

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
