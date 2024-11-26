# RZ EtherKit Development Board RS485 Usage Instructions

**English** | [**中文**](./README_zh.md)

## Introduction

This example demonstrates how to use the RS485 device on the EtherKit.

## Hardware Description

![image-20241126102525956](figures/image-20241126102525956.png)

## FSP Configuration Instructions

Open the FSP tool, create a new stack, and select `r_sci_uart5` with the following configuration:

![image-20241126102608069](figures/image-20241126102608069.png)

## Example Project Description

RS485 Send Function: The function sends data every 1 second, for a total of 10 times, with each transmission being 1 byte:

```c
int rs485_send_test(void)
{
   static uint8_t i;

   for(i =1; i <= 10; i++)
   {
      /* Send data */
      RS485_Send_Example(i);
      rt_thread_delay(1000);
   }
   return 0;
}
```

RS485 Receive Interrupt Function (Ensure to configure the receive interrupt name in FSP in advance):

```c
/* RS485_1 interrupt callback function */
void rs485_callback(uart_callback_args_t * p_args)
{
    rt_interrupt_enter();

    switch(p_args->event)
    {
        /* Print received data */
        case UART_EVENT_RX_CHAR:
          {
            rt_kprintf("%d\n", p_args->data);
            break;
          }
        default:
            break;
    }
}
```

## Compilation & Download

* **RT-Thread Studio**: In RT-Thread Studio’s package manager, download the EtherKit resource package, create a new project, and compile it.
* **IAR**: First, double-click `mklinks.bat` to create symbolic links between RT-Thread and the libraries folder. Then, use the `Env` tool to generate the IAR project. Finally, double-click `project.eww` to open the IAR project and compile it.

After compiling, connect the development board’s JLink interface to the PC and download the firmware to the development board.

### Run Effect

After entering the `rs485_send` command in the serial terminal, open another serial terminal to observe the received data:

![image-20241126102934625](figures/image-20241126102934625.png)

![image-20241126102958240](figures/image-20241126102958240.png)