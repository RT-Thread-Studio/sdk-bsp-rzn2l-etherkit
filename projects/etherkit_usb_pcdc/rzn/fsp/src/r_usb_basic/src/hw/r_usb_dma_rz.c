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

/***********************************************************************************************************************
 * File Name    : r_usb_dma.c
 * Description  : Setting code of DMA
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.09.2016 1.00 First release
 *         : 26.01.2017 1.21 Support DMAC Technical Update for RX71M/RX64M USBA.
 *         : 30.09.2017 1.22    Change functions name
 *                              "usb_hstd_buf2fifo"->"usb_hstd_buf_to_fifo"
 *                              "usb_pstd_buf2fifo"->"usb_pstd_buf_to_fifo"
 *                              "usb_dma_buf2fifo_restart"->"usb_cstd_dma_send_restart"
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/

/*******************************************************************************
 * Includes   <System Includes>, "Project Includes"
 *******************************************************************************/
#include "../driver/inc/r_usb_typedef.h"
#include "../driver/inc/r_usb_extern.h"

#if  USB_IP_EHCI_OHCI == 0
 #include "r_usb_bitdefine.h"
 #include "r_usb_reg_access.h"

 #if USB_CFG_DMA == USB_CFG_ENABLE
  #include "r_usb_dmaca_rz_if.h"
  #include "r_usb_dmaca_rz_target.h"
  #include "r_usb_dmac.h"

  #if defined(BSP_MCU_GROUP_RZN2L)

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/
   #if 0
uint32_t g_usb_cstd_dma_size[USB_NUM_USBIP][USB_DMA_USE_CH_MAX] __attribute__((section("USB_DATA_RAM")));   /* DMA0 and DMA1 buffer size */
uint16_t g_usb_cstd_dma_fifo[USB_NUM_USBIP][USB_DMA_USE_CH_MAX] __attribute__((section("USB_DATA_RAM")));   /* DMA0 and DMA1 FIFO buffer size */
uint16_t g_usb_cstd_dma_pipe[USB_NUM_USBIP][USB_DMA_USE_CH_MAX] __attribute__((section("USB_DATA_RAM")));   /* DMA0 and DMA1 pipe number */

uint8_t g_usb_cstd_dma_ch[USB_NUM_USBIP][USB_FIFO_ACCESS_NUM_MAX] __attribute__((section("USB_DATA_RAM"))); /* DMA ch no. table */

uint8_t  g_usb_cstd_dma_fraction_size[USB_NUM_USBIP][USB_DMA_USE_CH_MAX];                                   /* fraction size(1-3) */
uint32_t g_usb_cstd_dma_fraction_adr[USB_NUM_USBIP][USB_DMA_USE_CH_MAX];                                    /* fraction data address */
   #endif

void usb_dma_rcv_setting(usb_utr_t * ptr, uint32_t des_addr, uint16_t useport, uint32_t transfer_size);
void usb_dma_send_setting(usb_utr_t * ptr, uint32_t src_adr, uint16_t useport, uint32_t transfer_size);

// uint8_t usb_cstd_dma_ref_ch_no(uint8_t ip_no, uint16_t use_port);
void usb_dma_send_complete_event_set(uint8_t ip_no, uint16_t use_port);

   #if !defined(BSP_MCU_GROUP_RZN2L)
void usb_ip0_d0fifo_callback(dmac_callback_args_t * cb_data);
void usb_ip0_d1fifo_callback(dmac_callback_args_t * cb_data);
void usb_ip1_d0fifo_callback(dmac_callback_args_t * cb_data);
void usb_ip1_d1fifo_callback(dmac_callback_args_t * cb_data);

   #else
void usb_ip0_d0fifo_callback(void);
void usb_ip0_d1fifo_callback(void);
void usb_ip1_d0fifo_callback(void);
void usb_ip1_d1fifo_callback(void);

   #endif

/******************************************************************************
 * Function Name   : usb_cstd_dma_ref_ch_no
 * Description     : Get DMA channel no.
 * Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure
 *              : uint16_t  useport : FIFO select
 * Return value    : DMA channel no.
 ******************************************************************************/
uint8_t usb_cstd_dma_ref_ch_no (uint8_t ip_no, uint16_t use_port)
{
    return g_usb_cstd_dma_ch[ip_no][use_port]; /* DMA ch no. table */
}

/******************************************************************************
 * Function Name   : usb_cstd_dma_send_start
 * Description     : Start transfer using DMA. If transfer size is 0, write
 *              : more data to buffer.
 * Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure
 *              : uint16_t pipe   : Pipe number
 *              : uint16_t useport: FIFO select
 * Return value    : none
 ******************************************************************************/
void usb_cstd_dma_send_start (usb_utr_t * ptr, uint16_t pipe, uint16_t useport)
{
    uint32_t  dma_size;
    uint32_t  fifo_size;
    uint8_t * p_data_ptr;
    uint8_t   ip;
    uint8_t   ch;

   #if USB_CFG_USE_USBIP == USB_CFG_IP0

    /* USB0 */
    ip = USB_IP0;
   #else                               /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    /* USB1/USBA */
    ip = USB_IP1;
   #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    p_data_ptr = gp_usb_pstd_data[pipe];
    ch         = usb_cstd_dma_ref_ch_no(ip, useport);
    dma_size   = g_usb_cstd_dma_size[ip][ch];
    fifo_size  = g_usb_cstd_dma_fifo[ip][ch];

    if (0U != dma_size)
    {
        hw_usb_clear_dreqe(ptr, useport); /* DMA Transfer request disable */
        if (dma_size >= fifo_size)
        {
            /* Fifo size block transfer */
            dma_size = (dma_size - (dma_size % fifo_size));
        }
        else
        {
            /* fraction size(1-3) */
            g_usb_cstd_dma_fraction_size[ip][ch] = g_usb_cstd_dma_size[ip][ch] & USB_BIT_MBW32;
            g_usb_cstd_dma_fraction_adr[ip][ch]  = (uint32_t) (p_data_ptr + dma_size); /* fraction data address */
        }

        g_usb_cstd_dma_size[ip][ch] = dma_size;

        usb_dma_send_setting(ptr, (uint32_t) p_data_ptr, useport, dma_size);

        /* Changes the FIFO port by the pipe. */
        if (false == usb_check_use_usba_module(ptr))
        {
            usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        }

        hw_usb_set_dreqe(ptr, useport);

        if (true == usb_check_use_usba_module(ptr))
        {
            usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        }
    }
    else
    {
        if (USB_D0USE == useport)
        {
            useport = USB_D0USE;
        }
        else
        {
            useport = USB_D1USE;
        }

        usb_pstd_buf_to_fifo(pipe, useport, ptr);
    }
}

/******************************************************************************
 * End of function usb_cstd_dma_send_start
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cstd_dma_rcv_start
 * Description     : Start transfer using DMA. If transfer size is 0, clear DMA.
 * Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure
 *              : uint16_t pipe     : Pipe number
 *              : uint16_t useport  : FIFO select
 *              : uint32_t length   : Data length
 * Return value    : none
 ******************************************************************************/
void usb_cstd_dma_rcv_start (usb_utr_t * ptr, uint16_t pipe, uint16_t useport)
{
    uint16_t  mxps;
    uint32_t  dma_size;
    uint32_t  transfer_size;
    uint8_t * p_data_ptr;
    uint8_t   ip;
    uint8_t   ch;
   #if defined(BSP_MCU_GROUP_RZN2L)
    uint32_t length = g_usb_pstd_data_cnt[pipe];
   #endif                              /* defined(BSP_MCU_GROUP_RZN2L) */

   #if USB_CFG_USE_USBIP == USB_CFG_IP0

    /* USB0 */
    ip = USB_IP0;
   #else                               /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    /* USB1/USBA */
    ip = USB_IP1;
   #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    p_data_ptr    = gp_usb_pstd_data[pipe];
    ch            = usb_cstd_dma_ref_ch_no(ip, useport);
    dma_size      = g_usb_cstd_dma_size[ip][ch];
    transfer_size = g_usb_cstd_dma_size[ip][ch];

    /* Data size check */
    if (0U != dma_size)
    {
        hw_usb_clear_dreqe(ptr, useport); /* DMA Transfer request disable */
        usb_dma_rcv_setting(ptr, (uint32_t) p_data_ptr, useport, transfer_size);

        /* Changes the FIFO port by the pipe. */
        if (false == usb_check_use_usba_module(ptr))
        {
            usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        }

        /* Max Packet Size */
        mxps = usb_cstd_get_maxpacket_size(ptr, pipe);

        /* Set Transaction counter */
        usb_cstd_set_transaction_counter(ptr, pipe, (uint16_t) (((length - (uint32_t) 1U) / mxps) + (uint32_t) 1U));

        /* Enable Ready Interrupt */
        hw_usb_set_brdyenb(ptr, pipe);

        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(ptr, pipe);

        /* usb fifo set automatic clear mode  */
        hw_usb_clear_dclrm(ptr, useport);

        /* Set DREQ enable */
        hw_usb_set_dreqe(ptr, useport);

        if (true == usb_check_use_usba_module(ptr))
        {
            usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
        }

        /* Set BUF */
        usb_cstd_set_buf(ptr, pipe);
    }
    else
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);

        /* DMA buffer clear mode set */
        hw_usb_set_dclrm(ptr, useport);

        /* Set BUF */
        usb_cstd_set_buf(ptr, pipe);

        /* Enable Ready Interrupt */
        hw_usb_set_brdyenb(ptr, pipe);

        /* Enable Not Ready Interrupt */
        usb_cstd_nrdy_enable(ptr, pipe);
    }
}

/******************************************************************************
 * End of function usb_cstd_dma_rcv_start
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cstd_dxfifo_stop
 * Description     : Setup external variables used for USB data transfer; to reg-
 *              : ister if you want to stop the transfer of DMA.
 * Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure
 *              : uint16_t useport  : FIFO select
 * Return value    : none
 ******************************************************************************/
void usb_cstd_dfifo_end (usb_utr_t * ptr, uint16_t useport)
{
    uint16_t   pipe;
    uint8_t    ip;
    uint32_t * p_data_cnt;
    uint16_t   mbw_setting;
    uint8_t    channel;

   #if USB_CFG_USE_USBIP == USB_CFG_IP0
    ip = USB_IP0;
   #else                               /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    ip = USB_IP1;
   #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

    channel    = usb_cstd_dma_ref_ch_no(ip, useport);
    pipe       = g_usb_cstd_dma_pipe[ip][channel];
    p_data_cnt = &g_usb_pstd_data_cnt[pipe];

    if (USB_D0USE == useport)
    {
        if (USB_IP0 == ip)
        {
            mbw_setting = USB0_D0FIFO_MBW;
        }
        else
        {
            mbw_setting = USB1_D0FIFO_MBW;
        }
    }
    else
    {
        if (USB_IP0 == ip)
        {
            mbw_setting = USB0_D1FIFO_MBW;
        }
        else
        {
            mbw_setting = USB1_D1FIFO_MBW;
        }
    }

    hw_usb_set_mbw(ptr, useport, mbw_setting);

    /* received data size */
    *p_data_cnt = (*p_data_cnt) - g_usb_cstd_dma_size[ip][channel];
}

/******************************************************************************
 * End of function usb_cstd_dxfifo_stop
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cstd_dma_send_restart
 * Description     : Start transfer using DMA0. accsess size 32bytes.
 * Arguments       : usb_utr_t *ptr     : Pointer to usb_utr_t structure
 *              : uint32_t src       : transfer data pointer
 *              : uint32_t data_size : transfer data size
 *              : uint16_t pipe      : Pipe number
 * Return value    : none
 ******************************************************************************/
void usb_cstd_dma_send_restart (usb_utr_t * ptr, uint32_t src, uint32_t data_size, uint8_t pipe)
{
    uint16_t useport;

    if (USB_PIPE1 == pipe)
    {
        useport = USB_D0USE;
    }
    else
    {
        useport = USB_D1USE;
    }

    /* Changes the FIFO port by the pipe. */
    if (false == usb_check_use_usba_module(ptr))
    {
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
    }

    hw_usb_clear_dreqe(ptr, useport);  /* DMA Transfer request disable */

    /* dma trans setting Divisible by FIFO buffer size  */
    usb_dma_send_setting(ptr, src, useport, data_size);

    /* Set DREQ enable */
    hw_usb_set_dreqe(ptr, useport);

    if (true == usb_check_use_usba_module(ptr))
    {
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_FALSE);
    }
}

/******************************************************************************
 * End of function usb_cstd_dma_send_restart
 ******************************************************************************/
   #if USB_CFG_DMA == USB_CFG_ENABLE

/******************************************************************************
 * Function Name   : usb_dma_rcv_setting
 * Description     : FIFO to Buffer data read DMA start
 * Arguments       : usb_utr_t *ptr         : Pointer to usb_utr_t structure
 *              : uint32_t des_addr      : Source address
 *              : uint16_t useport       : FIFO Access mode
 *              : uint32_t transfer_size : Transfer size
 * Return value    : void
 ******************************************************************************/
void usb_dma_rcv_setting (usb_utr_t * ptr, uint32_t des_addr, uint16_t useport, uint32_t transfer_size)
{
    volatile usb_dmaca_return_t   ret;
    usb_dmaca_transfer_data_cfg_t td_cfg;
    usb_dmaca_stat_t              dmac_status;
    uint8_t  dma_ch;
    uint16_t fifo_mode = 0;
    uint8_t  ip;
    uint16_t use_fifo;
    uint32_t transfer_count;

    FSP_PARAMETER_NOT_USED(ptr);

    #if USB_CFG_USE_USBIP == USB_CFG_IP0

    /* USB01 */
    ip = USB_IP0;
    #else                              /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    /* USB11 */
    ip = USB_IP1;
    #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

    dma_ch = usb_cstd_dma_ref_ch_no(ip, useport);

    if (USB_D0USE == useport)
    {
        fifo_mode = USB_FIFO_TYPE_D0DMA;
        use_fifo  = USB_DMACA_CONFIG_SEL_D0FIFO;
    }
    else
    {
        fifo_mode = USB_FIFO_TYPE_D1DMA;
        use_fifo  = USB_DMACA_CONFIG_SEL_D1FIFO;
    }

    FSP_PARAMETER_NOT_USED(fifo_mode);

    transfer_count  = transfer_size;
    td_cfg.src_size = USB_DMACA_CONFIG_SRC_SIZE_LWORD;
    td_cfg.des_size = USB_DMACA_CONFIG_DES_SIZE_LWORD;

    /* Wait for Complete of DMA transfer. */
    do
    {
        r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_STATUS_GET, &dmac_status);
    } while (true == dmac_status.act_stat);

    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_CH_STATUS_CLR, &dmac_status);
    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_REG_SET_FIX, &dmac_status);

/* DMA test code start */
    /* Operation - No Extended Repeat Area Function and No Offset Subtraction */

    /* Source address is fixed
     * Transfer data size is 32-bit (long word).
     * DMAC transfer mode is Repeat mode & Source side is repeat area
     * At the beginning of transfer, clear the interrupt flag of the activation source
     * to 0.
     * Transfer Request source is software. *//* Set Transfer data configuration. */
    td_cfg.use_fifo       = (usb_dmaca_chcfg_fifo_select_t) use_fifo;
    td_cfg.req_dir        = USB_DMACA_CONFIG_USB_SRC;
    td_cfg.src_adr_dir    = USB_DMACA_CONFIG_SRC_ADDRESS_FIX;
    td_cfg.des_adr_dir    = USB_DMACA_CONFIG_DES_ADDRESS_INC;
    td_cfg.dmaint_msk     = USB_DMACA_CONFIG_DMAEND_INT_MASK_ON;
    td_cfg.dmatc_msk      = USB_DMACA_CONFIG_DMATC_MASK_ON;
    td_cfg.ch_interval    = USB_DMACA_INTERVAL_MIN;
    td_cfg.trx_priority   = USB_DMACA_DCTRL_PRIORITY_ROUND;
    td_cfg.p_src_addr     = 0;
    td_cfg.p_des_addr     = (void *) des_addr;
    td_cfg.transfer_count = transfer_count;

    /* Call r_usb_dmaca_create(). */
    ret = r_usb_dmaca_create(dma_ch, &td_cfg);

    ret = r_usb_dmaca_int_enable(dma_ch, USB_DMA_CH_PRI);
    if (USB_DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    /* Call r_usb_dmaca_control().
     * Enable DMAC transfer. */

    ret = r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_ENABLE, &dmac_status);
}

/******************************************************************************
 * End of function usb_dma_rcv_setting
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_dma_send_setting
 * Description     : Buffer to FIFO data write DMA start
 * Arguments       : usb_utr_t *ptr            : Pointer to usb_utr_t structure
 *              : uint32_t  src_adr         : Source address
 *              : uint16_t  useport         : FIFO Access mode
 *              : uint32_t  transfer_size   : Transfer size
 * Return value    : none
 ******************************************************************************/
void usb_dma_send_setting (usb_utr_t * ptr, uint32_t src_adr, uint16_t useport, uint32_t transfer_size)
{
    volatile usb_dmaca_return_t   ret;
    usb_dmaca_transfer_data_cfg_t td_cfg;
    usb_dmaca_stat_t              dmac_status;
    uint16_t fifo_mode = 0;
    uint8_t  dma_ch;
    uint8_t  ip;
    uint16_t use_fifo;
    uint32_t transfer_count;
    uint16_t pipe_no;

    #if USB_CFG_USE_USBIP == USB_CFG_IP0

    /* USB01 */
    ip = USB_IP0;
    #else                              /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    /* USB11 */
    ip = USB_IP1;
    #endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */

    dma_ch = usb_cstd_dma_ref_ch_no(ip, useport);

    if (USB_D0USE == useport)
    {
        fifo_mode = USB_FIFO_TYPE_D0DMA;
        use_fifo  = USB_DMACA_CONFIG_SEL_D0FIFO;
        if (USB_IP0 == ip)
        {
            ret = r_usb_dmaca_int_callback(dma_ch, (void *) usb_ip0_d0fifo_callback);
        }
        else
        {
            ret = r_usb_dmaca_int_callback(dma_ch, (void *) usb_ip1_d0fifo_callback);
        }
    }
    else
    {
        fifo_mode = USB_FIFO_TYPE_D1DMA;
        use_fifo  = USB_DMACA_CONFIG_SEL_D1FIFO;
        if (USB_IP0 == ip)
        {
            ret = r_usb_dmaca_int_callback(dma_ch, (void *) usb_ip0_d1fifo_callback);
        }
        else
        {
            ret = r_usb_dmaca_int_callback(dma_ch, (void *) usb_ip1_d1fifo_callback);
        }
    }

    FSP_PARAMETER_NOT_USED(fifo_mode);

    transfer_count  = transfer_size;
    td_cfg.src_size = USB_DMACA_CONFIG_SRC_SIZE_LWORD;
    td_cfg.des_size = USB_DMACA_CONFIG_DES_SIZE_LWORD;

    /* Wait for Complete of DMA transfer. */
    do
    {
        r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_STATUS_GET, &dmac_status);
    } while (true == dmac_status.act_stat);

    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_CH_STATUS_CLR, &dmac_status);
    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_REG_SET_FIX, &dmac_status);

/* DMA test code start */
    /* Operation - No Extended Repeat Area Function and No Offset Subtraction */

    /* Source address is fixed
     * Transfer data size is 32-bit (long word).
     * DMAC transfer mode is Repeat mode & Source side is repeat area
     * At the beginning of transfer, clear the interrupt flag of the activation source
     * to 0.
     * Transfer Request source is software. *//* Set Transfer data configuration. */
    td_cfg.use_fifo       = (usb_dmaca_chcfg_fifo_select_t) use_fifo;
    td_cfg.req_dir        = USB_DMACA_CONFIG_USB_DES;
    td_cfg.src_adr_dir    = USB_DMACA_CONFIG_SRC_ADDRESS_INC;
    td_cfg.des_adr_dir    = USB_DMACA_CONFIG_DES_ADDRESS_FIX;
    td_cfg.dmaint_msk     = USB_DMACA_CONFIG_DMAEND_INT_MASK_OFF;
    td_cfg.dmatc_msk      = USB_DMACA_CONFIG_DMATC_MASK_OFF;
    td_cfg.ch_interval    = USB_DMACA_INTERVAL_MIN;
    td_cfg.trx_priority   = USB_DMACA_DCTRL_PRIORITY_ROUND;
    td_cfg.p_src_addr     = (void *) src_adr;
    td_cfg.p_des_addr     = 0;
    td_cfg.transfer_count = transfer_count;

    /* Call r_usb_dmaca_create(). */
    ret = r_usb_dmaca_create(dma_ch, &td_cfg);

    pipe_no = g_usb_cstd_dma_pipe[ip][dma_ch];
    hw_usb_clear_status_bemp(ptr, pipe_no);
    hw_usb_set_bempenb(ptr, pipe_no);

    ret = r_usb_dmaca_int_enable(dma_ch, USB_DMA_CH2_PRI);
    if (USB_DMACA_SUCCESS != ret)
    {
        /* do something */
    }

    /* Call r_usb_dmaca_control().
     * Enable DMAC transfer. */
    ret = r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_ENABLE, &dmac_status);
}

/******************************************************************************
 * End of function usb_dma_send_setting
 ******************************************************************************/
   #endif                              /* USB_CFG_DMA == USB_CFG_ENABLE */

/******************************************************************************
 * Function Name   : usb_cstd_dma_stop
 * Description     : DMA stop
 * Arguments       : uint8_t   ip_type     : USB_IP0/USB_IP1
 *              : uint16_t  use_port    : FIFO select
 * Return value    : void
 ******************************************************************************/
void usb_cstd_dma_stop (usb_utr_t * p_utr, uint16_t use_port)
{
   #if USB_CFG_DMA == USB_CFG_ENABLE
    usb_dmaca_stat_t dmac_status;
    uint8_t          dma_ch;
    uint8_t          ip_type = p_utr->ip;

    dma_ch = usb_cstd_dma_ref_ch_no(ip_type, use_port); /* Get DMA channel no. */

    /* Wait for Complete of DMA transfer. */
    do
    {
        r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_STATUS_GET, &dmac_status);
    } while (true == dmac_status.act_stat);
    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_STATUS_CLR, &dmac_status); /* Clear DMA status */
   #endif /* USB_CFG_DMA == USB_CFG_ENABLE */
}

/******************************************************************************
 * End of function usb_cstd_dma_stop
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cstd_dma_status_clr
 * Description     : DMA stop
 * Arguments       : uint8_t   ip_type     : USB_IP0/USB_IP1
 *              : uint16_t  use_port    : FIFO select
 * Return value    : void
 ******************************************************************************/
void usb_cstd_dma_status_clr (usb_utr_t * p_utr, uint16_t use_port)
{
   #if USB_CFG_DMA == USB_CFG_ENABLE
    usb_dmaca_stat_t dmac_status;
    uint8_t          dma_ch;
    uint8_t          ip_type = p_utr->ip;

    dma_ch = usb_cstd_dma_ref_ch_no(ip_type, use_port);                     /* Get DMA channel no. */

    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_CH_STATUS_CLR, &dmac_status); /* Clear DMA status */
   #endif /* USB_CFG_DMA == USB_CFG_ENABLE */
}

/******************************************************************************
 * End of function usb_cstd_dma_status_clr
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_dma_set_ch_no
 * Description     : Set DMA channel no.
 * Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure
 *              : uint16_t useport  : FIFO select
 *              : uint8_t dma_ch_no : DMA channel no.
 * Return value    : none
 ******************************************************************************/
void usb_dma_set_ch_no (uint16_t ip_no, uint16_t use_port, uint8_t dma_ch_no)
{
    g_usb_cstd_dma_ch[ip_no][use_port] = dma_ch_no; /* DMA ch no. table */
}

/******************************************************************************
 * End of function usb_dma_set_ch_no
 ******************************************************************************/

/******************************************************************************
 * End of function usb_cstd_dma_ref_ch_no
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_dma_send_complete_event_set
 * Description     : Set event for DMA transfer complete of Buffer to DxFIFO.
 * Arguments       : uint8_t ip_no  : IP no.(USB_IP0/USB_IP1)
 *              : uint16_t useport: FIFO select(USB_D0USE/USB_D1USE)
 * Return value    : none
 ******************************************************************************/
   #if BSP_CFG_RTOS == 0
void usb_dma_send_complete_event_set (uint8_t ip_no, uint16_t use_port)
{
    usb_cfg_t * p_cfg;
    usb_utr_t   utr;

    #if defined(BSP_MCU_GROUP_RZN2L)
    p_cfg = (usb_cfg_t *) R_FSP_IsrContextGet(VECTOR_NUMBER_USB_FDMA1); // @@
    #else /* defined(BSP_MCU_GROUP_RZN2L) */
    if (ip_no)
    {
     #if defined(BSP_MCU_GROUP_RA6M3)
        p_cfg = (usb_cfg_t *) R_FSP_IsrContextGet((IRQn_Type) VECTOR_NUMBER_USBHS_FIFO_1); // @@
     #else /* defined(BSP_MCU_GROUP_RA6M3) */
        p_cfg = (usb_cfg_t *) R_FSP_IsrContextGet((IRQn_Type) VECTOR_NUMBER_USBFS_FIFO_1); // @@
     #endif /* defined(BSP_MCU_GROUP_RA6M3) */
    }
    else
    {
        p_cfg = (usb_cfg_t *) R_FSP_IsrContextGet((IRQn_Type) VECTOR_NUMBER_USBFS_FIFO_1); // @@
    }
    #endif /* defined(BSP_MCU_GROUP_RZN2L) */

    gs_usb_cstd_dma_int.buf[gs_usb_cstd_dma_int.wp].ip        = ip_no;
    gs_usb_cstd_dma_int.buf[gs_usb_cstd_dma_int.wp].fifo_type = use_port;
    gs_usb_cstd_dma_int.wp = (uint8_t) (((uint8_t) (gs_usb_cstd_dma_int.wp + 1)) % USB_INT_BUFSIZE);
    gs_usb_cstd_dma_int.buf[gs_usb_cstd_dma_int.wp].p_cfg = p_cfg;

    utr.ip = ip_no;
    if (USB_MODE_HOST == g_usb_usbmode[utr.ip])
    {
    }
    else
    {
    #if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
        hw_usb_clear_dreqe(&utr, use_port); /* DMA Transfer request disable */
    #endif /* ( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI ) */
    }
}

   #else
void usb_dma_send_complete_event_set (uint8_t ip_no, uint16_t use_port)
{
    usb_utr_t * p;

    p          = get_usb_int_buf();
    p->ip      = ip_no;
    p->msginfo = USB_MSG_PCD_INT;
    p->keyword = USB_INT_DXFIFO;
    p->status  = use_port;

    hw_usb_clear_dreqe(p, use_port);   /* DMA Transfer request disable */

    USB_ISND_MSG(USB_PCD_MBX, p);
}

/******************************************************************************
 * End of function usb_dma_send_complete_event_set
 ******************************************************************************/
   #endif

   #if !defined(BSP_MCU_GROUP_RZN2L)

/*******************************************************************************
 * Function Name: usb_ip0_d0fifo_callback
 * Description  : Interrupt service routine of D0FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip0_d0fifo_callback (dmac_callback_args_t * cb_data)
{
    USB_DMACA_CHCTRL(0) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP0, USB_D0USE);
}

/******************************************************************************
 * End of function usb_ip0_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip0_d1fifo_callback
 * Description  : Interrupt service routine of D1FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip0_d1fifo_callback (dmac_callback_args_t * cb_data)
{
    USB_DMACA_CHCTRL(1) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP0, USB_D1USE);
}

/******************************************************************************
 * End of function usb_ip1_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip1_d0fifo_callback
 * Description  : Interrupt service routine of D0FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip1_d0fifo_callback (dmac_callback_args_t * cb_data)
{
    USB_DMACA_CHCTRL(0) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP1, USB_D0USE);
}

/******************************************************************************
 * End of function usb_ip1_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip1_d1fifo_callback
 * Description  : Interrupt service routine of D1FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip1_d1fifo_callback (dmac_callback_args_t * cb_data)
{
    USB_DMACA_CHCTRL(1) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP1, USB_D1USE);
}

/******************************************************************************
 * End of function usb_ip1_d1fifo_callback
 ******************************************************************************/
   #else                               /* !defined(BSP_MCU_GROUP_RZN2L) */
void usb_ip0_d0fifo_callback (void)
{
    USB_DMACA_CHCTRL(0) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP0, USB_D0USE);
}

/******************************************************************************
 * End of function usb_ip0_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip0_d1fifo_callback
 * Description  : Interrupt service routine of D1FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip0_d1fifo_callback (void)
{
    USB_DMACA_CHCTRL(1) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP0, USB_D1USE);
}

/******************************************************************************
 * End of function usb_ip1_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip1_d0fifo_callback
 * Description  : Interrupt service routine of D0FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip1_d0fifo_callback (void)
{
    USB_DMACA_CHCTRL(0) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP1, USB_D0USE);
}

/******************************************************************************
 * End of function usb_ip1_d0fifo_callback
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usb_ip1_d1fifo_callback
 * Description  : Interrupt service routine of D1FIFO
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
void usb_ip1_d1fifo_callback (void)
{
    USB_DMACA_CHCTRL(1) |= USB_DMACA_CHSTAT_END_CLEAR;
    usb_dma_send_complete_event_set(USB_IP1, USB_D1USE);
}

/******************************************************************************
 * End of function usb_ip1_d1fifo_callback
 ******************************************************************************/
   #endif                              /* !defined(BSP_MCU_GROUP_RZN2L) */

/******************************************************************************
 * Function Name   : usb_dma_get_crtb
 * Description     : Get DMA Current Transaction Byte reg B(CRTB).
 * Arguments       : uint16_t dma_ch : DMA Channel no.
 * Return value    : DMA Current Transaction Byte reg B(CRTB)
 ******************************************************************************/
uint32_t usb_cstd_dma_get_crtb (uint8_t dma_ch)
{
   #if USB_CFG_DMA == USB_CFG_ENABLE
    usb_dmaca_stat_t dmac_status;

    r_usb_dmaca_control(dma_ch, USB_DMACA_CMD_STATUS_GET, &dmac_status);

    return dmac_status.transfer_count;
   #endif                              /* USB_CFG_DMA == USB_CFG_ENABLE */
}

/******************************************************************************
 * End of function usb_dma_get_crtb
 ******************************************************************************/
  #endif                               /* defined(BSP_MCU_GROUP_RZN2L) */
 #endif                                /* (USB_CFG_DMA == USB_CFG_ENABLE) */
#endif  /* USB_IP_EHCI_OHCI == 0 */

/******************************************************************************
 * End of file
 ******************************************************************************/
