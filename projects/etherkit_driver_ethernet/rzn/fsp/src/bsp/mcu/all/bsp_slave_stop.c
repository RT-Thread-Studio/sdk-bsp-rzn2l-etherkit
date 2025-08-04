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
#define BSP_SLAVE_STOP_SSTPCR_OFFSET_MASK    (0xFFFFU << 16U)
#define BSP_SLAVE_STOP_REQ_POS_MASK          (0xFFU << 8U)
#define BSP_SLAVE_STOP_ACK_POS_MASK          (0xFFU)

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
 * @addtogroup BSP_MCU
 *
 * @{
 **********************************************************************************************************************/
#if BSP_FEATURE_BSP_SLAVE_STOP_SUPPORTED

/*******************************************************************************************************************//**
 * Change the bus slaves from bus stop to normal operation in order to access to the AXI-type slave I/F safely
 *
 * @param[in] bus_slave to be released from bus stop state.
 **********************************************************************************************************************/
void R_BSP_SlaveStopRelease (bsp_bus_slave_t bus_slave)
{
    uint32_t            req_pos;
    uint32_t            req_bit_msk;
    uint32_t            ack_pos;
    uint32_t            ack_bit_msk;
    volatile uint32_t * p_reg;

    p_reg       = (uint32_t *) &R_SSC->SSTPCR4 + ((bus_slave & BSP_SLAVE_STOP_SSTPCR_OFFSET_MASK) >> 16U);
    req_pos     = (bus_slave & BSP_SLAVE_STOP_REQ_POS_MASK) >> 8U;
    req_bit_msk = 1U << req_pos;
    *p_reg     &= ~req_bit_msk;

    ack_pos     = (bus_slave & BSP_SLAVE_STOP_ACK_POS_MASK);
    ack_bit_msk = 1U << ack_pos;

    FSP_HARDWARE_REGISTER_WAIT((*p_reg & ack_bit_msk) >> ack_pos, 0);
}

/*******************************************************************************************************************//**
 * Stop the access to the bus slaves
 *
 * @param[in] bus_slave to be stoped
 **********************************************************************************************************************/
void R_BSP_SlaveStop (bsp_bus_slave_t bus_slave)
{
    uint32_t            req_pos;
    uint32_t            req_bit_msk;
    uint32_t            ack_pos;
    uint32_t            ack_bit_msk;
    volatile uint32_t * p_reg;

    p_reg       = (uint32_t *) &R_SSC->SSTPCR4 + ((bus_slave & BSP_SLAVE_STOP_SSTPCR_OFFSET_MASK) >> 16U);
    req_pos     = (bus_slave & BSP_SLAVE_STOP_REQ_POS_MASK) >> 8U;
    req_bit_msk = 1U << req_pos;
    *p_reg     |= req_bit_msk;

    ack_pos     = (bus_slave & BSP_SLAVE_STOP_ACK_POS_MASK);
    ack_bit_msk = 1U << ack_pos;

    FSP_HARDWARE_REGISTER_WAIT((*p_reg & ack_bit_msk) >> ack_pos, 1);
}

#endif

/** @} (end addtogroup BSP_MCU) */
