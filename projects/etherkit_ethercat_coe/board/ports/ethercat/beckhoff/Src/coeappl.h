/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
 * \addtogroup CoE CAN Application Profile over EtherCAT
 * @{
 */

/**
\file coeappl.h
\author EthercatSSC@beckhoff.com

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 COE6: add 0x10F0.2 (Backup parameter changed)<br>
<br>Changes to version V5.11:<br>
V5.12 COE4: add timestamp object (0x10F8) and update diagnosis handling<br>
V5.12 ECAT5: update Sync error counter/flag handling,check enum memory alignment depending on the processor,in case of a polled timer disable ESC interrupts during DC_CheckWatchdog<br>
<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "objdef.h"
#include "ecatappl.h"

#ifndef _COEAPPL_H_
#define _COEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
/*---------------------------------------------
-    0x1010
-----------------------------------------------*/
/**
* \brief 0x1010 (Store parameters) object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
    UINT32   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1010;

/*---------------------------------------------
-    0x1011
-----------------------------------------------*/
/**
* \brief 0x1011 (Restore default parameters) object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0;
    UINT32   aEntries[1];
} OBJ_STRUCT_PACKED_END
TOBJ1011;



/*---------------------------------------------
-    0x1018
-----------------------------------------------*/
/**
* \brief 0x1018 (Identity) object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
    UINT32   u32VendorID; /**< \brief EtherCAT vendor ID */
    UINT32   u32Productcode; /**< \brief Product code*/
    UINT32   u32Revision; /**< \brief Revision number*/
    UINT32   u32Serialnumber; /**< \brief Serial number*/
} OBJ_STRUCT_PACKED_END
TOBJ1018;



/*---------------------------------------------
-    0x10F0
-----------------------------------------------*/
/**
* \brief 0x10F0 (Backup parameter handling) object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
    UINT32   u32Checksum; /**< \brief Checksum*/
    /*ECATCHANGE_START(V5.13) COE6*/
	BOOLEAN(bBackupPrameterChanged); /**< \brief Backup Parameter Changed*/
    /*ECATCHANGE_END(V5.13) COE6*/
} OBJ_STRUCT_PACKED_END
TOBJ10F0;



/*---------------------------------------------
-    0x1C00
-----------------------------------------------*/
/**
* \brief 0x1C00 (Sync manager type) object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
    UINT16   aEntries[2]; /**< \brief Entry buffer (one for two SyncManagers)*/
} OBJ_STRUCT_PACKED_END
TOBJ1C00;

#define CHECK_DC_OVERRUN_IN_MS      4000 /**<\brief cycle time in ms to check the DC system time for overrun*/

#endif //_COEAPPL_H_

#if defined(_COEAPPL_) && (_COEAPPL_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global variables
------
-----------------------------------------------------------------------------------------*/

PROTO UINT64 u64Timestamp
#ifdef _COEAPPL_
= { 0 }
#endif
;

PROTO UINT32 u32LastDc32Value;
PROTO UINT32 u32CheckForDcOverrunCnt;
/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/
PROTO void COE_UpdateSyncErrorStatus(void);

PROTO void COE_ObjInit(void);
PROTO void COE_Main(void);
PROTO UINT16 COE_ObjDictionaryInit(void);
PROTO UINT16 COE_AddObjectToDic(TOBJECT OBJMEM * pNewObjEntry);
PROTO void COE_RemoveDicEntry(UINT16 index);
PROTO void COE_ClearObjDictionary(void);
PROTO OBJCONST TOBJECT OBJMEM * COE_GetObjectDictionary(void);
PROTO void COE_SyncTimeStamp(void);

PROTO UINT8 Read0x10F8(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess);
PROTO UINT16 AddObjectsToObjDictionary(TOBJECT OBJMEM * pObjEntry);
void InitStoreBackup(void);
PROTO UINT8 Write0x1010( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 Write0x1011( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );

#undef PROTO
/** @}*/
