# RZ EtherKit Development Board RS485 Usage Instructions

**中文** | [**English**](./README.md)

## Introduction

This example mainly introduces how to use the RS485 device on the EtherKit.

## Hardware Description

![img](./figures/wps41.jpg) 

## Software Description

### FSP Configuration Instructions

Open the FSP tool and create a new stack, selecting `r_sci_uart5`.

### RT-Thread Settings Configuration

None

### Project Example Description

The project is developed using FSP library functions.

## Running

### Compilation & Download

**RT-Thread Studio**: Download the EtherKit resource package in the RT-Thread Studio package manager, then create a new project and compile it.

**IAR**: First, double-click `mklinks.bat` to generate links for the rt-thread and libraries folders; then use Env to generate the IAR project; finally, double-click `project.eww` to open the IAR project and compile it.

After compilation, connect the Jlink interface of the development board to the PC, and download the firmware to the development board.

### Running Effects

Output the command `rs485_send` through the serial port and open another terminal on a different serial port to view the received data.

![image-20241122171605909](./figures/image-20241122171605909.png)

## Notes

None

## References

Device and Driver: [UART_V2 Device](#/rt-thread-version/rt-thread-standard/programming-manual/device/uart/uart_v2/uart)