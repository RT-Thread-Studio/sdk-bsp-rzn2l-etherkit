/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #include "bsp_api.h"
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (38)
        #endif
        /* ISR prototypes */
        void r_icu_isr(void);
        void gpt_capture_a_isr(void);
        void gpt_capture_b_isr(void);
        void gpt_counter_overflow_isr(void);
        void gmac_isr_pmt(void);
        void gmac_isr_sbd(void);
        void ethsw_isr_intr(void);
        void sci_uart_eri_isr(void);
        void sci_uart_rxi_isr(void);
        void sci_uart_txi_isr(void);
        void sci_uart_tei_isr(void);
        void iic_master_eri_isr(void);
        void iic_master_rxi_isr(void);
        void iic_master_txi_isr(void);
        void iic_master_tei_isr(void);
        void canfd_rx_fifo_isr(void);
        void canfd_error_isr(void);
        void canfd_channel_tx_isr(void);
        void rtc_alarm_periodic_isr(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_IRQ6 ((IRQn_Type) 12) /* IRQ6 (External pin interrupt 6) */
        #define VECTOR_NUMBER_IRQ7 ((IRQn_Type) 13) /* IRQ7 (External pin interrupt 7) */
        #define VECTOR_NUMBER_GPT1_CCMPA ((IRQn_Type) 125) /* GPT_CCMPA (GPT1 GTCCRA input capture/compare match) */
        #define VECTOR_NUMBER_GPT1_CCMPB ((IRQn_Type) 126) /* GPT_CCMPB (GPT1 GTCCRB input capture/compare match) */
        #define VECTOR_NUMBER_GPT1_OVF ((IRQn_Type) 131) /* GPT_OVF (GPT1 GTCNT overflow (GTPR compare match)) */
        #define VECTOR_NUMBER_GPT2_CCMPA ((IRQn_Type) 134) /* GPT_CCMPA (GPT2 GTCCRA input capture/compare match) */
        #define VECTOR_NUMBER_GPT2_CCMPB ((IRQn_Type) 135) /* GPT_CCMPB (GPT2 GTCCRB input capture/compare match) */
        #define VECTOR_NUMBER_GPT2_OVF ((IRQn_Type) 140) /* GPT_OVF (GPT2 GTCNT overflow (GTPR compare match)) */
        #define VECTOR_NUMBER_GPT3_CCMPA ((IRQn_Type) 143) /* GPT_CCMPA (GPT3 GTCCRA input capture/compare match) */
        #define VECTOR_NUMBER_GPT3_CCMPB ((IRQn_Type) 144) /* GPT_CCMPB (GPT3 GTCCRB input capture/compare match) */
        #define VECTOR_NUMBER_GPT3_OVF ((IRQn_Type) 149) /* GPT_OVF (GPT3 GTCNT overflow (GTPR compare match)) */
        #define VECTOR_NUMBER_GPT8_CCMPA ((IRQn_Type) 188) /* GPT_CCMPA (GPT8 GTCCRA input capture/compare match) */
        #define VECTOR_NUMBER_GPT8_OVF ((IRQn_Type) 194) /* GPT_OVF (GPT8 GTCNT overflow (GTPR compare match)) */
        #define VECTOR_NUMBER_GPT10_CCMPA ((IRQn_Type) 206) /* GPT_CCMPA (GPT10 GTCCRA input capture/compare match) */
        #define VECTOR_NUMBER_GPT10_OVF ((IRQn_Type) 212) /* GPT_OVF (GPT10 GTCNT overflow (GTPR compare match)) */
        #define VECTOR_NUMBER_GMAC_PMT ((IRQn_Type) 251) /* GMAC_PMT (GMAC1 power management) */
        #define VECTOR_NUMBER_GMAC_SBD ((IRQn_Type) 252) /* GMAC_SBD (GMAC1 general interrupt) */
        #define VECTOR_NUMBER_ETHSW_INTR ((IRQn_Type) 253) /* ETHSW_INTR (Ethernet Switch interrupt) */
        #define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type) 288) /* SCI0_ERI (SCI0 Receive error) */
        #define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type) 289) /* SCI0_RXI (SCI0 Receive data full) */
        #define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type) 290) /* SCI0_TXI (SCI0 Transmit data empty) */
        #define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type) 291) /* SCI0_TEI (SCI0 Transmit end) */
        #define VECTOR_NUMBER_IIC0_EEI ((IRQn_Type) 308) /* IIC0_EEI (IIC0 Transfer error or event generation) */
        #define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type) 309) /* IIC0_RXI (IIC0 Receive data full) */
        #define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type) 310) /* IIC0_TXI (IIC0 Transmit data empty) */
        #define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type) 311) /* IIC0_TEI (IIC0 Transmit end) */
        #define VECTOR_NUMBER_CAN_RXF ((IRQn_Type) 316) /* CAN_RXF (CANFD RX FIFO interrupt) */
        #define VECTOR_NUMBER_CAN_GLERR ((IRQn_Type) 317) /* CAN_GLERR (CANFD Global error interrupt) */
        #define VECTOR_NUMBER_CAN0_TX ((IRQn_Type) 318) /* CAN0_TX (CANFD0 Channel TX interrupt) */
        #define VECTOR_NUMBER_CAN0_CHERR ((IRQn_Type) 319) /* CAN0_CHERR (CANFD0 Channel CAN error interrupt) */
        #define VECTOR_NUMBER_CAN1_TX ((IRQn_Type) 321) /* CAN1_TX (CANFD1 Channel TX interrupt) */
        #define VECTOR_NUMBER_CAN1_CHERR ((IRQn_Type) 322) /* CAN1_CHERR (CANFD1 Channel CAN error interrupt) */
        #define VECTOR_NUMBER_RTC_ALM ((IRQn_Type) 432) /* RTC_ALM (Alarm interrupt) */
        #define VECTOR_NUMBER_RTC_PRD ((IRQn_Type) 434) /* RTC_PRD (Fixed interval interrupt) */
        #define VECTOR_NUMBER_SCI5_ERI ((IRQn_Type) 435) /* SCI5_ERI (SCI5 Receive error) */
        #define VECTOR_NUMBER_SCI5_RXI ((IRQn_Type) 436) /* SCI5_RXI (SCI5 Receive data full) */
        #define VECTOR_NUMBER_SCI5_TXI ((IRQn_Type) 437) /* SCI5_TXI (SCI5 Transmit data empty) */
        #define VECTOR_NUMBER_SCI5_TEI ((IRQn_Type) 438) /* SCI5_TEI (SCI5 Transmit end) */
        typedef enum IRQn {
            SoftwareGeneratedInt0 = -32,
            SoftwareGeneratedInt1 = -31,
            SoftwareGeneratedInt2 = -30,
            SoftwareGeneratedInt3 = -29,
            SoftwareGeneratedInt4 = -28,
            SoftwareGeneratedInt5 = -27,
            SoftwareGeneratedInt6 = -26,
            SoftwareGeneratedInt7 = -25,
            SoftwareGeneratedInt8 = -24,
            SoftwareGeneratedInt9 = -23,
            SoftwareGeneratedInt10 = -22,
            SoftwareGeneratedInt11 = -21,
            SoftwareGeneratedInt12 = -20,
            SoftwareGeneratedInt13 = -19,
            SoftwareGeneratedInt14 = -18,
            SoftwareGeneratedInt15 = -17,
            DebugCommunicationsChannelInt = -10,
            PerformanceMonitorCounterOverflowInt = -9,
            CrossTriggerInterfaceInt = -8,
            VritualCPUInterfaceMaintenanceInt = -7,
            HypervisorTimerInt = -6,
            VirtualTimerInt = -5,
            NonSecurePhysicalTimerInt = -2,
            IRQ6_IRQn = 12, /* IRQ6 (External pin interrupt 6) */
            IRQ7_IRQn = 13, /* IRQ7 (External pin interrupt 7) */
            GPT1_CCMPA_IRQn = 125, /* GPT_CCMPA (GPT1 GTCCRA input capture/compare match) */
            GPT1_CCMPB_IRQn = 126, /* GPT_CCMPB (GPT1 GTCCRB input capture/compare match) */
            GPT1_OVF_IRQn = 131, /* GPT_OVF (GPT1 GTCNT overflow (GTPR compare match)) */
            GPT2_CCMPA_IRQn = 134, /* GPT_CCMPA (GPT2 GTCCRA input capture/compare match) */
            GPT2_CCMPB_IRQn = 135, /* GPT_CCMPB (GPT2 GTCCRB input capture/compare match) */
            GPT2_OVF_IRQn = 140, /* GPT_OVF (GPT2 GTCNT overflow (GTPR compare match)) */
            GPT3_CCMPA_IRQn = 143, /* GPT_CCMPA (GPT3 GTCCRA input capture/compare match) */
            GPT3_CCMPB_IRQn = 144, /* GPT_CCMPB (GPT3 GTCCRB input capture/compare match) */
            GPT3_OVF_IRQn = 149, /* GPT_OVF (GPT3 GTCNT overflow (GTPR compare match)) */
            GPT8_CCMPA_IRQn = 188, /* GPT_CCMPA (GPT8 GTCCRA input capture/compare match) */
            GPT8_OVF_IRQn = 194, /* GPT_OVF (GPT8 GTCNT overflow (GTPR compare match)) */
            GPT10_CCMPA_IRQn = 206, /* GPT_CCMPA (GPT10 GTCCRA input capture/compare match) */
            GPT10_OVF_IRQn = 212, /* GPT_OVF (GPT10 GTCNT overflow (GTPR compare match)) */
            GMAC_PMT_IRQn = 251, /* GMAC_PMT (GMAC1 power management) */
            GMAC_SBD_IRQn = 252, /* GMAC_SBD (GMAC1 general interrupt) */
            ETHSW_INTR_IRQn = 253, /* ETHSW_INTR (Ethernet Switch interrupt) */
            SCI0_ERI_IRQn = 288, /* SCI0_ERI (SCI0 Receive error) */
            SCI0_RXI_IRQn = 289, /* SCI0_RXI (SCI0 Receive data full) */
            SCI0_TXI_IRQn = 290, /* SCI0_TXI (SCI0 Transmit data empty) */
            SCI0_TEI_IRQn = 291, /* SCI0_TEI (SCI0 Transmit end) */
            IIC0_EEI_IRQn = 308, /* IIC0_EEI (IIC0 Transfer error or event generation) */
            IIC0_RXI_IRQn = 309, /* IIC0_RXI (IIC0 Receive data full) */
            IIC0_TXI_IRQn = 310, /* IIC0_TXI (IIC0 Transmit data empty) */
            IIC0_TEI_IRQn = 311, /* IIC0_TEI (IIC0 Transmit end) */
            CAN_RXF_IRQn = 316, /* CAN_RXF (CANFD RX FIFO interrupt) */
            CAN_GLERR_IRQn = 317, /* CAN_GLERR (CANFD Global error interrupt) */
            CAN0_TX_IRQn = 318, /* CAN0_TX (CANFD0 Channel TX interrupt) */
            CAN0_CHERR_IRQn = 319, /* CAN0_CHERR (CANFD0 Channel CAN error interrupt) */
            CAN1_TX_IRQn = 321, /* CAN1_TX (CANFD1 Channel TX interrupt) */
            CAN1_CHERR_IRQn = 322, /* CAN1_CHERR (CANFD1 Channel CAN error interrupt) */
            RTC_ALM_IRQn = 432, /* RTC_ALM (Alarm interrupt) */
            RTC_PRD_IRQn = 434, /* RTC_PRD (Fixed interval interrupt) */
            SCI5_ERI_IRQn = 435, /* SCI5_ERI (SCI5 Receive error) */
            SCI5_RXI_IRQn = 436, /* SCI5_RXI (SCI5 Receive data full) */
            SCI5_TXI_IRQn = 437, /* SCI5_TXI (SCI5 Transmit data empty) */
            SCI5_TEI_IRQn = 438, /* SCI5_TEI (SCI5 Transmit end) */
            SHARED_PERIPHERAL_INTERRUPTS_MAX_ENTRIES = BSP_VECTOR_TABLE_MAX_ENTRIES
        } IRQn_Type;
        #endif /* VECTOR_DATA_H */