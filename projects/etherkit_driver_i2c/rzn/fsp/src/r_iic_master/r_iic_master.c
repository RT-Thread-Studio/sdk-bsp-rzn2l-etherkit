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
#include "r_iic_master.h"

#if IIC_MASTER_CFG_DMAC_ENABLE
 #include "r_dmac.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "IIC" in ASCII, used to determine if channel is open. */
#define IIC_MASTER_OPEN                             (0x52494943U)

#define I2C_CODE_READ                               (0x01U)
#define I2C_CODE_10BIT                              (0xF0U)

/* The timeout interrupt enable bit */
#define IIC_MASTER_TMO_EN_BIT                       (0x01)

/* The arbitration loss detection interrupt enable bit */
#define IIC_MASTER_ALD_EN_BIT                       (0x02)

/* The start condition detection interrupt enable bit */
#define IIC_MASTER_STR_EN_BIT                       (0x04)

/* The stop condition detection interrupt enable bit */
#define IIC_MASTER_STP_EN_BIT                       (0x08)

/* The NAK reception interrupt enable bit */
#define IIC_MASTER_NAK_EN_BIT                       (0x10)

/* The receive data full interrupt enable bit */
#define IIC_MASTER_RXI_EN_BIT                       (0x20)

/* The transmit end interrupt enable bit */
#define IIC_MASTER_TEI_EN_BIT                       (0x40)

/* The transmit data empty interrupt enable bit */
#define IIC_MASTER_TXI_EN_BIT                       (0x80)

/* Bit position for STOP condition flag in ICSR2 */
#define IIC_MASTER_ICSR2_STOP_BIT                   (0x08U)

#define IIC_MASTER_BUS_RATE_REG_RESERVED_BITS       (0xE0U)
#define IIC_MASTER_INTERNAL_REF_CLOCK_SELECT_MAX    (0x07U)

#define IIC_MASTER_SLAVE_10_BIT_ADDR_LEN_ADJUST     (2U)

#define IIC_MASTER_FUNCTION_ENABLE_REG_VAL          (0x02U)
#define IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK       (0xB3U)
#define IIC_MASTER_FUNCTION_ENABLE_INIT_SETTINGS    (0x77U)
#define IIC_MASTER_STATUS_REGISTER_2_ERR_MASK       (0x1FU)
#define IIC_MASTER_BUS_MODE_REGISTER_1_MASK         (0x08U)
#define IIC_MASTER_BUS_MODE_REGISTER_2_MASK         (0x04U)
#define IIC_MASTER_PRV_SCL_SDA_NOT_DRIVEN           (0x1FU)
#define IIC_MASTER_ICCR1_ICE_BIT_MASK               (0x80)
#define IIC_MASTER_ICCR1_IICRST_BIT_MASK            (0x40)
#define IIC_MASTER_ICCR2_SP_BIT_MASK                (0x08)
#define IIC_MASTER_ICCR2_RS_BIT_MASK                (0x04)
#define IIC_MASTER_ICCR2_ST_BIT_MASK                (0x02)
#define IIC_MASTER_SCKCR2_CPU0CLK_MASK              (0x00000001U)

/* Worst case ratio of (CPUnCLK/PCLKL) = 8 approximately.
 */
#define IIC_MASTER_PERIPHERAL_REG_MAX_WAIT          (0x08U)

#define IIC_MASTER_HARDWARE_REGISTER_WAIT(reg, required_value, timeout) \
    while ((timeout))                                                   \
    {                                                                   \
        if ((required_value) == (reg))                                  \
        {                                                               \
            break;                                                      \
        }                                                               \
        (timeout)--;                                                    \
    }

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Hardware errors and events that can occur on the IIC */
typedef enum e_iic_master_err_event
{
    IIC_MASTER_ERR_EVENT_NONE             = 0,
    IIC_MASTER_ERR_EVENT_TIMEOUT          = 1,
    IIC_MASTER_ERR_EVENT_ARBITRATION_LOSS = 2,
    IIC_MASTER_ERR_EVENT_START            = 4,
    IIC_MASTER_ERR_EVENT_STOP             = 8,
    IIC_MASTER_ERR_EVENT_NACK             = 16
} iic_master_err_t;

/* IIC read/write enumeration */
typedef enum e_iic_master_transfer_dir_option
{
    IIC_MASTER_TRANSFER_DIR_WRITE = 0x0,
    IIC_MASTER_TRANSFER_DIR_READ  = 0x1
} iic_master_transfer_dir_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);

void iic_master_tx_dmac_callback(iic_master_instance_ctrl_t * p_instance_ctrl);
void iic_master_rx_dmac_callback(iic_master_instance_ctrl_t * p_instance_ctrl);

/* Internal helper functions */
static void      iic_master_abort_seq_master(iic_master_instance_ctrl_t * const p_instance_ctrl, bool iic_reset);
static fsp_err_t iic_master_read_write(i2c_master_ctrl_t * const p_ctrl,
                                       uint8_t * const           p_buffer,
                                       uint32_t const            bytes,
                                       iic_master_transfer_dir_t direction);
static void iic_master_notify(iic_master_instance_ctrl_t * const p_instance_ctrl, i2c_master_event_t const event);

#if IIC_MASTER_CFG_DMAC_ENABLE
static fsp_err_t iic_master_transfer_open(i2c_master_cfg_t const * const p_cfg);
static fsp_err_t iic_master_transfer_configure(transfer_instance_t const * p_transfer,
                                               iic_master_transfer_dir_t   direction);

#endif

/* Interrupt handlers */
static void iic_master_rxi_master(iic_master_instance_ctrl_t * p_instance_ctrl);
static void iic_master_txi_master(iic_master_instance_ctrl_t * p_instance_ctrl);
static void iic_master_tei_master(iic_master_instance_ctrl_t * p_instance_ctrl);
static void iic_master_err_master(iic_master_instance_ctrl_t * p_instance_ctrl);

/* Functions that manipulate hardware */
static void iic_master_open_hw_master(iic_master_instance_ctrl_t * const p_instance_ctrl,
                                      i2c_master_cfg_t const * const     p_cfg);
static fsp_err_t iic_master_run_hw_master(iic_master_instance_ctrl_t * const p_instance_ctrl);
static void      iic_master_rxi_read_data(iic_master_instance_ctrl_t * const p_instance_ctrl);
static void      iic_master_txi_send_address(iic_master_instance_ctrl_t * const p_instance_ctrl);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/

/* IIC Implementation of I2C device master interface */
i2c_master_api_t const g_i2c_master_on_iic =
{
    .open            = R_IIC_MASTER_Open,
    .read            = R_IIC_MASTER_Read,
    .write           = R_IIC_MASTER_Write,
    .abort           = R_IIC_MASTER_Abort,
    .slaveAddressSet = R_IIC_MASTER_SlaveAddressSet,
    .close           = R_IIC_MASTER_Close,
    .statusGet       = R_IIC_MASTER_StatusGet,
    .callbackSet     = R_IIC_MASTER_CallbackSet
};

/*******************************************************************************************************************//**
 * @addtogroup IIC_MASTER
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Opens the I2C device.
 *
 * @retval  FSP_SUCCESS                       Requested clock rate was set exactly.
 * @retval  FSP_ERR_ALREADY_OPEN              Module is already open.
 * @retval  FSP_ERR_IP_CHANNEL_NOT_PRESENT    Channel is not available on this MCU.
 * @retval  FSP_ERR_INVALID_ARGUMENT          Invalid input parameter.
 * @retval  FSP_ERR_ASSERTION                 Parameter check failure due to one or more reasons below:
 *                                            1. p_ctrl or p_cfg is NULL.
 *                                            2. extended parameter is NULL.
 *                                            3. Callback parameter is NULL.
 *                                            4. Invalid IRQ number assigned
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_Open (i2c_master_ctrl_t * const p_ctrl, i2c_master_cfg_t const * const p_cfg)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;
#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_ctrl != NULL);
    FSP_ASSERT(p_cfg != NULL);
    FSP_ASSERT(p_cfg->p_extend != NULL);
    FSP_ASSERT(p_cfg->rxi_irq >= (IRQn_Type) 0);
    FSP_ASSERT(p_cfg->txi_irq >= (IRQn_Type) 0);
    FSP_ASSERT(p_cfg->tei_irq >= (IRQn_Type) 0);
    FSP_ASSERT(p_cfg->eri_irq >= (IRQn_Type) 0);
    FSP_ERROR_RETURN(IIC_MASTER_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);

    FSP_ERROR_RETURN(BSP_FEATURE_IIC_VALID_CHANNEL_MASK & (1 << p_cfg->channel), FSP_ERR_IP_CHANNEL_NOT_PRESENT);

 #if IIC_MASTER_CFG_DMAC_ENABLE
    if ((NULL != p_cfg->p_transfer_rx) || (NULL != p_cfg->p_transfer_tx))
    {
        /* DMAC activation is not available for safety channel. */
        FSP_ERROR_RETURN(BSP_FEATURE_IIC_SAFETY_CHANNEL != p_cfg->channel, FSP_ERR_INVALID_ARGUMENT);
    }
 #endif
#endif
#if IIC_MASTER_CFG_DMAC_ENABLE
    fsp_err_t err = FSP_SUCCESS;
#endif

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_IIC, p_cfg->channel);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    if (p_cfg->channel != BSP_FEATURE_IIC_SAFETY_CHANNEL)
    {
        /* Non-Safety Peripheral */
        p_instance_ctrl->p_reg =
            (R_IIC0_Type *) ((uint32_t) R_IIC0 + (p_cfg->channel * ((uint32_t) R_IIC1 - (uint32_t) R_IIC0)));
    }
    else
    {
        /* Safety Peripheral */
        p_instance_ctrl->p_reg = (R_IIC0_Type *) BSP_FEATURE_IIC_SAFETY_CHANNEL_BASE_ADDRESS;
    }

    /* Record the pointer to the configuration structure for later use */
    p_instance_ctrl->p_cfg             = p_cfg;
    p_instance_ctrl->slave             = p_cfg->slave;
    p_instance_ctrl->addr_mode         = p_cfg->addr_mode;
    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

    /* Open the hardware in master mode. Performs IIC initialization as described in hardware manual (see Section
     * 'Initial Settings' of the RZ microprocessor manual). */
    iic_master_open_hw_master(p_instance_ctrl, p_cfg);

#if IIC_MASTER_CFG_DMAC_ENABLE

    /* Open the IIC transfer interface if available */
    err = iic_master_transfer_open(p_cfg);
    if (FSP_SUCCESS != err)
    {
        /* module stop */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
        R_BSP_MODULE_STOP(FSP_IP_IIC, p_instance_ctrl->p_cfg->channel);
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

        return err;
    }
#endif

    p_instance_ctrl->p_buff    = NULL;
    p_instance_ctrl->total     = 0U;
    p_instance_ctrl->remain    = 0U;
    p_instance_ctrl->loaded    = 0U;
    p_instance_ctrl->read      = false;
    p_instance_ctrl->restart   = false;
    p_instance_ctrl->err       = false;
    p_instance_ctrl->restarted = false;
    p_instance_ctrl->open      = IIC_MASTER_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Performs a read from the I2C device.
 * The caller will be notified when the operation has completed (successfully) by an
 * I2C_MASTER_EVENT_RX_COMPLETE in the callback.
 *
 * @retval  FSP_SUCCESS             Function executed without issue.
 * @retval  FSP_ERR_ASSERTION       p_ctrl, p_dest or bytes is NULL.
 * @retval  FSP_ERR_IN_USE          Bus busy condition. Another transfer was in progress.
 * @retval  FSP_ERR_NOT_OPEN        Handle is not initialized.  Call R_IIC_MASTER_Open to initialize the control block.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_Read (i2c_master_ctrl_t * const p_ctrl,
                             uint8_t * const           p_dest,
                             uint32_t const            bytes,
                             bool const                restart)
{
#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_ctrl != NULL);
    FSP_ASSERT(bytes != 0U);
#endif
    fsp_err_t err = FSP_SUCCESS;

    /* Record the restart information about this transfer.
     * This is done here to keep the parameter (argument) list of iic_master_read_write to 4. */
    ((iic_master_instance_ctrl_t *) p_ctrl)->restart = restart;

    /* Call the common helper function to perform I2C Read operation.*/
    err = iic_master_read_write(p_ctrl, p_dest, bytes, IIC_MASTER_TRANSFER_DIR_READ);

    return err;
}

/*******************************************************************************************************************//**
 * Performs a write to the I2C device.
 * The caller will be notified when the operation has completed (successfully) by an
 * I2C_MASTER_EVENT_TX_COMPLETE in the callback.
 *
 * @retval  FSP_SUCCESS           Function executed without issue.
 * @retval  FSP_ERR_ASSERTION     p_ctrl or p_src is NULL.
 * @retval  FSP_ERR_IN_USE        Bus busy condition. Another transfer was in progress.
 * @retval  FSP_ERR_NOT_OPEN      Handle is not initialized.  Call R_IIC_MASTER_Open to initialize the control block.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_Write (i2c_master_ctrl_t * const p_ctrl,
                              uint8_t * const           p_src,
                              uint32_t const            bytes,
                              bool const                restart)
{
#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_ctrl != NULL);
#endif
    fsp_err_t err = FSP_SUCCESS;

    /* Record the restart information about this transfer.
     * This is done here to keep the parameter (argument) list of iic_master_read_write to 4. */
    ((iic_master_instance_ctrl_t *) p_ctrl)->restart = restart;

    /* Call the common helper function to perform I2C Write operation.*/
    err = iic_master_read_write(p_ctrl, p_src, bytes, IIC_MASTER_TRANSFER_DIR_WRITE);

    return err;
}

/*******************************************************************************************************************//**
 * Safely aborts any in-progress transfer and forces the IIC peripheral into ready state.
 *
 *
 * @retval  FSP_SUCCESS             Channel was reset successfully.
 * @retval  FSP_ERR_ASSERTION       p_ctrl is NULL.
 * @retval  FSP_ERR_NOT_OPEN        Handle is not initialized.  Call R_IIC_MASTER_Open to initialize the control block.
 *
 * @note A callback will not be invoked in case an in-progress transfer gets aborted by calling this API.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_Abort (i2c_master_ctrl_t * const p_ctrl)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ERROR_RETURN(IIC_MASTER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Abort any transfer happening on the channel */
    iic_master_abort_seq_master(p_instance_ctrl, true);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets address and addressing mode of the slave device.
 * This function is used to set the device address and addressing mode of the slave
 * without reconfiguring the entire bus.
 *
 * @retval  FSP_SUCCESS             Address of the slave is set correctly.
 * @retval  FSP_ERR_ASSERTION       Pointer to control structure is NULL.
 * @retval  FSP_ERR_IN_USE          Another transfer was in-progress.
 * @retval  FSP_ERR_NOT_OPEN        Handle is not initialized.  Call R_IIC_MASTER_Open to initialize the control block.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_SlaveAddressSet (i2c_master_ctrl_t * const    p_ctrl,
                                        uint32_t const               slave,
                                        i2c_master_addr_mode_t const addr_mode)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

    fsp_err_t err = FSP_SUCCESS;

#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ERROR_RETURN(IIC_MASTER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Fail if there is already a transfer in progress */
    FSP_ERROR_RETURN(((0 == p_instance_ctrl->remain) && (false == p_instance_ctrl->restart)), FSP_ERR_IN_USE);
#endif

    /* Sets the address of the slave device */
    p_instance_ctrl->slave = slave;

    /* Sets the mode of addressing */
    p_instance_ctrl->addr_mode = addr_mode;

    return err;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements i2c_master_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_CallbackSet (i2c_master_ctrl_t * const          p_ctrl,
                                    void (                           * p_callback)(i2c_master_callback_args_t *),
                                    void const * const                 p_context,
                                    i2c_master_callback_args_t * const p_callback_memory)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

#if (IIC_MASTER_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(IIC_MASTER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides driver status.
 *
 * @retval     FSP_SUCCESS                   Status stored in p_status.
 * @retval     FSP_ERR_ASSERTION             NULL pointer.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_StatusGet (i2c_master_ctrl_t * const p_ctrl, i2c_master_status_t * p_status)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ASSERT(p_status != NULL);
#endif

    p_status->open = (IIC_MASTER_OPEN == p_instance_ctrl->open);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Closes the I2C device. May power down IIC peripheral.
 * This function will safely terminate any in-progress I2C transfers.
 *
 * @retval  FSP_SUCCESS         Device closed without issue.
 * @retval  FSP_ERR_ASSERTION   p_ctrl is NULL.
 * @retval  FSP_ERR_NOT_OPEN    Handle is not initialized.  Call R_IIC_MASTER_Open to initialize the control block.
 *
 * @note A callback will not be invoked in case an in-progress transfer gets aborted by calling this API.
 **********************************************************************************************************************/
fsp_err_t R_IIC_MASTER_Close (i2c_master_ctrl_t * const p_ctrl)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ERROR_RETURN(IIC_MASTER_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Abort an in-progress transfer with this device only */
    iic_master_abort_seq_master(p_instance_ctrl, true);

    /* Disable I2C interrupts. Described in hardware manual (see Section
     * 'I2C Bus Interrupt Enable Register (ICIER)' of the RZ microprocessor manual). */
    p_instance_ctrl->p_reg->ICIER = 0x00;

#if IIC_MASTER_CFG_DMAC_ENABLE

    /* Close the handles for the transfer interfaces */
    if (NULL != p_instance_ctrl->p_cfg->p_transfer_rx)
    {
        p_instance_ctrl->p_cfg->p_transfer_rx->p_api->close(p_instance_ctrl->p_cfg->p_transfer_rx->p_ctrl);
    }

    if (NULL != p_instance_ctrl->p_cfg->p_transfer_tx)
    {
        p_instance_ctrl->p_cfg->p_transfer_tx->p_api->close(p_instance_ctrl->p_cfg->p_transfer_tx->p_ctrl);
    }
#endif

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->eri_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->rxi_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->tei_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);

    /* module stop */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_STOP(FSP_IP_IIC, p_instance_ctrl->p_cfg->channel);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* The device is now considered closed */
    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup IIC_MASTER)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Helper function for handling I2C Read or Write.
 *
 * @param      p_ctrl          Pointer to control block
 * @param      p_buffer        Pointer to the buffer to store read/write data.
 * @param[in]  bytes           Number of bytes to be read/written.
 * @param[in]  direction       Read or Write
 *
 * @retval  FSP_SUCCESS           Function executed successfully.
 * @retval  FSP_ERR_ASSERTION     p_ctrl or p_buffer is NULL.
 * @retval  FSP_ERR_IN_USE        Another transfer was in progress.
 * @retval  FSP_ERR_NOT_OPEN      Handle is not initialized. Call R_IIC_MASTER_Open to initialize the control block.
 **********************************************************************************************************************/
static fsp_err_t iic_master_read_write (i2c_master_ctrl_t * const p_ctrl,
                                        uint8_t * const           p_buffer,
                                        uint32_t const            bytes,
                                        iic_master_transfer_dir_t direction)
{
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) p_ctrl;

#if IIC_MASTER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_buffer != NULL);
    FSP_ERROR_RETURN((p_instance_ctrl->open == IIC_MASTER_OPEN), FSP_ERR_NOT_OPEN);
    FSP_ASSERT(((iic_master_instance_ctrl_t *) p_ctrl)->p_callback != NULL);
#endif

    fsp_err_t err = FSP_SUCCESS;

    p_instance_ctrl->p_buff = p_buffer;
    p_instance_ctrl->total  = bytes;

    /* Handle the (different) addressing mode(s) */
    if (p_instance_ctrl->addr_mode == I2C_MASTER_ADDR_MODE_7BIT)
    {
        /* Set the address bytes according to a 7-bit slave read command */
        p_instance_ctrl->addr_high  = 0U;
        p_instance_ctrl->addr_low   = (uint8_t) ((p_instance_ctrl->slave << 1U) | (uint8_t) direction);
        p_instance_ctrl->addr_total = 1U;
    }

#if IIC_MASTER_CFG_ADDR_MODE_10_BIT_ENABLE
    else
    {
        /* Set the address bytes according to a 10-bit slave read command */
        p_instance_ctrl->addr_high =
            (uint8_t) (((p_instance_ctrl->slave >> 7U) | I2C_CODE_10BIT) & (uint8_t) ~(I2C_CODE_READ));
        p_instance_ctrl->addr_low = (uint8_t) p_instance_ctrl->slave;

        /* Addr total = 3 for Read and 2 for Write.
         * See Section "Communication Data Format" of the RZ microprocessor manual
         */
        p_instance_ctrl->addr_total = (uint8_t) ((uint8_t) direction + IIC_MASTER_SLAVE_10_BIT_ADDR_LEN_ADJUST);
    }
#endif

    p_instance_ctrl->read = (bool) direction;

    /* Kickoff the read operation as a master */
    err = iic_master_run_hw_master(p_instance_ctrl);

    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Single point for managing the logic around notifying a transfer has finished.
 *
 * @param[in]       p_instance_ctrl      Pointer to transfer that is ending.
 * @param[in]       event                The event code to pass to the callback.
 **********************************************************************************************************************/
static void iic_master_notify (iic_master_instance_ctrl_t * const p_instance_ctrl, i2c_master_event_t const event)
{
    i2c_master_callback_args_t args;

    /* Store callback arguments in memory provided by user if available. */
    i2c_master_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
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

    p_args->p_context = p_instance_ctrl->p_context;
    p_args->event     = event;

#if IIC_MASTER_CFG_DMAC_ENABLE

    /* Stop any DMAC assisted transfer for tx */
    const transfer_instance_t * p_transfer_tx = p_instance_ctrl->p_cfg->p_transfer_tx;
    if ((NULL != p_transfer_tx) && (!p_instance_ctrl->read))
    {
        p_transfer_tx->p_api->disable(p_transfer_tx->p_ctrl);
    }

    /* Stop any DMAC assisted transfer for rx */
    const transfer_instance_t * p_transfer_rx = p_instance_ctrl->p_cfg->p_transfer_rx;
    if ((NULL != p_transfer_rx) && (p_instance_ctrl->read))
    {
        p_transfer_rx->p_api->disable(p_transfer_rx->p_ctrl);
    }
#endif

    /* Now do the callback here */
    p_instance_ctrl->p_callback(p_args);

    if (NULL != p_instance_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_instance_ctrl->p_callback_memory = args;
    }

    /* Clear the err flags */
    p_instance_ctrl->err = false;
}

/*******************************************************************************************************************//**
 * Single point for managing the logic around aborting a transfer when operating as a master.
 *
 * @param[in]       p_instance_ctrl  Pointer to control structure of specific device.
 * @param[in]       iic_reset        Flag to enable IIC reset
 **********************************************************************************************************************/
static void iic_master_abort_seq_master (iic_master_instance_ctrl_t * const p_instance_ctrl, bool iic_reset)
{
    uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;

    /* Safely stop the hardware from operating. */

    /* Reset the peripheral */
    if (true == iic_reset)
    {
        /* Disable channel interrupts */
        p_instance_ctrl->p_reg->ICIER = 0x00;

        /* Wait for the value to reflect at the peripheral.
         * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
        IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER, 0, timeout_count);

        /* This helper function would do a full IIC reset
         * followed by re-initializing the required peripheral registers. */
        iic_master_open_hw_master(p_instance_ctrl, p_instance_ctrl->p_cfg);
    }

    /* Update the transfer descriptor to show no longer in-progress and an error */
    p_instance_ctrl->remain = 0U;

    /* Update the transfer descriptor to make sure interrupts no longer process */
    p_instance_ctrl->addr_loaded = p_instance_ctrl->addr_total;
    p_instance_ctrl->loaded      = p_instance_ctrl->total;
    p_instance_ctrl->restarted   = false;
    p_instance_ctrl->restart     = false;

    /* Enable Interrupts: TMOIE, ALIE, NAKIE, RIE, TIE.
     * Disable Interrupt: TEIE, STIE, SPIE
     * (see Section 'I2C Bus Interrupt Enable Register (ICIER)' of the RZ microprocessor manual).
     */
    p_instance_ctrl->p_reg->ICIER = IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK;

    /* Wait for the value to reflect at the peripheral.
     * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
    timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
    IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER,
                                      IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK,
                                      timeout_count);
}

/*******************************************************************************************************************//**
 * Performs the hardware initialization sequence when operating as a master (see Section
 * 'Initial Settings' of the RZ microprocessor manual).
 *
 * @param[in]  p_instance_ctrl       Pointer to IIC specific control structure
 * @param[in]  p_cfg                 Pointer to IIC specific configuration structure.
 **********************************************************************************************************************/
static void iic_master_open_hw_master (iic_master_instance_ctrl_t * const p_instance_ctrl,
                                       i2c_master_cfg_t const * const     p_cfg)
{
    /* Perform IIC reset */
    p_instance_ctrl->p_reg->ICCR1 = (uint8_t) IIC_MASTER_PRV_SCL_SDA_NOT_DRIVEN;

    /* Reset */
    p_instance_ctrl->p_reg->ICCR1 = (uint8_t) (IIC_MASTER_ICCR1_IICRST_BIT_MASK | IIC_MASTER_PRV_SCL_SDA_NOT_DRIVEN);

    /* Set valid interrupt contexts and user provided priority. Enable the interrupts at the GIC  */
    R_BSP_IrqCfgEnable(p_cfg->eri_irq, p_cfg->ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->txi_irq, p_cfg->ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->tei_irq, p_cfg->ipl, p_instance_ctrl);
    R_BSP_IrqCfgEnable(p_cfg->rxi_irq, p_cfg->ipl, p_instance_ctrl);

    /* Come out of IIC reset to internal reset */
    p_instance_ctrl->p_reg->ICCR1 =
        (uint8_t) (IIC_MASTER_ICCR1_ICE_BIT_MASK | IIC_MASTER_ICCR1_IICRST_BIT_MASK |
                   IIC_MASTER_PRV_SCL_SDA_NOT_DRIVEN);

    /* Configure the clock settings. This is set in the configuration structure by the tooling. */
    /* Set the number of counts that the clock remains low, bit 7 to 5 should be written as 1 */
    p_instance_ctrl->p_reg->ICBRL =
        (uint8_t) (IIC_MASTER_BUS_RATE_REG_RESERVED_BITS |
                   ((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend)->clock_settings.brl_value);

    /* Set the number of counts that the clock remains high, bit 7 to 5 should be written as 1 */
    p_instance_ctrl->p_reg->ICBRH = (uint8_t) (IIC_MASTER_BUS_RATE_REG_RESERVED_BITS |
                                               ((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend)->
                                               clock_settings.brh_value);

    /* Set the internal reference clock source for generating IIC clock */
    p_instance_ctrl->p_reg->ICMR1 = (uint8_t) (IIC_MASTER_BUS_MODE_REGISTER_1_MASK |
                                               (uint8_t) ((((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->
                                                            p_extend)->
                                                           clock_settings.
                                                           cks_value &
                                                           IIC_MASTER_INTERNAL_REF_CLOCK_SELECT_MAX) << 4U));

    /* Allow timeouts to be generated on the low value of SCL using either long or short mode */

    /* TMOL 'Timeout L Count Control' and TMOH 'Timeout H Count Control' will be set at the time of I2C reset.
     * This will enable time out detection for both SCLn high and low.
     * Only Set/Clear TMOS here to select long or short mode.
     * (see Section 'I2C Bus Mode Register 2 (ICMR2)' of the RZ microprocessor manual).
     */
    p_instance_ctrl->p_reg->ICMR2 = (uint8_t) (IIC_MASTER_BUS_MODE_REGISTER_2_MASK |
                                               (uint8_t) (IIC_MASTER_TIMEOUT_MODE_SHORT ==
                                                          ((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->
                                                           p_extend)->timeout_mode)
                                               |
                                               (uint8_t) (((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->
                                                           p_extend)->
                                                          timeout_scl_low << R_IIC0_ICMR2_TMOL_Pos));

    /* ICFER Register Settings:
     * 1. Enable timeout function.
     * 2. Enable master arbitration loss detection.
     * 3. Enable NACK arbitration loss detection.
     * 4. Disable Slave arbitration loss detection.
     * 5. Enable NACK reception transfer suspension.
     * 6. Use the digital noise filter circuit. Upon I2C reset, 'NF[1:0] Noise Filter Stage Select' will be set to 0x00
     * This would imply  'Filter out noise of up to 1 IIC cycle (single-stage filter)'
     * (see Section 'I2C Bus Mode Register 3 (ICMR3)' of the RZ microprocessor manual)
     * 7. Do not use the SCL synchronous circuit.
     * 8. Enable FM+ slope circuit if fast mode plus is enabled.
     * (see Section 'I2C Bus Function Enable Register' of the RZ microprocessor manual for more details)
     */
    p_instance_ctrl->p_reg->ICFER = IIC_MASTER_FUNCTION_ENABLE_INIT_SETTINGS;

    /* Ensure the HW is in master mode and does not behave as a slave to another master on the same channel. */
    p_instance_ctrl->p_reg->ICSER = 0x00;

    /* Enable Interrupts: TMOIE, ALIE, NAKIE, RIE, TIE.
     * Disable Interrupt: TEIE, STIE, SPIE
     * (see Section 'I2C Bus Interrupt Enable Register (ICIER)' of the RZ microprocessor manual).
     */
    p_instance_ctrl->p_reg->ICIER = IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK;

    /* Release IIC from internal reset */

    /* Reset */
    p_instance_ctrl->p_reg->ICCR1 = (uint8_t) (IIC_MASTER_ICCR1_ICE_BIT_MASK | IIC_MASTER_PRV_SCL_SDA_NOT_DRIVEN);
}

/*******************************************************************************************************************//**
 * Performs the data transfer described by the parameters when operating as a master.
 * See "Master Transmit Operation" and section "Master Receive Operation"
 * of the RZ microprocessor manual.
 *
 * @param[in]       p_instance_ctrl  Pointer to control structure of specific device.
 *
 * @retval  FSP_SUCCESS       Data transfer success.
 * @retval  FSP_ERR_IN_USE    If data transfer is in progress.
 **********************************************************************************************************************/
static fsp_err_t iic_master_run_hw_master (iic_master_instance_ctrl_t * const p_instance_ctrl)
{
    /* IICn operates using PCLKL. The ratio CPUnCLK (System Clock)/PCLKL gives the CPU cycles for 1 ICBRL count.
     * (CPUnCLK/PCLKL)*ICBRL gives the CPU cycles for entire ICBRL count.
     * Since each time we loop the timeout count will be decremented by 1 this would require at least 4 CPU clocks,
     * making the final timeout count as:
     * Timeout = ((CPUnCLK/PCLKL)*ICBRL)/4.
     */
    uint32_t cpu0fsel = (uint32_t) (R_SYSC_S->SCKCR2 & IIC_MASTER_SCKCR2_CPU0CLK_MASK);

    /* With this masking, cpu0fsel becomes 0U or 1U.
     * When cpu0fsel is 0U, CPU0CLK/PCLKL is 4U. Similarly, 1U is 8U.
     */

    uint32_t timeout_count = ((4U << cpu0fsel) * p_instance_ctrl->p_reg->ICBRL) >> 2U;

    /* Bus free time is counted by the Internal reference clock, so consider the division ratio. */
    timeout_count <<= p_instance_ctrl->p_reg->ICMR1_b.CKS;

    /* Check if this is a new transaction or a continuation */
    if (!p_instance_ctrl->restarted)
    {
        /* As per BBSY clearing conditions in section of the RZ microprocessor manual,
         * the BBSY bit is 0 after the bus free time (ICBRL setting)
         * if a start condition is not detected after a stop condition detection.
         */
        IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICCR2_b.BBSY, 0U, timeout_count);

        /* If bus is busy, return error */
        FSP_ERROR_RETURN((0U != timeout_count), FSP_ERR_IN_USE);

        /* This is not a restarted transaction. Enable TXI for the next transfer.
         * This had been disabled at the end of TXI interrupt.
         * The intention is to only enable IIC_MASTER_TXI_EN_BIT.
         * Writing the whole mask - IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK saves cycles.
         */
        p_instance_ctrl->p_reg->ICIER = IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK;
    }

    /* Initialize fields used during transfer */
    p_instance_ctrl->addr_loaded          = 0U;
    p_instance_ctrl->loaded               = 0U;
    p_instance_ctrl->remain               = p_instance_ctrl->total;
    p_instance_ctrl->addr_remain          = p_instance_ctrl->addr_total;
    p_instance_ctrl->err                  = false;
    p_instance_ctrl->dummy_read_completed = false;
    p_instance_ctrl->activation_on_rxi    = false;
    p_instance_ctrl->activation_on_txi    = false;
    p_instance_ctrl->address_restarted    = false;

    /* Allow timeouts to be generated on the low value of SCL using either short or long mode.
     * This gets disabled in case the previous transaction issues a restart.
     */

    /* TMOL 'Timeout L Count Control' and TMOH 'Timeout H Count Control' will be set at the time of I2C reset.
     * This will enable time out detection for both SCLn high and low.
     * Only Set/Clear TMOS here to select long or short mode.
     * (see Section 'I2C Bus Mode Register 2 (ICMR2)' of the RZ microprocessor manual).
     */
    p_instance_ctrl->p_reg->ICMR2 = (uint8_t) (IIC_MASTER_BUS_MODE_REGISTER_2_MASK |
                                               (uint8_t) (IIC_MASTER_TIMEOUT_MODE_SHORT ==
                                                          ((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->
                                                           p_extend)->timeout_mode)
                                               |
                                               (uint8_t) (((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->
                                                           p_extend)->
                                                          timeout_scl_low << R_IIC0_ICMR2_TMOL_Pos));

    /* Enable timeout function */
    p_instance_ctrl->p_reg->ICFER_b.TMOE = 1U;

    /* Clear STOP flag.
     * The stop condition may be detected in multi-master setups by another IIC module transaction or
     * after the previous IIC master transaction is aborted by reset.
     * In these cases, the STOP flag is not cleared in ISRs and it can lead to undesired interrupts
     * after SPIE is enabled. So it is cleared here.
     */
    p_instance_ctrl->p_reg->ICSR2 &= (uint8_t) ~(IIC_MASTER_ICSR2_STOP_BIT);

    /* Wait for the value to reflect at the peripheral.
     * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual. */
    timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
    IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICSR2_b.STOP, 0U, timeout_count);

    /* Enable TXI. This is treated differently to support restart functionality.
     * In case the previous IIC master transaction enabled restart, the queued TXI will fire a this point.
     *
     * The TXI had been GIC-disabled (but Peripheral enabled) before setting the
     * RS bit by the previous restart enabled transaction.
     * The RS bit mimics a "stop" followed by a "start" and keeps the bus busy.
     * As a part of the previous transaction, TXI fires at the peripheral level and is queued at the CPU.
     *
     * If the previous transaction was not restart enabled -
     * GIC-enable TXI which will fire after the start condition below.
     */
    R_BSP_IrqEnableNoClear(p_instance_ctrl->p_cfg->txi_irq);

    /* Enable SPIE to detect unexpected STOP condition. This is disabled between communication events as it can lead
     * to undesired interrupts in multi-master setups. */
    p_instance_ctrl->p_reg->ICIER = IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK | R_IIC0_ICIER_STIE_Msk |
                                    R_IIC0_ICIER_SPIE_Msk;

    /* If previous transaction did not end with restart, send a start condition */
    if (!p_instance_ctrl->restarted)
    {
        /* Request IIC to issue the start condition */
        p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_ST_BIT_MASK;
    }
    else
    {
        p_instance_ctrl->restarted = false;
    }

    /*
     * The Flowchart under "Master Transmit Operation" and section "Master Receive Operation"
     * of the RZ microprocessor manual is covered in the interrupts:
     *
     * 1. NACKF processing is handled in the ERI interrupt.
     *    For receive, dummy reading ICDRR is not required because the NACK processing in this driver resets the IIC peripheral.
     * 2. Data is written to ICDRT in the transmit interrupt (TDRE is set when a transmit interrupt fires).
     * 3. For transmit, stop is issued in the transmit end interrupt (TEND is set when a transmit end interrupt fires).
     * 4. For transmit, ICSR2 is cleared in the transmit end interrupt.
     * 5. For receive, remaining processing including reading ICDRR happens in the receive interrupt (RDRF is set when a receive interrupt fires).
     */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Handles the receive data full interrupt when operating as a master.
 *
 * @param[in]       p_instance_ctrl     The target IIC block's control block.
 **********************************************************************************************************************/
static void iic_master_rxi_master (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    volatile uint8_t dummy_read;

    /* First receive interrupt: Handle the special case of 1 or 2 byte read here */
    if (false == p_instance_ctrl->dummy_read_completed)
    {
        /* Enable WAIT for 1 or 2 byte read */
        if (2U <= p_instance_ctrl->total)
        {
            p_instance_ctrl->p_reg->ICMR3_b.WAIT = 1;
        }

        /* Enable NACK for 1 byte read */
        if (1U == p_instance_ctrl->remain)
        {
            /* Writes to be done separately.
             * See Note 1 in Section 'I2C Bus Mode Register 3 (ICMR3)' of the RZ microprocessor manual
             */
            p_instance_ctrl->p_reg->ICMR3_b.ACKWP = 1; /* Write enable ACKBT */
            p_instance_ctrl->p_reg->ICMR3_b.ACKBT = 1;
        }

#if IIC_MASTER_CFG_DMAC_ENABLE
        uint8_t volatile const * p_iic_master_rx_buffer = &(p_instance_ctrl->p_reg->ICDRR);

        /* Enable transfer support if possible */
        if ((NULL != p_instance_ctrl->p_cfg->p_transfer_rx) && (p_instance_ctrl->read) && (p_instance_ctrl->total > 3U))
        {
            p_instance_ctrl->p_cfg->p_transfer_rx->p_cfg->p_info->p_src  = (uint8_t *) (p_iic_master_rx_buffer);
            p_instance_ctrl->p_cfg->p_transfer_rx->p_cfg->p_info->p_dest = (void *) (p_instance_ctrl->p_buff);
            p_instance_ctrl->p_cfg->p_transfer_rx->p_cfg->p_info->length = p_instance_ctrl->total - 3U;

            /* Disable the corresponding IRQ when transferring using DMAC. */
            R_BSP_IrqDisable(p_instance_ctrl->p_cfg->rxi_irq);

            p_instance_ctrl->p_cfg->p_transfer_rx->p_api->reconfigure(p_instance_ctrl->p_cfg->p_transfer_rx->p_ctrl,
                                                                      p_instance_ctrl->p_cfg->p_transfer_rx->p_cfg->p_info);

            p_instance_ctrl->remain            = 3U;
            p_instance_ctrl->loaded            = p_instance_ctrl->total - 3U;
            p_instance_ctrl->activation_on_rxi = true;
        }
#endif

        /* Do a dummy read to clock the data into the ICDRR. */
        dummy_read = p_instance_ctrl->p_reg->ICDRR;
        FSP_PARAMETER_NOT_USED(dummy_read);

        /* Update the counter */
        p_instance_ctrl->dummy_read_completed = true;
    }

#if IIC_MASTER_CFG_DMAC_ENABLE

    /* If this is the interrupt that got fired after DMAC transfer,
     * ignore it as the DMAC has already taken care of the data transfer */
    else if (true == p_instance_ctrl->activation_on_rxi)
    {
        p_instance_ctrl->activation_on_rxi = false;
    }
#endif

    /* ICDRR contain valid received data */
    else if (0U < p_instance_ctrl->remain)
    {
        iic_master_rxi_read_data(p_instance_ctrl);
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * Handles the transmit data empty interrupt when operating as a master.
 *
 * @param[in]       p_instance_ctrl     The target IIC block's control block.
 **********************************************************************************************************************/
static void iic_master_txi_master (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;

    /* Check if we are issuing the slave address */
    if (0U < p_instance_ctrl->addr_remain)
    {
        iic_master_txi_send_address(p_instance_ctrl);
    }
    else if (!p_instance_ctrl->read)
    {
#if IIC_MASTER_CFG_DMAC_ENABLE

        /* If this is the interrupt that got fired after DMAC transfer,
         * ignore it as the DMAC has already taken care of the data transfer */
        if (true == p_instance_ctrl->activation_on_txi)
        {
            p_instance_ctrl->activation_on_txi = false;
        }
        else if (p_instance_ctrl->remain > 0U)
#else
        if (p_instance_ctrl->remain > 0U)
#endif
        {
            /* Write the data to ICDRT register */
            p_instance_ctrl->p_reg->ICDRT = p_instance_ctrl->p_buff[p_instance_ctrl->loaded];

            /* Update the number of bytes remaining for next pass */
            p_instance_ctrl->loaded++;
            p_instance_ctrl->remain--;
        }
        else
        {
            /* Do nothing */
        }

        /* We are done loading ICDRT, wait for TEND to send a stop/restart */
        if (0U == p_instance_ctrl->remain)
        {
            p_instance_ctrl->p_reg->ICIER_b.TIE = 0U;

            /* Wait for the value to reflect at the peripheral.
             * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
            IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER_b.TIE, 0U, timeout_count);

            /* Enable the transmit end IRQ, to issue a STOP or RESTART */
            /* Clear any pending TEND interrupts */
            R_BSP_IrqClearPending(p_instance_ctrl->p_cfg->tei_irq);

            /* Enable the TXEND interrupt */
            p_instance_ctrl->p_reg->ICIER_b.TEIE = 1U;

            /* No need to wait to check TEIE has actually become 1U; because if that's not the case,
             * no other operation can occur at this point */
        }
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * Handles the transmit end interrupt when operating as a master.
 * @note This interrupt is configured to be generated at the end of last byte of the requested transfer.
 *
 * @param[in]       p_instance_ctrl     The target IIC block's control block.
 **********************************************************************************************************************/
static void iic_master_tei_master (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;

    /* This is a 10 bit address read, issue a restart prior to the last address byte transmission  */
    if ((p_instance_ctrl->read) && (p_instance_ctrl->addr_remain == 1U) &&
        (false == p_instance_ctrl->address_restarted))
    {
#if IIC_MASTER_CFG_ADDR_MODE_10_BIT_ENABLE

        /* Enable TXI so that it fires after restart condition. */
        p_instance_ctrl->p_reg->ICIER_b.TIE = 1U;

        /* Request IIC to issue the restart condition */
        p_instance_ctrl->p_reg->ICCR2      = (uint8_t) IIC_MASTER_ICCR2_RS_BIT_MASK;
        p_instance_ctrl->address_restarted = true;
#endif
    }
    /* We are done with the transfer, send STOP or RESTART */
    else if (0U == p_instance_ctrl->remain)
    {
        /* Send RESTART */
        if (p_instance_ctrl->restart)
        {
            /* NOTE:Only disable in GIC, disabling in I2C would cause the
             * restart condition to fail because we are using the buffered
             * interrupt to start the next sequence */
            R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);
            p_instance_ctrl->p_reg->ICIER_b.TIE = 1U;

            /* Request IIC to issue the restart condition. At this point we will queue a TXI at the GIC level. */
            p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_RS_BIT_MASK;

            /* Disable timeout function */
            p_instance_ctrl->p_reg->ICFER_b.TMOE = 0;

            /* Remember that we issued a restart for the next transfer */
            p_instance_ctrl->restarted = true;
        }
        /* Send STOP */
        else
        {
            /* Clear STOP flag and set SP.
             * It is ok to clear other status' as this transaction is over.
             */
            p_instance_ctrl->p_reg->ICSR2 &= (uint8_t) ~(IIC_MASTER_ICSR2_STOP_BIT);

            /* Wait for the value to reflect at the peripheral.
             * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual. */
            timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
            IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICSR2_b.STOP, 0U, timeout_count);

            /* Request IIC to issue the stop condition */
            p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_SP_BIT_MASK; /* It is safe to write 0's to other bits. */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Disable the interrupt as we are done with the transfer */
    p_instance_ctrl->p_reg->ICIER_b.TEIE = 0U;

    /* Wait for the value to reflect at the peripheral.
     * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
    timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
    IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER_b.TEIE, 0U, timeout_count);
}

/*******************************************************************************************************************//**
 * Handles the error interrupts when operating as a master.
 *
 * @param[in]       p_instance_ctrl  Pointer to transfer control block
 **********************************************************************************************************************/
static void iic_master_err_master (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    /* Clear all the event flags except the receive data full, transmit end and transmit data empty flags*/
    uint8_t errs_events = IIC_MASTER_STATUS_REGISTER_2_ERR_MASK & p_instance_ctrl->p_reg->ICSR2;
    p_instance_ctrl->p_reg->ICSR2 = (uint8_t) ~IIC_MASTER_STATUS_REGISTER_2_ERR_MASK;

    /* Wait for the value to reflect at the peripheral.
     * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
    uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
    IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICSR2 & IIC_MASTER_STATUS_REGISTER_2_ERR_MASK,
                                      0U,
                                      timeout_count);

    /* If the event was an error event, then handle it */
    if ((errs_events &
         (uint8_t) ((uint8_t) (IIC_MASTER_ERR_EVENT_TIMEOUT) | (uint8_t) (IIC_MASTER_ERR_EVENT_ARBITRATION_LOSS))) |
        ((errs_events & (uint8_t) (IIC_MASTER_ERR_EVENT_NACK)) && (1U != p_instance_ctrl->p_reg->ICCR2_b.MST)))
    {
        /* Conditions to get here:
         * 1. This is Timeout and/or arbitration loss error during an ongoing transaction
         * 2. This is a NACK error and this device is no longer the active master on the bus.
         *    The MST bit here can get cleared:
         *     a. In case of an arbitration loss error.also occurs.
         *     b. If the slave timeout is lesser than master timeout and the slave releases
         *        the bus by performing an internal reset.
         *        Refer Section "I2C Bus Control Register 2 (ICCR2) - Clearing conditions for MST"
         *        of the RZ microprocessor manual.
         * 3. This is a Timeout error after attempting to issue a stop after detecting a NACK previously.
         *//* Set the error flag when an error event occurred */
        p_instance_ctrl->err = true;

        /* Abort an in-progress transfer with the current device */
        iic_master_abort_seq_master(p_instance_ctrl, true); /* This will reset the IIC Master driver */
        /* Notify anyone waiting that the transfer is Aborted due to error. */
        iic_master_notify(p_instance_ctrl, I2C_MASTER_EVENT_ABORTED);
    }
    else if ((errs_events & (uint8_t) (IIC_MASTER_ERR_EVENT_NACK)) && (1U == p_instance_ctrl->p_reg->ICCR2_b.MST))
    {
        /* MST bit must be set to issue a stop condition.
         * Refer Section "Issuing a Stop Condition" of the RZ microprocessor manual.
         */

        /* Set the error flag when an error event occurred
         * This will be checked after the stop condition is detected from the request below. */
        p_instance_ctrl->err = true;

        /* The sequence below is to handle a NACK received from slave in the middle of a write.
         * See item '[4]' under 'Figure Example master transmission flow' of the RZ microprocessor manual. */

        /* Request IIC to issue the stop condition */
        p_instance_ctrl->p_reg->ICSR2 &= (uint8_t) ~(IIC_MASTER_ICSR2_STOP_BIT);

        /* Wait for the value to reflect at the peripheral.
         * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual. */
        timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
        IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICSR2_b.STOP, 0U, timeout_count);

        p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_SP_BIT_MASK; /* It is safe to write 0's to other bits. */
        /* Allow timeouts to be generated on the low value of SCL using either long or short mode */
        p_instance_ctrl->p_reg->ICMR2 = (uint8_t) 0x02U |
                                        (uint8_t) (IIC_MASTER_TIMEOUT_MODE_SHORT ==
                                                   ((iic_master_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend)->
                                                   timeout_mode);
        p_instance_ctrl->p_reg->ICFER_b.TMOE = 1;

        /* This interrupt will be fired again when wither stop condition is sent
         * or the hardware detects the line is stuck low causing a timeout */
    }
    /* This is a STOP, START or RESTART event. We need to process these events only at the
     * end of the requisite transfers.
     * NOTE: Do not use p_transfer->loaded or p_transfer->remain to check whether the transfer is
     * completed, since using them would lead to a race condition between txi and eri interrupts in case
     * of one byte transfer which will result in BUS arbitration loss error */
    else if ((errs_events & (uint8_t) IIC_MASTER_ERR_EVENT_STOP) ||
             ((p_instance_ctrl->restarted) && (errs_events & (uint8_t) IIC_MASTER_ERR_EVENT_START)))
    {
        i2c_master_event_t event = I2C_MASTER_EVENT_ABORTED;
        if (false == p_instance_ctrl->err) /* Successful transaction */
        {
            /* Get the correct event to notify the user */
            event = (p_instance_ctrl->read) ? I2C_MASTER_EVENT_RX_COMPLETE : I2C_MASTER_EVENT_TX_COMPLETE;

            /* Disable STIE/SPIE to prevent errant interrupts in multi-master scenarios */
            p_instance_ctrl->p_reg->ICIER = IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK;

            /* Wait for the value to reflect at the peripheral.
             * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual */
            timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
            IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER,
                                              IIC_MASTER_INTERRUPT_ENABLE_INIT_MASK,
                                              timeout_count);
        }
        else if ((errs_events & (uint8_t) IIC_MASTER_ERR_EVENT_STOP))
        {
            /* This is the STOP condition requested due to a NACK error earlier.
             * Since the stop condition is successfully issued there is no need to reset the driver.
             */
            iic_master_abort_seq_master(p_instance_ctrl, false); /* Clear the transaction flags only */
        }
        else
        {
            /* Do nothing */
        }

        /* Notify anyone waiting */
        iic_master_notify(p_instance_ctrl, event);
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * Check valid receive data and set WAIT, NACK and STOP/RESTART bit in RXI handler.
 *
 * @param[in]       p_instance_ctrl  Pointer to transfer control block
 **********************************************************************************************************************/
static void iic_master_rxi_read_data (iic_master_instance_ctrl_t * const p_instance_ctrl)
{
    /* If next data = (final byte - 2), enable WAIT */
    if (3U == p_instance_ctrl->remain)
    {
        p_instance_ctrl->p_reg->ICMR3_b.WAIT = 1;
    }
    /* If next data = (final byte - 1), enable NACK  */
    else if (2U == p_instance_ctrl->remain)
    {
        /* Writes to be done separately.
         * See Note 1 in Section 'I2C Bus Mode Register 3 (ICMR3)' of the RZ microprocessor manual.
         */
        p_instance_ctrl->p_reg->ICMR3_b.ACKWP = 1; /* Write enable ACKBT */
        p_instance_ctrl->p_reg->ICMR3_b.ACKBT = 1;
    }
    /* If next data = final byte, send STOP or RESTART */
    else if (1U == p_instance_ctrl->remain)
    {
        if (p_instance_ctrl->restart)
        {
            /* NOTE:Only disable in GIC, disabling in I2C would cause the
             * restart condition to fail because we are using the buffered
             * interrupt to start the next sequence */
            R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);
            p_instance_ctrl->p_reg->ICIER_b.TIE = 1U;

            p_instance_ctrl->p_reg->ICMR3_b.ACKWP = 1; /* Write enable ACKBT */

            /* This bit clears to 0 automatically by issuing stop condition.
             * For restart condition, clear bit by software.
             */
            p_instance_ctrl->p_reg->ICMR3_b.ACKBT = 0;

            /* Request IIC to issue the restart condition */
            p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_RS_BIT_MASK;

            /* Disable timeout function */
            p_instance_ctrl->p_reg->ICFER_b.TMOE = 0;

            /* Remember that we issued a restart when doing the next transfer */
            p_instance_ctrl->restarted = true;
        }
        else
        {
            /* Clear STOP flag and set SP.
             * It is ok to clear other status' as this transaction is over.
             */
            p_instance_ctrl->p_reg->ICSR2 &= (uint8_t) ~(IIC_MASTER_ICSR2_STOP_BIT);

            /* Wait for the value to reflect at the peripheral.
             * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual. */
            uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;
            IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICSR2_b.STOP, 0U, timeout_count);

            /* Request IIC to issue the stop condition */
            p_instance_ctrl->p_reg->ICCR2 = (uint8_t) IIC_MASTER_ICCR2_SP_BIT_MASK; /* It is safe to write 0's to other bits. */

            /* STOP flag will not be set just yet. STOP will be set only after reading the last byte from ICDRR and clearing the WAIT.
             * See Point #7 under 'Master Receive Operation' of the RZ microprocessor manual.
             */
        }
    }
    else
    {
        /* Do nothing */
    }

    p_instance_ctrl->p_buff[p_instance_ctrl->loaded] = p_instance_ctrl->p_reg->ICDRR;

    /* Update the counter values */
    p_instance_ctrl->loaded++;
    p_instance_ctrl->remain--;

    /* If we are done with the reception, clear the WAIT bit */
    if (0U == p_instance_ctrl->remain)
    {
        p_instance_ctrl->p_reg->ICMR3_b.WAIT = 0;

        /* If this transaction does not have the restart flag set to true,
         * last byte has been read and WAIT has been cleared.
         * Callback will be issued by the ERI once the stop condition is detected
         * In case of restart flag set to true a callback will be issued by the ERI once the start
         * (from restart) condition is detected
         */
    }
}

/*******************************************************************************************************************//**
 * Write the address byte to the iic bus
 *
 * @param[in]       p_instance_ctrl  Pointer to transfer control block
 **********************************************************************************************************************/
static void iic_master_txi_send_address (iic_master_instance_ctrl_t * const p_instance_ctrl)
{
    /* This is a 10 bit read and we have transmitted the low byte, next is restart */
    if ((3U == p_instance_ctrl->addr_total) && (2U == p_instance_ctrl->addr_loaded) &&
        (false == p_instance_ctrl->address_restarted))
    {
#if IIC_MASTER_CFG_ADDR_MODE_10_BIT_ENABLE
        uint32_t timeout_count = IIC_MASTER_PERIPHERAL_REG_MAX_WAIT;

        /* For Read operation an extra address byte needs to be sent after issuing restart.
         * At this point we have sent the first 2 address bytes. Disable TXI.
         */
        p_instance_ctrl->p_reg->ICIER_b.TIE = 0U;

        /* Wait for the value to reflect at the peripheral.
         * See 'Note' under Table "Interrupt sources" of the RZ microprocessor manual. */
        IIC_MASTER_HARDWARE_REGISTER_WAIT(p_instance_ctrl->p_reg->ICIER_b.TIE, 0U, timeout_count);

        /* Enable the transmit end IRQ, so that we can generate a RESTART condition */
        /* Clear any pending TEND interrupts */
        R_BSP_IrqClearPending(p_instance_ctrl->p_cfg->tei_irq);

        /* Enable the TXEND interrupt */
        p_instance_ctrl->p_reg->ICIER_b.TEIE = 1U;

        /* No need to wait to check TEIE has actually become 1U; because if that's not the case,
         * no other operation can occur at this point */
#endif
    }
    else
    {
        /* Address low byte, this could either be a 7 bit address or low byte of 10 bit address */
        uint8_t address_byte = p_instance_ctrl->addr_low;

#if IIC_MASTER_CFG_ADDR_MODE_10_BIT_ENABLE

        /* 10 bit address, handle accordingly */
        if (p_instance_ctrl->addr_total > 1U)
        {
            /* MSB transfer, send address high byte with with R/W set to 0 */
            if (0U == p_instance_ctrl->addr_loaded)
            {
                address_byte = p_instance_ctrl->addr_high;
            }
            /* MSB transfer after restart of 10 bit read, send high byte with R/W set to 1 */
            else if ((2U == p_instance_ctrl->addr_loaded) && (3U == p_instance_ctrl->addr_total))
            {
                address_byte = p_instance_ctrl->addr_high | (uint8_t) I2C_CODE_READ;
            }
            /* Low byte transfer */
            else
            {
                address_byte = p_instance_ctrl->addr_low;
            }
        }
#endif

#if IIC_MASTER_CFG_DMAC_ENABLE
        uint8_t volatile const * p_iic_master_tx_buffer = &(p_instance_ctrl->p_reg->ICDRT);

        /* If this is the last address byte, enable transfer */
        if (1U == p_instance_ctrl->addr_remain)
        {
            if ((NULL != p_instance_ctrl->p_cfg->p_transfer_tx) && !(p_instance_ctrl->read) &&
                (p_instance_ctrl->total > 0U))
            {
                p_instance_ctrl->p_cfg->p_transfer_tx->p_cfg->p_info->p_src  = (void *) (p_instance_ctrl->p_buff);
                p_instance_ctrl->p_cfg->p_transfer_tx->p_cfg->p_info->p_dest = (uint8_t *) (p_iic_master_tx_buffer);
                p_instance_ctrl->p_cfg->p_transfer_tx->p_cfg->p_info->length = p_instance_ctrl->remain;

                /* Disable the corresponding IRQ when transferring using DMAC. */
                R_BSP_IrqDisable(p_instance_ctrl->p_cfg->txi_irq);

                p_instance_ctrl->p_cfg->p_transfer_tx->p_api->reconfigure(p_instance_ctrl->p_cfg->p_transfer_tx->p_ctrl,
                                                                          p_instance_ctrl->p_cfg->p_transfer_tx->p_cfg->p_info);

                p_instance_ctrl->remain            = 0U;
                p_instance_ctrl->loaded            = p_instance_ctrl->total;
                p_instance_ctrl->activation_on_txi = true;
            }
        }
#endif

        /* Write the address byte */
        p_instance_ctrl->p_reg->ICDRT = address_byte;

        /* Update the number of address bytes loaded for next pass */
        p_instance_ctrl->addr_loaded++;
        p_instance_ctrl->addr_remain--;
    }
}

#if IIC_MASTER_CFG_DMAC_ENABLE

/*******************************************************************************************************************//**
 * Configures IIC related transfer drivers (if enabled).
 *
 * @param[in]   p_cfg              Pointer to IIC specific configuration structure
 *
 * @retval      FSP_SUCCESS                Transfer interface initialized successfully.
 * @retval      FSP_ERR_ASSERTION          Pointer to transfer instance for I2C receive in p_cfg is NULL.
 **********************************************************************************************************************/
static fsp_err_t iic_master_transfer_open (i2c_master_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NULL != p_cfg->p_transfer_rx)
    {
        err = iic_master_transfer_configure(p_cfg->p_transfer_rx, IIC_MASTER_TRANSFER_DIR_READ);
        FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
    }

    if (NULL != p_cfg->p_transfer_tx)
    {
        err = iic_master_transfer_configure(p_cfg->p_transfer_tx, IIC_MASTER_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            if (NULL != p_cfg->p_transfer_rx)
            {
                p_cfg->p_transfer_rx->p_api->close(p_cfg->p_transfer_rx->p_ctrl);
            }

            return err;
        }
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Configures  IIC RX related transfer.
 * @param[in]     p_transfer                 Pointer to transfer instance
 * @param[in]     direction                  IIC transfer direction
 *
 * @retval        FSP_SUCCESS                Transfer interface is configured with valid parameters.
 * @retval        FSP_ERR_ASSERTION          Pointer to transfer instance for I2C receive in p_cfg is NULL.
 **********************************************************************************************************************/
static fsp_err_t iic_master_transfer_configure (transfer_instance_t const * p_transfer,
                                                iic_master_transfer_dir_t   direction)
{
    fsp_err_t err;

    /* Set default transfer info and open receive transfer module, if enabled. */
 #if (IIC_MASTER_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(NULL != p_transfer->p_api);
    FSP_ASSERT(NULL != p_transfer->p_ctrl);
    FSP_ASSERT(NULL != p_transfer->p_cfg);
    FSP_ASSERT(NULL != p_transfer->p_cfg->p_info);
 #endif
    transfer_info_t * p_info = p_transfer->p_cfg->p_info;
    if (IIC_MASTER_TRANSFER_DIR_READ == direction)
    {
        p_info->mode           = TRANSFER_MODE_NORMAL;
        p_info->src_addr_mode  = TRANSFER_ADDR_MODE_FIXED;
        p_info->dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED;
    }
    else
    {
        p_info->mode           = TRANSFER_MODE_NORMAL;
        p_info->src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED;
        p_info->dest_addr_mode = TRANSFER_ADDR_MODE_FIXED;
    }

    transfer_cfg_t * p_cfg = (transfer_cfg_t *) p_transfer->p_cfg;
    err = p_transfer->p_api->open(p_transfer->p_ctrl, p_cfg);
    FSP_ERROR_RETURN((FSP_SUCCESS == err), err);

    return FSP_SUCCESS;
}

#endif

/***********************************************************************************************************************
 * Interrupt Vectors
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Receive data full interrupt routine.
 *
 * This function implements the IIC Receive buffer full ISR routine.
 *
 **********************************************************************************************************************/
void iic_master_rxi_isr (void)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    iic_master_rxi_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Callback that must be called after a RX DMAC transfer completes.
 *
 * @param[in]     p_instance_ctrl     Pointer to IIC_MASTER instance control block
 **********************************************************************************************************************/
void iic_master_rx_dmac_callback (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Now that the transfer using DMAC is finished, enable the corresponding IRQ. */
    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->rxi_irq);

    iic_master_rxi_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Transmit data empty interrupt routine.
 *
 * This function implements the Transmit buffer empty ISR routine.
 *
 **********************************************************************************************************************/
void iic_master_txi_isr (void)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    iic_master_txi_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Callback that must be called after a TX DMAC transfer completes.
 *
 * @param[in]     p_instance_ctrl     Pointer to IIC_MASTER instance control block
 **********************************************************************************************************************/
void iic_master_tx_dmac_callback (iic_master_instance_ctrl_t * p_instance_ctrl)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Now that the transfer using DMAC is finished, enable the corresponding IRQ. */
    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->txi_irq);

    iic_master_txi_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Transmit end interrupt routine.
 *
 * This function implements the IIC Transmission End ISR routine.
 *
 **********************************************************************************************************************/
void iic_master_tei_isr (void)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    iic_master_tei_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Error and event interrupt routine.
 *
 * This function implements the IIC Event/Error.
 *
 **********************************************************************************************************************/
void iic_master_eri_isr (void)
{
    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    iic_master_instance_ctrl_t * p_instance_ctrl = (iic_master_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    iic_master_err_master(p_instance_ctrl);

    IIC_MASTER_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}
