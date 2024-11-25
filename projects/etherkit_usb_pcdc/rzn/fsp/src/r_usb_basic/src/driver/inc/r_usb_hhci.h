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
 * File Name    : r_usb_hhci.h
 * Version      : 1.0
 * Description  : Header file USB HCI
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USB_HHCI_H
#define R_USB_HHCI_H

#include "r_usb_basic_local.h"

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#define USB_HCI_TRREQ_NUM              (USB_MAXPIPE)     /* Max Transfer Request Number */
#define USB_HCI_DEVICE_NUM             (USB_MAXDEVADDR)  /* Max Device Number */

#define USB_HCI_CONFIGDESC_BUFFSIZE    (USB_CONFIGSIZE)  /* Configuration Descriptor Buffer Size */

#define USB_HCI_HUBPORT_NUM            (USB_HUBDOWNPORT) /* Hub port Number */

#define USB_HCI_HCTYPE_OHCI            (1U)              /* HCI type is OHCI */
#define USB_HCI_HCTYPE_EHCI            (2U)              /* HCI type is EHCI */

#define USB_HCI_DIRECTION_OUT          (0U)              /* Direction is output */
#define USB_HCI_DIRECTION_IN           (1U)              /* Direction is input */

#define USB_HCI_DEVSPEED_FS            (0U)              /* Full speed */
#define USB_HCI_DEVSPEED_LS            (1U)              /* Low speed */
#define USB_HCI_DEVSPEED_HS            (2U)              /* High speed */

#define USB_HCI_SETUP_DATA_SIZE        (8U)              /* SetUp data size */

#define USB_HCI_NODEVICE               (0xFF)            /* No Device Define */

#define USB_HCI_GET_ENDIAN             (0U)              /* Endian Setting */

#define RZA2_USB_OHCI_IP               (1)

/***************************************(*******)*************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct
{
    uint8_t  blength;
    uint8_t  bdescriptor_type;
    uint16_t bcd_usb;
    uint8_t  bdevice_class;
    uint8_t  bdevice_sub_class;
    uint8_t  bdevice_protocol;
    uint8_t  bmax_packet_size_0;
    uint16_t id_vendor;
    uint16_t id_product;
    uint16_t bcd_device;
    uint8_t  imanufacturer;
    uint8_t  iproduct;
    uint8_t  iserial_number;
    uint8_t  bnum_configurations;
} st_usb_hci_device_descriptor_t;

typedef struct
{
    uint8_t  blength;
    uint8_t  bdescriptor_type;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} st_usb_hci_config_descriptor_t;

typedef struct
{
    uint8_t blength;
    uint8_t bdescriptor_type;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} st_usb_hci_interface_descriptor_t;

typedef struct
{
    uint8_t  blength;
    uint8_t  bdescriptor_type;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
} st_usb_hci_endpoint_descriptor_t;

typedef struct
{
    struct
    {
        uint32_t devnum         : 7;                         /* Device Number */
        uint32_t disconnect_req : 1;                         /* Disconnect Request Flag */
        uint32_t hubaddr        : 7;                         /* Hub Address */
        uint32_t sprit_flag     : 1;                         /* Sprit Transaction Flag */
        uint32_t hubportnum     : 7;                         /* Hub Port Number */
        uint32_t                : 1;
        uint32_t hctype         : 2;                         /* HCI Type */
        uint32_t devspeed       : 2;                         /* Device Speed */
        uint32_t                : 3;
        uint32_t enable         : 1;                         /* Enbale Flag */
    } bit;
    st_usb_hci_device_descriptor_t devicedesc;               /* Device Discriptor */
    uint32_t rawconfigdesc[USB_HCI_CONFIGDESC_BUFFSIZE / 4]; /* Configuration Descriptor Raw Data */
    struct
    {
        uint8_t devnum[USB_HCI_HUBPORT_NUM];                 /* Device Number for HUB downport device */
        uint8_t devspeed[USB_HCI_HUBPORT_NUM];               /* Device Speed for HUB downport device */
    }      hubdevinfo;
    void * hci_device;                                       /* HCI Characteristic Device Infomation pointer */
} st_usb_hci_device_t;

typedef struct
{
    struct
    {
        uint32_t devadrs    : 7;         /* Device Address */
        uint32_t cancel_req : 1;         /* Cancel Transfer Request Flag */
        uint32_t epnum      : 4;         /* Endpoint Number */
        uint32_t            : 3;
        uint32_t direction  : 1;         /* Direction */
        uint32_t mps        : 11;        /* Max Packet Size */
        uint32_t            : 2;
        uint32_t eptype     : 2;         /* Endpoint Type */
        uint32_t enable     : 1;         /* Enable Flag */
    }                     bit;
    uint32_t              status;        /* Status */
    st_usb_hci_device_t * devinfo;       /* Device Infomation Pointer */
    uint32_t              pollrate : 14; /* Polling rate(for interrupt/isochronous transfer) */
    uint32_t                       : 18;
    uint32_t * setupbuf;                 /* SETUP Data Pointer(for control transfer only) */
    uint32_t   databuf;                  /* Transfer Data Buffer */
    uint32_t   trsize;                   /* Transfer Request Size */
    uint32_t   actual_size;              /* Actual Transfer Size */
    void     * utr_p;                    /* UTR data pointer */
    void     * hci_info;                 /* HCI Characteristic Infomation pointer */
} st_usb_hci_tr_req_t;

/*typedef struct
 * {
 *  void    (*attach_cb)(uint16_t port_no);              Attach Call Back Function Pointer
 *  void    (*detach_cb)(uint16_t port_no);              Detach Call Back Function Pointer
 *  void    (*over_current_cb)(uint16_t port_no);        Over Current Call Back Function Pointer
 *  void    (*port_reset_cb)(uint16_t port_no);          Port Reset Call Back Function Pointer
 *  void    (*tr_end_cb)(void *p_utr, uint32_t actual_size, uint16_t status);    Transfer End Call Back Function Pointer
 * } st_usb_hci_cb_info_t;*/

typedef struct
{
    void (* attach_cb)(usb_utr_t * p_utr);                                                    /* Attach Call Back Function Pointer */
    void (* detach_cb)(usb_utr_t * p_utr);                                                    /* Detach Call Back Function Pointer */
    void (* over_current_cb)(void * p_utr);                                                   /* Over Current Call Back Function Pointer */
    void (* port_reset_cb)(usb_utr_t * p_utr);                                                /* Port Reset Call Back Function Pointer */
    void (* tr_end_cb)(usb_utr_t * ptr, void * p_utr, uint32_t actual_size, uint16_t status); /* Transfer End Call Back Function Pointer */
} st_usb_hci_cb_info_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern uint32_t usb_hcrh_port_status1;
extern uint32_t usb_hcrh_descriptor_a;
extern uint32_t usb_hcrh_descriptor_b;
extern uint32_t usb_portsc1;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/* function */
int32_t  r_usb_hstd_hci_init(st_usb_hci_cb_info_t * cb_info);
uint16_t r_usb_hstd_hci_make_transfer_request(void   * p_utr,
                                              void   * setup,
                                              uint32_t devaddr,
                                              uint32_t epnum,
                                              uint32_t eptype,
                                              uint32_t epdir,
                                              uint32_t tranlen,
                                              uint32_t tranadr,
                                              uint32_t mps);
void r_usb_hstd_hci_cancel_transfer_request(usb_utr_t * ptr,
                                            uint32_t    devaddr,
                                            uint32_t    epnum,
                                            uint32_t    eptype,
                                            uint32_t    epdir);
void                  r_usb_hstd_hci_disconnect(usb_utr_t * ptr, uint32_t devadd);
uint16_t              r_usb_hstd_hci_get_device_address_of_rootpoot(uint16_t rootport);
uint16_t              r_usb_hstd_hci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t devaddr);
uint32_t              r_usb_hstd_hci_get_device_speed(uint16_t devaddr);
st_usb_hci_device_t * r_usb_hstd_hci_get_device_infomation(uint16_t devaddr);
void                  r_usb_hstd_hci_port_reset(usb_utr_t * p_utr);
void                  r_usb_hstd_hci_port_suspend(uint16_t rootport);
void                  r_usb_hstd_hci_port_resume(uint16_t rootport);
void                  r_usb_hstd_hci_set_hub_info(uint32_t hubaddr,
                                                  uint32_t hubportnum,
                                                  uint32_t devaddr,
                                                  uint32_t devspeed);
void r_usb_hstd_hci_set_device_address_of_hub_port(uint16_t hubaddr, uint16_t devaddr);
void r_usb_hstd_hci_electrical_test(uint16_t rootport, uint8_t mode);
void r_usb_hstd_hci_wait_time(uint32_t ms);

void r_usb_module_init(void);
void r_usb_tusb320hi_host_setting(void);

#endif                                 /* #ifndef R_USB_HHCI_H */
