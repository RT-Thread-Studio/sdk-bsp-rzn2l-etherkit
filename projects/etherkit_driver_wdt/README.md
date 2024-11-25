# RZ EtherKit Development Board WDT Usage Instructions

**English** | **[Chinese](./README_zh.md)**

## Introduction

This example mainly introduces how to use the WDT (Watchdog Timer) device on the EtherKit.

## Hardware Description

None

## Software Description

### FSP Configuration Instructions

Open the FSP tool and create a new stack, selecting `r_wdt`.

### RT-Thread Settings Configuration

![img](./figures/wps28.jpg)

## Running

### Compilation & Download

**RT-Thread Studio**: Download the EtherKit resource package in the RT-Thread Studio package manager, then create a new project and compile it.

**IAR**: First, double-click `mklinks.bat` to generate links for the rt-thread and libraries folders; then use Env to generate the IAR project; finally, double-click `project.eww` to open the IAR project and compile it.

After compilation, connect the Jlink interface of the development board to the PC, and download the firmware to the development board.

### Running Effects

![img](./figures/wps29.jpg)

## Notes

None

## References

Device and Driver: [WDT Device](#/rt-thread-version/rt-thread-standard/programming-manual/device/watchdog/watchdog)