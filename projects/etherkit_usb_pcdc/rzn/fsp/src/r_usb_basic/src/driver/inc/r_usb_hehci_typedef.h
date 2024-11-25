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
 * File Name    : r_usb_hehci_typedef.h
 * Version      : 1.0
 * Description  : EHCI Type Define Header
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USB_HEHCITYPEDEF_H
#define R_USB_HEHCITYPEDEF_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define USB_EHCI_VERSON            (0x0110U) /* HCIVERSION */

#define USB_EHCI_MAXROOTPORTS      (1U)      /* The maximum number of route ports */

#define USB_EHCI_LINE_STATE_SE0    (0U)      /* USB State : SE0 */
#define USB_EHCI_LINE_STATE_J      (2U)      /* USB State : J-state */
#define USB_EHCI_LINE_STATE_K      (1U)      /* USB State : K-state */

#define USB_EHCI_TYP_ITD           (0U)      /* Isochronous Transfer Descriptor */
#define USB_EHCI_TYP_QH            (1U)      /* Queue Head */
#define USB_EHCI_TYP_SITD          (2U)      /* Split Transaction Isochronous Transfer Descriptor */
#define USB_EHCI_TYP_FSTN          (3U)      /* Periodic Frame Span Traversal Node */

#define USB_EHCI_TP_ALL            (0U)      /* Transaction position : All */
#define USB_EHCI_TP_BEGIN          (1U)      /* Transaction position : Begin */
#define USB_EHCI_TP_MID            (2U)      /* Transaction position : Middle */
#define USB_EHCI_TP_END            (3U)      /* Transaction position : End */

#define USB_EHCI_PID_OUT           (0U)      /* PID : OUT */
#define USB_EHCI_PID_IN            (1U)      /* PID : IN */
#define USB_EHCI_PID_SETUP         (2U)      /* PID : SETUP */

#define USB_EHCI_MAXQTDSIZE        (20480U)  /* The maximum transfer size of QTD */

#define USB_EHCI_SITD_DATA_SIZE    (188U)    /* Maximum size that can be transed with one micro frame of SITD */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Refer to EHCI Spec.3.1 */
typedef union usb_ehci_flep_tag
{
    struct
    {
        uint32_t t       : 1;
        uint32_t typ     : 2;
        uint32_t         : 2;
        uint32_t pointer : 27;
    } bit;
    union usb_ehci_flep_tag * pointer;
    uint32_t address;
} u_usb_ehci_flep_t;

/* Refer to EHCI Spec.3.5.3 */
typedef union usb_ehci_transfer_info_tag
{
    struct
    {
        uint32_t status_ping_state              : 1;
        uint32_t status_split_transaction_state : 1;
        uint32_t status_missed_micro_frame      : 1;
        uint32_t status_transaction_error       : 1;
        uint32_t status_babble_detected         : 1;
        uint32_t status_data_buffer_error       : 1;
        uint32_t status_halted                  : 1;
        uint32_t status_active                  : 1;
        uint32_t pid_code                       : 2;
        uint32_t cerr                           : 2;
        uint32_t c_page                         : 3;
        uint32_t ioc                            : 1;
        uint32_t total_bytes_to_transfer        : 15;
        uint32_t data_toggle                    : 1;
    }        bit;
    uint32_t dword;
} u_usb_ehci_transfer_info_t;

/* Queue Head */
typedef struct usb_ehci_qh_tag
{
    /* Refer to EHCI Spec.3.6.1 */
    u_usb_ehci_flep_t queue_head_horizontal_link;

    /* Refer to EHCI Spec.3.6.2 */
    union endpoint1_tag
    {
        struct
        {
            uint32_t device_address        : 7;
            uint32_t i                     : 1;
            uint32_t endpt                 : 4;
            uint32_t eps                   : 2;
            uint32_t dtc                   : 1;
            uint32_t h                     : 1;
            uint32_t maximum_packet_length : 11;
            uint32_t c                     : 1;
            uint32_t rl                    : 4;
        }        bit;
        uint32_t dword;
    } endpoint1;

    /* Refer to EHCI Spec.3.6.2 */
    union endpoint2_tag
    {
        struct
        {
            uint32_t mframe_smask : 8;
            uint32_t mframe_cmask : 8;
            uint32_t hub_addr     : 7;
            uint32_t port_number  : 7;
            uint32_t mult         : 2;
        }        bit;
        uint32_t dword;
    } endpoint2;

    union
    {
        struct
        {
            uint32_t         : 5;
            uint32_t pointer : 27;
        } bit;
        struct usb_ehci_qtd_tag * pointer;
        uint32_t address;
    } current_qtd;

    union
    {
        struct
        {
            uint32_t t       : 1;
            uint32_t         : 4;
            uint32_t pointer : 27;
        } bit;
        struct usb_ehci_qtd_tag * pointer;
        uint32_t address;
    } next_qtd;

    union
    {
        struct
        {
            uint32_t t       : 1;
            uint32_t nakcnt  : 4;
            uint32_t pointer : 27;
        } bit;
        struct usb_ehci_qtd_tag * pointer;
        uint32_t address;
    } alternate_next_qtd;

    /* Refer to EHCI Spec.3.5.3 */
    u_usb_ehci_transfer_info_t transfer_info;

    /* Refer to EHCI Spec.3.6.3 */
    union
    {
        struct
        {
            uint32_t current_offset : 12;
            uint32_t pointer        : 20;
        } bit;
        struct
        {
            uint32_t c_prog_mask : 8;
            uint32_t             : 4;
            uint32_t pointer     : 20;
        } bit1;
        struct
        {
            uint32_t frame_tag : 5;
            uint32_t s_bytes   : 7;
            uint32_t pointer   : 20;
        }        bit2;
        uint32_t address;
    } buffer[5];

// hcd-related
    uint32_t reserve[1];
    struct
    {
        uint32_t pollrate        : 14;  /* pollrate */
        uint32_t direction       : 2;   /* direction */
        uint32_t pollrate_offset : 14;  /* pollrate offset */
        uint32_t tr_req_flag     : 1;   /* Transfer Request flag */
        uint32_t enable          : 1;   /* QH enable flag */
    } info;
    struct usb_ehci_qtd_tag * qtd_head; /* QTD head pointer */
    struct usb_ehci_qtd_tag * qtd_end;  /* QTD end pointer */
} st_usb_ehci_qh_t;

/* Queue Element Transfer Descriptor */
typedef struct usb_ehci_qtd_tag
{
    /* Refer to EHCI Spec.3.5.1 */
    union
    {
        struct
        {
            uint32_t t       : 1;
            uint32_t         : 4;
            uint32_t pointer : 27;
        } bit;
        struct usb_ehci_qtd_tag * pointer;
        uint32_t address;
    } next_qtd;

    /* Refer to EHCI Spec.3.5.2 */
    union
    {
        struct
        {
            uint32_t t       : 1;
            uint32_t         : 4;
            uint32_t pointer : 27;
        } bit;
        struct usb_ehci_qtd_tag * pointer;
        uint32_t address;
    } alternate_next_qtd;

    /* Refer to EHCI Spec.3.5.3 */
    u_usb_ehci_transfer_info_t transfer_info;

    /* Refer to EHCI Spec.3.5.4 */
    union
    {
        struct
        {
            uint32_t current_offset : 12;
            uint32_t pointer        : 20;
        }        bit;
        uint32_t address;
    } buffer[5];
} st_usb_ehci_qtd_t;

/* Isochronous Transfer Descriptor */
typedef struct  usb_ehci_itd_tag
{
    /* Refer to EHCI Spec.3.3.1 */
    u_usb_ehci_flep_t next_link;

    /* Refer to EHCI Spec.3.3.2 */
    union
    {
        struct
        {
            uint32_t offset                   : 12;
            uint32_t pg                       : 3;
            uint32_t ioc                      : 1;
            uint32_t length                   : 12;
            uint32_t status_transaction_error : 1;
            uint32_t status_babble_detected   : 1;
            uint32_t status_data_buffer_error : 1;
            uint32_t status_active            : 1;
        }        bit;
        uint32_t dword;
    } transaction[8];

    /* Refer to EHCI Spec.3.3.3 */
    union
    {
        struct
        {
            uint32_t device_address : 7;
            uint32_t                : 1;
            uint32_t endpt          : 4;
            uint32_t pointer        : 20;
        } bit0;
        struct
        {
            uint32_t maximum_packet_length : 11;
            uint32_t direction             : 1;
            uint32_t pointer               : 20;
        } bit1;
        struct
        {
            uint32_t multi   : 2;
            uint32_t         : 10;
            uint32_t pointer : 20;
        }        bit2;
        uint32_t address;
    } buffer[7];

// hcd-related
    uint32_t reserve[2];
    struct
    {
        uint32_t pollrate        : 14;             /* pollrate */
        uint32_t direction       : 2;              /* direction */
        uint32_t pollrate_offset : 14;             /* pollrate offset */
        uint32_t tr_req_flag     : 1;              /* Transfer Request flag */
        uint32_t enable          : 1;              /* QH enable flag */
    } info;

    uint32_t setup_cnt;                            /* setup count */
    uint32_t ini_setup_cnt;                        /* initialize setup count */
    uint32_t next_setup_uframe;                    /* next setup micro frame no */
    uint32_t next_active_uframe;                   /* next active micro frame no */
    uint32_t next_trend_uframe;                    /* next transfer end micro frame no */

    uint8_t tmp_buffer[8][USB_EHCI_ITD_DATA_SIZE]; /* transfe temporary buffer for ITD */
} st_usb_ehci_itd_t;

/* Split Transaction Isochronous Transfer Descriptor */
typedef struct usb_ehci_sitd_tag
{
    /* Refer to EHCI Spec.3.4.1 */
    u_usb_ehci_flep_t next_link;

    /* Refer to EHCI Spec.3.4.2 */
    union
    {
        struct
        {
            uint32_t device_address : 7;
            uint32_t                : 1;
            uint32_t endpt          : 4;
            uint32_t                : 4;
            uint32_t hub_addr       : 7;
            uint32_t                : 1;
            uint32_t port_number    : 7;
            uint32_t direction      : 1;
        }        bit;
        uint32_t dword;
    } endpoint1;

    /* Refer to EHCI Spec.3.4.2 */
    union
    {
        struct
        {
            uint32_t mframe_smask : 8;
            uint32_t mframe_cmask : 8;
            uint32_t              : 16;
        }        bit;
        uint32_t dword;
    } endpoint2;

    /* Refer to EHCI Spec.3.4.3 */
    union state_tag
    {
        struct
        {
            uint32_t                                : 1;
            uint32_t status_split_transaction_state : 1;
            uint32_t status_missed_micro_frame      : 1;
            uint32_t status_transaction_error       : 1;
            uint32_t status_babble_detected         : 1;
            uint32_t status_data_buffer_error       : 1;
            uint32_t status_err                     : 1;
            uint32_t status_active                  : 1;
            uint32_t c_prog_mask                    : 8;
            uint32_t total_bytes_to_transfer        : 10;
            uint32_t                                : 4;
            uint32_t page_select                    : 1;
            uint32_t ioc                            : 1;
        }        bit;
        uint32_t dword;
    } state;

    /* Refer to EHCI Spec.3.4.4 */
    union
    {
        struct
        {
            uint32_t current_offset : 12;
            uint32_t pointer        : 20;
        } bit0;
        struct
        {
            uint32_t t_count : 3;
            uint32_t tp      : 2;
            uint32_t         : 7;
            uint32_t pointer : 20;
        }        bit1;
        uint32_t address;
    } buffer[2];

    /* Refer to EHCI Spec.3.4.5 */
    u_usb_ehci_flep_t back;

// hcd-related
    uint32_t reserved[7];
    uint32_t mps;                      /* Max Packet Size */
    struct
    {
        uint32_t pollrate        : 14; /* pollrate */
        uint32_t direction       : 2;  /* direction */
        uint32_t pollrate_offset : 14; /* pollrate offset */
        uint32_t tr_req_flag     : 1;  /* Transfer Request flag */
        uint32_t enable          : 1;  /* QH enable flag */
    } info;
} st_usb_ehci_sitd_t;

#endif  /* R_USB_HEHCITYPEDEF_H */
