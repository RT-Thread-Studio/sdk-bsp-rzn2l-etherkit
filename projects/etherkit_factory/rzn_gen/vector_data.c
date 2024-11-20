/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
                        [12] = r_icu_isr, /* IRQ6 (External pin interrupt 6) */
            [13] = r_icu_isr, /* IRQ7 (External pin interrupt 7) */
            [125] = gpt_capture_a_isr, /* GPT_CCMPA (GPT1 GTCCRA input capture/compare match) */
            [126] = gpt_capture_b_isr, /* GPT_CCMPB (GPT1 GTCCRB input capture/compare match) */
            [131] = gpt_counter_overflow_isr, /* GPT_OVF (GPT1 GTCNT overflow (GTPR compare match)) */
            [134] = gpt_capture_a_isr, /* GPT_CCMPA (GPT2 GTCCRA input capture/compare match) */
            [135] = gpt_capture_b_isr, /* GPT_CCMPB (GPT2 GTCCRB input capture/compare match) */
            [140] = gpt_counter_overflow_isr, /* GPT_OVF (GPT2 GTCNT overflow (GTPR compare match)) */
            [143] = gpt_capture_a_isr, /* GPT_CCMPA (GPT3 GTCCRA input capture/compare match) */
            [144] = gpt_capture_b_isr, /* GPT_CCMPB (GPT3 GTCCRB input capture/compare match) */
            [149] = gpt_counter_overflow_isr, /* GPT_OVF (GPT3 GTCNT overflow (GTPR compare match)) */
            [188] = gpt_capture_a_isr, /* GPT_CCMPA (GPT8 GTCCRA input capture/compare match) */
            [194] = gpt_counter_overflow_isr, /* GPT_OVF (GPT8 GTCNT overflow (GTPR compare match)) */
            [206] = gpt_capture_a_isr, /* GPT_CCMPA (GPT10 GTCCRA input capture/compare match) */
            [212] = gpt_counter_overflow_isr, /* GPT_OVF (GPT10 GTCNT overflow (GTPR compare match)) */
            [251] = gmac_isr_pmt, /* GMAC_PMT (GMAC1 power management) */
            [252] = gmac_isr_sbd, /* GMAC_SBD (GMAC1 general interrupt) */
            [253] = ethsw_isr_intr, /* ETHSW_INTR (Ethernet Switch interrupt) */
            [288] = sci_uart_eri_isr, /* SCI0_ERI (SCI0 Receive error) */
            [289] = sci_uart_rxi_isr, /* SCI0_RXI (SCI0 Receive data full) */
            [290] = sci_uart_txi_isr, /* SCI0_TXI (SCI0 Transmit data empty) */
            [291] = sci_uart_tei_isr, /* SCI0_TEI (SCI0 Transmit end) */
            [308] = iic_master_eri_isr, /* IIC0_EEI (IIC0 Transfer error or event generation) */
            [309] = iic_master_rxi_isr, /* IIC0_RXI (IIC0 Receive data full) */
            [310] = iic_master_txi_isr, /* IIC0_TXI (IIC0 Transmit data empty) */
            [311] = iic_master_tei_isr, /* IIC0_TEI (IIC0 Transmit end) */
            [316] = canfd_rx_fifo_isr, /* CAN_RXF (CANFD RX FIFO interrupt) */
            [317] = canfd_error_isr, /* CAN_GLERR (CANFD Global error interrupt) */
            [318] = canfd_channel_tx_isr, /* CAN0_TX (CANFD0 Channel TX interrupt) */
            [319] = canfd_error_isr, /* CAN0_CHERR (CANFD0 Channel CAN error interrupt) */
            [321] = canfd_channel_tx_isr, /* CAN1_TX (CANFD1 Channel TX interrupt) */
            [322] = canfd_error_isr, /* CAN1_CHERR (CANFD1 Channel CAN error interrupt) */
            [432] = rtc_alarm_periodic_isr, /* RTC_ALM (Alarm interrupt) */
            [434] = rtc_alarm_periodic_isr, /* RTC_PRD (Fixed interval interrupt) */
            [435] = sci_uart_eri_isr, /* SCI5_ERI (SCI5 Receive error) */
            [436] = sci_uart_rxi_isr, /* SCI5_RXI (SCI5 Receive data full) */
            [437] = sci_uart_txi_isr, /* SCI5_TXI (SCI5 Transmit data empty) */
            [438] = sci_uart_tei_isr, /* SCI5_TEI (SCI5 Transmit end) */
        };
        #endif