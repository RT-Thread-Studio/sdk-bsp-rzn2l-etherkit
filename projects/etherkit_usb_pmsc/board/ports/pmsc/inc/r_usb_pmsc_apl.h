/* ${REA_DISCLAIMER_PLACEHOLDER} */

/******************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_usb_basic_api.h"
#include "r_usb_pmsc_api.h"

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
#define NUM_STRING_DESCRIPTOR    (7u)

#define USB_APL_DISABLE          (0)
#define USB_APL_ENABLE           (1)

#define USB_APL_DISABLE          (0)
#define USB_APL_ENABLE           (1)

/******************************************************************************
 * Exported global variables (to be accessed by other files)
 ******************************************************************************/
extern uint8_t g_apl_device[];

extern uint8_t   g_apl_qualifier_descriptor[];
extern uint8_t   g_apl_hs_configuration[];
extern uint8_t   g_apl_configuration[];
extern uint8_t * g_apl_string_table[];

/******************************************************************************
 * Typedef definitions
 ******************************************************************************/

/******************************************************************************
 * Exported global functions
 ******************************************************************************/

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 * End  Of File
 ******************************************************************************/
