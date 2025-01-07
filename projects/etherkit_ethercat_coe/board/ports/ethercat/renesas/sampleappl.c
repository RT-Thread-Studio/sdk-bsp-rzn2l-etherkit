/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
\addtogroup SampleAppl Sample Application
@{
*/

/**
\file Sampleappl.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.12

<br>Changes to version V5.11:<br>
V5.12 ECAT1: update SM Parameter measurement (based on the system time), enhancement for input only devices and no mailbox support, use only 16Bit pointer in process data length caluclation<br>
V5.12 ECAT2: big endian changes<br>
V5.12 EOE1: move icmp sample to the sampleappl,add EoE application interface functions<br>
V5.12 EOE2: prevent static ethernet buffer to be freed<br>
V5.12 EOE3: fix memory leaks in sample ICMP application<br>
V5.12 FOE1: update new interface,move the FoE sample to sampleappl,add FoE application callback functions<br>
<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
V5.11 ECAT4: enhance SM/Sync monitoring for input/output only slaves<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT10: Add missing include 'objdef.h'<br>
              Add process data size calculation to sampleappl<br>
V5.10 ECAT6: Add "USE_DEFAULT_MAIN" to enable or disable the main function<br>
V5.10 FC1100: Stop stack if hardware init failed<br>
<br>Changes to version V5.0:<br>
V5.01 APPL2: Update Sample Application Output mapping<br>
V5.0: file created
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "applInterface.h"
#include "coeappl.h"

#define _SAMPLE_APPLICATION_
#include "sampleappl.h"
#undef _SAMPLE_APPLICATION_

#include "cia402appl.h"
#include "hal_data.h"
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/
#if (CiA402_SAMPLE_APPLICATION == 1)
extern TCiA402Axis       LocalAxes[MAX_AXES];
#endif // #if (CiA402_SAMPLE_APPLICATION == 1)
/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{
	FSP_PARAMETER_NOT_USED(stateTrans);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
             all general settings were checked to start the mailbox handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case this function will be called cyclically
            until a value unequal NOERROR_INWORK is returned

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
           all general settings were checked to start the input handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
	 UINT32 Sync0CycleTime = 0;
	    if(sConfiguredModuleIdentList.u16SubIndex0 == 0)
	    {
	        /* Object 0xF030 was not written before
	        * => update object 0xF010 (Module profile list) and 0xF050 (Detected Module List)*/

	        UINT8 cnt = 0;


	        /*Update 0xF010.0 */
//	        sModuleProfileInfo.u16SubIndex0 = sRxPDOassign.u16SubIndex0;

	        /*Update 0xF050.0*/
	        sDetectedModuleIdentList.u16SubIndex0 = sRxPDOassign.u16SubIndex0;

	        for (cnt = 0 ; cnt < sRxPDOassign.u16SubIndex0; cnt++)
	        {
	            /*all Modules have the same profile number*/
	/*ECATCHANGE_START(V5.13) CIA402 2*/
//	            sModuleProfileInfo.aEntries[cnt] = (DEVICE_PROFILE_TYPE >> 16);
	/*ECATCHANGE_END(V5.13) CIA402 2*/

	            switch((sRxPDOassign.aEntries[cnt] & 0x000F)) //get only identification of the PDO mapping object
	            {
	                case 0x0:   //csv/csp PDO selected
	                    sDetectedModuleIdentList.aEntries[cnt] = CSV_CSP_MODULE_ID;
	                break;
	                case 0x1:   //csp PDO selected
	                    sDetectedModuleIdentList.aEntries[cnt] = CSP_MODULE_ID;
	                break;
	                case 0x2:   //csv PDO selected
	                    sDetectedModuleIdentList.aEntries[cnt] = CSV_MODULE_ID;
	                break;

	            }

	        }
	    }

	    HW_EscReadDWord(Sync0CycleTime, ESC_DC_SYNC0_CYCLETIME_OFFSET);
	    Sync0CycleTime = SWAPDWORD(Sync0CycleTime);

	    /*Init CiA402 structure if the device is in SM Sync mode
	    the CiA402 structure will be Initialized after calculation of the Cycle time*/
	    if(bDcSyncActive == TRUE)
	    {
	        UINT16 i;
	        Sync0CycleTime = Sync0CycleTime / 1000; //get cycle time in us
	        for(i = 0; i< MAX_AXES;i++)
	        {
	                if (LocalAxes[i].bAxisIsActive)
	                {
	                    LocalAxes[i].u32CycleTime = Sync0CycleTime;
	                }
	        }
	    }
#endif // #if (CiA402_SAMPLE_APPLICATION == 1)
	    FSP_PARAMETER_NOT_USED(pIntMask);
	    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16* pInputSize,UINT16* pOutputSize)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 PDOAssignEntryCnt = 0;
    UINT8 AxisIndex = 0;
    UINT16 u16cnt = 0;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;
    TOBJECT OBJMEM *pDiCEntry = NULL;


    if (sRxPDOassign.u16SubIndex0 > MAX_AXES)
    {
        return ALSTATUSCODE_NOVALIDOUTPUTS;
    }

    /*Update object dictionary according to activated axis
    which axes are activated is calculated by object 0x1C12*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        /*The PDO mapping objects are specified with an 0x10 offset => get the axis index*/
        AxisIndex = (sRxPDOassign.aEntries[PDOAssignEntryCnt] & 0xF0) >> 4;

        if(AxisIndex == PDOAssignEntryCnt)
        {
            /*Axis is mapped to process data check if axis objects need to be added to the object dictionary*/

            if(!LocalAxes[PDOAssignEntryCnt].bAxisIsActive)
            {
                /*add objects to dictionary*/
                pDiCEntry = LocalAxes[PDOAssignEntryCnt].ObjDic;

                while(pDiCEntry->Index != 0xFFFF)
                {
                    result = (UINT16)COE_AddObjectToDic(pDiCEntry);

                    if(result != 0)
                    {
                        return result;
                    }

                    pDiCEntry++;    //get next entry
                }


                LocalAxes[PDOAssignEntryCnt].bAxisIsActive = TRUE;
            }

        }
        else
        {
            /*Axis is not mapped to process data check if axis objects need to be removed from object dictionary*/
            if(LocalAxes[PDOAssignEntryCnt].bAxisIsActive)
            {
                /*add objects to dictionary*/
                pDiCEntry = LocalAxes[PDOAssignEntryCnt].ObjDic;

                while(pDiCEntry->Index != 0xFFFF)
                {
                    COE_RemoveDicEntry(pDiCEntry->Index);

                    pDiCEntry++;    //get next entry
                }


                LocalAxes[PDOAssignEntryCnt].bAxisIsActive = FALSE;
            }
        }

    }

    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        switch ((sRxPDOassign.aEntries[PDOAssignEntryCnt] & 0x000F))    //mask Axis type (supported modes)
        {
            case 0:
                /*drive mode supported    csv(cyclic sync velocity) : bit 8
                                        csp (cyclic sync position) : bit 7*/
                LocalAxes[PDOAssignEntryCnt].Objects.objSupportedDriveModes = 0x180;
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap0.u16SubIndex0;u16cnt++)
                {
                    OutputSize +=(UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap0.aEntries[u16cnt] & 0xFF);
                }
                break;
            case 1:
                /*drive mode supported    csp (cyclic sync position) : bit 7*/
                LocalAxes[PDOAssignEntryCnt].Objects.objSupportedDriveModes = 0x80;
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap1.u16SubIndex0;u16cnt++)
                {
                    OutputSize +=(UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap1.aEntries[u16cnt] & 0xFF);
                }
                break;
            case 2:
                    /*drive mode supported    csv(cyclic sync velocity) : bit 8*/
                LocalAxes[PDOAssignEntryCnt].Objects.objSupportedDriveModes= 0x100;
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap2.u16SubIndex0;u16cnt++)
                {
                    OutputSize += (UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sRxPDOMap2.aEntries[u16cnt] & 0xFF);;
                }
                break;
        }
#if CDP_SAMPLE
        /* Object 0x17FF is mapped in RxPDO. */
        if (sRxPDOassign.aEntries[PDOAssignEntryCnt] == 0x17FF)
        {
			for(u16cnt =0 ; u16cnt < DeviceUserRxPDOMap0x17FF.u16SubIndex0;u16cnt++)
			{
				OutputSize +=(UINT16)(DeviceUserRxPDOMap0x17FF.aEntries[u16cnt] & 0xFF);
			}
			break;
        }
#endif	// #if CDP_SAMPLE

    }

    OutputSize = OutputSize >> 3;

    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            switch ((sTxPDOassign.aEntries[PDOAssignEntryCnt] & 0x000F))    //mask Axis type (supported modes)
            {
            case 0: /*csp/csv*/
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap0.u16SubIndex0;u16cnt++)
                {
                    InputSize +=(UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap0.aEntries[u16cnt] & 0xFF);
                }
                break;
            case 1: /*csp*/
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap1.u16SubIndex0;u16cnt++)
                {
                    InputSize +=(UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap1.aEntries[u16cnt] & 0xFF);
                }
                break;
            case 2: /*csv*/
                for(u16cnt =0 ; u16cnt < LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap2.u16SubIndex0;u16cnt++)
                {
                    InputSize +=(UINT16)(LocalAxes[PDOAssignEntryCnt].Objects.sTxPDOMap2.aEntries[u16cnt] & 0xFF);
                }
                break;
            }
#if CDP_SAMPLE
			/* Object 0x1BFF is mapped in TxPDO. */
			if (sTxPDOassign.aEntries[PDOAssignEntryCnt] == 0x1BFF)
			{
				for(u16cnt =0 ; u16cnt < DeviceUserRxPDOMap0x17FF.u16SubIndex0;u16cnt++)
				{
					InputSize +=(UINT16)(DeviceUserTxPDOMap0x1BFF.aEntries[u16cnt] & 0xFF);
				}
				break;
			}
#endif	// #if CDP_SAMPLE
        }

        InputSize = InputSize >> 3;
    }

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;

#else

    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM * pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
        if(pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
            for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
/*ECATCHANGE_START(V5.12) ECAT1*/
                pPDOEntry = (UINT32 *)((UINT16 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3)/2);    //goto PDO entry
/*ECATCHANGE_END(V5.12) ECAT1*/
                // we increment the expected output size depending on the mapped Entry
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;

    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if(pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
                for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
/*ECATCHANGE_START(V5.12) ECAT1*/
                    pPDOEntry = (UINT32 *)((UINT16 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3)/2);    //goto PDO entry
/*ECATCHANGE_END(V5.12) ECAT1*/
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;

#endif // #if (CiA402_SAMPLE_APPLICATION == 1)
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data
\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_InputMapping(UINT16* pData)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
    UINT16 j = 0;
    UINT8 *pTmpData = (UINT8 *)pData;
    UINT8 AxisIndex;

    for (j = 0; j < sTxPDOassign.u16SubIndex0; j++)
    {
        /*The Axis index is based on the PDO mapping offset (0x10)*/
        AxisIndex = ((sTxPDOassign.aEntries[j] & 0xF0) >> 4);

        switch ((sTxPDOassign.aEntries[j]& 0x000F))
        {
        case 0:    //copy csp/csv TxPDO entries
            {
                TCiA402PDO1A00 *pInputs = (TCiA402PDO1A00 *)pTmpData;
                pInputs->ObjStatusWord = SWAPWORD(LocalAxes[AxisIndex].Objects.objStatusWord);
                pInputs->ObjPositionActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objPositionActualValue);
                pInputs->ObjVelocityActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objVelocityActualValue);
                pInputs->ObjModesOfOperationDisplay = SWAPWORD((LocalAxes[AxisIndex].Objects.objModesOfOperationDisplay & 0x00FF));
                pInputs->ObjDigitalInputs = SWAPWORD(LocalAxes[AxisIndex].Objects.objDigitalInputs);

                /*shift pointer PDO mapping object following*/
                if(j < (sTxPDOassign.u16SubIndex0 - 1))
                    pTmpData += SIZEOF(TCiA402PDO1A00);
            }
            break;
        case 1://copy csp TxPDO entries
            {
                TCiA402PDO1A01 *pInputs = (TCiA402PDO1A01 *)pTmpData;
                pInputs->ObjErrorCode = SWAPWORD(LocalAxes[AxisIndex].Objects.objErrorCode);
                pInputs->ObjStatusWord = SWAPWORD(LocalAxes[AxisIndex].Objects.objStatusWord);
                pInputs->ObjPositionActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objPositionActualValue);
                pInputs->ObjVelocityActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objVelocityActualValue);
                pInputs->ObjTorqueActualValue  = SWAPWORD(LocalAxes[AxisIndex].Objects.objTorqueActualValue);
                pInputs->ObjTouchProbeStatus = SWAPWORD(LocalAxes[AxisIndex].Objects.objTouchProbeStatus);
                pInputs->ObjTouchProbePos1PosValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objTouchProbePos1PosValue);
                pInputs->ObjTouchProbePos1NegValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.ObjTouchProbePos1NegValue);
                pInputs->ObjFollowingErrorActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objFollowingErrorActualValue);
                pInputs->ObjDigitalInputs = SWAPWORD(LocalAxes[AxisIndex].Objects.objDigitalInputs);

                /*shift pointer PDO mapping object following*/
                if (j < (sTxPDOassign.u16SubIndex0 - 1))
                {
                    pTmpData += SIZEOF(TCiA402PDO1A01);
                }
            }
            break;
        case 2://copy csv TxPDO entries
            {
                TCiA402PDO1A02 *pInputs = (TCiA402PDO1A02 *)pTmpData;
                pInputs->ObjErrorCode = SWAPWORD(LocalAxes[AxisIndex].Objects.objErrorCode);
                pInputs->ObjStatusWord = SWAPWORD(LocalAxes[AxisIndex].Objects.objStatusWord);
                pInputs->ObjPositionActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objPositionActualValue);
                pInputs->ObjVelocityActualValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objVelocityActualValue);
                pInputs->ObjTorqueActualValue = SWAPWORD(LocalAxes[AxisIndex].Objects.objTorqueActualValue);
                pInputs->ObjTouchProbeStatus = SWAPWORD(LocalAxes[AxisIndex].Objects.objTouchProbeStatus);
                pInputs->ObjTouchProbePos1PosValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.objTouchProbePos1PosValue);
                pInputs->ObjTouchProbePos1NegValue = SWAPDWORD(LocalAxes[AxisIndex].Objects.ObjTouchProbePos1NegValue);
                pInputs->ObjDigitalInputs = SWAPWORD(LocalAxes[AxisIndex].Objects.objDigitalInputs);

                /*shift pointer PDO mapping object following*/
                if (j < (sTxPDOassign.u16SubIndex0 - 1))
                {
                    pTmpData += SIZEOF(TCiA402PDO1A02);
                }
            }
            break;
        }    //switch TXPDO entry
#if CDP_SAMPLE
        if (sTxPDOassign.aEntries[j] == 0x1BFF)	// copy Device User TxPDO-Map(0x1BFF) entries
        {
    		UINT16 u16cnt;
    		for(u16cnt =0 ; u16cnt < DeviceUserTxPDOMap0x1BFF.u16SubIndex0;u16cnt++)
			{
    			if (((DeviceUserTxPDOMap0x1BFF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
				{
    				UINT8 *pInputs = (UINT8 *)pTmpData;
    				*pInputs = OutputIdentifier0xF9F5.aEntries[0];
				}
			}

    		/*shift pointer PDO mapping object following*/
    		if (((DeviceUserTxPDOMap0x1BFF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
			{
    			if (j < (sTxPDOassign.u16SubIndex0 - 1))
				{
					pTmpData += SIZEOF(UINT8);
				}
			}
        }
#endif
    }	// for (j = 0; j < sTxPDOassign.u16SubIndex0; j++)

#else
    MEMCPY(pData,&InputCounter,SIZEOF(InputCounter));
#if CDP_SAMPLE
    UINT32 *ptmp = (UINT32 *)pData;
    ptmp++;
    if (sTxPDOassign.aEntries[1] == 0x1BFF) // copy Device User TxPDO-Map(0x1BFF) entries
    {
        UINT16 u16cnt;
        for(u16cnt =0 ; u16cnt < DeviceUserTxPDOMap0x1BFF.u16SubIndex0;u16cnt++)
        {
            if (((DeviceUserTxPDOMap0x1BFF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
            {
                UINT8 *pInputs = (UINT8 *)ptmp;
                *pInputs = OutputIdentifier0xF9F5.aEntries[0];
            }
        }
    }
#endif
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_OutputMapping(UINT16* pData)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
    UINT16 j = 0;
    UINT8 *pTmpData = (UINT8 *)pData;
    UINT8 AxisIndex;

    for (j = 0; j < sRxPDOassign.u16SubIndex0; j++)
    {
        /*The Axis index is based on the PDO mapping offset (0x10)*/
        AxisIndex = ((sRxPDOassign.aEntries[j] & 0xF0) >> 4);

        switch ((sRxPDOassign.aEntries[j] & 0x000F))
        {
        case 0:    //csp/csv RxPDO    entries
            {
            TCiA402PDO1600 *pOutputs = (TCiA402PDO1600 *)pTmpData;

            LocalAxes[AxisIndex].Objects.objControlWord = SWAPWORD(pOutputs->ObjControlWord);
            LocalAxes[AxisIndex].Objects.objModesOfOperation = SWAPWORD((pOutputs->ObjModesOfOperation & 0x00FF));
            LocalAxes[AxisIndex].Objects.objTargetPosition = SWAPDWORD(pOutputs->ObjTargetPosition);
            LocalAxes[AxisIndex].Objects.objTargetVelocity    = SWAPDWORD(pOutputs->ObjTargetVelocity);
            /*DigitalOutputs use only Axis index 0.*/
			if ( AxisIndex == 0 )
			{
				LocalAxes[0].Objects.objDigitalOutputs.u32PhysicalOutputs = SWAPWORD(pOutputs->ObjDigitalOutputs);
			}
            /*shift pointer PDO mapping object following*/
            if (j < (sRxPDOassign.u16SubIndex0 - 1))
            {
                pTmpData += SIZEOF(TCiA402PDO1600);
            }
            }
            break;
        case 1:    //csp RxPDO    entries
            {
            TCiA402PDO1601 *pOutputs = (TCiA402PDO1601 *)pTmpData;

            LocalAxes[AxisIndex].Objects.objControlWord = SWAPWORD(pOutputs->ObjControlWord);
            LocalAxes[AxisIndex].Objects.objMaxTorque = SWAPWORD(pOutputs->ObjMaxTorque);
            LocalAxes[AxisIndex].Objects.objTargetPosition = SWAPDWORD(pOutputs->ObjTargetPosition);
            LocalAxes[AxisIndex].Objects.objPositionOffset = SWAPDWORD(pOutputs->ObjPositionOffset);
            LocalAxes[AxisIndex].Objects.objVelocityOffset = SWAPWORD(pOutputs->ObjVelocityOffset);
            LocalAxes[AxisIndex].Objects.objTorqueOffset = SWAPWORD(pOutputs->ObjTorqueOffset);
            LocalAxes[AxisIndex].Objects.objTouchProbeFunction = SWAPWORD(pOutputs->ObjTouchProbeFunction);
            LocalAxes[AxisIndex].Objects.objPositiveTorqueLimitValue = SWAPWORD(pOutputs->ObjPositiveTorqueLimitValue);
            LocalAxes[AxisIndex].Objects.objNegativeTorqueLimitValue = SWAPWORD(pOutputs->ObjNegativeTorqueLimitValue);

            /*DigitalOutputs use only Axis index 0.*/
			if ( AxisIndex == 0 )
			{
				LocalAxes[0].Objects.objDigitalOutputs.u32PhysicalOutputs = SWAPWORD(pOutputs->ObjDigitalOutputs);
			}

            /*shift pointer PDO mapping object following*/
            if (j < (sRxPDOassign.u16SubIndex0 - 1))
            {
                pTmpData += SIZEOF(TCiA402PDO1601);
            }
            }
            break;
        case 2:    //csv RxPDO    entries
            {
            TCiA402PDO1602 *pOutputs = (TCiA402PDO1602 *)pTmpData;

            LocalAxes[AxisIndex].Objects.objControlWord = SWAPWORD(pOutputs->ObjControlWord);
            LocalAxes[AxisIndex].Objects.objMaxTorque = SWAPWORD(pOutputs->ObjMaxTorque);
            LocalAxes[AxisIndex].Objects.objVelocityOffset = SWAPDWORD(pOutputs->ObjVelocityOffset);
            LocalAxes[AxisIndex].Objects.objTorqueOffset = SWAPDWORD(pOutputs->ObjTorqueOffset);
            LocalAxes[AxisIndex].Objects.objTouchProbeFunction = SWAPWORD(pOutputs->ObjTouchProbeFunction);
            LocalAxes[AxisIndex].Objects.objPositiveTorqueLimitValue = SWAPWORD(pOutputs->ObjPositiveTorqueLimitValue);
            LocalAxes[AxisIndex].Objects.objNegativeTorqueLimitValue = SWAPWORD(pOutputs->ObjNegativeTorqueLimitValue);
            LocalAxes[AxisIndex].Objects.objTargetVelocity    = SWAPDWORD(pOutputs->ObjTargetVelocity);
            /*DigitalOutputs use only Axis index 0.*/
			if ( AxisIndex == 0 )
			{
				LocalAxes[0].Objects.objDigitalOutputs.u32PhysicalOutputs = SWAPWORD(pOutputs->ObjDigitalOutputs);
			}

            /*shift pointer PDO mapping object following*/
            if (j < (sRxPDOassign.u16SubIndex0 - 1))
            {
                pTmpData += SIZEOF(TCiA402PDO1602);
            }
            }
            break;
        }
#if CDP_SAMPLE
        if (sRxPDOassign.aEntries[j] == 0x17FF) //copy Device User RxPDO-Map(0x17FF) entries
		{
			UINT16 u16cnt;
			for(u16cnt =0 ; u16cnt < DeviceUserRxPDOMap0x17FF.u16SubIndex0;u16cnt++)
			{
				if (((DeviceUserRxPDOMap0x17FF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
				{
					UINT8 *pOutputs = (UINT8 *)pTmpData;
					OutputIdentifier0xF9F5.aEntries[0] = *pOutputs;
				}
			}
			/*shift pointer PDO mapping object following*/
			if (((DeviceUserRxPDOMap0x17FF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
			{
				if (j < (sTxPDOassign.u16SubIndex0 - 1))
				{
					pTmpData += SIZEOF(UINT8);
				}
			}
		}
#endif
    }	// for (j = 0; j < sRxPDOassign.u16SubIndex0; j++)

#else
    MEMCPY(&OutputCounter,pData,SIZEOF(OutputCounter));
#if CDP_SAMPLE
    UINT32 *ptmp = (UINT32 *)pData;
    ptmp++;
    if (sRxPDOassign.aEntries[1] == 0x17FF) //copy Device User RxPDO-Map(0x17FF) entries
    {
        UINT16 u16cnt;
        for(u16cnt =0 ; u16cnt < DeviceUserRxPDOMap0x17FF.u16SubIndex0;u16cnt++)
        {
            if (((DeviceUserRxPDOMap0x17FF.aEntries[u16cnt] & 0xFFFF0000)>>16) == 0xF9F5)
            {
                UINT8 *pOutputs = (UINT8 *)ptmp;
                OutputIdentifier0xF9F5.aEntries[0] = *pOutputs;
            }
        }
    }
#endif
#endif

}
/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
#if defined(BOARD_RZT2L_RSK) | defined(BOARD_RZN2L_RSK)
void APPL_Application(void)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
    APPL_SetLed((UINT16)(LocalAxes[0].Objects.objDigitalOutputs.u32PhysicalOutputs & 0x000F));
    LocalAxes[0].Objects.objDigitalInputs = (UINT32)APPL_GetDipSw() & 0x000F;   // SW3.1-4

    UINT16 i;

    for(i = 0; i < MAX_AXES;i++)
    {
        if (LocalAxes[i].bAxisIsActive)
        {
            CiA402_Application(&LocalAxes[i], i);
        }
    }
#else
    /*Hardware independent sample application*/
        InputCounter = APPL_GetDipSw() & 0x000f;        // SW3.1-4
        APPL_SetLed((UINT16)OutputCounter);
#endif
}
#elif defined(BOARD_RZT2M_RSK) && (BSP_CFG_CORE_CR52 == 1)
void APPL_Application(void)
{
#if (CiA402_SAMPLE_APPLICATION == 1)
    read_shared_memory();

    APPL_SetLed((UINT16)(LocalAxes[0].Objects.objDigitalOutputs.u32PhysicalOutputs & 0x000F));
    LocalAxes[0].Objects.objDigitalInputs = (UINT32)APPL_GetDipSw() & 0x000F;   // SW3.1-4

    UINT16 i;
    for(i = 0; i < MAX_AXES;i++)
    {
        /* Update data received from CPU0 */
        LocalAxes[i].Objects.objPositionActualValue = g_shared_memory_local_data.from_CPU0_to_CPU1.Axis[i].actual_position;
        LocalAxes[i].Objects.objVelocityActualValue = g_shared_memory_local_data.from_CPU0_to_CPU1.Axis[i].actual_velocity;

        if (LocalAxes[i].bAxisIsActive)
        {
            CiA402_Application(&LocalAxes[i], i);
        }

        /* Update data transmitted to CPU0. */
        g_shared_memory_local_data.from_CPU1_to_CPU0.Axis[i].CiA402_modes_of_operation = LocalAxes[i].Objects.objModesOfOperation;
        g_shared_memory_local_data.from_CPU1_to_CPU0.Axis[i].CiA402_state_machine = LocalAxes[i].i16State;
        g_shared_memory_local_data.from_CPU1_to_CPU0.Axis[i].target_position = LocalAxes[i].Objects.objTargetPosition;
        g_shared_memory_local_data.from_CPU1_to_CPU0.Axis[i].target_velocity = LocalAxes[i].Objects.objTargetVelocity;

    }

    write_shared_memory();
#else
    /*Hardware independent sample application*/
    read_shared_memory();
    InputCounter = APPL_GetDipSw() & 0x000f;        // SW3.1-4
    APPL_SetLed((UINT16)OutputCounter);
    write_shared_memory();
#endif
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Calculate the Explicit Device ID
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
#if defined(BOARD_RZT2M_RSK)
	return (APPL_GetDipSw() & 0x00FF);	// SW3.1-8
#elif defined(BOARD_RZN2L_RSK) | defined(BOARD_RZT2L_RSK)
	return (APPL_GetDipSw() & 0x000F);	// SW3.1-4
#else
    return 0x5;
#endif
}

#if defined(BOARD_RZT2M_RSK) | defined(BOARD_RZN2L_RSK) | defined(BOARD_RZT2L_RSK)
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    UINT16 LED output value. The value one means ON.

 \brief    SET LED
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_SetLed(UINT16 value)
{
#if defined(BOARD_RZT2M_RSK) && (BSP_CFG_CORE_CR52 == 1)
    g_shared_memory_local_data.from_CPU1_to_CPU0.output_LED = (UINT16)(value & 0x00FF);
#elif defined(BOARD_RZT2L_RSK) | defined(BOARD_RZN2L_RSK)
	/* LED type structure */
	sample_leds_t leds = g_sample_leds;

    /* Holds level to set for pins */
    bsp_io_level_t pin_level[4];

	/* This code uses BSP IO functions to show how it is used.*/
	R_BSP_PinAccessEnable();

	pin_level[SAMPLE_LED_RLED0] = ((value & 1) ?  BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
	pin_level[SAMPLE_LED_RLED1] = ((value & 2) ?  BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
	pin_level[SAMPLE_LED_RLED2] = ((value & 4) ?  BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
	pin_level[SAMPLE_LED_RLED3] = ((value & 8) ?  BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);

	R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[SAMPLE_LED_RLED0], pin_level[SAMPLE_LED_RLED0]);
	R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[SAMPLE_LED_RLED1], pin_level[SAMPLE_LED_RLED1]);

#if defined(BOARD_RZT2M_RSK) | defined(BOARD_RZN2L_RSK)
	R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[SAMPLE_LED_RLED2], pin_level[SAMPLE_LED_RLED2]);
	R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[SAMPLE_LED_RLED3], pin_level[SAMPLE_LED_RLED3]);
#endif

	/* Protect PFS registers */
	R_BSP_PinAccessDisable();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \retuen   UINT16 DIP SW value. Low input level means ON.

 \brief    Get DIP SW
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDipSw(void)
{
	UINT16 u16DipSw;

#if defined(BOARD_RZT2M_RSK) && (BSP_CFG_CORE_CR52 == 1)
	u16DipSw = (UINT16)(g_shared_memory_local_data.from_CPU0_to_CPU1.input_DIPSW & 0x00FF);
#elif defined(BOARD_RZT2L_RSK) | defined(BOARD_RZN2L_RSK)

	u16DipSw = 0;

	/* DIP SW type structure */
	sample_dip_sws_t dipsws = g_sample_dip_sws;

	/* This code uses BSP IO functions to show how it is used.*/
	R_BSP_PinAccessEnable();

	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_0]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_0]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x01;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_1]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_1]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x02;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_2]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_2]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x04;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_3]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_3]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x08;

#if defined(BOARD_RZT2M_RSK)
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_4]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_4]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x10;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_5]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_5]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x20;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_6]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_6]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x40;
	if (R_BSP_FastPinRead(R_BSP_IoRegionGet((bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_7]), (bsp_io_port_pin_t)dipsws.p_sws[SAMPLE_DIPSW_7]) == BSP_IO_LEVEL_LOW) u16DipSw |= 0x80;
#endif

	/* Protect PFS registers */
	R_BSP_PinAccessDisable();

#endif
	return u16DipSw;
}
#endif //#if defined(BOARD_RZT2M_RSK) | defined(BOARD_RZN2L_RSK) | defined(BOARD_RZT2L_RSK)
/** @} */

