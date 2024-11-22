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
            [300] = sci_uart_eri_isr, /* SCI3_ERI (SCI3 Receive error) */
            [301] = sci_uart_rxi_isr, /* SCI3_RXI (SCI3 Receive data full) */
            [302] = sci_uart_txi_isr, /* SCI3_TXI (SCI3 Transmit data empty) */
            [303] = sci_uart_tei_isr, /* SCI3_TEI (SCI3 Transmit end) */
        };
        #endif