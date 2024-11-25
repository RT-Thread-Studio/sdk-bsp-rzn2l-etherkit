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

/******************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include <r_usb_basic.h>
#include <r_usb_basic_api.h>
#include <r_usb_basic_cfg.h>

#include "../driver/inc/r_usb_typedef.h"
#include "../driver/inc/r_usb_extern.h"
#include "inc/r_usb_bitdefine.h"
#include "inc/r_usb_reg_access.h"

#if (BSP_CFG_RTOS == 2)
 #include "../driver/inc/r_usb_cstd_rtos.h"
#endif                                 /* #if (BSP_CFG_RTOS == 2) */

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
 #include "inc/r_usb_dmac.h"
#endif                                 /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

/******************************************************************************
 * Macro definitions
 ******************************************************************************/

#if defined(USB_CFG_HHID_USE) || defined(USB_CFG_PHID_USE)
 #if USB_CFG_DTC == USB_CFG_ENABLE
  #error  In HID class, can not set USB_CFG_ENABLE to USB_CFG_DTC in r_usb_basic_cfg.h.
 #endif                                /* USB_CFG_DTC == USB_CFG_ENABLE */

 #if USB_CFG_DMA == USB_CFG_ENABLE
  #error  In HID class, can not set USB_CFG_ENABLE to USB_CFG_DMA in r_usb_basic_cfg.h.
 #endif                                /* USB_CFG_DMA == USB_CFG_ENABLE */

#endif                                 /* defined(USB_CFG_HHID_USE) || defined(USB_CFG_PHID_USE) */

#if USB_CFG_COMPLIANCE == USB_CFG_DISABLE
 #if USB_CFG_ELECTRICAL == USB_CFG_ENABLE
  #error  Can not set USB_CFG_DISABLE to USB_CFG_COMPLIANCE when setting USB_CFG_ENABLE to USB_CFG_ELECTRICAL \
    in r_usb_basic_cfg.h.

 #endif                                /* USB_CFG_ELECTRICAL == USB_CFG_ENABLE */
#endif                                 /* USB_CFG_COMPLIANCE == USB_CFG_DISABLE */

#if USB_CFG_BC == USB_CFG_DISABLE
 #if USB_CFG_DCP == USB_CFG_ENABLE
  #error  Can not set USB_CFG_DISABLE to USB_CFG_BC when setting USB_CFG_ENABLE to USB_CFG_DCP \
    in r_usb_basic_cfg.h.

 #endif                                /* USB_CFG_DCP == USB_CFG_ENABLE */
#endif                                 /* USB_CFG_BC == USB_CFG_DISABLE */

#if USB_CFG_DMA == USB_CFG_ENABLE
 #if USB_CFG_DTC == USB_CFG_ENABLE
  #error  Can not set USB_CFG_ENABLE to the definitions (USB_CFG_DMA and USB_CFG_DTC) at the same time \
    in r_usb_basic_cfg.h.

 #endif                                /* USB_CFG_DTC == USB_CFG_ENABLE */
#endif                                 /* USB_CFG_DMA == USB_CFG_ENABLE */

#if USB_CFG_MODE == USB_CFG_HOST
 #if defined(USB_CFG_PCDC_USE) || defined(USB_CFG_PHID_USE) || defined(USB_CFG_PMSC_USE) || defined(USB_CFG_PVNDR_USE)
  #error  Can not enable these definitions(USB_CFG_PCDC_USE/USB_CFG_PHID_USE/USB_CFG_PMSC_USE/USB_CFG_PVNDR_USE) \
    when setting USB_MODE_HOST to USB_CFG_MODE in r_usb_basic_cfg.h.

 #endif                                /* defined(USB_CFG_PCDC_USE || USB_CFG_PHID_USE || USB_CFG_PMSC_USE || USB_CFG_PVNDR_USE) */
#endif                                 /* USB_CFG_MODE == USB_MODE_HOST */

#if USB_CFG_MODE == USB_CFG_PERI
 #if defined(USB_CFG_HCDC_USE) || defined(USB_CFG_HHID_USE) || defined(USB_CFG_HMSC_USE) || defined(USB_CFG_HVNDR_USE)
  #error  Can not enable these definitions(USB_CFG_HCDC_USE/USB_CFG_HHID_USE/USB_CFG_HMSC_USE/USB_CFG_HVNDR_USE) \
    when setting USB_MODE_PERI to USB_CFG_MODE in r_usb_basic_cfg.h.

 #endif                                /* defined(USB_CFG_HCDC_USE || USB_CFG_HHID_USE || USB_CFG_HMSC_USE || USB_CFG_HVNDR_USE) */
#endif                                 /* USB_CFG_MODE == USB_MODE_PERI */

#if !defined(BSP_MCU_GROUP_RA6M3) && !defined(BSP_MCU_GROUP_RZN2L)
 #if USB_CFG_ELECTRICAL == USB_CFG_ENABLE
  #error  Can not set USB_CFG_ENABLE to USB_CFG_ELECTRICAL when using other than Hi-speed module in r_usb_basic_cfg.h.
 #endif                                /* USB_CFG_ELECTRICAL == USB_CFG_ENABLE */

#endif                                 /* !defined(BSP_MCU_GROUP_RA6M3) */

#if defined(BSP_MCU_GROUP_RZN2L)
 #define USB_VAL_50           (50U)
 #define USB_VAL_100          (100U)
 #define USB_VAL_450          (450U)
 #define USB_VAL_500          (500U)

 #define USB_VAL_SUSPM        (0x4000U)
 #define USB_VAL_INTENB0      (0x9D00U)
 #define USB_VAL_MSTPCRE_8    (0x00000100)
#endif

/******************************************************************************
 * Exported global variables (to be accessed by other files)
 ******************************************************************************/
usb_cfg_t * host_cfg;

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/
void usbfs_interrupt_handler(void);
void usbfs_resume_handler(void);
void usbfs_d0fifo_handler(void);
void usbfs_d1fifo_handler(void);
void usbhs_interrupt_handler(void);
void usbhs_d0fifo_handler(void);
void usbhs_d1fifo_handler(void);
void usbfs_usbi_isr(void);

#if  USB_IP_EHCI_OHCI == 1
void usb_ahb_pci_bridge_init(void);

#endif                                 /* USB_IP_EHCI_OHCI == 1 */

/******************************************************************************
 * Renesas Abstracted RSK functions
 ******************************************************************************/

/**************************************************************************//**
 * @brief This function stops USB module.
 *
 * @retval FSP_SUCCESS           Success.
 * @retval FSP_ERR_USB_BUSY      USB is in use.
 ******************************************************************************/
#if defined(BSP_MCU_GROUP_RZN2L)
fsp_err_t usb_module_start (uint8_t ip_type)
{
    FSP_PARAMETER_NOT_USED(ip_type);
 #if USB_CFG_MODE == USB_CFG_PERI
    uint8_t dummy_read;

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_SYSC_NS->MSTPCRE = R_SYSC_NS->MSTPCRE & 0x0000001F;
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    USB00->COMMCTRL_b.PERI = 1;
    USB00->USBCTR         &= ~0x00000002U; /* UTMI+PHY reset OFF */
    R_BSP_SoftwareDelay(USB_VAL_100, BSP_DELAY_UNITS_MICROSECONDS);

    USB_M0->SYSCFG0_b.DRPD = 0;

    USB_M0->SYSCFG0_b.USBE = 1;
    USB_M0->LPSTS          = USB_VAL_SUSPM;
    R_BSP_SoftwareDelay(USB_VAL_100, BSP_DELAY_UNITS_MICROSECONDS);

    USB_M0->INTENB0 = USB_VAL_INTENB0;
 #else
    uint8_t                dummy_read;
    volatile unsigned long dummy;
    volatile uint8_t       dummy_buf = 0;

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_SYSC_NS->MSTPCRE = 0x0000001F;
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

  #if !defined(BSP_MCU_GROUP_RZN2L)
    CPG.STBCR6.BYTE &= 0xFD;             /* Release module standby */
    dummy_buf        = CPG.STBCR6.BYTE;

   #if (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    USB00.PHYCLK_CTRL.LONG = 0x00000001; /* USBPHY select : USB_X1 */
   #else
    USB00.PHYCLK_CTRL.LONG = 0x00000000; /* USBPHY select : EXTAL */
   #endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */
  #else /* !defined(BSP_MCU_GROUP_RZN2L) */
    FSP_PARAMETER_NOT_USED(dummy_buf);
  #endif /* !defined(BSP_MCU_GROUP_RZN2L) */
    for (dummy = 0; dummy < (USB_VAL_450 * USB_VAL_500); dummy++)
    {
        ;                              /* Wait 500us */
    }

    USB00->COMMCTRL_b.PERI = 0;        /* Operation mode setting (0 : Host, 1 : Peri) */

    USB00->USBCTR &= ~0x00000002UL;    /* UTMI+PHY reset OFF */               /* UTMI+PHY reset release */

    for (dummy = 0; dummy < (USB_VAL_450 * USB_VAL_100); dummy++)
    {
        ;                              /* Wait 100us */
    }

    /* AHB-PCI bridge Register setting */
    usb_ahb_pci_bridge_init();

    /* Core defined register setting */

/*       USB00->REGEN_CG_CTRL.BIT.NONUSE_CLK_MSK  = 1;
 *     USB00.REGEN_CG_CTRL.BIT.HOST_CLK_MSK    = 0;
 *     USB00.REGEN_CG_CTRL.BIT.PERI_CLK_MSK    = 0;
 *
 *     USB00.SPD_CTRL.BIT.SUSPENDM_ENABLE      = 0;
 *     USB00.SPD_CTRL.BIT.SLEEPM_ENABLE        = 0;
 *     USB00.SPD_CTRL.BIT.WKCNNT_ENABLE        = 0;
 *     USB00.SPD_CTRL.BIT.GLOBAL_SUSPENDM_P1   = 0;*/
 #endif

    return FSP_SUCCESS;
}

#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
fsp_err_t usb_module_start (uint8_t ip_type)
{
    if (USB_IP0 == ip_type)
    {
        FSP_ERROR_RETURN(0 != R_MSTP->MSTPCRB_b.MSTPB11, FSP_ERR_USB_BUSY)

        /* Enable power for USB0 */
        R_BSP_MODULE_START(FSP_IP_USBFS, 0);
    }
    else
    {
 #if defined(BSP_MCU_GROUP_RA6M3)
        FSP_ERROR_RETURN(0 != R_MSTP->MSTPCRB_b.MSTPB12, FSP_ERR_USB_BUSY)

        /* Enable power for USBA */
        R_BSP_MODULE_START(FSP_IP_USBHS, 0);
 #else
        FSP_ERROR_RETURN(0 != R_MSTP->MSTPCRB_b.MSTPB12, FSP_ERR_USB_BUSY)

        /* Enable power for USBA */
        R_BSP_MODULE_START(FSP_IP_USBFS, 0);
 #endif                                /* defined(BSP_MCU_GROUP_RA6M3) */
    }

    return FSP_SUCCESS;
}

#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */

/******************************************************************************
 * End of function usb_module_start
 ******************************************************************************/

/**************************************************************************//**
 * @brief This function stops USB module.
 *
 * @retval FSP_SUCCESS           Success.
 * @retval FSP_ERR_USB_FAILED    The function could not be completed successfully.
 * @retval FSP_ERR_USB_NOT_OPEN  USB module is not open.
 * @retval FSP_ERR_USB_PARAMETER USB IP type is wrong.
 ******************************************************************************/
fsp_err_t usb_module_stop (uint8_t ip_type)
{
#if defined(BSP_MCU_GROUP_RZN2L)
    uint8_t dummy_read;
 #if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
    FSP_ERROR_RETURN(1 != R_SYSC_NS->MSTPCRE_b.MSTPCRE08, FSP_ERR_USB_FAILED)

    USB_M0->DVSTCTR0    = 0;
    USB_M0->DCPCTR      = USB_SQSET;
    USB_M0->PIPE_CTR[0] = 0;
    USB_M0->PIPE_CTR[1] = 0;
    USB_M0->PIPE_CTR[2] = 0;
    USB_M0->PIPE_CTR[3] = 0;
    USB_M0->PIPE_CTR[4] = 0;
    USB_M0->PIPE_CTR[5] = 0;
    USB_M0->PIPE_CTR[6] = 0;
    USB_M0->PIPE_CTR[7] = 0;
    USB_M0->PIPE_CTR[8] = 0;
    USB_M0->BRDYENB     = 0;
    USB_M0->NRDYENB     = 0;
    USB_M0->BEMPENB     = 0;
    USB_M0->INTENB0     = 0;
    USB_M0->INTENB1     = 0;
    USB_M0->SYSCFG0     = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DPRPU));
    USB_M0->SYSCFG0     = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DRPD));
    USB_M0->SYSCFG0     = (uint16_t) (USB_M0->SYSCFG0 & (~USB_USBE));
    USB_M0->SYSCFG0     = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DCFM));
    USB_M0->BRDYSTS     = 0;
    USB_M0->NRDYSTS     = 0;
    USB_M0->BEMPSTS     = 0;

    FSP_ERROR_RETURN(0 == R_SYSC_NS->MSTPCRE_b.MSTPCRE08, FSP_ERR_USB_NOT_OPEN)
 #endif                                /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */

    if (R_SYSC_NS->MSTPCRE_b.MSTPCRE08 == 1)
    {
        return FSP_ERR_USB_PARAMETER;
    }

    /* Disable power for USBA */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_SYSC_NS->MSTPCRE = R_SYSC_NS->MSTPCRE | USB_VAL_MSTPCRE_8;
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    dummy_read = R_SYSC_NS->MSTPCRE_b.MSTPCRE08;
    FSP_PARAMETER_NOT_USED(dummy_read);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
    FSP_PARAMETER_NOT_USED(ip_type);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    if (USB_IP0 == ip_type)
    {
        FSP_ERROR_RETURN(1 != R_MSTP->MSTPCRB_b.MSTPB11, FSP_ERR_USB_FAILED)

        USB_M0->DVSTCTR0    = 0;
        USB_M0->DCPCTR      = USB_SQSET;
        USB_M0->PIPE_CTR[0] = 0;
        USB_M0->PIPE_CTR[1] = 0;
        USB_M0->PIPE_CTR[2] = 0;
        USB_M0->PIPE_CTR[3] = 0;
        USB_M0->PIPE_CTR[4] = 0;
        USB_M0->PIPE_CTR[5] = 0;
        USB_M0->PIPE_CTR[6] = 0;
        USB_M0->PIPE_CTR[7] = 0;
        USB_M0->PIPE_CTR[8] = 0;
        USB_M0->BRDYENB     = 0;
        USB_M0->NRDYENB     = 0;
        USB_M0->BEMPENB     = 0;
        USB_M0->INTENB0     = 0;
        USB_M0->INTENB1     = 0;
 #if defined(BSP_MCU_GROUP_RZN2L)
        USB_M0->SYSCFG0 = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DPRPU));
        USB_M0->SYSCFG0 = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DRPD));
        USB_M0->SYSCFG0 = (uint16_t) (USB_M0->SYSCFG0 & (~USB_USBE));
        USB_M0->SYSCFG0 = (uint16_t) (USB_M0->SYSCFG0 & (~USB_DCFM));
 #else                                 /* defined(BSP_MCU_GROUP_RZN2L) */
        USB_M0->SYSCFG = (uint16_t) (USB_M0->SYSCFG & (~USB_DPRPU));
        USB_M0->SYSCFG = (uint16_t) (USB_M0->SYSCFG & (~USB_DRPD));
        USB_M0->SYSCFG = (uint16_t) (USB_M0->SYSCFG & (~USB_USBE));
        USB_M0->SYSCFG = (uint16_t) (USB_M0->SYSCFG & (~USB_DCFM));
 #endif                                /* defined(BSP_MCU_GROUP_RZN2L) */
        USB_M0->BRDYSTS = 0;
        USB_M0->NRDYSTS = 0;
        USB_M0->BEMPSTS = 0;

        FSP_ERROR_RETURN(0 == R_MSTP->MSTPCRB_b.MSTPB11, FSP_ERR_USB_NOT_OPEN)

        /* Disable power for USB0 */
        R_BSP_MODULE_STOP(FSP_IP_USBFS, 0);
    }
    else if (USB_IP1 == ip_type)
    {
 #if defined(BSP_MCU_GROUP_RA6M3)
        FSP_ERROR_RETURN(1 != R_MSTP->MSTPCRB_b.MSTPB12, FSP_ERR_USB_FAILED)

        USB_M1->DVSTCTR0    = 0;
        USB_M1->DCPCTR      = USB_SQSET;
        USB_M1->PIPE_CTR[0] = 0;
        USB_M1->PIPE_CTR[1] = 0;
        USB_M1->PIPE_CTR[2] = 0;
        USB_M1->PIPE_CTR[3] = 0;
        USB_M1->PIPE_CTR[4] = 0;
        USB_M1->PIPE_CTR[5] = 0;
        USB_M1->PIPE_CTR[6] = 0;
        USB_M1->PIPE_CTR[7] = 0;
        USB_M1->PIPE_CTR[8] = 0;
        USB_M1->BRDYENB     = 0;
        USB_M1->NRDYENB     = 0;
        USB_M1->BEMPENB     = 0;
        USB_M1->INTENB0     = 0;
        USB_M1->INTENB1     = 0;
        USB_M1->SYSCFG      = (uint16_t) (USB_M1->SYSCFG & (~USB_DPRPU));
        USB_M1->SYSCFG      = (uint16_t) (USB_M1->SYSCFG & (~USB_DRPD));
        USB_M1->SYSCFG      = (uint16_t) (USB_M1->SYSCFG & (~USB_USBE));
        USB_M1->SYSCFG      = (uint16_t) (USB_M1->SYSCFG & (~USB_DCFM));
        USB_M1->BRDYSTS     = 0;
        USB_M1->NRDYSTS     = 0;
        USB_M1->BEMPSTS     = 0;

        FSP_ERROR_RETURN(0 == R_MSTP->MSTPCRB_b.MSTPB12, FSP_ERR_USB_NOT_OPEN)

        /* Enable power for USBA */
        R_BSP_MODULE_STOP(FSP_IP_USBHS, 0);
 #endif                                /* defined(BSP_MCU_GROUP_RA6M3) */
    }
    else
    {
        return FSP_ERR_USB_PARAMETER;
    }
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */
    return FSP_SUCCESS;
}

/******************************************************************************
 * End of function usb_module_stop
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cpu_usbint_init
 * Description     : USB interrupt Initialize
 * Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 * Return value    : void
 ******************************************************************************/
void usb_cpu_usbint_init (uint8_t ip_type, usb_cfg_t const * const cfg)
{
#if defined(BSP_MCU_GROUP_RZN2L)
 #if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
    R_BSP_IrqDetectTypeSet(cfg->irq, 1);
 #else                                 /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)*/
    R_BSP_IrqDetectTypeSet(cfg->irq, 0);
 #endif  /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)*/
 #if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
    R_BSP_IrqDetectTypeSet(cfg->irq_d0, 1);
    R_BSP_IrqDetectTypeSet(cfg->irq_d1, 1);
 #endif                                /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */
    if (USB_IP0 == ip_type)
    {
#if (!defined(BSP_MCU_GROUP_RA4M1)) && (!defined(BSP_MCU_GROUP_RA2A1) && !defined(BSP_MCU_GROUP_RZN2L))

        /* Deep standby USB monitor register
         * b0      SRPC0    USB0 single end control
         * b1      PRUE0    DP Pull-Up Resistor control
         *         (Use this bit during operation in Deep Software Standby mode.)
         * b2      Reserved 0
         * b3      DRPD0    DP/DM Pull-Down Resistor control
         *         (Use this bit during operation in Deep Software Standby mode.)
         * b4      FIXPHY0  USB0 transceiver output fix
         * b15-b5   Reserved 0
         * b16     DP0      USB0 DP input
         * b17     DM0      USB0 DM input
         * b19-b18 Reserved 0
         * b20     DOVCA0   USB0 OVRCURA input
         * b21     DOVCB0   USB0 OVRCURB input
         * b22     Reserved 0
         * b23     DVBSTS0  USB1 VBUS input
         * b31-b24 Reserved 0
         */
        USB_M0->DPUSR0R_FS_b.FIXPHY0 = 0U; /* USB0 Transceiver Output fixed */
#endif /* (!defined(BSP_MCU_GROUP_RA4M1)) && (!defined(BSP_MCU_GROUP_RA2A1)) */

        /* Interrupt enable register
         * b0 IEN0 Interrupt enable bit
         * b1 IEN1 Interrupt enable bit
         * b2 IEN2 Interrupt enable bit
         * b3 IEN3 Interrupt enable bit
         * b4 IEN4 Interrupt enable bit
         * b5 IEN5 Interrupt enable bit
         * b6 IEN6 Interrupt enable bit
         * b7 IEN7 Interrupt enable bit
         */
#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
        R_BSP_IrqCfgEnable(cfg->irq_d0, cfg->ipl_d0, (void *) cfg); /* Enable D0FIFO interrupt */

        R_BSP_IrqCfgEnable(cfg->irq_d1, cfg->ipl_d1, (void *) cfg); /* Enable D1FIFO interrupt */
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

#if !defined(BSP_MCU_GROUP_RZN2L)
        R_BSP_IrqCfgEnable(cfg->irq_r, cfg->ipl_r, (void *) cfg);   /* USBR enable */
#endif /* !defined(BSP_MCU_GROUP_RZN2L) */
        host_cfg = (usb_cfg_t *) cfg;
        R_BSP_IrqCfgEnable(cfg->irq, cfg->ipl, (void *) cfg);       /* USBI enable */
    }

    if (ip_type == USB_IP1)
    {
#if defined(BSP_MCU_GROUP_RA6M3)

        /* Interrupt enable register
         * b0 IEN0 Interrupt enable bit
         * b1 IEN1 Interrupt enable bit
         * b2 IEN2 Interrupt enable bit
         * b3 IEN3 Interrupt enable bit
         * b4 IEN4 Interrupt enable bit
         * b5 IEN5 Interrupt enable bit
         * b6 IEN6 Interrupt enable bit
         * b7 IEN7 Interrupt enable bit
         */
 #if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
        R_BSP_IrqCfgEnable(cfg->hsirq_d0, cfg->hsipl_d0, (void *) cfg); /* Enable D0FIFO interrupt */

        R_BSP_IrqCfgEnable(cfg->hsirq_d1, cfg->hsipl_d1, (void *) cfg); /* Enable D1FIFO interrupt */
 #endif /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

        R_BSP_IrqCfgEnable(cfg->hsirq, cfg->hsipl, (void *) cfg);       /* USBIR enable */
        host_cfg = (usb_cfg_t *) cfg;
#endif /* defined(BSP_MCU_GROUP_RA6M3) */
    }
}

/******************************************************************************
 * End of function usb_cpu_usbint_init
 ******************************************************************************/

/******************************************************************************
 * TIMER function
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cpu_delay_1us
 * Description     : 1us Delay timer
 * Arguments       : uint16_t  time        ; Delay time(*1us)
 * Return value    : none
 * Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_1us (uint16_t time)
{
    R_BSP_SoftwareDelay((uint32_t) time, BSP_DELAY_UNITS_MICROSECONDS);
}

/******************************************************************************
 * End of function usb_cpu_delay_1us
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cpu_delay_xms
 * Description     : xms Delay timer
 * Arguments       : uint16_t  time        ; Delay time(*1ms)
 * Return value    : void
 * Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_xms (uint16_t time)
{
#if (BSP_CFG_RTOS == 0)
    R_BSP_SoftwareDelay((uint32_t) time, BSP_DELAY_UNITS_MILLISECONDS);
#endif                                 /* (BSP_CFG_RTOS == 0) */

#if (BSP_CFG_RTOS == 2)
    vTaskDelay((TickType_t) (time / portTICK_PERIOD_MS));

/* R_BSP_SoftwareDelay((uint32_t)time, BSP_DELAY_UNITS_MILLISECONDS); */
#endif                                 /* (BSP_CFG_RTOS == 2) */
}

/******************************************************************************
 * End of function usb_cpu_delay_xms
 ******************************************************************************/

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)

/******************************************************************************
 * Function Name   : usb_cpu_int_enable
 * Description     : USB Interrupt Enable
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void usb_cpu_int_enable (void)
{
    /* Interrupt enable register (USB0 USBIO enable)
     * b0 IEN0 Interrupt enable bit
     * b1 IEN1 Interrupt enable bit
     * b2 IEN2 Interrupt enable bit
     * b3 IEN3 Interrupt enable bit
     * b4 IEN4 Interrupt enable bit
     * b5 IEN5 Interrupt enable bit
     * b6 IEN6 Interrupt enable bit
     * b7 IEN7 Interrupt enable bit
     */
    R_BSP_IrqCfgEnable(host_cfg->irq, host_cfg->ipl, host_cfg); /* USBI enable */

 #if defined(BSP_MCU_GROUP_RA6M3)

    /* Interrupt enable register (USB1 USBIO enable)
     * b0 IEN0 Interrupt enable bit
     * b1 IEN1 Interrupt enable bit
     * b2 IEN2 Interrupt enable bit
     * b3 IEN3 Interrupt enable bit
     * b4 IEN4 Interrupt enable bit
     * b5 IEN5 Interrupt enable bit
     * b6 IEN6 Interrupt enable bit
     * b7 IEN7 Interrupt enable bit
     */
    R_BSP_IrqCfgEnable(host_cfg->hsirq, host_cfg->hsipl, host_cfg); /* USBIR enable */
 #endif /* defined(BSP_MCU_GROUP_RA6M3) */
}

/******************************************************************************
 * End of function usb_cpu_int_enable
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cpu_int_disable
 * Description     : USB Interrupt disable
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void usb_cpu_int_disable (void)
{
    /* Interrupt enable register (USB0 USBIO disable)
     * b0 IEN0 Interrupt enable bit
     * b1 IEN1 Interrupt enable bit
     * b2 IEN2 Interrupt enable bit
     * b3 IEN3 Interrupt enable bit
     * b4 IEN4 Interrupt enable bit
     * b5 IEN5 Interrupt enable bit
     * b6 IEN6 Interrupt enable bit
     * b7 IEN7 Interrupt enable bit
     */
    R_BSP_IrqDisable(host_cfg->irq);   /* USBI enable */

 #if defined(BSP_MCU_GROUP_RA6M3)

    /* Interrupt enable register (USB1 USBIO disable)
     * b0 IEN0 Interrupt enable bit
     * b1 IEN1 Interrupt enable bit
     * b2 IEN2 Interrupt enable bit
     * b3 IEN3 Interrupt enable bit
     * b4 IEN4 Interrupt enable bit
     * b5 IEN5 Interrupt enable bit
     * b6 IEN6 Interrupt enable bit
     * b7 IEN7 Interrupt enable bit
     */
    R_BSP_IrqDisable(host_cfg->hsirq); /* USBIR enable */
 #endif /* defined(BSP_MCU_GROUP_RA6M3) */
}

/******************************************************************************
 * End of function usb_cpu_int_disable
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_chattaring
 * Description     : Remove chattaring processing
 * Arguments       : uint16_t *syssts : SYSSTS register value
 * Return value    : LNST bit value
 ******************************************************************************/
uint16_t usb_chattaring (uint16_t * syssts)
{
    uint16_t lnst[4];

    /* WAIT_LOOP */
    while (1)
    {
        lnst[0] = (*syssts) & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[1] = (*syssts) & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[2] = (*syssts) & USB_LNST;
        if ((lnst[0] == lnst[1]) && (lnst[0] == lnst[2]))
        {
            break;
        }
    }

    return lnst[0];
}

/******************************************************************************
 * End of function usb_chattaring
 ******************************************************************************/

#endif                                 /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/*******************************************************************************
 * Function Name: usbfs_usbi_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/

void usbfs_usbi_isr (void)
{
    /* Call USB interrupt routine */
    if (USB_MODE_HOST == g_usb_usbmode[USB_IP0])
    {
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
 #if  USB_IP_EHCI_OHCI == 0
        usb_hstd_usb_handler();        /* Call interrupt routine */
 #endif /* USB_IP_EHCI_OHCI == 0 */
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
        usb_pstd_usb_handler();        /* Call interrupt routine */
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
}

/******************************************************************************
 * End of function usbfs_usbi_isr
 ******************************************************************************/

#if defined(BSP_MCU_GROUP_RA6M3)

/*******************************************************************************
 * Function Name: usbhs_usbir_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
static void usbhs_usbir_isr (void)
{
    /* Condition compilation by the difference of USB function */
    if (USB_MODE_HOST == g_usb_usbmode[USB_IP1])
    {
 #if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
  #if USB_NUM_USBIP == 2
        usb2_hstd_usb_handler();       /* Call interrupt routine */
  #endif /* USB_NUM_USBIP == 2 */
 #endif /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
 #if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
        usb_pstd_usb_handler();
 #endif                                /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
}

/******************************************************************************
 * End of function usbhs_usbir_isr
 ******************************************************************************/
#endif                                 /* defined(BSP_MCU_GROUP_RA6M3)*/

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

/******************************************************************************
 * Function Name   : usb_cpu_usb_int_hand_isr
 * Description     :
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb_cpu_usb_int_hand_isr (uint8_t usb_ip)
{
    hw_usb_pclear_sts_resm(usb_ip);
}                                      /* End of function usb_cpu_usb_int_hand_isr */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

#if USB_CFG_DTC == USB_CFG_ENABLE

/******************************************************************************
 * Function Name   : usb_cpu_d0fifo_int_hand
 * Description     : D0FIFO interrupt Handler
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb_cpu_d0fifo_int_hand (void)
{
    usb_cstd_dma_send_complete(USB_IP0, USB_D0USE);
}

/******************************************************************************
 * End of function usb_cpu_d0fifo_int_hand
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb_cpu_d1fifo_int_hand
 * Description     : D0FIFO interrupt Handler
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb_cpu_d1fifo_int_hand (void)
{
    usb_cstd_dma_send_complete(USB_IP0, USB_D1USE);
}

/******************************************************************************
 * End of function usb_cpu_d1fifo_int_hand
 ******************************************************************************/

 #if defined(BSP_MCU_GROUP_RA6M3)

/******************************************************************************
 * Function Name   : usb2_cpu_d0fifo_int_hand
 * Description     : D0FIFO interrupt Handler
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb2_cpu_d0fifo_int_hand (void)
{
    usb_cstd_dma_send_complete(USB_IP1, USB_D0USE);
}

/******************************************************************************
 * End of function usb2_cpu_d0fifo_int_hand
 ******************************************************************************/

/******************************************************************************
 * Function Name   : usb2_cpu_d1fifo_int_hand
 * Description     : D0FIFO interrupt Handler
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
static void usb2_cpu_d1fifo_int_hand (void)
{
    usb_cstd_dma_send_complete(USB_IP1, USB_D1USE);
}

/******************************************************************************
 * End of function usb2_cpu_d1fifo_int_hand
 ******************************************************************************/
 #endif                                /* defined(BSP_MCU_GROUP_RA6M3) */

#endif                                 /* USB_CFG_DTC == USB_CFG_ENABLE */

/******************************************************************************
 * Function Name   : usb_check_use_usba_module
 * Description     :
 * Arguments       : usb_utr_t *ptr     : Pointer to usb_utr_t structure
 * Return value    :
 ******************************************************************************/
bool usb_check_use_usba_module (usb_utr_t * ptr)
{
    bool ret_code = false;

    FSP_PARAMETER_NOT_USED(ptr);
#if defined(BSP_MCU_GROUP_RA6M3)
    if (USB_IP1 == ptr->ip)
    {
        ret_code = true;
    }
#elif defined(BSP_MCU_GROUP_RZN2L)     /* defined(BSP_MCU_GROUP_RA6M3) */
    ret_code = true;
#endif                                 /* defined(BSP_MCU_GROUP_RA6M3) */

    return ret_code;
}                                      /* End of function usb_check_use_usba_module */

void usbfs_interrupt_handler (void)
{
    USB_CFG_MULTIPLEX_INTERRUPT_ENABLE;
#if defined(BSP_MCU_GROUP_RZN2L)
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */

    usbfs_usbi_isr();
    USB_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

void usbfs_resume_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
    usb_cfg_t * p_cfg;

    p_cfg = (usb_cfg_t *) R_FSP_IsrContextGet(irq);
    usb_cpu_usb_int_hand_isr(p_cfg->module_number);
#endif                                 /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */
}

void usbfs_d0fifo_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */

#if USB_CFG_DTC == USB_CFG_ENABLE
    usb_cpu_d0fifo_int_hand();
#endif                                 /* USB_CFG_DTC == USB_CFG_ENABLE */
}

void usbfs_d1fifo_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */
#if USB_CFG_DTC == USB_CFG_ENABLE
    usb_cpu_d1fifo_int_hand();
#endif                                 /* USB_CFG_DTC == USB_CFG_ENABLE */
}

void usbhs_interrupt_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */
#if defined(BSP_MCU_GROUP_RA6M3)
    usbhs_usbir_isr();
#endif                                 /* defined (BSP_MCU_GROUP_RA6M3) */
}

void usbhs_d0fifo_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */

#if USB_CFG_DTC == USB_CFG_ENABLE
 #if defined(BSP_MCU_GROUP_RA6M3)
    usb2_cpu_d0fifo_int_hand();
 #endif                                /* defined(BSP_MCU_GROUP_RA6M3) */
#endif                                 /* USB_CFG_DTC == USB_CFG_ENABLE */
}

void usbhs_d1fifo_handler (void)
{
    IRQn_Type irq = R_FSP_CurrentIrqGet();
#if defined(BSP_MCU_GROUP_RZN2L)
    FSP_PARAMETER_NOT_USED(irq);
#else                                  /* defined(BSP_MCU_GROUP_RZN2L) */
    R_BSP_IrqStatusClear(irq);
#endif                                 /* defined(BSP_MCU_GROUP_RZN2L) */
#if USB_CFG_DTC == USB_CFG_ENABLE
 #if defined(BSP_MCU_GROUP_RA6M3)
    usb2_cpu_d1fifo_int_hand();
 #endif                                /* defined(BSP_MCU_GROUP_RA6M3) */
#endif                                 /* USB_CFG_DTC == USB_CFG_ENABLE */
}

#if  USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * Function Name: usbh0_ahb_pci_bridge_init
 * Description  :
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
void usb_ahb_pci_bridge_init (void)
{
    USB00->INTENABLE_b.WAKEON_INTEN = 1;
    USB00->INTENABLE_b.UCOM_INTEN   = 1;
    USB00->INTENABLE_b.USBH_INTBEN  = 1;
    USB00->INTENABLE_b.USBH_INTAEN  = 1;
    USB00->INTENABLE_b.AHB_INTEN    = 1;

    USB00->AHBBUSCTR_b.PROT_TYPE     = 0;
    USB00->AHBBUSCTR_b.PROT_MODE     = 0;
    USB00->AHBBUSCTR_b.ALIGN_ADDRESS = 0;
    USB00->AHBBUSCTR_b.MAX_BURST_LEN = 0;
}                                      /* End of function usbh0_ahb_pci_bridge_init() */

#endif  /* USB_IP_EHCI_OHCI == 1 */

/******************************************************************************
 * End  Of File
 ******************************************************************************/
