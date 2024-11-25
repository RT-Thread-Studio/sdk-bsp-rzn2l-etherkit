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
 * File Name    : r_usb_cscheduler.c
 * Description  : USB Host scheduler code
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/

#if (BSP_CFG_RTOS == 2)
 #include "../driver/inc/r_usb_cstd_rtos.h"
#endif                                 /* (BSP_CFG_RTOS == 2) */

#include "../driver/inc/r_usb_basic_local.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

#if  USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/
extern void usb_hstd_int_enable(void);
extern void usb_hstd_int_disable(void);

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/

usb_er_t R_USB_CstdiSndMsg(uint8_t id, usb_msg_t * p_mess);

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/

/* Priority Table */
static usb_msg_t * gsp_usb_hstd_table_addr[USB_PRIMAX][USB_TABLEMAX];
static uint8_t     gs_usb_hstd_table_id[USB_PRIMAX][USB_TABLEMAX];
static uint8_t     gs_usb_hstd_pri_r[USB_PRIMAX];
static uint8_t     gs_usb_hstd_pri_w[USB_PRIMAX];
static uint8_t     gs_usb_hstd_pri[USB_IDMAX];

/* Schedule Set Flag  */
static uint8_t gs_usb_hstd_schedule_flag;

/* Fixed-sized memory pools */
static st_usb_utr_t gs_usb_hstd_block[USB_BLKMAX];
static uint8_t      gs_usb_hstd_blk_flg[USB_BLKMAX];

static usb_msg_t * gsp_usb_hstd_add_use;
static uint8_t     gs_usb_hstd_id_use;

/* Wait MSG */
static usb_msg_t * gsp_usb_hstd_wait_add[USB_IDMAX];
static uint16_t    gs_usb_hstd_wait_counter[USB_IDMAX];

 #if (BSP_CFG_RTOS == 2)

/** Declare an array of mailbox handlers. **/
 #endif                                /* (BSP_CFG_RTOS == 2) */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_rec_msg
 * Description     : Receive a message to the specified id (mailbox).
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t** mess  : Message pointer
 * Return          : uint16_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t usb_hstd_rec_msg (uint8_t id, usb_msg_t ** pp_mess)
{
    if ((id < USB_IDMAX) && (gs_usb_hstd_id_use < USB_IDMAX))
    {
        if (gs_usb_hstd_id_use == id)
        {
            *pp_mess = gsp_usb_hstd_add_use;

            return USB_OK;
        }
    }

    return USB_ERROR;
}                                      /* End of function usb_hstd_rec_msg() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdRecMsg
 * Description     : Receive a message to the specified id (mailbox).
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t** mess  : Message pointer
 * Return          : uint16_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdRecMsg (uint8_t id, usb_msg_t ** pp_mess)
{
    usb_er_t err;

    err = usb_hstd_rec_msg(id, pp_mess);

    return err;
}                                      /* End of function R_USB_CstdRecMsg() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_isnd_msg
 * Description     : Send a message to the specified id (mailbox) while executing
 *              : an interrupt.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t* mess   : Message pointer
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t usb_hstd_isnd_msg (uint8_t id, usb_msg_t * p_mess)
{
    uint8_t usb_pri;                   /* Task Priority */
    uint8_t usb_write;                 /* Priority Table Writing pointer */

    if (id < USB_IDMAX)
    {
        /* Read priority and table pointer */
        usb_pri   = gs_usb_hstd_pri[id];
        usb_write = gs_usb_hstd_pri_w[usb_pri];
        if (usb_pri < USB_PRIMAX)
        {
            /* Renewal write pointer */
            usb_write++;
            if (usb_write >= USB_TABLEMAX)
            {
                usb_write = USB_TBLCLR;
            }

            /* Check pointer */
            if (usb_write == gs_usb_hstd_pri_r[usb_pri])
            {
                return USB_ERROR;
            }

            /* Save message */
            /* Set priority table */
            gs_usb_hstd_table_id[usb_pri][usb_write]    = id;
            gsp_usb_hstd_table_addr[usb_pri][usb_write] = p_mess;
            gs_usb_hstd_pri_w[usb_pri] = usb_write;

            return USB_OK;
        }
    }

    USB_PRINTF0("SND_MSG ERROR !!\n");

    return USB_ERROR;
}                                      /* End of function usb_hstd_isnd_msg() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdiSndMsg
 * Description     : Send a message to the specified id (mailbox) while executing
 *              : an interrupt.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t* mess   : Message pointer
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdiSndMsg (uint8_t id, usb_msg_t * p_mess)
{
    usb_er_t err;

    err = usb_hstd_isnd_msg(id, p_mess);

    return err;
}                                      /* End of function R_USB_CstdiSndMsg() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_snd_msg
 * Description     : Send a message to the specified id (mailbox).
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t* mess   : Message pointer
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t usb_hstd_snd_msg (uint8_t id, usb_msg_t * p_mess)
{
    usb_er_t status;

    usb_hstd_int_disable();            /* USB_ interrupt disable */

    status = usb_hstd_isnd_msg(id, p_mess);

    usb_hstd_int_enable();             /* USB_ interrupt enable */

    return status;
} /* End of function usb_hstd_snd_msg() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdSndMsg
 * Description     : Send a message to the specified id (mailbox).
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : usb_msg_t* mess   : Message pointer
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdSndMsg (uint8_t id, usb_msg_t * p_mess)
{
    usb_er_t status;

    status = usb_hstd_snd_msg(id, p_mess);

    return status;
}                                      /* End of function R_USB_CstdSndMsg() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_pget_blk
 * Description     : Get a memory block for the caller.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : st_usb_utr_t** blk   : Memory block pointer.
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t usb_hstd_pget_blk (uint8_t id, st_usb_utr_t ** pp_blk)
{
    uint8_t usb_s_pblk_c;

    if (id < USB_IDMAX)
    {
        usb_s_pblk_c = USB_CNTCLR;

        while (USB_BLKMAX != usb_s_pblk_c)
        {
            if (USB_FLGCLR == gs_usb_hstd_blk_flg[usb_s_pblk_c])
            {
                /* Acquire fixed-size memory block */
                *pp_blk = &gs_usb_hstd_block[usb_s_pblk_c];
                gs_usb_hstd_blk_flg[usb_s_pblk_c] = USB_FLGSET;

                return USB_OK;
            }

            usb_s_pblk_c++;
        }

        /* Error of BLK Table Full !!  */
        USB_PRINTF1("usb_scBlkFlg[%d][] Full !!\n", id);
    }

    return USB_ERROR;
}                                      /* End of function usb_hstd_pget_blk() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdPgetBlk
 * Description     : Call the get memory block function to get a memory block for
 *              : the caller.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : st_usb_utr_t** blk   : Memory block pointer.
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdPgetBlk (uint8_t id, st_usb_utr_t ** pp_blk)
{
    usb_er_t err;

    err = usb_hstd_pget_blk(id, pp_blk);

    return err;
}                                      /* End of function R_USB_CstdPgetBlk() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_rel_blk
 * Description     : Release a memory block.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : st_usb_utr_t* blk    : Memory block pointer.
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t usb_hstd_rel_blk (uint8_t id, st_usb_utr_t * p_blk)
{
    uint16_t usb_s_rblk_c;

    if (id < USB_IDMAX)
    {
        usb_s_rblk_c = USB_CNTCLR;
        while (USB_BLKMAX != usb_s_rblk_c)
        {
            if (p_blk == &gs_usb_hstd_block[usb_s_rblk_c])
            {
                /* Release fixed-size memory block */
                gs_usb_hstd_blk_flg[usb_s_rblk_c] = USB_FLGCLR;

                return USB_OK;
            }

            usb_s_rblk_c++;
        }

        /* Error of BLK Flag is not CLR !!  */
        USB_PRINTF0("TskBlk NO CLR !!\n");
    }

    return USB_ERROR;
}                                      /* End of function usb_hstd_rel_blk() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdRelBlk
 * Description     : Call the function to release a memory block.
 * Argument        : uint8_t id        : ID number (mailbox).
 *              : st_usb_utr_t* blk    : Memory block pointer.
 * Return          : usb_er_t          : USB_OK / USB_ERROR
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdRelBlk (uint8_t id, st_usb_utr_t * p_blk)
{
    usb_er_t err;

    err = usb_hstd_rel_blk(id, p_blk);

    return err;
}                                      /* End of function R_USB_CstdRelBlk() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_wai_msg
 * Description     : Runs USB_SND_MSG after running the scheduler the specified
 *              : number of times.
 * Argument        : uint8_t  id       : ID number (mailbox).
 *              : usb_msg_t *mess   : Message pointer.
 *              : uint16_t  times   : Timeout value.
 * Return          : usb_er_t          : USB_OK / USB_ERROR.
 ***********************************************************************************************************************/
usb_er_t usb_hstd_wai_msg (uint8_t id, usb_msg_t * p_mess, uint16_t times)
{
    if (id < USB_IDMAX)
    {
        if (0 == gs_usb_hstd_wait_counter[id])
        {
            gsp_usb_hstd_wait_add[id]    = p_mess;
            gs_usb_hstd_wait_counter[id] = times;

            return USB_OK;
        }
    }

    /* Error !!  */
    USB_PRINTF0("WAI_MSG ERROR !!\n");

    return USB_ERROR;
}                                      /* End of function usb_hstd_wai_msg() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdWaiMsg
 * Description     : Will run USB_SND_MSG after running the scheduler the specified
 *              : number of times.
 * Argument        : uint8_t  id       : ID number (mailbox).
 *              : usb_msg_t *mess   : Message pointer.
 *              : uint16_t  times   : Timeout value.
 * Return          : usb_er_t          : USB_OK / USB_ERROR.
 ***********************************************************************************************************************/
usb_er_t R_USB_CstdWaiMsg (uint8_t id, usb_msg_t * p_mess, uint16_t times)
{
    usb_er_t err;

    err = usb_hstd_wai_msg(id, p_mess, times);

    return err;
}                                      /* End of function R_USB_CstdWaiMsg() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_wait_scheduler
 * Description     : Schedules a wait request.
 * Argument        : none
 * Return          : none
 ***********************************************************************************************************************/
void usb_hstd_wait_scheduler (void)
{
    usb_er_t err;
    uint8_t  id;

    for (id = 0; id < USB_IDMAX; id++)
    {
        if (0 != gs_usb_hstd_wait_counter[id])
        {
            gs_usb_hstd_wait_counter[id]--;
            if (0 == gs_usb_hstd_wait_counter[id])
            {
                err = usb_hstd_snd_msg(id, gsp_usb_hstd_wait_add[id]);
                if (USB_OK != err)
                {
                    gs_usb_hstd_wait_counter[id]++;
                }
            }
        }
    }
}                                      /* End of function usb_hstd_wait_scheduler() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_sche_init
 * Description     : Scheduler initialization.
 * Argument        : none
 * Return          : none
 ***********************************************************************************************************************/
void usb_hstd_sche_init (void)
{
    uint8_t i;
    uint8_t j;

    /* Initial Scheduler */
    gs_usb_hstd_id_use        = 0;
    gs_usb_hstd_schedule_flag = 0;

    /* Initialize  priority table pointer and priority table */
    for (i = 0; i != USB_PRIMAX; i++)
    {
        gs_usb_hstd_pri_r[i] = 0;
        gs_usb_hstd_pri_w[i] = 0;
        for (j = 0; j != USB_TABLEMAX; j++)
        {
            gs_usb_hstd_table_id[i][j] = USB_IDMAX;
        }
    }

    /* Initialize block table */
    for (i = 0; i != USB_BLKMAX; i++)
    {
        gs_usb_hstd_blk_flg[i] = 0;
    }

    /* Initialize priority */
    for (i = 0; i != USB_IDMAX; i++)
    {
        gs_usb_hstd_pri[i]          = (uint8_t) USB_IDCLR;
        gsp_usb_hstd_wait_add[i]    = (usb_msg_t *) USB_NULL;
        gs_usb_hstd_wait_counter[i] = 0;
    }
}                                      /* End of function usb_hstd_sche_init() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdScheduler
 * Description     : The scheduler.
 * Argument        : none
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_CstdScheduler (void)
{
    uint8_t usb_pri;                   /* Priority Counter */
    uint8_t usb_read;                  /* Priority Table read pointer */

    /* wait msg */
    usb_hstd_wait_scheduler();

    /* Priority Table reading */
    usb_pri = USB_CNTCLR;
    while (usb_pri < USB_PRIMAX)
    {
        usb_read = gs_usb_hstd_pri_r[usb_pri];
        if (usb_read != gs_usb_hstd_pri_w[usb_pri])
        {
            /* Priority Table read pointer increment */
            usb_read++;
            if (usb_read >= USB_TABLEMAX)
            {
                usb_read = USB_TBLCLR;
            }

            /* Set practise message */
            gs_usb_hstd_id_use   = gs_usb_hstd_table_id[usb_pri][usb_read];
            gsp_usb_hstd_add_use = gsp_usb_hstd_table_addr[usb_pri][usb_read];
            gs_usb_hstd_table_id[usb_pri][usb_read] = USB_IDMAX;
            gs_usb_hstd_pri_r[usb_pri]              = usb_read;
            gs_usb_hstd_schedule_flag               = USB_FLGSET;

            usb_pri = USB_PRIMAX;
        }
        else
        {
            usb_pri++;
        }
    }
}                                      /* End of function R_USB_CstdScheduler() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdSetTaskPri
 * Description     : Set a task's priority.
 * Argument        : uint8_t tasknum   : Task id.
 *              : uint8_t pri       : The task priority to be set.
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_CstdSetTaskPri (uint8_t tasknum, uint8_t pri)
{
    if (tasknum < USB_IDMAX)
    {
        if (pri < USB_PRIMAX)
        {
            gs_usb_hstd_pri[tasknum] = pri;
        }
        else if (USB_IDCLR == pri)
        {
            gs_usb_hstd_pri[tasknum] = USB_IDCLR;
        }
        else
        {
        }
    }
}                                      /* End of function R_USB_CstdSetTaskPri() */

/***********************************************************************************************************************
 * Function Name   : R_USB_CstdCheckSchedule
 * Description     : Check schedule flag to see if caller's "time has come", then
 *              : clear it.
 * Argument        : none
 * Return          : flg   : gs_usb_hstd_schedule_flag
 ***********************************************************************************************************************/
uint8_t R_USB_CstdCheckSchedule (void)
{
    uint8_t flg;

    flg = gs_usb_hstd_schedule_flag;
    gs_usb_hstd_schedule_flag = USB_FLGCLR;

    return flg;
}                                      /* End of function R_USB_CstdCheckSchedule() */

#endif                                 /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
