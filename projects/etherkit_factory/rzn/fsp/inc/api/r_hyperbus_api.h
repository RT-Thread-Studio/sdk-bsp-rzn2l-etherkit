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
 * @ingroup RENESAS_STORAGE_INTERFACES
 * @defgroup HYPERBUS_API HyperBus Interface
 * @brief Interface for accessing memory device via HyperBus.
 *
 * @section HYPERBUS_API_SUMMARY Summary
 * The HyperBus API provides an interface that configures, writes, and erases sectors in HyperBus memory devices.
 *
 * @{
 **********************************************************************************************************************/

#ifndef R_HYPERBUS_API_H
#define R_HYPERBUS_API_H

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
#define HYPERBUS_ERASE_SIZE_CHIP_ERASE    (UINT32_MAX)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Select the type of burst. */
typedef enum e_hyperbus_burst_type
{
    HYPERBUS_BURST_TYPE_WRAP   = 0x0,  ///< Wrap burst
    HYPERBUS_BURST_TYPE_LINEAR = 0x1,  ///< Linear burst
} hyperbus_burst_type_t;

/** Select access address space. */
typedef enum e_hyperbus_space_select
{
    HYPERBUS_SPACE_SELECT_MEMORY_SPACE   = 0x0, ///< Access for memory space
    HYPERBUS_SPACE_SELECT_REGISTER_SPACE = 0x1, ///< Access for register space
} hyperbus_space_select_t;

/** Initial read/write latency. */
typedef enum e_hyperbus_latency_count
{
    HYPERBUS_LATENCY_COUNT_0 = 0,      ///< No count
    HYPERBUS_LATENCY_COUNT_1,          ///< Latency count 1
    HYPERBUS_LATENCY_COUNT_2,          ///< Latency count 2
    HYPERBUS_LATENCY_COUNT_3,          ///< Latency count 3
    HYPERBUS_LATENCY_COUNT_4,          ///< Latency count 4
    HYPERBUS_LATENCY_COUNT_5,          ///< Latency count 5
    HYPERBUS_LATENCY_COUNT_6,          ///< Latency count 6
    HYPERBUS_LATENCY_COUNT_7,          ///< Latency count 7
    HYPERBUS_LATENCY_COUNT_8,          ///< Latency count 8
    HYPERBUS_LATENCY_COUNT_9,          ///< Latency count 9
    HYPERBUS_LATENCY_COUNT_10,         ///< Latency count 10
    HYPERBUS_LATENCY_COUNT_11,         ///< Latency count 11
    HYPERBUS_LATENCY_COUNT_12,         ///< Latency count 12
    HYPERBUS_LATENCY_COUNT_13,         ///< Latency count 13
    HYPERBUS_LATENCY_COUNT_14,         ///< Latency count 14
    HYPERBUS_LATENCY_COUNT_15,         ///< Latency count 15
    HYPERBUS_LATENCY_COUNT_16,         ///< Latency count 16
    HYPERBUS_LATENCY_COUNT_17,         ///< Latency count 17
    HYPERBUS_LATENCY_COUNT_18,         ///< Latency count 18
    HYPERBUS_LATENCY_COUNT_19,         ///< Latency count 19
    HYPERBUS_LATENCY_COUNT_20,         ///< Latency count 20
    HYPERBUS_LATENCY_COUNT_21,         ///< Latency count 21
    HYPERBUS_LATENCY_COUNT_22,         ///< Latency count 22
    HYPERBUS_LATENCY_COUNT_23,         ///< Latency count 23
    HYPERBUS_LATENCY_COUNT_24,         ///< Latency count 24
    HYPERBUS_LATENCY_COUNT_25,         ///< Latency count 25
    HYPERBUS_LATENCY_COUNT_26,         ///< Latency count 26
    HYPERBUS_LATENCY_COUNT_27,         ///< Latency count 27
    HYPERBUS_LATENCY_COUNT_28,         ///< Latency count 28
    HYPERBUS_LATENCY_COUNT_29,         ///< Latency count 29
    HYPERBUS_LATENCY_COUNT_30,         ///< Latency count 30
    HYPERBUS_LATENCY_COUNT_31,         ///< Latency count 31
} hyperbus_latency_count_t;

/** Status */
typedef struct st_hyperbus_status
{
    /** Whether or not a write or erase is in progress. */
    bool device_busy;
} hyperbus_status_t;

/** User configuration structure used by the open function */
typedef struct st_hyperbus_cfg
{
    hyperbus_burst_type_t    burst_type;                   ///< Burst type in HyperBus protocol
    hyperbus_space_select_t  access_space;                 ///< Access space in HyperBus protocol
    hyperbus_latency_count_t read_latency_count;           ///< Latency count in reading
    hyperbus_latency_count_t memory_write_latency_count;   ///< Latency count in memory writing
    hyperbus_latency_count_t register_write_latency_count; ///< Latency count in register writing
    uint32_t                 sector_erase_size;            ///< Size of erase for sector erase
    void const             * p_extend;                     ///< Pointer to implementation specific extended configurations
} hyperbus_cfg_t;

/** HyperBus control block. Allocate an instance specific control block to pass into the HyperBus API calls. */
typedef void hyperbus_ctrl_t;

/** HyperBus implementations follow this API. */
typedef struct st_hyperbus_api
{
    /** Open the HyperBus driver module.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[in] p_cfg                Pointer to a configuration structure
     **/
    fsp_err_t (* open)(hyperbus_ctrl_t * p_ctrl, hyperbus_cfg_t const * const p_cfg);

    /** Set Burst type.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[in] burst_type           Burst type
     **/
    fsp_err_t (* burstTypeSet)(hyperbus_ctrl_t * p_ctrl, hyperbus_burst_type_t burst_type);

    /** Set Access space.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[in] access_space         Access space
     **/
    fsp_err_t (* accessSpaceSet)(hyperbus_ctrl_t * p_ctrl, hyperbus_space_select_t access_space);

    /** Program any number of bytes of data within a boundary into the flash.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[in] p_src                The memory address of the data to write to the flash device
     * @param[in] p_dest               The location in the flash device address space to write the data to. Refer to the accessible address in the data sheet of the connected device and set it within this range.
     * @param[in] byte_count           The number of bytes to write. Refer to the write data size and boundaries in the data sheet of the connected device and specify within a range that does not exceed these.
     **/
    fsp_err_t (* write)(hyperbus_ctrl_t * p_ctrl, uint8_t const * const p_src, uint8_t * const p_dest,
                        uint32_t byte_count);

    /** Erase a certain number of bytes of the flash.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[in] p_device_address     The location in the flash device address space to start the erase from
     * @param[in] byte_count           The number of bytes to erase. Set to HYPERBUS_ERASE_SIZE_CHIP_ERASE to erase entire
     *                                 chip.
     **/
    fsp_err_t (* erase)(hyperbus_ctrl_t * p_ctrl, uint8_t * const p_device_address, uint32_t byte_count);

    /** Get the write or erase status of the flash.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     * @param[out] p_status            Current status of the HyperBus flash device stored here.
     **/
    fsp_err_t (* statusGet)(hyperbus_ctrl_t * ctrl_t, hyperbus_status_t * const p_status);

    /** AutoCalibrate the HyperBus driver module. Expected to be used when auto-calibrating HyperRAM device.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     **/
    fsp_err_t (* autoCalibrate)(hyperbus_ctrl_t * p_ctrl);

    /** Close the HyperBus driver module.
     *
     * @param[in] p_ctrl               Pointer to a driver handle
     **/
    fsp_err_t (* close)(hyperbus_ctrl_t * ctrl_t);
} hyperbus_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_hyperbus_instance
{
    hyperbus_ctrl_t      * p_ctrl;     ///< Pointer to the control structure for this instance
    hyperbus_cfg_t const * p_cfg;      ///< Pointer to the configuration structure for this instance
    hyperbus_api_t const * p_api;      ///< Pointer to the API structure for this instance
} hyperbus_instance_t;

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif

/*******************************************************************************************************************//**
 * @} (end defgroup HYPERBUS_API)
 **********************************************************************************************************************/
