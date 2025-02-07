/*******************************************************************************
 * Copyright (c) 2012, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "opener_api.h"
#include "appcontype.h"
#include "trace.h"
#include "cipidentity.h"
#include "ciptcpipinterface.h"
#include "cipqos.h"

#include "hal_data.h"
#include <rtthread.h>

static bsp_io_level_t _pin_read (bsp_io_port_pin_t io_port_pin);
static void _pin_write (bsp_io_port_pin_t io_port_pin, uint8_t set_value);

static rt_err_t _set_pin_array( uint8_t const * const p_byte_array,
                                 size_t const byte_array_size,
                                 bsp_io_port_pin_t const * const p_pin_array,
                                 bsp_io_level_t const * const p_pin_inactive_levels,
                                 size_t const num_pins);

static rt_err_t _get_pin_array( uint8_t * const byte_array,
                                 size_t const byte_array_size,
                                 bsp_io_port_pin_t const * const p_pin_array,
                                 bsp_io_level_t const * const p_pin_inactive_levels,
                                 size_t const num_pins);

/**
 * Definition of the number of assembly object instances.
 */
#define DEMO_ASSEMBLY_LED_INPUT                     (100) /// 0x64, Instance Type: Static Input */
#define DEMO_ASSEMBLY_SW_INPUT                      (101) /// 0x65, Instance Type: Static Input */
#define DEMO_ASSEMBLY_LED_OUTPUT                    (150) /// 0x96, Instance Type: Static Output */
#define DEMO_ASSEMBLY_HEARTBEAT_FOR_INPUT_ONLY      (238) /// 0xEE, Instance Type: Static Output */
#define DEMO_ASSEMBLY_HEARTBEAT_FOR_LISTEN_ONLY     (237) /// 0xED, Instance Type: Static Output */
#define DEMO_ASSEMBLY_CONFIG                        (151) /// 0x97, Instance Type: Static Configuration */
#define DEMO_ASSEMBLY_EXPLICT                       (154) /// 0x9A, Instance Type: Static I/O */

/**
 * Definition of connection object instance IDs
 */
#define DEMO_CONNECTION_LED_EXCLUSIVE_OWNER (0)    /// For input assembly #100 and output assembly #150
#define DEMO_CONNECTION_LED_LISTEN_ONLY     (0)    /// For input assembly #100 and output assembly #237 (heart beat)
#define DEMO_CONNECTION_SW_INPUT_ONLY       (0)    /// For input assembly #101 and output assembly #238 (heart beat)

/**
 * Definition of Assembly object instance attribute #4 Size of data
 */
#define DEMO_ASSEMBLY_LED_INPUT_BYTES    (1)    /// For instance ID #100
#define DEMO_ASSEMBLY_SW_INPUT_BYTES     (1)    /// For instance ID #101
#define DEMO_ASSEMBLY_LED_OUTPUT_BYTES   (1)    /// For instance ID #150
#define DEMO_ASSEMBLY_CONFIG_BYTES       (4)    /// For instance ID #151
#define DEMO_ASSEMBLY_EXPLICT_BYTES      (4)    /// For instance ID #154

/**
 * Buffers assembled to Assembly object instances.
 */
static EipUint8 g_assembly_data_led_input[DEMO_ASSEMBLY_LED_INPUT_BYTES]     = {0}; /** Input */
static EipUint8 g_assembly_data_sw_input[DEMO_ASSEMBLY_SW_INPUT_BYTES]       = {0}; /** Input */
static EipUint8 g_assembly_data_led_output[DEMO_ASSEMBLY_LED_OUTPUT_BYTES]   = {0}; /** Output */
static EipUint8 g_assembly_data_config[DEMO_ASSEMBLY_CONFIG_BYTES]           = {0}; /** Configuration */
static EipUint8 g_assembly_data_explicit[DEMO_ASSEMBLY_EXPLICT_BYTES]        = {0}; /** Explicit */

/**
 * The number of pins of pin arrays which access to LED SW efficiently.
 */
#define DEMO_APP_PIN_ARRAY_LED_PIN_NUM           (2)
#define DEMO_APP_PIN_ARRAY_SW_PIN_NUM            (2)

#define BOARD_LED0_RED   (BSP_IO_PORT_14_PIN_3)
#define BOARD_LED2_GREEN (BSP_IO_PORT_14_PIN_1)

#define BOARD_KEY_PIN0    (BSP_IO_PORT_14_PIN_2)
#define BOARD_KEY_PIN1    (BSP_IO_PORT_16_PIN_3)

/**
 * Arrays for efficiently accessing LED/SW pins.
 */
static const bsp_io_port_pin_t g_led_pin_array[DEMO_APP_PIN_ARRAY_LED_PIN_NUM] =
{
     BOARD_LED0_RED,
     BOARD_LED2_GREEN
};

static const bsp_io_port_pin_t g_sw_pin_array[DEMO_APP_PIN_ARRAY_SW_PIN_NUM] =
{
     BOARD_KEY_PIN0,
     BOARD_KEY_PIN1
};

#define BOARD_IOLEVEL_LED_ON    (BSP_IO_LEVEL_HIGH)
#define BOARD_IOLEVEL_LED_OFF   (BSP_IO_LEVEL_LOW)
#define BOARD_IOLEVEL_SW_ON     (BSP_IO_LEVEL_LOW)
#define BOARD_IOLEVEL_SW_OFF    (BSP_IO_LEVEL_HIGH)

static const bsp_io_level_t g_led_pin_array_incactive_levels[DEMO_APP_PIN_ARRAY_LED_PIN_NUM] =
{
     BOARD_IOLEVEL_LED_OFF,
     BOARD_IOLEVEL_LED_OFF
};

static const bsp_io_level_t g_sw_pin_array_incactive_levels[DEMO_APP_PIN_ARRAY_SW_PIN_NUM] =
{
     BOARD_IOLEVEL_SW_OFF,
     BOARD_IOLEVEL_SW_OFF
};

/* local functions */

/* global functions called by the stack */
EipStatus ApplicationInitialization(void) {
  /** Create Assembly objects to be accessed by implicit connection. */
  CreateAssemblyObject( DEMO_ASSEMBLY_LED_INPUT, g_assembly_data_led_input, sizeof(g_assembly_data_led_input));
  CreateAssemblyObject( DEMO_ASSEMBLY_CONFIG, g_assembly_data_config, sizeof(g_assembly_data_config));
  CreateAssemblyObject( DEMO_ASSEMBLY_SW_INPUT, g_assembly_data_sw_input, sizeof(g_assembly_data_sw_input));
  CreateAssemblyObject(DEMO_ASSEMBLY_LED_OUTPUT, g_assembly_data_led_output, sizeof(g_assembly_data_led_output));
  CreateAssemblyObject(DEMO_ASSEMBLY_HEARTBEAT_FOR_INPUT_ONLY, NULL, 0);
  CreateAssemblyObject(DEMO_ASSEMBLY_HEARTBEAT_FOR_LISTEN_ONLY, NULL, 0);

  /** Create Assembly object to be accessed by explicit connection only. */
  CreateAssemblyObject(DEMO_ASSEMBLY_EXPLICT, g_assembly_data_explicit, sizeof(g_assembly_data_explicit));

  /**
   * Exclusive Owner Connection for LED I/O.
   * Configure Connection Points as Exclusive Owner connection (Vol.1 3-6.5.3 Exclusive Owner)
   *  T->O: application data
   *  O->T: application data
   */
  ConfigureExclusiveOwnerConnectionPoint(DEMO_CONNECTION_LED_EXCLUSIVE_OWNER,
                                          DEMO_ASSEMBLY_LED_OUTPUT,
                                          DEMO_ASSEMBLY_LED_INPUT,
                                          DEMO_ASSEMBLY_CONFIG);

  /**
   * Input Only Connection for SW input
   * Configure Connection Points as Input Only Connection. (Vol.1 3-6.6 Input Only)
   *  T->O: application data
   *  O->T: heart beat
   */
  ConfigureInputOnlyConnectionPoint(DEMO_CONNECTION_SW_INPUT_ONLY,
                                    DEMO_ASSEMBLY_HEARTBEAT_FOR_INPUT_ONLY,
                                    DEMO_ASSEMBLY_SW_INPUT,
                                    DEMO_ASSEMBLY_CONFIG);

  /**
   * Listen Only Connection for LED input of Exclusive Owner Connection.
   * Configure Connection Points as Input Listen Only Connection. (Vol.1 3-6.5 Listen Only)
   *  T->O: application data
   *  O->T: heart beat
   */
  ConfigureListenOnlyConnectionPoint(DEMO_CONNECTION_LED_LISTEN_ONLY,
                                      DEMO_ASSEMBLY_HEARTBEAT_FOR_LISTEN_ONLY,
                                      DEMO_ASSEMBLY_LED_INPUT,
                                      DEMO_ASSEMBLY_CONFIG);

#if defined(OPENER_ETHLINK_CNTRS_ENABLE) && 0 != OPENER_ETHLINK_CNTRS_ENABLE
  /* For the Ethernet Interface & Media Counters connect a PreGetCallback and
   *  a PostGetCallback.
   * The PreGetCallback is used to fetch the counters from the hardware.
   * The PostGetCallback is utilized by the GetAndClear service to clear
   *  the hardware counters after the current data have been transmitted.
   */
  {
    CipClass *p_eth_link_class = GetCipClass(kCipEthernetLinkClassCode);
    InsertGetSetCallback(p_eth_link_class,
                         EthLnkPreGetCallback,
                         kPreGetFunc);
    InsertGetSetCallback(p_eth_link_class,
                         EthLnkPostGetCallback,
                         kPostGetFunc);
    /* Specify the attributes for which the callback should be executed. */
    for (int idx = 0; idx < OPENER_ETHLINK_INSTANCE_CNT; ++idx)
    {
      CipAttributeStruct *p_eth_link_attr;
      CipInstance *p_eth_link_inst =
        GetCipInstance(p_eth_link_class, idx + 1);
      OPENER_ASSERT(p_eth_link_inst);

      /* Interface counters attribute */
      p_eth_link_attr = GetCipAttribute(p_eth_link_inst, 4);
      p_eth_link_attr->attribute_flags |= (kPreGetFunc | kPostGetFunc);
      /* Media counters attribute */
      p_eth_link_attr = GetCipAttribute(p_eth_link_inst, 5);
      p_eth_link_attr->attribute_flags |= (kPreGetFunc | kPostGetFunc);
    }
  }
#endif

  return kEipStatusOk;
}

void HandleApplication(void) {
  /* check if application needs to trigger an connection */
}

void CheckIoConnectionEvent(unsigned int output_assembly_id,
                            unsigned int input_assembly_id,
                            IoConnectionEvent io_connection_event) {
  /* maintain a correct output state according to the connection state*/

  (void) output_assembly_id; /* suppress compiler warning */
  (void) input_assembly_id; /* suppress compiler warning */
  (void) io_connection_event; /* suppress compiler warning */
}

EipStatus AfterAssemblyDataReceived (CipInstance * instance)
{
    /**
     * Check the instance number of Assembly object isntance.
     */
    switch (instance->instance_number)
    {
    case DEMO_ASSEMBLY_LED_OUTPUT:
        /** Write received assembly data into LED. */
        _set_pin_array(g_assembly_data_led_output,
                       DEMO_ASSEMBLY_LED_OUTPUT_BYTES,
                       g_led_pin_array,
                       g_led_pin_array_incactive_levels,
                       DEMO_APP_PIN_ARRAY_LED_PIN_NUM);
        break;
    default:
        break;
    }

    /** Return success code. */
    return kEipStatusOk;
}

EipBool8 BeforeAssemblyDataSend (CipInstance * pa_pstInstance)
{
    EipBool8 has_changed = false;

    /** Check instance number */
    switch( pa_pstInstance->instance_number )
    {
    case DEMO_ASSEMBLY_LED_INPUT:

        /** Read LED array data to assembly data */
        _get_pin_array(g_assembly_data_led_input,
                       DEMO_ASSEMBLY_LED_INPUT_BYTES,
                       g_led_pin_array,
                       g_led_pin_array_incactive_levels,
                       DEMO_APP_PIN_ARRAY_LED_PIN_NUM);
        has_changed = true;
        break;

    case DEMO_ASSEMBLY_SW_INPUT:

        /** Ready SW array data to assembly data */
        _get_pin_array(g_assembly_data_sw_input,
                       DEMO_ASSEMBLY_SW_INPUT_BYTES,
                       g_sw_pin_array,
                       g_sw_pin_array_incactive_levels,
                       DEMO_APP_PIN_ARRAY_SW_PIN_NUM);
        has_changed = true;
        break;

    default:
        break;
    }

    return has_changed;
}

EipStatus ResetDevice (void)
{
    /** Close All connections */
    CloseAllConnections();

    /** Enable Object parameters which should be enabled when Reset Service is required. */
    CipQosUpdateUsedSetQosValues(); ///< QoS DSCP values (#2 ~ #8) of QoS object instance.

    /** Reset Applications */
    memset(g_assembly_data_led_input,  0, DEMO_ASSEMBLY_LED_INPUT_BYTES);
    memset(g_assembly_data_sw_input,   0, DEMO_ASSEMBLY_SW_INPUT_BYTES);
    memset(g_assembly_data_led_output, 0, DEMO_ASSEMBLY_LED_OUTPUT_BYTES);
    memset(g_assembly_data_config,     0, DEMO_ASSEMBLY_CONFIG_BYTES);
    memset(g_assembly_data_explicit,   0, DEMO_ASSEMBLY_EXPLICT_BYTES);

    /** Return success code */
    return kEipStatusOk;
}

EipStatus ResetDeviceToInitialConfiguration (void)
{
    /** Reset encapsulation inactivity timeout (#13) of TCP/IP interface object instance to default value. */
    g_tcpip.encapsulation_inactivity_timeout = 120;

    /** Perform Type 0: Reset */
    ResetDevice();

    /** Return success code */
    return kEipStatusOk;
}

void*
CipCalloc(size_t number_of_elements,
          size_t size_of_element) {
  return calloc(number_of_elements, size_of_element);
}

void CipFree(void *data) {
  free(data);
}

void RunIdleChanged(EipUint32 run_idle_value) 
{
  OPENER_TRACE_INFO("Run/Idle handler triggered\n");
  if ((0x0001 & run_idle_value) == 1) 
  {
    CipIdentitySetExtendedDeviceStatus(kAtLeastOneIoConnectionInRunMode);
  } 
  else 
  {
    CipIdentitySetExtendedDeviceStatus(kAtLeastOneIoConnectionEstablishedAllInIdleMode);
  }
  (void) run_idle_value;
}

/*******************************************************************************************************************//**
 * @brief set array pins to the specified value
 **********************************************************************************************************************/
static rt_err_t _set_pin_array( uint8_t const * const p_byte_array,
                                 size_t const byte_array_size,
                                 bsp_io_port_pin_t const * const p_pin_array,
                                 bsp_io_level_t const * const p_pin_inactive_levels,
                                 size_t const num_pins)
{
    /** For scanning pins and bytes. */
    size_t pin_idx = 0;
    size_t byte_idx = 0;

    /** For bit data to be written to pin. */
    bsp_io_level_t bit_data = BSP_IO_LEVEL_LOW;

    R_BSP_PinAccessEnable();
    for ( pin_idx = 0; pin_idx < num_pins; pin_idx++ )
    {
        /** If the pin is disabled, */
        if( NULL == (int8_t) p_pin_array[pin_idx] )
        {
            /** Skip and continue */
            continue;
        }

        /** If byte index is over, */
        byte_idx = pin_idx / 8;
        if ( byte_idx >= byte_array_size )
        {
            /** Return with error */
            return RT_ERROR;
        }

        /** Write data */
        bit_data = (bsp_io_level_t) ((p_byte_array[byte_idx] >> (pin_idx % 8)) & BSP_IO_LEVEL_HIGH);
        _pin_write(p_pin_array[pin_idx], (bsp_io_level_t) ((bit_data ^ p_pin_inactive_levels[pin_idx]) & BSP_IO_LEVEL_HIGH));
    }
    R_BSP_PinAccessDisable();

    /** Return success code */
    return RT_EOK;
}

/*******************************************************************************************************************//**
 * @brief Get the values of array pins.
 **********************************************************************************************************************/
static rt_err_t _get_pin_array( uint8_t * const byte_array,
                                 size_t const byte_array_size,
                                 bsp_io_port_pin_t const * const p_pin_array,
                                 bsp_io_level_t const * const p_pin_inactive_levels,
                                 size_t const num_pins)
{
    /** For scanning pins and bytes. */
    size_t pin_idx = 0;
    size_t byte_idx = 0;

    /** For bit data to be written to pin. */
    bsp_io_level_t bit_data = BSP_IO_LEVEL_LOW;

    R_BSP_PinAccessEnable();
    for (pin_idx = 0; pin_idx < num_pins; pin_idx++ )
    {
        /** If the pin is disabled, */
        if( RT_NULL == (int8_t) p_pin_array[pin_idx] )
        {
            /** Skip and continue */
            continue;
        }

        /** If byte index is over, */
        byte_idx = pin_idx / 8;
        if ( byte_idx >= byte_array_size )
        {
            /** Return with error */
            return RT_ERROR;
        }

        /** Read data */
        bit_data = (bsp_io_level_t)( (_pin_read(p_pin_array[pin_idx]) ^ p_pin_inactive_levels[pin_idx]) & BSP_IO_LEVEL_HIGH );

        /** Apply to byte array */
        if (bit_data)
        {
            byte_array[byte_idx] = (byte_array[byte_idx] | (uint8_t)(bit_data << (pin_idx % 8)));
        }
        else
        {
            byte_array[byte_idx] = (byte_array[byte_idx] & (uint8_t)~(!bit_data << (pin_idx % 8)));
        }
    }
    R_BSP_PinAccessDisable();

    /** Return success code */
    return RT_EOK;
}

/*******************************************************************************************************************//**
 * Read the input value of the port in the specified region.
 *
 * @param[in]  port             The port
 *
 * @retval     Current input level
 **********************************************************************************************************************/
static bsp_io_level_t _pin_read (bsp_io_port_pin_t io_port_pin)
{
    bsp_io_level_t current_level = BSP_IO_LEVEL_LOW;

    R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)io_port_pin, &current_level);
    /** Return current pin level */
    return current_level;
}

/*******************************************************************************************************************//**
 * Set the output level of the port in the specified region.
 *
 * @param[in]  port             The port
 * @param[in]  set_value        The setting value
 **********************************************************************************************************************/
static void _pin_write (bsp_io_port_pin_t io_port_pin, uint8_t set_value)
{
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)io_port_pin, (bsp_io_level_t)set_value);
    R_BSP_PinAccessDisable();
}
