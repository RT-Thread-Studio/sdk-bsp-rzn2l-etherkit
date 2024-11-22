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
 * Includes
 **********************************************************************************************************************/
#include <string.h>
#include "r_sci_spi.h"
#include "r_sci_spi_cfg.h"

#if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1
 #include "r_dmac.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define SCI_SPI_PRV_CLK_MIN_DIV      (4U)
#define SCI_SPI_PRV_CLK_MAX_DIV      ((UINT16_MAX + 1U) * 2U)
#define SCI_SPI_PRV_CHR_RST_VALUE    (0x0200U)
#define SCI_SPI_PRV_DATA_REG_MASK    (0xFFFFFF00)
#define SCI_SPI_PRV_RDAT_MASK        (0xFFU)

/** "SCIS" in ASCII, used to determine if channel is open. */
#define SCI_SPI_OPEN                 (0x53434953ULL)

/***********************************************************************************************************************
 * Private global variables.
 **********************************************************************************************************************/

const spi_api_t g_spi_on_sci =
{
    .open        = R_SCI_SPI_Open,
    .read        = R_SCI_SPI_Read,
    .write       = R_SCI_SPI_Write,
    .writeRead   = R_SCI_SPI_WriteRead,
    .close       = R_SCI_SPI_Close,
    .callbackSet = R_SCI_SPI_CallbackSet
};

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function declarations.
 **********************************************************************************************************************/

static void r_sci_spi_hw_config(sci_spi_instance_ctrl_t * const p_instance_ctrl);

#if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1
static fsp_err_t r_sci_spi_transfer_config(sci_spi_instance_ctrl_t * const p_instance_ctrl);

#endif
static fsp_err_t r_sci_spi_write_read_common(sci_spi_instance_ctrl_t * const p_instance_ctrl,
                                             void const                    * p_src,
                                             void                          * p_dest,
                                             uint32_t const                  length);
static void r_sci_spi_start_transfer(sci_spi_instance_ctrl_t * const p_instance_ctrl);
static void r_sci_spi_transmit(sci_spi_instance_ctrl_t * p_instance_ctrl);
static void r_sci_spi_call_callback(sci_spi_instance_ctrl_t * p_instance_ctrl, spi_event_t event);

static void sci_spi_txi_common(sci_spi_instance_ctrl_t * p_instance_ctrl);
static void sci_spi_rxi_common(sci_spi_instance_ctrl_t * p_instance_ctrl);

void sci_spi_txi_isr(void);
void sci_spi_rxi_isr(void);

void sci_spi_tx_dmac_callback(sci_spi_instance_ctrl_t * p_instance_ctrl);
void sci_spi_rx_dmac_callback(sci_spi_instance_ctrl_t * p_instance_ctrl);

void sci_spi_tei_isr(void);
void sci_spi_eri_isr(void);

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup SCI_SPI
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initialize a channel for SPI communication mode. Implements @ref spi_api_t::open.
 *
 * This function performs the following tasks:
 *   - Performs parameter checking and processes error conditions.
 *   - Enables the clock for the SCI channel.
 *   - Initializes the associated registers with default value and the user-configurable options.
 *   - Provides the channel handle for use with other API functions.
 *
 * @param      p_ctrl                          Pointer to the control structure.
 * @param      p_cfg                           Pointer to a configuration structure.
 *
 * @retval     FSP_SUCCESS                     Channel initialized successfully.
 * @retval     FSP_ERR_ASSERTION               An input parameter is invalid or NULL.
 * @retval     FSP_ERR_ALREADY_OPEN            The instance has already been opened.
 * @retval     FSP_ERR_IP_CHANNEL_NOT_PRESENT  The channel number is invalid.
 * @retval     FSP_ERR_INVALID_ARGUMENT        Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_Open (spi_ctrl_t * p_ctrl, spi_cfg_t const * const p_cfg)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;
    fsp_err_t                 err             = FSP_SUCCESS;

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(SCI_SPI_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_cfg->p_extend);
    FSP_ASSERT(NULL != p_cfg->p_callback);
    FSP_ERROR_RETURN(BSP_FEATURE_SCI_CHANNELS & (1U << p_cfg->channel), FSP_ERR_IP_CHANNEL_NOT_PRESENT);
    FSP_ASSERT(p_cfg->rxi_irq >= 0);
    FSP_ASSERT(p_cfg->txi_irq >= 0);
    FSP_ASSERT(p_cfg->tei_irq >= 0);
    FSP_ASSERT(p_cfg->eri_irq >= 0);

 #if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1
    if ((NULL != p_cfg->p_transfer_rx) || (NULL != p_cfg->p_transfer_tx))
    {
        /* DMAC activation is not available for safety channel. */
        FSP_ERROR_RETURN(BSP_FEATURE_SCI_SAFETY_CHANNEL != p_cfg->channel, FSP_ERR_INVALID_ARGUMENT);
    }
 #endif
#endif

    if (p_cfg->channel == BSP_FEATURE_SCI_SAFETY_CHANNEL)
    {
        /* Safety Peripheral */
        p_instance_ctrl->p_reg = (R_SCI0_Type *) BSP_FEATURE_SCI_SAFETY_CHANNEL_BASE_ADDRESS;
    }
    else
    {
        /* Non-Safety Peripheral */
        p_instance_ctrl->p_reg =
            (R_SCI0_Type *) (((uint32_t) R_SCI1 - (uint32_t) R_SCI0) * p_cfg->channel + (uint32_t) R_SCI0);
    }

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

#if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1

    /* Open the SCI_SPI transfer interface if available. */
    err = r_sci_spi_transfer_config(p_instance_ctrl);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
#endif

    /* Write user configuration to registers. */
    r_sci_spi_hw_config(p_instance_ctrl);

    /* Enable required interrupts. */
    R_BSP_IrqCfgEnable(p_cfg->rxi_irq, p_cfg->rxi_ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->txi_irq, p_cfg->txi_ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->tei_irq, p_cfg->tei_ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->eri_irq, p_cfg->eri_ipl, p_instance_ctrl);

    p_instance_ctrl->open = SCI_SPI_OPEN;

    return err;
}

/*******************************************************************************************************************//**
 * Receive data from an SPI device. Implements @ref spi_api_t::read.
 *
 * The function performs the following tasks:
 *   - Performs parameter checking and processes error conditions.
 *   - Enable transmitter.
 *   - Enable receiver.
 *   - Enable interrupts.
 *   - Start data transmission by writing data to the TXD register.
 *   - Receive data from receive buffer full interrupt occurs and copy data to the buffer of destination.
 *   - Complete data reception via receive buffer full interrupt and transmitting dummy data.
 *   - Disable transmitter.
 *   - Disable receiver.
 *   - Disable interrupts.
 *
 * @param      p_ctrl               Pointer to the control structure.
 * @param      p_dest               Pointer to the destination buffer.
 * @param[in]  length               The number of bytes to transfer.
 * @param[in]  bit_width            Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 *
 * @retval     FSP_SUCCESS          Read operation successfully completed.
 * @retval     FSP_ERR_ASSERTION    One of the following invalid parameters passed:
 *                                  - Pointer p_ctrl is NULL
 *                                  - Length is equal to 0
 *                                  - Pointer to destination is NULL
 * @retval     FSP_ERR_NOT_OPEN     The channel has not been opened. Open the channel first.
 * @retval     FSP_ERR_UNSUPPORTED  The given bit_width is not supported.
 * @retval     FSP_ERR_IN_USE       A transfer is already in progress.
 *
 * @return     See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes. This
 *             function calls:
 *               - @ref transfer_api_t::reconfigure
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_Read (spi_ctrl_t * const    p_ctrl,
                          void                * p_dest,
                          uint32_t const        length,
                          spi_bit_width_t const bit_width)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE

    /* Check bit_width parameter, in simple SPI, only 8 bits operation is allowed. */
    FSP_ERROR_RETURN(SPI_BIT_WIDTH_8_BITS == bit_width, FSP_ERR_UNSUPPORTED);

    /* Check the destination, should not be NULL. */
    FSP_ASSERT(NULL != p_dest);
#else
    FSP_PARAMETER_NOT_USED(bit_width);
#endif

    return r_sci_spi_write_read_common(p_instance_ctrl, NULL, p_dest, length);
}

/*******************************************************************************************************************//**
 * Transmit data to a SPI device. Implements @ref spi_api_t::write.
 *
 * The function performs the following tasks:
 *   - Performs parameter checking and processes error conditions.
 *   - Enable transmitter.
 *   - Enable interrupts.
 *   - Start data transmission with data via transmit buffer empty interrupt.
 *   - Copy data from source buffer to the SPI data register for transmission.
 *   - Complete data transmission via transmit buffer empty interrupt.
 *   - Disable transmitter.
 *   - Disable receiver.
 *   - Disable interrupts.
 *
 * @param      p_ctrl               Pointer to the control structure.
 * @param      p_src                Pointer to the source buffer.
 * @param[in]  length               The number of bytes to transfer.
 * @param[in]  bit_width            Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 *
 * @retval     FSP_SUCCESS          Write operation successfully completed.
 * @retval     FSP_ERR_ASSERTION    One of the following invalid parameters passed:
 *                                  - Pointer p_ctrl is NULL
 *                                  - Pointer to source is NULL
 *                                  - Length is equal to 0
 * @retval     FSP_ERR_NOT_OPEN     The channel has not been opened. Open the channel first.
 * @retval     FSP_ERR_UNSUPPORTED  The given bit_width is not supported.
 * @retval     FSP_ERR_IN_USE       A transfer is already in progress.
 *
 * @return     See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes. This
 *             function calls:
 *               - @ref transfer_api_t::reconfigure
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_Write (spi_ctrl_t * const    p_ctrl,
                           void const          * p_src,
                           uint32_t const        length,
                           spi_bit_width_t const bit_width)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ERROR_RETURN(SPI_BIT_WIDTH_8_BITS == bit_width, FSP_ERR_UNSUPPORTED);
    FSP_ASSERT(NULL != p_src);
#else
    FSP_PARAMETER_NOT_USED(bit_width);
#endif

    return r_sci_spi_write_read_common(p_instance_ctrl, p_src, NULL, length);
}

/*******************************************************************************************************************//**
 * Simultaneously transmit data to SPI device while receiving data from SPI device (full duplex).
 * Implements @ref spi_api_t::writeRead.
 *
 * The function performs the following tasks:
 *   - Performs parameter checking and processes error conditions.
 *   - Enable transmitter.
 *   - Enable receiver.
 *   - Enable interrupts.
 *   - Start data transmission using transmit buffer empty interrupt (or by writing to the TDR register).
 *   - Copy data from source buffer to the SPI data register for transmission.
 *   - Receive data from receive buffer full interrupt and copy data to the destination buffer.
 *   - Complete data transmission and reception via transmit end interrupt.
 *   - Disable transmitter.
 *   - Disable receiver.
 *   - Disable interrupts.
 *
 * @param      p_ctrl               Pointer to the control structure.
 * @param      p_src                Pointer to the source buffer.
 * @param      p_dest               Pointer to the destination buffer.
 * @param[in]  length               The number of bytes to transfer.
 * @param[in]  bit_width            Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 *
 * @retval     FSP_SUCCESS          Write operation successfully completed.
 * @retval     FSP_ERR_ASSERTION    One of the following invalid parameters passed:
 *                                  - Pointer p_ctrl is NULL
 *                                  - Pointer to source is NULL
 *                                  - Pointer to destination is NULL
 *                                  - Length is equal to 0
 * @retval     FSP_ERR_NOT_OPEN     The channel has not been opened. Open the channel first.
 * @retval     FSP_ERR_UNSUPPORTED  The given bit_width is not supported.
 * @retval     FSP_ERR_IN_USE       A transfer is already in progress.
 *
 * @return     See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes. This
 *             function calls:
 *               - @ref transfer_api_t::reconfigure
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_WriteRead (spi_ctrl_t * const    p_ctrl,
                               void const          * p_src,
                               void                * p_dest,
                               uint32_t const        length,
                               spi_bit_width_t const bit_width)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ERROR_RETURN(SPI_BIT_WIDTH_8_BITS == bit_width, FSP_ERR_UNSUPPORTED);
    FSP_ASSERT(NULL != p_src);
    FSP_ASSERT(NULL != p_dest);
#else
    FSP_PARAMETER_NOT_USED(bit_width);
#endif

    return r_sci_spi_write_read_common(p_instance_ctrl, p_src, p_dest, length);
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements spi_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_CallbackSet (spi_ctrl_t * const          p_ctrl,
                                 void (                    * p_callback)(spi_callback_args_t *),
                                 void const * const          p_context,
                                 spi_callback_args_t * const p_callback_memory)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;

#if (SCI_SPI_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(SCI_SPI_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Disable the SCI channel and set the instance as not open. Implements @ref spi_api_t::close.
 *
 * @param      p_ctrl             Pointer to an opened instance.
 *
 * @retval     FSP_SUCCESS        Channel successfully closed.
 * @retval     FSP_ERR_ASSERTION  The parameter p_ctrl is NULL.
 * @retval     FSP_ERR_NOT_OPEN   The channel has not been opened. Open the channel first.
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_Close (spi_ctrl_t * const p_ctrl)
{
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) p_ctrl;

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(SCI_SPI_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Clear the RE and TE bits in CCR0. */
    p_instance_ctrl->p_reg->CCR0 = 0;
    FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.RE, 0);
    FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.TE, 0);

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->rxi_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->eri_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->tei_irq);

    /* Disable the clock to the SCI channel. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_STOP(FSP_IP_SCI, p_instance_ctrl->p_cfg->channel);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    if (NULL != p_instance_ctrl->p_cfg->p_transfer_rx)
    {
        p_instance_ctrl->p_cfg->p_transfer_rx->p_api->close(p_instance_ctrl->p_cfg->p_transfer_rx->p_ctrl);
    }

    if (NULL != p_instance_ctrl->p_cfg->p_transfer_tx)
    {
        p_instance_ctrl->p_cfg->p_transfer_tx->p_api->close(p_instance_ctrl->p_cfg->p_transfer_tx->p_ctrl);
    }

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Calculate the register settings required to achieve the desired bitrate.
 *
 * @param[in]  bitrate            bitrate [bps]. For example, 250,000; 500,00; 2,500,000; etc.
 * @param      clock_source       clock source (PCLKM or SCInASYNCCLK) used for bit rate calculation.
 * @param      sclk_div           Pointer to sci_spi_div_setting_t used to configure baudrate settings.
 *
 * @retval     FSP_SUCCESS        Baud rate is set successfully.
 * @retval     FSP_ERR_ASSERTION  Baud rate is not achievable.
 * @note       The application must pause for 1 bit time after the BRR register is loaded before transmitting/receiving
 *             to allow time for the clock to settle.
 **********************************************************************************************************************/
fsp_err_t R_SCI_SPI_CalculateBitrate (uint32_t                bitrate,
                                      sci_spi_clock_source_t  clock_source,
                                      sci_spi_div_setting_t * sclk_div)
{
    uint32_t peripheral_clock;
    if (SCI_SPI_CLOCK_SOURCE_PCLKM == clock_source)
    {
        peripheral_clock = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKM);
    }
    else
    {
        peripheral_clock =
            R_FSP_SystemClockHzGet((fsp_priv_clock_t) ((uint8_t) FSP_PRIV_CLOCK_PCLKSCI0 + (uint8_t) clock_source));
    }

#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != sclk_div);
    FSP_ASSERT(bitrate);
    FSP_ASSERT(bitrate >= (peripheral_clock + SCI_SPI_PRV_CLK_MAX_DIV - 1U) / SCI_SPI_PRV_CLK_MAX_DIV);
#endif

    int32_t divisor = 0;
    int32_t brr     = 0;
    int32_t cks     = -1;

    for (uint32_t i = 0; i <= 3; i++)
    {
        cks++;
        divisor = (1 << (2 * (i + 1))) * (int32_t) bitrate;

        /* Calculate BRR so that the bit rate is the largest possible value less than or equal to the desired
         * bitrate. */
        brr = ((int32_t) peripheral_clock + divisor - 1) / divisor - 1;

        if (brr <= UINT8_MAX)
        {
            break;
        }
    }

    sclk_div->brr = (uint8_t) brr;
    sclk_div->cks = (uint8_t) (cks & 3);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup SCI_SPI)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Configures SCI registers based on the user configuration.
 * @param      p_instance_ctrl          Pointer to control structure.
 **********************************************************************************************************************/
static void r_sci_spi_hw_config (sci_spi_instance_ctrl_t * const p_instance_ctrl)
{
    spi_cfg_t const        * p_cfg    = p_instance_ctrl->p_cfg;
    sci_spi_extended_cfg_t * p_extend = (sci_spi_extended_cfg_t *) p_cfg->p_extend;

    /* Initialize registers to their reset values. */
    uint32_t ccr0 = 0U;
    uint32_t ccr1 = R_SCI0_CCR1_SPB2DT_Msk;
    uint32_t ccr2 = 0U;
    uint32_t ccr3 = R_SCI0_CCR3_CPHA_Msk | R_SCI0_CCR3_CPOL_Msk | R_SCI0_CCR3_LSBF_Msk |
                    SCI_SPI_PRV_CHR_RST_VALUE;
    uint32_t ccr4  = 0U;
    uint32_t cfclr = 0U;
    uint32_t ffclr = 0U;

    /* SCI Initialization in Simple SPI Mode (See Section 'SCI Initialization in Simple-SPI Mode' and 'Initializing
     * the SCI (Clock Synchronous Mode)' in RZ microprocessor manual). */

    /* Write settings to CCR3:
     * - Set Bypass Synchronizer circuit between bus clock and operation clock.
     * - Set Character Length to 8-Bit.
     * - Set MSB first.
     * - Set CPHA and CPOL.
     * - Set Simple SPI Mode.
     */
    if (SPI_BIT_ORDER_MSB_FIRST == p_cfg->bit_order)
    {
        /* Configure MSB first (Default is LSB). */
        ccr3 &= ~R_SCI0_CCR3_LSBF_Msk;
    }

    if (SPI_MODE_SLAVE == p_cfg->operating_mode)
    {
        /* Select slave mode. */
        ccr3 |= 2U << R_SCI0_CCR3_CKE_Pos;

        /* Select to use SSLn input pin */
        ccr0 |= R_SCI0_CCR0_SSE_Msk;
    }

    /* Apply CPOL and CPHA settings as required. By default CCR2.CPHA=1, CCR2.CPOL=1. */
    if (SPI_CLK_PHASE_EDGE_ODD == p_cfg->clk_phase)
    {
        /* In order to get sampling on the ODD edge set CCR3.CPHA = 0. */
        ccr3 &= ~R_SCI0_CCR3_CPHA_Msk;
    }

    /* In order to get sampling on the even clock edge set CCR3.CPHA = 1. */
    if (SPI_CLK_POLARITY_LOW == p_cfg->clk_polarity)
    {
        /* Set CCR3.CPOL = 0 for clock polarity as low during idle (See Figure 'Relationship between clock and transmit
         * data/receive data in simple-SPI mode' in RZ microprocessor manual). */
        ccr3 &= ~R_SCI0_CCR3_CPOL_Msk;
    }

    /* Apply the synchronization bypass settings. */
    if (SCI_SPI_CLOCK_SOURCE_PCLKM == p_extend->clock_source)
    {
        ccr3 |= R_SCI0_CCR3_BPEN_Msk;
    }

    /* Set Simple SPI mode. */
    ccr3 |= 3U << R_SCI0_CCR3_MOD_Pos;

    /* Write settings to CCR2:
     * - Write the BRR setting.
     * - Write the clock divider setting.
     */
    ccr2 |= (uint32_t) (p_extend->clk_div.cks << R_SCI0_CCR2_CKS_Pos);

    ccr2 |= (uint32_t) ((p_extend->clk_div.brr) << R_SCI0_CCR2_BRR_Pos);

    /* Enable Clock for the SCI Channel. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_SCI, p_cfg->channel);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Set TEIE, TIE, RIE, TE, and RE to 0. */
    p_instance_ctrl->p_reg->CCR0 = 0;
    FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.RE, 0);
    FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.TE, 0);

    /* Clear status flags. */
    cfclr = R_SCI0_CFCLR_RDRFC_Msk | R_SCI0_CFCLR_FERC_Msk | R_SCI0_CFCLR_PERC_Msk | R_SCI0_CFCLR_MFFC_Msk |
            R_SCI0_CFCLR_ORERC_Msk | R_SCI0_CFCLR_DFERC_Msk | R_SCI0_CFCLR_DPERC_Msk | R_SCI0_CFCLR_DCMFC_Msk |
            R_SCI0_CFCLR_ERSC_Msk;
    ffclr = R_SCI0_FFCLR_DRC_Msk;

    /* Set FCR. Reset fifo/data registers. */
    p_instance_ctrl->p_reg->FCR = (8U << R_SCI0_FCR_RTRG_Pos);

    /* Write settings to registers. */
    p_instance_ctrl->p_reg->CCR3  = ccr3;
    p_instance_ctrl->p_reg->CCR2  = ccr2;
    p_instance_ctrl->p_reg->CCR1  = ccr1;
    p_instance_ctrl->p_reg->CCR4  = ccr4;
    p_instance_ctrl->p_reg->CFCLR = cfclr;
    p_instance_ctrl->p_reg->FFCLR = ffclr;
    p_instance_ctrl->p_reg->CCR0  = ccr0;
}

#if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1

/*******************************************************************************************************************//**
 * Configures SCI SPI related transfer drivers (if enabled).
 *
 * @param[in]  p_instance_ctrl           Pointer to the control block.
 *
 * @retval     FSP_SUCCESS               Operation successfully completed.
 *
 * @return     See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes. This
 *             function calls:
 *               - @ref transfer_api_t::open
 *               - @ref transfer_api_t::close
 **********************************************************************************************************************/
static fsp_err_t r_sci_spi_transfer_config (sci_spi_instance_ctrl_t * const p_instance_ctrl)
{
    fsp_err_t               err   = FSP_SUCCESS;
    spi_cfg_t const * const p_cfg = p_instance_ctrl->p_cfg;

    if (NULL != p_cfg->p_transfer_rx)
    {
        /* Set the initial configuration for the rx transfer instance. */
        transfer_instance_t const * p_transfer = p_cfg->p_transfer_rx;
        p_transfer->p_cfg->p_info->mode           = TRANSFER_MODE_NORMAL;
        p_transfer->p_cfg->p_info->src_addr_mode  = TRANSFER_ADDR_MODE_FIXED;
        p_transfer->p_cfg->p_info->dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED;
        p_transfer->p_cfg->p_info->p_src          = (void *) &p_instance_ctrl->p_reg->RDR;

        /* Open the transfer instance. */
        err = p_transfer->p_api->open(p_transfer->p_ctrl, p_transfer->p_cfg);
        FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
    }

    if (NULL != p_cfg->p_transfer_tx)
    {
        /* Set the initial configuration for the tx transfer instance. */
        transfer_instance_t const * p_transfer = p_cfg->p_transfer_tx;
        p_transfer->p_cfg->p_info->mode           = TRANSFER_MODE_NORMAL;
        p_transfer->p_cfg->p_info->src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED;
        p_transfer->p_cfg->p_info->dest_addr_mode = TRANSFER_ADDR_MODE_FIXED;
        p_transfer->p_cfg->p_info->p_dest         = (void *) &p_instance_ctrl->p_reg->TDR;

        /* Open the transfer instance. */
        err = p_transfer->p_api->open(p_transfer->p_ctrl, p_transfer->p_cfg);
        if (FSP_SUCCESS != err)
        {
            if (NULL != p_cfg->p_transfer_rx)
            {
                /* If the tx transfer instance could not be opened, close the rx transfer instance. */
                p_cfg->p_transfer_rx->p_api->close(p_cfg->p_transfer_rx->p_ctrl);
            }

            return err;
        }
    }

    return err;
}

#endif

/*******************************************************************************************************************//**
 * Initiates write or read process. Common routine used by SPI API write or read functions.
 *
 * @param[in]  p_instance_ctrl    Pointer to the control block.
 * @param[in]  p_src              Pointer to data buffer which need to be sent.
 * @param[out] p_dest             Pointer to buffer where received data will be stored.
 * @param[in]  length             Number of data transactions to be performed.
 *
 * @retval     FSP_SUCCESS        Operation successfully completed.
 * @retval     FSP_ERR_NOT_OPEN   The channel has not been opened. Open the channel first.
 * @retval     FSP_ERR_ASSERTION  One of the following invalid parameters passed:
 *                                  - Pointer p_instance_ctrl is NULL
 *                                  - length == 0
 * @retval     FSP_ERR_IN_USE     A transfer is already in progress.
 *
 * @return     See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes. This
 *             function calls:
 *               - @ref transfer_api_t::reconfigure
 **********************************************************************************************************************/
static fsp_err_t r_sci_spi_write_read_common (sci_spi_instance_ctrl_t * const p_instance_ctrl,
                                              void const                    * p_src,
                                              void                          * p_dest,
                                              uint32_t const                  length)
{
#if SCI_SPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(SCI_SPI_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ASSERT(0 != length);
#endif

    /* TE and RE must be zero in order to write one to TE or RE (TE and RE will only be set if there is a transfer in
     * progress). Reference section 'CCR0 : Common Control Register 0' in the RZ microprocessor manual. */
    FSP_ERROR_RETURN(0 == (p_instance_ctrl->p_reg->CCR0 & (R_SCI0_CCR0_RE_Msk | R_SCI0_CCR0_TE_Msk)), FSP_ERR_IN_USE);

    /* Setup the control block. */
    p_instance_ctrl->count    = length;
    p_instance_ctrl->tx_count = 0U;
    p_instance_ctrl->rx_count = 0U;
    p_instance_ctrl->p_src    = (uint8_t *) p_src;
    p_instance_ctrl->p_dest   = (uint8_t *) p_dest;

#if SCI_SPI_CFG_DMAC_SUPPORT_ENABLE == 1
    if (p_instance_ctrl->p_cfg->p_transfer_tx)
    {
        /* Configure the tx transfer instance. */
        p_instance_ctrl->tx_count = length;
        transfer_instance_t const * p_transfer = p_instance_ctrl->p_cfg->p_transfer_tx;
        p_transfer->p_cfg->p_info->length = length;

        if (NULL == p_src)
        {
            /* If the source is NULL transmit using a dummy value using FIXED mode. */
            static uint8_t tx_dummy = 0;
            p_transfer->p_cfg->p_info->src_addr_mode = TRANSFER_ADDR_MODE_FIXED;
            p_transfer->p_cfg->p_info->p_src         = &tx_dummy;
        }
        else
        {
            p_transfer->p_cfg->p_info->src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED;
            p_transfer->p_cfg->p_info->p_src         = p_src;
        }

        p_transfer->p_cfg->p_info->src_size  = TRANSFER_SIZE_1_BYTE;
        p_transfer->p_cfg->p_info->dest_size = TRANSFER_SIZE_1_BYTE;

        /* Disable the corresponding IRQ when transferring using DMAC. */
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);

        /* Enable the transfer instance. */
        fsp_err_t err = p_transfer->p_api->reconfigure(p_transfer->p_ctrl, p_transfer->p_cfg->p_info);
        FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
    }

    /* The rx transfer instance is not used if p_dest is NULL. */
    if ((NULL != p_instance_ctrl->p_cfg->p_transfer_rx) && (NULL != p_dest))
    {
        /* Configure the rx transfer instance. */
        p_instance_ctrl->rx_count = length;
        transfer_instance_t const * p_transfer = p_instance_ctrl->p_cfg->p_transfer_rx;
        p_transfer->p_cfg->p_info->length = length;
        p_transfer->p_cfg->p_info->p_dest = p_dest;

        p_transfer->p_cfg->p_info->src_size  = TRANSFER_SIZE_1_BYTE;
        p_transfer->p_cfg->p_info->dest_size = TRANSFER_SIZE_1_BYTE;

        /* Disable the corresponding IRQ when transferring using DMAC. */
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->rxi_irq);

        /* Enable the transfer instance. */
        fsp_err_t err = p_transfer->p_api->reconfigure(p_transfer->p_ctrl, p_transfer->p_cfg->p_info);
        FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
    }
#endif

    /* Enable transmit and receive interrupts. */
    r_sci_spi_start_transfer(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enables and disables Receive and Transmit mode based on the current configuration.
 *
 * @param      p_instance_ctrl          Pointer to control structure.
 **********************************************************************************************************************/
static void r_sci_spi_start_transfer (sci_spi_instance_ctrl_t * const p_instance_ctrl)
{
    /* TE must always be enabled even when receiving data. When RE is enabled without also enabling TE, the SCI will
     * continue transferring data until the RE bit is cleared. At high bitrates, it is not possible to clear the RE bit
     * fast enough and there will be additional clock pulses at the end of the transfer. */
    uint32_t interrupt_settings = R_SCI0_CCR0_TE_Msk;

    if ((NULL == p_instance_ctrl->p_dest) || (NULL != p_instance_ctrl->p_cfg->p_transfer_tx) ||
        (NULL != p_instance_ctrl->p_cfg->p_transfer_rx))
    {
        /* Enable the transmit IRQ. */
        interrupt_settings |= R_SCI0_CCR0_TIE_Msk;
    }

    if (NULL != p_instance_ctrl->p_dest)
    {
        /* Enable Receive mode and the Receive buffer full IRQ. */
        interrupt_settings |= (R_SCI0_CCR0_RE_Msk | R_SCI0_CCR0_RIE_Msk);
    }

    /* Write the transfer settings. */
    p_instance_ctrl->p_reg->CCR0 |= interrupt_settings;
    if (NULL != p_instance_ctrl->p_dest)
    {
        FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.RE, 1);
    }

    FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CCR0_b.TE, 1);

    /* Transmit from RXI interrupt. */
    if ((NULL == p_instance_ctrl->p_cfg->p_transfer_tx) && (NULL == p_instance_ctrl->p_cfg->p_transfer_rx) &&
        (NULL != p_instance_ctrl->p_dest))
    {
        /* The RXI interrupt must be disabled so that r_sci_spi_transmit is not interrupted before it updates the
         * tx_count. */
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->rxi_irq);

        /* When transmitting from the RXI interrupt, the first byte must be written here because the transmit buffer
         * empty IRQ is disabled. */
        r_sci_spi_transmit(p_instance_ctrl);

        /* If trying to write the next data to TDR immediately after writing the first data to TDR above, the previous
         * data may be lost. Therefore, it is necessary to wait for TDRE to become 1 (transmission data empty occurs)
         * in order to write the next data after the previous data transitions from TDR to TSR.
         * Reference section 'Writing Data to TDR(Transmit FIFO)' in the RZ microprocessor manual.*/
        FSP_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->CSR_b.TDRE, 1)

        if ((SPI_MODE_SLAVE == p_instance_ctrl->p_cfg->operating_mode) && (1 < p_instance_ctrl->count))
        {
            /* First call writes directly to the TSR register. The second call writes to the TDR register. */
            r_sci_spi_transmit(p_instance_ctrl);
        }

        /* In master mode the RXI interrupt will fire as soon as it is enabled. */
        R_BSP_IrqEnableNoClear(p_instance_ctrl->p_cfg->rxi_irq);
    }
}

/*******************************************************************************************************************//**
 * Transmit a single byte of data.
 * @param      p_instance_ctrl          Pointer to the control structure.
 **********************************************************************************************************************/
static void r_sci_spi_transmit (sci_spi_instance_ctrl_t * p_instance_ctrl)
{
    if (p_instance_ctrl->tx_count < p_instance_ctrl->count)
    {
        if (p_instance_ctrl->p_src)
        {
            p_instance_ctrl->p_reg->TDR =
                (SCI_SPI_PRV_DATA_REG_MASK | (p_instance_ctrl->p_src[p_instance_ctrl->tx_count]));
        }
        else
        {
            /* Do a dummy write if there is no tx buffer. */
            p_instance_ctrl->p_reg->TDR = (SCI_SPI_PRV_DATA_REG_MASK | 0U);
        }

        p_instance_ctrl->tx_count++;
    }
}

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_instance_ctrl     Pointer to SPI instance control block
 * @param[in]     event               Event code
 **********************************************************************************************************************/
static void r_sci_spi_call_callback (sci_spi_instance_ctrl_t * p_instance_ctrl, spi_event_t event)
{
    spi_callback_args_t args;

    /* Store callback arguments in memory provided by user if available. */
    spi_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
    if (NULL == p_args)
    {
        /* Store on stack */
        p_args = &args;
    }
    else
    {
        /* Save current arguments on the stack in case this is a nested interrupt. */
        args = *p_args;
    }

    p_args->channel   = p_instance_ctrl->p_cfg->channel;
    p_args->event     = event;
    p_args->p_context = p_instance_ctrl->p_context;

    p_instance_ctrl->p_callback(p_args);

    if (NULL != p_instance_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_instance_ctrl->p_callback_memory = args;
    }
}

/*******************************************************************************************************************//**
 * Common processing for RXI interrupt and DMA transfer completion interrupt in SCI_SPI write operation.
 **********************************************************************************************************************/
static void sci_spi_txi_common (sci_spi_instance_ctrl_t * p_instance_ctrl)
{
    /* Write the next byte to the TDR register. */
    r_sci_spi_transmit(p_instance_ctrl);

    if ((p_instance_ctrl->tx_count == p_instance_ctrl->count) && (NULL == p_instance_ctrl->p_dest))
    {
        /* If the last byte is transmitted and there is no data to receive then enable the transmit end IRQ and disable
         * the transmit IRQ. */
        uint32_t ccr0_temp = p_instance_ctrl->p_reg->CCR0;
        ccr0_temp |= R_SCI0_CCR0_TEIE_Msk;
        ccr0_temp &= (uint32_t) ~(R_SCI0_CCR0_TIE_Msk);
        p_instance_ctrl->p_reg->CCR0 = ccr0_temp;
    }
}

/*******************************************************************************************************************//**
 * This function is the ISR handler for R_SCI_SPI Transmit Buffer Empty IRQ.
 * This function calls sci_spi_txi_common().
 *
 * The Transmit Buffer Empty IRQ is enabled in the following conditions:
 *   - The transfer is started using R_SCI_SPI_Write API (There is no data to receive).
 *   - The RXI IRQ is serviced using a DMAC instance.
 *   - The TXI IRQ is serviced using a DMAC instance (The interrupt will fire on the last byte transfered).
 *
 **********************************************************************************************************************/
void sci_spi_txi_isr (void)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used. */
    FSP_CONTEXT_SAVE;

    IRQn_Type                 irq             = R_FSP_CurrentIrqGet();
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    sci_spi_txi_common(p_instance_ctrl);

    /* Restore context if RTOS is used. */
    FSP_CONTEXT_RESTORE;

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Callback that must be called after a TX DMAC transfer completes.
 *
 * @param[in]     p_instance_ctrl     Pointer to SCI_SPI instance control block
 **********************************************************************************************************************/
void sci_spi_tx_dmac_callback (sci_spi_instance_ctrl_t * p_instance_ctrl)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Now that the transfer using DMAC is finished, enable the corresponding IRQ. */
    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->txi_irq);

    sci_spi_txi_common(p_instance_ctrl);

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Common processing for RXI interrupt and DMA transfer completion interrupt in SCI_SPI read operation.
 **********************************************************************************************************************/
static void sci_spi_rxi_common (sci_spi_instance_ctrl_t * p_instance_ctrl)
{
    /* Write the next byte to the TDR register
     * (Whenever the RXI ISR is enabled, the transmit ISR is disabled and transmit is handled here). */
    r_sci_spi_transmit(p_instance_ctrl);

    /* Read the next byte from the RDR register. */
    if (p_instance_ctrl->rx_count < p_instance_ctrl->count)
    {
        p_instance_ctrl->p_dest[p_instance_ctrl->rx_count++] = (uint8_t) p_instance_ctrl->p_reg->RDR;
    }

    if (p_instance_ctrl->rx_count == p_instance_ctrl->count)
    {
        /* If the last byte is received then enable the transmit end IRQ and disable the receive and transmit IRQs. */
        uint32_t ccr0_temp = p_instance_ctrl->p_reg->CCR0;
        ccr0_temp |= R_SCI0_CCR0_TEIE_Msk;
        ccr0_temp &= (uint32_t) ~(R_SCI0_CCR0_TIE_Msk | R_SCI0_CCR0_RIE_Msk);
        p_instance_ctrl->p_reg->CCR0 = ccr0_temp;
    }
}

/*******************************************************************************************************************//**
 * This function is the ISR handler for R_SCI_SPI Receive Buffer Full IRQ.
 * This handler also handles Transmit Buffer Empty IRQs.
 * This function calls sci_spi_rxi_common().
 *
 * The Receive Buffer Full IRQ is enabled in the following conditions:
 *   - The transfer is started using either the R_SCI_SPI_Read or R_SCI_SPI_WriteRead API.
 *
 **********************************************************************************************************************/
void sci_spi_rxi_isr (void)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used. */
    FSP_CONTEXT_SAVE;

    IRQn_Type                 irq             = R_FSP_CurrentIrqGet();
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    sci_spi_rxi_common(p_instance_ctrl);

    /* Restore context if RTOS is used. */
    FSP_CONTEXT_RESTORE;

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Callback that must be called after a RX DMAC transfer completes.
 *
 * @param[in]     p_instance_ctrl     Pointer to SCI_SPI instance control block
 **********************************************************************************************************************/
void sci_spi_rx_dmac_callback (sci_spi_instance_ctrl_t * p_instance_ctrl)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Now that the transfer using DMAC is finished, enable the corresponding IRQ. */
    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->rxi_irq);

    sci_spi_rxi_common(p_instance_ctrl);

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * This function is the ISR handler for R_SCI_SPI Transmit End IRQ.
 *
 * The Transmit End IRQ is enabled after the last byte of data has been transfered.
 *
 **********************************************************************************************************************/
void sci_spi_tei_isr (void)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used. */
    FSP_CONTEXT_SAVE;

    IRQn_Type                 irq             = R_FSP_CurrentIrqGet();
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Disables receiver, transmitter and transmit end IRQ. */
    uint32_t ccr0_temp = p_instance_ctrl->p_reg->CCR0;
    ccr0_temp &=
        (uint32_t) ~(R_SCI0_CCR0_TIE_Msk | R_SCI0_CCR0_TE_Msk | R_SCI0_CCR0_RE_Msk | R_SCI0_CCR0_TEIE_Msk);
    p_instance_ctrl->p_reg->CCR0 = ccr0_temp;

    /* Dummy read to ensure that interrupts are disabled. */
    volatile uint32_t dummy = p_instance_ctrl->p_reg->CCR0;
    FSP_PARAMETER_NOT_USED(dummy);

    /* Notify the user that the transfer has completed. */
    r_sci_spi_call_callback(p_instance_ctrl, SPI_EVENT_TRANSFER_COMPLETE);

    /* Restore context if RTOS is used. */
    FSP_CONTEXT_RESTORE;

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * This function is the ISR handler for R_SCI_SPI Error IRQs.
 *
 * This handler is only enabled if the user passes in a valid IRQ number in Pointer to a configuration structure.
 *
 **********************************************************************************************************************/
void sci_spi_eri_isr (void)
{
    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used. */
    FSP_CONTEXT_SAVE;

    IRQn_Type                 irq             = R_FSP_CurrentIrqGet();
    sci_spi_instance_ctrl_t * p_instance_ctrl = (sci_spi_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Disables receiver, transmitter and transmit end IRQ. */
    p_instance_ctrl->p_reg->CCR0 &= (uint32_t) ~(R_SCI0_CCR0_TIE_Msk | R_SCI0_CCR0_RIE_Msk | R_SCI0_CCR0_TEIE_Msk);

    /* Dummy read to ensure that interrupts are disabled. */
    volatile uint32_t dummy = p_instance_ctrl->p_reg->CCR0;
    FSP_PARAMETER_NOT_USED(dummy);

    /* Clear the error status flags (The only possible error is an RX overflow). */
    p_instance_ctrl->p_reg->CFCLR = R_SCI0_CFCLR_ORERC_Msk;

    /* Dummy read to ensure that interrupt event is cleared. */
    dummy = p_instance_ctrl->p_reg->CSR;
    FSP_PARAMETER_NOT_USED(dummy);

    /* Notify the user that an error occurred. */
    r_sci_spi_call_callback(p_instance_ctrl, SPI_EVENT_ERR_READ_OVERFLOW);

    /* Restore context if RTOS is used. */
    FSP_CONTEXT_RESTORE;

    SCI_SPI_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}
