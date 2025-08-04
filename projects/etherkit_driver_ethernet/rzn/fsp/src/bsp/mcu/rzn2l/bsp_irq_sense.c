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

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU_RZN2L
 * @{
 **********************************************************************************************************************/

/** Array of GICD_ICFGR initialization value. */
const uint32_t BSP_GICD_ICFGR_INIT[BSP_NON_SELECTABLE_ICFGR_MAX] =
{
    0xAAAAAAAAUL,                      /* Event No. 0 to 15    */
    0x000000AAUL,                      /* Event No. 16 to 31   */
    0x00000000UL,                      /* Event No. 32 to 47   */
    0xAAAAA800UL,                      /* Event No. 48 to 63   */
    0xAAAAAAAAUL,                      /* Event No. 64 to 79   */
    0xAAAAAAAAUL,                      /* Event No. 80 to 95   */
    0xAAAAAAAAUL,                      /* Event No. 96 to 111  */
    0xAAAAAA00UL,                      /* Event No. 112 to 127 */
    0xAAAAAAAAUL,                      /* Event No. 128 to 143 */
    0xAAAAAAAAUL,                      /* Event No. 144 to 159 */
    0xAAAAAAAAUL,                      /* Event No. 160 to 175 */
    0xAAAAAAAAUL,                      /* Event No. 176 to 191 */
    0xAAAAAAAAUL,                      /* Event No. 192 to 207 */
    0xAAAAAAAAUL,                      /* Event No. 208 to 223 */
    0xAAAAAAAAUL,                      /* Event No. 224 to 239 */
    0x0000000AUL,                      /* Event No. 240 to 255 */
    0xA8000000UL,                      /* Event No. 256 to 271 */
    0xA82A2AAAUL,                      /* Event No. 272 to 287 */
    0x28282828UL,                      /* Event No. 288 to 303 */
    0x00282828UL,                      /* Event No. 304 to 319 */
    0xA82A0A00UL,                      /* Event No. 320 to 335 */
    0xA0AA8020UL,                      /* Event No. 336 to 351 */
    0x00000002UL,                      /* Event No. 352 to 367 */
    0x00000000UL,                      /* Event No. 368 to 383 */
    0xAAAA0A0AUL,                      /* Event No. 384 to 399 */
    0xAAAAAAAAUL,                      /* Event No. 400 to 415 */
    0x00AAAAAAUL,                      /* Event No. 416 to 431 */
    0x828A0A2AUL,                      /* Event No. 432 to 447 */
};

const uint32_t BSP_GICR_SGI_PPI_ICFGR_INIT[BSP_EVENT_SGI_PPI_ARRAY_NUM] =
{
    0xAAAAAAAAUL,                      /* event SGI */
    0x00020000UL,                      /* event PPI */
};

/** @} (end addtogroup BSP_MCU_RZN2L) */

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
