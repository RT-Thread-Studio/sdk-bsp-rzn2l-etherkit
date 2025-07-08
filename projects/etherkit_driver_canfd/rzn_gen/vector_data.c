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
            [316] = canfd_rx_fifo_isr, /* CAN_RXF (CANFD RX FIFO interrupt) */
            [317] = canfd_error_isr, /* CAN_GLERR (CANFD Global error interrupt) */
            [318] = canfd_channel_tx_isr, /* CAN0_TX (CANFD0 Channel TX interrupt) */
            [319] = canfd_error_isr, /* CAN0_CHERR (CANFD0 Channel CAN error interrupt) */
            [320] = canfd_common_fifo_rx_isr, /* CAN0_COMFRX (CANFD0 Common RX FIFO or TXQ interrupt) */
            [321] = canfd_channel_tx_isr, /* CAN1_TX (CANFD1 Channel TX interrupt) */
            [322] = canfd_error_isr, /* CAN1_CHERR (CANFD1 Channel CAN error interrupt) */
            [323] = canfd_common_fifo_rx_isr, /* CAN1_COMFRX (CANFD1 Common RX FIFO or TXQ interrupt) */
        };
        #if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [288] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_ERI), /* SCI0_ERI (SCI0 Receive error) */
            [289] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_RXI), /* SCI0_RXI (SCI0 Receive data full) */
            [290] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_TXI), /* SCI0_TXI (SCI0 Transmit data empty) */
            [291] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_TEI), /* SCI0_TEI (SCI0 Transmit end) */
            [316] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN_RXF), /* CAN_RXF (CANFD RX FIFO interrupt) */
            [317] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN_GLERR), /* CAN_GLERR (CANFD Global error interrupt) */
            [318] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN0_TX), /* CAN0_TX (CANFD0 Channel TX interrupt) */
            [319] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN0_CHERR), /* CAN0_CHERR (CANFD0 Channel CAN error interrupt) */
            [320] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN0_COMFRX), /* CAN0_COMFRX (CANFD0 Common RX FIFO or TXQ interrupt) */
            [321] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN1_TX), /* CAN1_TX (CANFD1 Channel TX interrupt) */
            [322] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN1_CHERR), /* CAN1_CHERR (CANFD1 Channel CAN error interrupt) */
            [323] = BSP_PRV_CR52_SEL_ENUM(EVENT_CAN1_COMFRX), /* CAN1_COMFRX (CANFD1 Common RX FIFO or TXQ interrupt) */
        };
        #endif
        #endif