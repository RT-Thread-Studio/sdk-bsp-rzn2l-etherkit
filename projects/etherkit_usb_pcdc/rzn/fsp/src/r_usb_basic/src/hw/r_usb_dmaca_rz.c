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

/*******************************************************************************
 * File Name    : r_usb_dmaca_rx.c
 * Device(s)    : RX
 * Tool-Chain   : Renesas RXC Toolchain v2.05.00
 * OS           : not use
 * H/W Platform : not use
 * Description  : Functions for DMACA driver
 *******************************************************************************/

/*******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.06.2014 1.00    Initial release
 *         : 27.08.2014 1.01    Not change program due to updating application note only.
 *         : 30.01.2015 1.02    Added RX71M.
 *         : 13.04.2015 1.03    Added RX231 and RX230.
 *         : 30.09.2016 1.04    Added transfer_count and block_size to "case USB_DMACA_CMD_STATUS_GET"
 *         :                    of r_usb_dmaca_control().
 *******************************************************************************/

/*******************************************************************************
 * Includes   <System Includes>, "Project Includes"
 *******************************************************************************/

/* Definitions for DMACA support */
#include "../hw/inc/r_usb_dmaca_rz_if.h"
#include "../hw/inc/r_usb_dmaca_rz_private.h"
#include "../hw/inc/r_usb_bitdefine.h"
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
 #if USB_CFG_DMA == USB_CFG_ENABLE

/*******************************************************************************
 * Macro definitions
 *******************************************************************************/
  #define USB_VAL_0x60    (0x00000060U)

/*******************************************************************************
 * Typedef definitions
 *******************************************************************************/

/*******************************************************************************
 * Exported global variables (to be accessed by other files)
 *******************************************************************************/

/* This array holds callback functions. */
void * p_USB_DMACI_Handlers[USB_DMACA_NUM_CHANNELS];

/*******************************************************************************
 * Private variables and functions
 *******************************************************************************/
static bool r_usb_dmaca_set_transfer_data(uint8_t channel, usb_dmaca_transfer_data_cfg_t * p_cfg);

/*******************************************************************************
 * Function Name: r_usb_dmaca_create
 * Description  : Sets registers of the specified channel and the interrupt source.
 * Arguments    : channel -
 *                    Which channel to use
 *                *p_data_cfg -
 *                    Pointer to information of transfer data
 * Return Value : USB_DMACA_SUCCESS -
 *                    Successful operation
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *                USB_DMACA_ERR_INVALID_ARG -
 *                    Parameters are invalid.
 *                USB_DMACA_ERR_NULL_PTR -
 *                    Argument pointers are NULL.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_create (uint8_t channel, usb_dmaca_transfer_data_cfg_t * p_data_cfg)
{
  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_usb_dmaca_channel_valid_check(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }

    if ((uint32_t) NULL == (uint32_t) p_data_cfg)
    {
        return USB_DMACA_ERR_NULL_PTR;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    if (false == r_usb_dmaca_set_transfer_data(channel, p_data_cfg))
    {
        /* Fail to apply configurations for Transfer data. */
        return USB_DMACA_ERR_INVALID_ARG;
    }

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_control
 * Description  : Starts/Stops DMACA and checks an interrupt source as DMACA activation.
 * Arguments    : channel -
 *                    Which channel to use
 *                command -
 *                Control command
 *                *p_stat -
 *                    Pointer information of DMACA driver status
 *                    When a command is USB_DMACA_CMD_STATUS_GET, casts to void *.
 * Return Value : USB_DMACA_SUCCESS -
 *                    Successful operation
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *                USB_DMACA_ERR_INVALID_COMMAND -
 *                    Command is invalid.
 *                USB_DMACA_ERR_NULL_PTR -
 *                    Argument pointers are NULL.
 *                USB_DMACA_ERR_SOFTWARE_REQUESTED -
 *                    DMA transfer request by software is generated (DMREQ.SWREQ=1b) already.
 *                    So that cannot execute command.
 *                USB_DMACA_ERR_SOFTWARE_REQUEST_DISABLED -
 *                    Transfer Request Source is not Software (DMTMD.DCTG[1:0]=01b).
 *                    So that cannot execute command.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_control (uint8_t channel, usb_dmaca_command_t command, usb_dmaca_stat_t * p_stat)
{
    uint32_t dmaca_channel_status = 0x0000;

  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_usb_dmaca_channel_valid_check(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    switch (command)
    {
        case USB_DMACA_CMD_ENABLE:     /* Enable DMA transfer of the specified channel. */
        case USB_DMACA_CMD_RESUME:     /* Resume DMA transfer of the specified channel. */
        {
            USB_DMACA_CHCTRL(channel) = USB_DMACA_DMA_ENABLE;
            if (USB_DMACA_DMA_ENABLE != (USB_DMACA_CHSTAT(channel) & USB_DMACA_DMA_ENABLE))
            {
                /* do nothing */
            }

            break;
        }

        case USB_DMACA_CMD_DISABLE:    /* Disable DMA transfer of the specified channel. */
        {
            USB_DMACA_CHCTRL(channel) = USB_DMACA_DMA_DISABLE;
            if (USB_DMACA_DMA_ENABLE == (USB_DMACA_CHSTAT(channel) & USB_DMACA_DMA_ENABLE))
            {
                /* do nothing */
            }

            break;
        }

        case USB_DMACA_CMD_SOFT_REQ:   /* DMA transfer by Software using STG */
        {
            USB_DMACA_CHCTRL(channel) = USB_DMACA_SOFT_TRIG;

            if (USB_DMACA_STAT_RQST != (USB_DMACA_CHSTAT(channel) & USB_DMACA_STAT_RQST))
            {
                /* do nothing */
            }

            break;
        }

        case USB_DMACA_CMD_SOFT_REQ_CLR: /* Clear DMREQ.SWREQ bit. */
        {
            USB_DMACA_CHCTRL(channel) = USB_DMACA_CHSTAT_RQST_CLEAR;

            if (USB_DMACA_STAT_RQST == (USB_DMACA_CHSTAT(channel) & USB_DMACA_STAT_RQST))
            {
                /* do nothing */
            }

            break;
        }

        case USB_DMACA_CMD_STATUS_GET:
        {
  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)
            if ((uint32_t) NULL == (uint32_t) p_stat)
            {
                return USB_DMACA_ERR_NULL_PTR;
            }
  #endif                                                                /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

            dmaca_channel_status = USB_DMACA_CHSTAT(channel);

            if (0 == (dmaca_channel_status & USB_DMACA_CHSTAT_EN_MASK)) /* Get the current status of DMACA. */
            {
                /* DMACA operation is suspended. */
                p_stat->act_stat = false;
            }
            else
            {
                /* DMACA is operating. */
                p_stat->act_stat = true;
            }

            if (0 == (dmaca_channel_status & USB_DMACA_CHSTAT_END_MASK)) /* Get the transfer end interrupt flag. */
            {
                /* A transfer end interrupt has not been generated. */
                p_stat->end_stat = false;
            }
            else
            {
                /* A transfer end interrupt has been generated. */
                p_stat->end_stat = true;
            }

            if (0 == (dmaca_channel_status & USB_DMACA_CHSTAT_ER_MASK)) /* Get the error end interrupt flag. */
            {
                /* A transfer escape end interrupt has not been generated. */
                p_stat->er_stat = false;
            }
            else
            {
                /* A transfer escape end interrupt has been generated. */
                p_stat->er_stat = true;
            }

            p_stat->transfer_count = USB_DMACA_N0TB(channel);
            break;
        }

        case USB_DMACA_CMD_CH_STATUS_CLR:
        {
            USB_DMACA_CHCTRL(channel) = USB_DMACA_CHSTAT_CLEAR | USB_DMACA_CHSTAT_RQST_CLEAR |
                                        USB_DMACA_CHSTAT_END_CLEAR | USB_DMACA_CHSTAT_TC_CLEAR |
                                        USB_DMACA_CHSTAT_USBFDMA_CLEAR;
            if (0x00 ==
                (USB_DMACA_CHSTAT(channel) &
                 (USB_DMACA_CHSTAT_END_CLEAR | USB_DMACA_CHSTAT_TC_CLEAR | USB_DMACA_CHSTAT_USBFDMA_CLEAR)))
            {
                /* do nothing */
            }

            break;
        }

        case USB_DMACA_CMD_REG_SET_FIX:
        {
            USB_DMACA_CHCTRL(channel) &= (uint32_t) (~USB_DMACA_CHCTRL_CLEAR);
            if (0x00 == (USB_DMACA_CHCTRL(channel) & USB_DMACA_CHCTRL_CLEAR))
            {
                /* do nothing */
            }

            USB_DMACA_CHCFG(channel) &=
                (uint32_t) (~(USB_DMACA_CONFIG_WONLY_MASK | USB_DMACA_CONFIG_SBE_MASK | USB_DMACA_CONFIG_RSEL_MASK |
                              USB_DMACA_CONFIG_RSW_MASK | USB_DMACA_CONFIG_REN_MASK | USB_DMACA_CONFIG_DMS_MASK));
            if (0x00UL ==
                (USB_DMACA_CHCFG(channel) &
                 (uint32_t) ((USB_DMACA_CONFIG_WONLY_MASK | USB_DMACA_CONFIG_SBE_MASK | USB_DMACA_CONFIG_RSEL_MASK |
                              USB_DMACA_CONFIG_RSW_MASK | USB_DMACA_CONFIG_REN_MASK | USB_DMACA_CONFIG_DMS_MASK))))
            {
                /* do nothing */
            }

            USB_DMACA_DCTRL |= USB_DMACA_INT_DETECT_LEVEL;
            break;
        }

        default:
        {
            return USB_DMACA_ERR_INVALID_COMMAND;
            break;
        }
    }

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_init
 * Description  : Initializes a callback function array for DMACmI interrupt.
 *                                                          (m = 0 to 3, or 74)
 * Arguments    : None
 * Return Value : None
 *******************************************************************************/
void r_usb_dmaca_init (void)
{
    uint32_t i;

    for (i = 0; i < USB_DMACA_NUM_CHANNELS; i++)
    {
        p_USB_DMACI_Handlers[i] = NULL;
    }
}

/********************************************************************************
 * Function Name: r_usb_dmaca_int_callback
 * Description  : Registers a callback function for DMACmI interrupts.
 *                                                     (m = 0 to 3, or 74)
 *                If 'FIT_NO_FUNC' or 'NULL' is specified for argument *p_callback then
 *               any previously registered callback is unregistered.
 * Arguments    : channel -
 *                    Which channel to use
 *                *p_callback -
 *                    Pointer to function to call when DMACmI interrupt occurs.
 * Return Value : USB_DMACA_SUCCESS -
 *                    Callback function is registered.
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *                USB_DMACA_ERR_INVALID_HANDLER_ADDR -
 *                    Invalid function address is set and any previous function
 *                    has been unregistered.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_int_callback (uint8_t channel, void * p_callback)
{
  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_usb_dmaca_channel_valid_check(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }

    /* Check for valid address. */
    if ((uint32_t) NULL == (uint32_t) p_callback)
    {
        p_USB_DMACI_Handlers[channel] = NULL;

        return USB_DMACA_ERR_INVALID_HANDLER_ADDR;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    p_USB_DMACI_Handlers[channel] = p_callback;

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_int_enable
 * Description  : Enables DMACmI interrupt. (m = 0 to 3, or 74)
 * Arguments    : channel -
 *                    Which channel to use
 *              : priority -
 *                    DMACmI interrupt priority level
 * Return Value : USB_DMACA_SUCCESS -
 *                    Successful operation
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_int_enable (uint8_t channel, uint8_t priority)
{
  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_usb_dmaca_channel_valid_check(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Set the DMACmI priority level. */
    /* Set the DMACmI interrupt Enable bit. */
    if (USB_DMACA_SUCCESS != r_usb_dmaca_int_enable_set(channel, priority))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_int_disable
 * Description  : Disables DMACmI interrupt. (m = 0 to 3, or 74)
 * Arguments    : channel -
 *                    Which channel to use
 * Return Value : USB_DMACA_SUCCESS -
 *                    Successful operation
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_int_disable (uint8_t channel)
{
  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_usb_dmaca_channel_valid_check(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Clear the DMACmI interrupt Enable bit. */
    /* Clear the DMACmI priority level. */
    if (USB_DMACA_SUCCESS != r_usb_dmaca_int_disable_set(channel))
    {
        return USB_DMACA_ERR_INVALID_CH;
    }

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_getversion
 * Description  : Returns the version of this module. The version number is
 *                encoded such that the top two bytes are the major version
 *                number and the bottom two bytes are the minor version number.
 * Arguments    : none
 * Return Value : version number
 *******************************************************************************/
uint32_t r_usb_dmaca_getversion (void)
{
    uint32_t version = 0;

    version = (USB_DMACA_VERSION_MAJOR << 16) | USB_DMACA_VERSION_MINOR;

    return version;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_set_transfer_data
 * Description  : Applies configurations to Transfer data area.
 *                It is an internal function called by r_usb_dmaca_create().
 *                and all arguments are validated in r_usb_dmaca_create().
 * Arguments    : channel -
 *                    Which channel to use
 *                *p_data_cfg -
 *                    Pointer to information of Transfer data
 * Return Value : true -
 *                    Apply configurations for Transfer data successfully.
 *                false -
 *                    Fail to apply configurations for Transfer data.
 *******************************************************************************/
static bool r_usb_dmaca_set_transfer_data (uint8_t channel, usb_dmaca_transfer_data_cfg_t * p_cfg)
{
    void * p_src_pa_adr;
    void * p_des_pa_adr;

  #if (1 == USB_DMACA_CFG_PARAM_CHECKING_ENABLE)
    if (USB_DMACA_CONFIG_SRC_SIZE_LWORD < (p_cfg->src_size & USB_DMACA_INVALID_SRC_SIZE_MASK))
    {
        return false;
    }

    if (USB_DMACA_CONFIG_DES_SIZE_LWORD < (p_cfg->des_size & USB_DMACA_INVALID_DES_SIZE_MASK))
    {
        return false;
    }
  #endif                               /* USB_DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Clear CHCTRL_n register. CLRRQ bit  */
    USB_DMACA_CHCTRL(channel) = USB_DMACA_STAT_RQST_CLEAR;

/* R_USBF->CHa[channel].CHCTRL = USB_DMACA_STAT_RQST_CLEAR; */

    /* Disable DMA transfers. */
    USB_DMACA_CHCTRL(channel) = USB_DMACA_DMA_DISABLE;

/* USB_DMACA_CHCTRL(channel) = USB_DMACA_DMA_ENABLE; */

    USB_DMACA_CHCFG(channel) |= USB_VAL_0x60;               /* debug *//* b'6-5 : 1 fix */

    /* CHCFG Register. SEL bit. Select D0FIFO0 or D1FIFO */
    USB_DMACA_CHCFG(channel) &= (uint32_t) (~USB_DMACA_CHCFG_SEL_MASK);
    USB_DMACA_CHCFG(channel) |= (uint32_t) p_cfg->use_fifo; /* SEL_D1FIFO */

    /* CHCFG Register. SAD/DAD bit. */
    USB_DMACA_CHCFG(channel) &=
        (uint32_t) (~(USB_DMACA_CHCFG_SAD_MASK | USB_DMACA_CHCFG_DAD_MASK | USB_DMACA_CHCFG_REQD_MASK));
    USB_DMACA_CHCFG(channel) |=
        ((uint32_t) p_cfg->src_adr_dir | (uint32_t) p_cfg->des_adr_dir | (uint32_t) p_cfg->req_dir);

    /* CHCFG Register. SDS/DDS bit. */
    USB_DMACA_CHCFG(channel) &= (uint32_t) (~(USB_DMACA_CHCFG_SDS_MASK | USB_DMACA_CHCFG_DDS_MASK));
    USB_DMACA_CHCFG(channel) |= ((uint32_t) p_cfg->src_size | (uint32_t) p_cfg->des_size);

    /* Set N0SA_0 register. */
    if (0 == p_cfg->p_src_addr)
    {
        p_src_pa_adr = 0;
    }
    else
    {
        p_src_pa_adr = p_cfg->p_src_addr;
    }

    /*USB_DMACA_N0SA(channel) = (uint32_t)p_cfg->p_src_addr;*/
    USB_DMACA_NOSA(channel) = (uint32_t) p_src_pa_adr;

/* R_USBF->CHa[channel].N[0].SA_b.SAWD = (uint32_t)p_src_pa_adr; */

    /* Set N0DA_0 register. */
    if (0 == p_cfg->p_des_addr)
    {
        p_des_pa_adr = 0;
    }
    else
    {
        p_des_pa_adr = p_cfg->p_des_addr;
    }

    /*USB_DMACA_N0DA(channel) = (uint32_t)p_cfg->p_des_addr;*/
    USB_DMACA_N0DA(channel) = (uint32_t) p_des_pa_adr;

    /* R_USBF->CHa[channel].N[0].SA_b.SAWD = (uint32_t)p_src_pa_adr; */

    /* Set N0TB_0 register. */
    USB_DMACA_N0TB(channel) = p_cfg->transfer_count;

/* R_USBF->CHa[channel].N[0].TB_b.TB = (uint32_t)p_cfg->transfer_count; */
    USB_DMACA_CHCFG(channel) &= (uint32_t) (~(USB_DMACA_CHCFG_DEM_MASK | USB_DMACA_CHCFG_TCM_MASK));
    USB_DMACA_CHCFG(channel) |= ((uint32_t) p_cfg->dmaint_msk | (uint32_t) p_cfg->dmatc_msk);

    if (((uint32_t) p_cfg->dmaint_msk | (uint32_t) p_cfg->dmatc_msk) ==
        (USB_DMACA_CHCFG(channel) & (USB_DMACA_CHCFG_DEM_MASK | USB_DMACA_CHCFG_TCM_MASK)))
    {
        /* do nothing */
    }

    return true;
}

 #endif
#endif                                 /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */
/* End of File */
