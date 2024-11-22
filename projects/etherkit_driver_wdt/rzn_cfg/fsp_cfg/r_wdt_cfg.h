/* generated configuration header file - do not edit */
#ifndef R_WDT_CFG_H_
#define R_WDT_CFG_H_
#define WDT_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
             #define WDT_CFG_MULTIPLEX_INTERRUPT_SUPPORTED (0)
            #if WDT_CFG_MULTIPLEX_INTERRUPT_SUPPORTED
             #define WDT_CFG_MULTIPLEX_INTERRUPT_ENABLE         BSP_INTERRUPT_ENABLE
             #define WDT_CFG_MULTIPLEX_INTERRUPT_DISABLE        BSP_INTERRUPT_DISABLE
            #else
             #define WDT_CFG_MULTIPLEX_INTERRUPT_ENABLE
             #define WDT_CFG_MULTIPLEX_INTERRUPT_DISABLE
            #endif
#endif /* R_WDT_CFG_H_ */
