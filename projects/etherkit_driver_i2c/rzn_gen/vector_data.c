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
            [308] = iic_master_eri_isr, /* IIC0_EEI (IIC0 Transfer error or event generation) */
            [309] = iic_master_rxi_isr, /* IIC0_RXI (IIC0 Receive data full) */
            [310] = iic_master_txi_isr, /* IIC0_TXI (IIC0 Transmit data empty) */
            [311] = iic_master_tei_isr, /* IIC0_TEI (IIC0 Transmit end) */
        };
        #if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [288] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_ERI), /* SCI0_ERI (SCI0 Receive error) */
            [289] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_RXI), /* SCI0_RXI (SCI0 Receive data full) */
            [290] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_TXI), /* SCI0_TXI (SCI0 Transmit data empty) */
            [291] = BSP_PRV_CR52_SEL_ENUM(EVENT_SCI0_TEI), /* SCI0_TEI (SCI0 Transmit end) */
            [308] = BSP_PRV_CR52_SEL_ENUM(EVENT_IIC0_EEI), /* IIC0_EEI (IIC0 Transfer error or event generation) */
            [309] = BSP_PRV_CR52_SEL_ENUM(EVENT_IIC0_RXI), /* IIC0_RXI (IIC0 Receive data full) */
            [310] = BSP_PRV_CR52_SEL_ENUM(EVENT_IIC0_TXI), /* IIC0_TXI (IIC0 Transmit data empty) */
            [311] = BSP_PRV_CR52_SEL_ENUM(EVENT_IIC0_TEI), /* IIC0_TEI (IIC0 Transmit end) */
        };
        #endif
        #endif