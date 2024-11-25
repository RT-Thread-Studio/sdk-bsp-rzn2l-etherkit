# RZ EtherKit Development Board RGB Usage Guide

**English** | [**Chinese**](./README_zh.md)

## Introduction

This example serves as the first and simplest demonstration in the SDK, akin to a programmer's "Hello World." The main functionality is to make the onboard RGB-LED blink cyclically.

## Hardware Description

### Circuit and Pin Details

![img](./figures/wps31.jpg)  
**LED Circuit Diagram**

![img](./figures/wps32.jpg)  
**LED Pin Diagram**

As shown, the EtherKit provides three user LEDs:  
- **LED0 (RED)**: Corresponding to pin `P14_3`
- **LED1 (BLUE)**: Corresponding to pin `P14_0`
- **LED2 (GREEN)**: Corresponding to pin `P14_1`

The LEDs light up when the microcontroller pin outputs a low level and turn off when the output is high.

The physical location of the LEDs on the development board is shown below:

![img](./figures/wps33.jpg)  
**LED Placement**

## Software Description

The source code for this example is located in `/projects/etherkit_blink_led`.  

The RGB-LED pin definitions and RGB transition logic are detailed in the `src/hal_data.c` file.  

### RGB Control Code

```c
/* Configure LED pins */
#define LED_PIN_R   BSP_IO_PORT_14_PIN_3 /* Onboard RED LED pin */
#define LED_PIN_B   BSP_IO_PORT_14_PIN_0 /* Onboard BLUE LED pin */
#define LED_PIN_G   BSP_IO_PORT_14_PIN_1 /* Onboard GREEN LED pin */

do
{
    /* Get the current group number */
    group_current = count % group_num;

    /* Control RGB LEDs */
    rt_pin_write(LED_PIN_R, _blink_tab[group_current][0]);
    rt_pin_write(LED_PIN_B, _blink_tab[group_current][1]);
    rt_pin_write(LED_PIN_G, _blink_tab[group_current][2]);

    /* Log the LED states */
    LOG_D("group: %d | red led [%-3.3s] | | blue led [%-3.3s] | | green led [%-3.3s]",
          group_current,
          _blink_tab[group_current][0] == LED_ON ? "ON" : "OFF",
          _blink_tab[group_current][1] == LED_ON ? "ON" : "OFF",
          _blink_tab[group_current][2] == LED_ON ? "ON" : "OFF");

    count++;

    /* Delay for a short period */
    rt_thread_mdelay(500);

} while (count > 0);
```

## Execution

### Compilation & Download

- **RT-Thread Studio**: Use the package manager in RT-Thread Studio to download the EtherKit resource package. Create a new project and compile it.  
- **IAR**: Run `mklinks.bat` to create symbolic links for the `rt-thread` and `libraries` folders. Use `Env` to generate the IAR project. Open `project.eww` in IAR and compile it.

After compiling, connect the development board’s JLink interface to the PC and download the firmware to the board.

### Operation Effect

Press the reset button to restart the development board. Observe the RGB-LEDs blinking cyclically as shown:

![img](./figures/wps34.jpg)  
**Figure 2-4 RGB-LED Demonstration**

#### Serial Port Output
You can also connect to the default configured serial port of the development board on your PC using a terminal tool with a baud rate of **115200N**. The runtime logs from the board will be displayed in real-time:

```
[D/main] group: 0 | red led [OFF] | | blue led [OFF] | | green led [OFF]
[D/main] group: 1 | red led [ON ] | | blue led [OFF] | | green led [OFF]
[D/main] group: 2 | red led [OFF] | | blue led [ON ] | | green led [OFF]
[D/main] group: 3 | red led [OFF] | | blue led [OFF] | | green led [ON ]
[D/main] group: 4 | red led [ON ] | | blue led [OFF] | | green led [ON ]
[D/main] group: 5 | red led [ON ] | | blue led [ON ] | | green led [OFF]
[D/main] group: 6 | red led [OFF] | | blue led [ON ] | | green led [ON ]
[D/main] group: 7 | red led [ON ] | | blue led [ON ] | | green led [ON ]
```

## Notes

None at the moment.

## References

Devices and Drivers: [PIN Device](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/pin/pin)