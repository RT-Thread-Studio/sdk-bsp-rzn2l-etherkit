/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_NETWORKING_INTERFACES
 * @defgroup ETHER_SELECTOR_API Ethernet Selector Interface
 * @brief Interface for Ethernet Selector functions.
 *
 * @section ETHER_SELECTOR_API_Summary Summary
 * The Ethernet Selector module (r_ether_selector) provides an API for standard Ethernet Selector communications applications that use
 * the Ethernet Subsystem peripheral.
 *
 * The Ethernet Selector interface supports the following features:
 * - Ethernet port select support
 * - Selector control support
 * - PHY link signal polarity control support
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef R_ETHER_SELECTOR_API_H
#define R_ETHER_SELECTOR_API_H

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

/** PHY link signal polarity */
typedef enum e_ether_selector_phylink_polarity
{
    ETHER_SELECTOR_PHYLINK_POLARITY_LOW  = 0, ///< Active-Low link signal
    ETHER_SELECTOR_PHYLINK_POLARITY_HIGH = 1, ///< Active-High link signal
} ether_selector_phylink_polarity_t;

/** Selector mode */
typedef enum e_ether_selector_interface
{
    ETHER_SELECTOR_INTERFACE_MII   = 0, ///< MII
    ETHER_SELECTOR_INTERFACE_RMII  = 1, ///< RMII
    ETHER_SELECTOR_INTERFACE_GMII  = 2, ///< GMII
    ETHER_SELECTOR_INTERFACE_RGMII = 3, ///< RGMII
} ether_selector_interface_t;

/** Selector speed */
typedef enum e_ether_selector_speed
{
    ETHER_SELECTOR_SPEED_10_MBPS   = 0, ///< 10Mbit/s
    ETHER_SELECTOR_SPEED_100_MBPS  = 1, ///< 100Mbit/s
    ETHER_SELECTOR_SPEED_1000_MBPS = 2, ///< 1000Mbit/s
} ether_selector_speed_t;

/** Selector duplex */
typedef enum e_ether_selector_duplex
{
    ETHER_SELECTOR_DUPLEX_HALF = 0,    ///< Half-Duplex
    ETHER_SELECTOR_DUPLEX_FULL = 1,    ///< Full-Duplex
} ether_selector_duplex_t;

/** Selector reference clock */
typedef enum e_ether_selector_ref_clock
{
    ETHER_SELECTOR_REF_CLOCK_INPUT  = 0, ///< REF_CLK input
    ETHER_SELECTOR_REF_CLOCK_OUTPUT = 1, ///< REF_CLK output
} ether_selector_ref_clock_t;

/** Control block.  Allocate an instance specific control block to pass into the API calls. */
typedef void ether_selector_ctrl_t;

/** Configuration parameters. */
typedef struct st_ether_selector_cfg
{
    uint8_t channel;                           ///< Channel number
    ether_selector_phylink_polarity_t phylink; ///< PHY link signal polarity

    ether_selector_interface_t interface;      ///< Interface mode
    ether_selector_speed_t     speed;          ///< Interface speed
    ether_selector_duplex_t    duplex;         ///< Interface duplex
    ether_selector_ref_clock_t ref_clock;      ///< Interface REF_CLK
    void const               * p_extend;       ///< Placeholder for user extension.
} ether_selector_cfg_t;

/** Functions implemented at the HAL layer will follow this API. */
typedef struct st_ether_selector_api
{
    /** Open driver. Set Ethernet mode such as Ethernet MAC or Ethernet Switch or EtherCAT for each port.
     *
     * @param[in]  p_api_ctrl       Pointer to control structure.
     * @param[in]  p_cfg            Pointer to selector configuration structure.
     */
    fsp_err_t (* open)(ether_selector_ctrl_t * const p_api_ctrl, ether_selector_cfg_t const * const p_cfg);

    /** Close driver.
     *
     * @param[in]  p_api_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(ether_selector_ctrl_t * const p_api_ctrl);

    /** Set the Converter speed and duplex in runtime.
     *
     * @param[in]  p_api_ctrl       Pointer to control structure.
     * @param[in]  speed            Converter speed.
     * @param[in]  duplex           Converter duplex mode.
     */
    fsp_err_t (* converterSet)(ether_selector_ctrl_t * const p_api_ctrl, ether_selector_speed_t speed,
                               ether_selector_duplex_t duplex);
} ether_selector_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_ether_selector_instance
{
    ether_selector_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    ether_selector_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    ether_selector_api_t const * p_api;  ///< Pointer to the API structure for this instance
} ether_selector_instance_t;

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHER_SELECTOR_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* R_ETHER_SELECTOR_API_H */
