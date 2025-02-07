/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_NETWORKING_INTERFACES
 * @defgroup ETHER_SWITCH_API Ethernet Switch Interface
 * @brief Interface for Ethernet Switch functions.
 *
 * @section ETHER_SWITCH_API_Summary Summary
 * The Ether Switch module provides an API for ethernet switch peripheral.
 * And the general ethernet switch peripheral have forwarding functionality.
 *
 * @{
 **********************************************************************************************************************/

#ifndef R_ETHER_SWITCH_API_H
#define R_ETHER_SWITCH_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Register definitions, common services and error codes. */
#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

#ifndef BSP_OVERRIDE_ETHER_SWITCH_CALLBACK_ARGS_T

/** Callback function parameter data */
typedef struct st_ether_switch_callback_args
{
    uint32_t channel;                  ///< Device channel number

    void const * p_context;            ///< Placeholder for user data.  Set in @ref ether_switch_api_t::open function in @ref ether_switch_cfg_t.
} ether_switch_callback_args_t;
#endif

/** Control block.  Allocate an instance specific control block to pass into the API calls.
 */
typedef void ether_switch_ctrl_t;

/** Configuration parameters. */
typedef struct st_ether_switch_cfg
{
    uint8_t channel;                                            ///< Channel

    IRQn_Type irq;                                              ///< MCU interrupt number
    uint8_t   ipl;                                              ///< MCU interrupt priority

    void (* p_callback)(ether_switch_callback_args_t * p_args); ///< Callback provided when an ISR occurs.

    /** Placeholder for user data.  Passed to the user callback in ether_switch_callback_args_t. */
    void const * p_context;
    void const * p_extend;                                      ///< Placeholder for user extension.
} ether_switch_cfg_t;

/** Functions implemented at the HAL layer will follow this API. */
typedef struct st_ether_switch_api
{
    /** Open driver.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to pin configuration structure.
     */
    fsp_err_t (* open)(ether_switch_ctrl_t * const p_ctrl, ether_switch_cfg_t const * const p_cfg);

    /** Close driver.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(ether_switch_ctrl_t * const p_ctrl);
} ether_switch_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_ether_switch_instance
{
    ether_switch_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    ether_switch_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    ether_switch_api_t const * p_api;  ///< Pointer to the API structure for this instance
} ether_switch_instance_t;

/*******************************************************************************************************************//**
 * @} (end defgroup ETHER_SWITCH_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* R_ETHER_SWITCH_API_H */
