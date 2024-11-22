/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
                        [288] = sci_uart_eri_isr, /* SCI0_ERI (SCI0 Receive error) */
            [289] = sci_uart_rxi_isr, /* SCI0_RXI (SCI0 Receive data full) */
            [290] = sci_uart_txi_isr, /* SCI0_TXI (SCI0 Transmit data empty) */
            [291] = sci_uart_tei_isr, /* SCI0_TEI (SCI0 Transmit end) */
            [435] = sci_uart_eri_isr, /* SCI5_ERI (SCI5 Receive error) */
            [436] = sci_uart_rxi_isr, /* SCI5_RXI (SCI5 Receive data full) */
            [437] = sci_uart_txi_isr, /* SCI5_TXI (SCI5 Transmit data empty) */
            [438] = sci_uart_tei_isr, /* SCI5_TEI (SCI5 Transmit end) */
        };
        #endif