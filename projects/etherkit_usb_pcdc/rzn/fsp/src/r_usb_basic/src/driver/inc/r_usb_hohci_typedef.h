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
 * File Name    : r_usb_hOhciTypedef.c
 * Description  : OHCI Type Define Header
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

#ifndef R_USB_HOHCI_TYPEDEF_H
#define R_USB_HOHCI_TYPEDEF_H

//= =======================================================================
// Definitions
//= =======================================================================
#define USB_OHCI_SHAREDMEMORYTYPE              (USB_OHCI_SHAREDMEMORYTYPE_PCI) /* Shared Memory Type */
#define USB_OHCI_SHAREDMEMORY_OFFSET           (0x00000000U)                   /* Shared Memory Offset */

#define USB_OHCI_NUM_REQUEST                   (USB_OHCI_NUM_TD)               /* The maximum number of Transfer Request memory to td*/

#define USB_OHCI_MAXDEVICE                     (USB_HCI_DEVICE_NUM)            /* The maximum number of OHCI Device */

#define USB_OHCI_DEVICE_ENDPOINT_MAX           (4)                             /* Max number of endpoints device have */

#define USB_OHCI_MAXENDPOINT                   (16)                            /* The maximum number of OHCI Endpoint No. */

#define USB_OHCI_NUM_DEVICEDATA                (1U)                            /* The maximum number of Dvice Data (Num of IP : HCCA address etc... )*/

#define USB_OHCI_MAXROOTPORTS                  (1U)                            /* The maximum number of OHCI Rootport */

#define USB_OHCI_MAXTDSIZE                     (0x00002000U)                   /* Max Transfer Descriptor Size */

#define USB_OHCI_MAXIMUM_OVERHEAD              (210U)                          /* The maximum number of OHCI Isochronous Overhead */

#define USB_OHCI_ISO_MAX_FRAME                 (8U)                            /* The maximum number of OHCI Isochronous Frame Size */

#define USB_OHCI_SHAREDMEMORYTYPE_EXCLUSIVE    (0U)                            /* Exclusive Shared Memory Type */
#define USB_OHCI_SHAREDMEMORYTYPE_COMMOM       (1U)                            /* Common Shared Memory Type */
#define USB_OHCI_SHAREDMEMORYTYPE_PCI          (2U)                            /* PCI Shared Memory Type */

/* TD->td_status */
#define USB_OHCI_TD_NOTFILLED                  (0U)                            /* NOTFILLED */
#define USB_OHCI_TD_CANCELED                   (1U)                            /* CANCELED */
#define USB_OHCI_TD_COMPLETED                  (2U)                            /* COMPLETED */
#define USB_OHCI_TD_PENDING                    (3U)                            /* PENDING */

/* TD ConditionCode */
#define USB_OHCI_CC_NOERROR                    (0x0U)                          /* NOERROR */
#define USB_OHCI_CC_CRC                        (0x1U)                          /* CRC */
#define USB_OHCI_CC_BITSTUFFING                (0x2U)                          /* BITSTUFFING */
#define USB_OHCI_CC_DATATOGGLEMISMATCH         (0x3U)                          /* DATATOGGLEMISMATCH */
#define USB_OHCI_CC_STALL                      (0x4U)                          /* STALL */
#define USB_OHCI_CC_DEVICENOTRESPOND           (0x5U)                          /* DEVICENOTRESPOND */
#define USB_OHCI_CC_PIDCHECKFAILURE            (0x6U)                          /* PIDCHECKFAILURE */
#define USB_OHCI_CC_UNEXPECTEDPID              (0x7U)                          /* UNEXPECTEDPID */
#define USB_OHCI_CC_DATAOVERRUN                (0x8U)                          /* DATAOVERRUN */
#define USB_OHCI_CC_DATAUNDERRUN               (0x9U)                          /* DATAUNDERRUN */
#define USB_OHCI_CC_BUFFEROVERRUN              (0xCU)                          /* BUFFEROVERRUN */
#define USB_OHCI_CC_BUFFERUNDERRUN             (0xDU)                          /* BUFFERUNDERRUN */
#define USB_OHCI_CC_NOTACCESSED                (0xFU)                          /* NOTACCESSED */

/* Request->Status */
#define USB_OHCI_RS_NORMAL_COMPLETION          (0x00U)                         /* NORMAL COMPLETION */
#define USB_OHCI_RS_NOT_DONE                   (0x10U)                         /* NOT DONE */
#define USB_OHCI_RS_NOT_REQUESTED              (0x11U)                         /* NOT REQUESTED */
#define USB_OHCI_RS_CANCELING                  (0x12U)                         /* CANCELING */
#define USB_OHCI_RS_CANCELED                   (0x13U)                         /* CANCELED */

/* HC_Transfer_Control.DP */
#define USB_OHCI_PID_SETUP                     (0U)                            /* SETUP */
#define USB_OHCI_PID_OUT                       (1U)                            /* OUT */
#define USB_OHCI_PID_IN                        (2U)                            /* IN */

/* Regs */
#define USB_OHCI_CONTROLLISTFILLED             (0x00000002U)                   /* Control */
#define USB_OHCI_BULKLISTFILLED                (0x00000004U)                   /* Bulk */

/* For usb_hstd_ohci_scan_containing_record() */
#define USB_OHCI_CR_ENDPOINT_DESCRIPTOR        (0U)                            /* ENDPOINT DESCRIPTOR */
#define USB_OHCI_CR_TRANSFER_DESCRIPTOR        (1U)                            /* TRANSFER DESCRIPTOR */

#define USB_OHCI_CR_LINK                       (0U)                            /* LINK */
#define USB_OHCI_CR_REQUESTLIST                (1U)                            /* REQUEST LIST */
#define USB_OHCI_CR_PAUSEDLINK                 (2U)                            /* PAUSED LINK */

/* Others */
#define USB_OHCI_HC_INT_SOF                    (0x00000004U)                   /* SOF */

/* InterruptStatus */
#define USB_OHCI_IS_SCHEDULINGOVERRUN          (0x00000001U)                   /* SCHEDULINGOVERRUN */
#define USB_OHCI_IS_WRITEBACKDONEHEAD          (0x00000002U)                   /* WRITEBACKDONEHEAD */
#define USB_OHCI_IS_STARTOFFRAME               (0x00000004U)                   /* STARTOFFRAME */
#define USB_OHCI_IS_RESUMEDETECTED             (0x00000008U)                   /* RESUMEDETECTED */
#define USB_OHCI_IS_UNRECOVERABLERROR          (0x00000010U)                   /* UNRECOVERABLERROR */
#define USB_OHCI_IS_FRAMENUMBEROVERRUN         (0x00000020U)                   /* FRAMENUMBEROVERRUN */
#define USB_OHCI_IS_ROOTHUBSTATUSCHANGE        (0x00000040U)                   /* ROOTHUBSTATUSCHANGE */
#define USB_OHCI_IS_OWNERSHIPCHANGE            (0x40000000U)                   /* OWNERSHIPCHANGE */
#define USB_OHCI_IS_MASTERINTENABLE            (0x80000000U)                   /* MASTERINTENABLE */

#define USB_OHCI_ISOCHRONOUSENABLE             (0x00000008U)                   /* ISOCHRONOUS ENABLE */
#define USB_OHCI_BULKLISTENABLE                (0x00000020U)                   /* BULK LIST ENABLE */
#define USB_OHCI_CONTROLLISTENABLE             (0x00000010U)                   /* CONTROL LIST ENABLE */
#define USB_OHCI_INTERRUPTLISTENABLE           (0x00000004U)                   /* PERIODIC LIST ENABLE */
#define USB_OHCI_PERIODICLISTENABLE            (0x0000000CU)                   /* PERIODIC+ISO LIST ENABLE */

/* The different ED lists are as follows. */
#define USB_OHCI_ED_INTERRUPT_1ms              (0U)                            /* For Interrupt(Polling rate = 1ms) */
#define USB_OHCI_ED_INTERRUPT_2ms              (1U)                            /* For Interrupt(Polling rate = 2ms) */
#define USB_OHCI_ED_INTERRUPT_4ms              (3U)                            /* For Interrupt(Polling rate = 4ms) */
#define USB_OHCI_ED_INTERRUPT_8ms              (7U)                            /* For Interrupt(Polling rate = 8ms) */
#define USB_OHCI_ED_INTERRUPT_16ms             (15U)                           /* For Interrupt(Polling rate = 16ms) */
#define USB_OHCI_ED_INTERRUPT_32ms             (31U)                           /* For Interrupt(Polling rate = 32ms) */
#define USB_OHCI_ED_CONTROL                    (63U)                           /* For Control */
#define USB_OHCI_ED_BULK                       (64U)                           /* For Bulk */
#define USB_OHCI_ED_ISOCHRONOUS                (0U)                            /* For Isochronous( same as 1ms interrupt queue ) */
#define USB_OHCI_NO_ED_LISTS                   (65U)                           /* No ED Lists */
#define USB_OHCI_ED_EOF                        (0xffU)                         /* ED EOF */

#define USB_OHCI_HCED_HEADP_HALT               (0x00000001U)                   /* hardware stopped bit */
#define USB_OHCI_HCED_HEADP_CARRY              (0x00000002U)                   /* hardware toggle carry bit */

#define USB_VAL_32                             (32U)

/************************************************************************/
/*                      Typedefs for OHCI                               */
/************************************************************************/
#define boolean_t                              uint8_t

/************************************/
/*  5.2.2 Miscellaneous Definitions */
/************************************/
/* Doubly linked list */
typedef struct usb_ohci_list_entry
{
    struct usb_ohci_list_entry * f_link;
    struct usb_ohci_list_entry * b_link;
} st_usb_ohci_list_entry_t, * st_usb_ohci_list_entry_p_t;

/****************************************************/
/*  5.2.3 Host Controller Descriptors Definitions   */
/****************************************************/
typedef struct  usb_ohci_hc_endpoint_control
{
    uint32_t fa : 7;                   /*  Function Address    7bit    0-127   */
    uint32_t en : 4;                   /*  EndPoint No.        4bit    0-15    */
    uint32_t d  : 2;                   /*  Direction           2bit    D=0,3:get direction from TD ,   */
                                       /*                              D=1:OUT , D=2:IN                */
    uint32_t s   : 1;                  /*  SPEED               1bit    S=0:full speed,S=1:low speed    */
    uint32_t k   : 1;                  /*  SKIP                1bit    K=1:go to the next ED           */
    uint32_t f   : 1;                  /*  Format              1bit    F=1:isochronous,F=0:others      */
    uint32_t mps : 16;                 /*  MaxPacketSize       11bit   0-2047  */
} st_usb_ohci_hc_endpoint_control_t, * st_usb_ohci_hc_endpoint_control_p_t;

typedef struct  usb_ohci_hc_transfer_control
{
    uint32_t dummy1 : 16;              /*  dummy   */
    uint32_t dummy  : 2;               /*  dummy   */
    uint32_t r      : 1;               /*  Buffer Rounding  0:must exactly define buffer               */
                                       /*                   1: smaller packet than buffer not error    */
    uint32_t dp : 2;                   /*  Direction PID  00:SETUP,    */
                                       /*                 01:OUT ,     */
                                       /*                 10:IN  ,     */
                                       /*                 11:reserved  */
    uint32_t int_d  : 3;               /*  Delay Interrupt    111:no interrupt */
    uint32_t toggle : 2;               /*  Data toggle, T= 0x:acquied from ED toggle carry, */
                                       /*                  10:DATA0,   */
                                       /*                  11:DATA1    */
    uint32_t ec : 2;                   /*  Error count */
    uint32_t cc : 4;                   /*  condition code  */
} st_usb_ohci_hc_transfer_control_t, * st_usb_ohci_hc_transfer_control_p_t;

/* Host Controller Endpoint Descriptor, refer to Section 4.2, Endpoint Descriptor */
typedef struct usb_ohci_hc_endpoint_descriptor
{
    st_usb_ohci_hc_endpoint_control_t control; /* dword 0 */
    uint32_t tail_p;                           /* physical pointer to USB_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t head_p;                           /* flags + phys ptr to USB_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t next_ed;                          /* phys ptr to USB_OHCI_HC_ENDPOINT_DESCRIPTOR */
} st_usb_ohci_hc_endpoint_descriptor_t, * st_usb_ohci_hc_endpoint_descriptor_p_t;

/* Host Controller Transfer Descriptor, refer to Section 4.3, Transfer Descriptors */
typedef struct usb_ohci_hc_transfer_descriptor
{
    struct usb_ohci_hc_transfer_control control; /* dword 0 */
    uint32_t cbp;                                /* current buffer pointer */
    uint32_t next_td;                            /* phys ptr to USB_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t be;                                 /* buffer end */
} st_usb_ohci_hc_transfer_descriptor_t, * st_usb_ohci_hc_transfer_descriptor_p_t;

/********************************************************/
/*  5.2.6 Host Controller Driver Internal Definitions   */
/********************************************************/
/* OHCI USBD Transfer Request */
typedef struct usb_ohci_request
{
    uint8_t                * buffer;         /* Pointer to data to be transferred */
    uint32_t                 buffer_length;  /* Length of data buffer in bytes */
    uint8_t                  xferinfo;       /* Direction (In/Out) for control and bulk */
    uint8_t                  max_int_delay;  /* Maximum allowable delay from completion to completion notification to USBD */
    uint8_t                  shortxferok;    /* Transfer of less than BufferLength is to be treated as an error unless this is TRUE */
    uint8_t                  using_flag;     /* Using Flag */
    uint8_t                * setup;          /* Data for setup packet (control endpoints only) */
    uint32_t                 status;         /* Completion status from HCD to USBD */
    st_usb_ohci_list_entry_t hcd_list;       /* List of all HCD_TRANSFER_DESCRIPTORs in use for this request */
    uint32_t               * driver_request; /* Driver Request Infomation pointer */
} st_usb_ohci_request_t, * st_usb_ohci_request_p_t;

/* Each Host Controller Endpoint Descriptor is also doubly linked into a list tracked by HCD. */
/* Each ED queue is managed via an USB_OHCI_HCD_ED_LIST */
typedef struct usb_ohci_hcd_ed_list
{
    st_usb_ohci_list_entry_t head;          /* Head of an HCD_ENDPOINT_DESCRIPTOR list */
    uint32_t               * physical_head; /* Address of location to put the physical head pointer when it changes */
    uint16_t                 bandwidth;     /* Allocated bandwidth on this timeslice. */
    uint8_t next;                           /* Index to the next HCD_ED_LIST for this timeslice */
    uint8_t reserved;
} st_usb_ohci_hcd_ed_list_t, * st_usb_ohci_hcd_ed_list_p_t;

/* Refer to Section 4.4 Host Controller Communications Area */
typedef struct usb_ohci_hcca_block
{
    uint32_t hcca_interrupt_list[USB_VAL_32];
    uint32_t hcca_frame_number : 16;
    uint32_t hcca_pad1         : 16;
    uint32_t hcca_done_head;
    uint32_t reserve[30];
} st_usb_ohci_hcca_block_t, * st_usb_ohci_hcca_block_p_t;

/* HCD Device Data (Refer to Section 5.2.6 Host Controller Driver Internal Definitions) */
typedef struct usb_ohci_hcd_device_data
{
    st_usb_ohci_hcca_block_p_t hcca;
    st_usb_ohci_list_entry_t   endpoints;
    st_usb_ohci_list_entry_t   freeed;
    st_usb_ohci_list_entry_t   freetd;
    st_usb_ohci_list_entry_t   stalled_ed_reclamation;
    st_usb_ohci_list_entry_t   running_ed_reclamation;
    st_usb_ohci_list_entry_t   paused_ed_restart;
    st_usb_ohci_hcd_ed_list_t  ed_list[USB_OHCI_NO_ED_LISTS];
    uint32_t frame_high_part;
    uint32_t available_bandwidth;
    uint32_t max_bandwidth_inuse;
    uint32_t socount;
    uint32_t sostallframe;
    uint32_t solimitframe;
    uint8_t  solimithit;
    uint8_t  sostallhit;
    uint8_t  root_device[USB_OHCI_MAXROOTPORTS];
    uint8_t  using_flag;
} st_usb_ohci_hcd_device_data_t, * st_usb_ohci_hcd_device_data_p_t;

/************************************/
/*  5.2.5 Host Controller Endpoints */
/************************************/
/* Ext. struct */
typedef struct usb_ohci_iso_transfer_info
{
    uint16_t  size;                    /* Transfer Size */
    uint8_t   control_cc;              /* Control CC */
    uint8_t   psw_cc;                  /* PSW CC */
    uint8_t * buffer;                  /* Data Buffer pointer */
} st_usb_ohci_iso_transfer_info_t, * st_usb_ohci_iso_transfer_info_p_t;

/* Ext. struct */
typedef struct usb_ohci_iso_info
{
    uint8_t                         using_flag;                            /* Using Flag */
    uint8_t                         copy_cnt;                              /* Data copy count (for out) */
    uint8_t                         start_cnt;                             /* Transfer start count (for in/out) */
    uint8_t                         end_cnt;                               /* Transfer end count (for in/out) */
    uint8_t                         copy_p;                                /* Data copy infomaition pointer (for out) */
    uint8_t                         start_p;                               /* Transfer start infomaition pointer (for in/out) */
    uint8_t                         end_p;                                 /* Transfer end infomaition pointer (for in/out) */
    uint8_t                         complete_p;                            /* Transfer complete information pointer (for in/out) */
    st_usb_ohci_request_p_t         usb_drequest;                          /* USBD Transfer Request pointer */
    st_usb_ohci_iso_transfer_info_t transfer_info[USB_OHCI_ISO_MAX_FRAME]; /* Transfer Infomation */
} st_usb_ohci_iso_info_t, * st_usb_ohci_iso_info_p_t;

typedef struct usb_ohci_hcd_endpoint
{
    uint8_t type;
    uint8_t list_index;
    uint8_t using_flag;
    uint8_t reserved[1];
    st_usb_ohci_hcd_device_data_p_t           device_data;
    st_usb_ohci_hc_endpoint_control_t         control;
    struct usb_ohci_hcd_endpoint_descriptor * hcd_ed;
    struct usb_ohci_hcd_transfer_descriptor * hcd_head_p;
    struct usb_ohci_hcd_transfer_descriptor * hcd_tail_p;
    uint32_t                 rate;
    uint32_t                 bandwidth;
    uint32_t                 max_packet;
    st_usb_ohci_list_entry_t link;     /* Ext. */
    st_usb_ohci_iso_info_p_t iso_info; /* Ext. */
} st_usb_ohci_hcd_endpoint_t, * st_usb_ohci_hcd_endpoint_p_t;

/********************************************************/
/*  5.2.4 Host Controller Driver Descriptor Definitions */
/********************************************************/
/* HCD Endpoint Descriptor */
typedef struct usb_ohci_hcd_endpoint_descriptor
{
    uint8_t                              list_index;
    uint8_t                              paused_flag;
    uint8_t                              using_flag;
    uint8_t                              reserved[1];
    uint32_t                             physical_address;
    st_usb_ohci_list_entry_t             link;
    st_usb_ohci_hcd_endpoint_p_t         endpoint;
    uint32_t                             reclamation_frame;
    st_usb_ohci_list_entry_t             paused_link;
    st_usb_ohci_hc_endpoint_descriptor_t hc_ed;
} st_usb_ohci_hcd_endpoint_descriptor_t, * st_usb_ohci_hcd_endpoint_descriptor_p_t;

/* HCD Transfer Descriptor */
typedef struct usb_ohci_hcd_transfer_descriptor
{
    uint8_t  td_status;
    uint8_t  using_flag     : 4;
    uint8_t  cancel_pending : 4;
    uint32_t physical_address;
    struct usb_ohci_hcd_transfer_descriptor * next_hcd_td;
    st_usb_ohci_list_entry_t     request_list;
    struct usb_ohci_request    * usb_drequest;
    st_usb_ohci_hcd_endpoint_p_t endpoint;
    uint32_t transfer_count;
    st_usb_ohci_hc_transfer_descriptor_t hc_td;
    uint32_t dummy[4];
} st_usb_ohci_hcd_transfer_descriptor_t, * st_usb_ohci_hcd_transfer_descriptor_p_t;

/* HCD Transfer Descriptor for Isochronous */
typedef struct usb_ohci_hc_iso_transfer_control
{
    uint32_t sf     : 16;              /*  StartingFrame   */
    uint32_t dummy2 : 5;
    uint32_t int_d  : 3;               /*  Delay Interrupt    111:no interrupt */
    uint32_t fc     : 3;               /*  Frame count */
    uint32_t dummy  : 1;
    uint32_t cc     : 4;               /*  condition code  */
} st_usb_ohci_hc_iso_transfer_control_t, * st_usb_ohci_hc_iso_transfer_control_p_t;

typedef union usb_ohci_offset_psw
{
    struct
    {
        uint16_t size  : 11;
        uint16_t dummy : 1;
        uint16_t cc    : 4;
    } psw;
    struct
    {
        uint16_t offset : 13;
        uint16_t cc     : 3;
    } offset;
} u_usb_ohci_offset_psw_t, * u_usb_ohci_offset_psw_p_t;

typedef struct usb_ohci_hc_iso_transfer_descriptor
{
    struct usb_ohci_hc_iso_transfer_control control;
    uint32_t                bp0;
    uint32_t                next_td;
    uint32_t                be;
    u_usb_ohci_offset_psw_t usb_ohci_offset_psw[8];
} st_usb_ohci_hc_iso_transfer_descriptor_t, * st_usb_ohci_hc_iso_transfer_descriptor_p_t;

#endif                                 /* R_USB_HOHCI_TYPEDEF_H */
