//
// CScsi_Farm_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CScsi_Farm_Log.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::CSCSI_Farm_Log()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CSCSI_Farm_Log::CSCSI_Farm_Log()
	: m_totalPages()
	, m_logSize(0)
	, m_pageSize(0)
	, m_heads(0)
	, m_MaxHeads(0)
	, m_copies(0)
	, m_pHeader()
	, m_logParam()
	, m_alreadySet(false)
	, m_MinorRev(0)
	, m_showStatusBits(false)
{
	m_status = IN_PROGRESS;

}
//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::CSCSI_Farm_Log()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//! \parma securityPrintLevel = the level of the print
//! \param bufferData = pointer to the buffer data.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------

CSCSI_Farm_Log::CSCSI_Farm_Log( uint8_t *bufferData, size_t bufferSize, bool showStatus)
	: m_totalPages()
	, m_logSize(0)
	, m_pageSize(0)
	, m_heads(0)
	, m_MaxHeads(0)
	, m_copies(0)
	, m_pHeader()
	, m_logParam()
	, m_alreadySet(false)
	, m_MinorRev(0)
	, m_showStatusBits(false)
{
    m_status = IN_PROGRESS;

	pBuf = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pBuf, bufferData, bufferSize);
#else
    memcpy_s(pBuf, bufferSize, bufferData, bufferSize);// copy the buffer data to the class member pBuf
#endif
    if (pBuf != NULL)
    {
		m_status = init_Header_Data();							// init the data for getting the log
    }
    else
    {
        m_status = FAILURE;
    }

}
//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::~CSCSI_Farm_Log()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param pData  pointer to the buffer data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CSCSI_Farm_Log::~CSCSI_Farm_Log()
{
    if (!vFarmFrame.empty())
    {
        vFarmFrame.clear();                                    // clear the vector
    }
	safe_Free(pBuf);
}
//-----------------------------------------------------------------------------
//
//! \fn init_Header_Data()
//
//! \brief
//!   Description:  initialize teh header data and the objects member data.
//
//  Entry:
//! None
//
//  Exit:
//!   \return eReturnValues - MEMORY_FAILURE - if the buffer is NULL
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::init_Header_Data()
{
	if (pBuf == NULL)
	{
		return MEMORY_FAILURE;
	}
	else
	{
		m_logParam = (sScsiLogParameter *)&pBuf[0];
		m_logSize = m_logParam->length;									    // set the class log size 
		byte_Swap_16(&m_logSize);

		m_pHeader = (sFarmHeader *)&pBuf[4];								
		swap_Bytes_sFarmHeader(m_pHeader);									// swap all the bytes for the header
		m_totalPages = M_DoubleWord0(m_pHeader->pagesSupported);			// get the total pages
		m_pageSize = M_DoubleWord0(m_pHeader->pageSize);					// get the page size
		if (check_For_Active_Status(&m_pHeader->headsSupported))			// the the number of heads if supported
		{
			if ((m_pHeader->headsSupported & 0x00FFFFFFFFFFFFFF) > 0)
			{
				m_heads = M_DoubleWord0(m_pHeader->headsSupported);
				m_MaxHeads = M_DoubleWord0(m_pHeader->headsSupported);
			}
		}
		m_copies = M_DoubleWord0(m_pHeader->copies);						// finish up with the number of copies (not supported "YET" in SAS)
	}
	return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn strip_Active_Status()
//
//! \brief
//!   Description:  field supported and Field valid bits stripped off
//
//  Entry:
//! \param value  =  64 bit value have the field supported and Field valid bits stripped off
//
//  Exit:
//!   \return bool - false or true
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::strip_Active_Status(uint64_t *value)
{
    if( check_For_Active_Status(value))
    {
       *value = *value & 0x00FFFFFFFFFFFFFFLL;
       return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
//
//! \fn set_Head_Header()
//
//! \brief
//!   Description:  set the header name in a string for printing
//
//  Entry:
//! \param headerName  =  pointer the string variable
//! \param index  =  index of the log page types
//
//  Exit:
//!  Void 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::set_Head_Header(std::string &headerName, eLogPageTypes index)
{

    switch (index)
    {
    case FARM_HEADER_PARAMETER:
    case GENERAL_DRIVE_INFORMATION_PARAMETER:
    case WORKLOAD_STATISTICS_PARAMETER:
    case ERROR_STATISTICS_PARAMETER:
    case ENVIRONMENTAL_STATISTICS_PARAMETER:
    case RELIABILITY_STATISTICS_PARAMETER:
    case RESERVED_FOR_FUTURE_STATISTICS_1:
    case RESERVED_FOR_FUTURE_STATISTICS_2:
    case RESERVED_FOR_FUTURE_STATISTICS_3:
    case RESERVED_FOR_FUTURE_STATISTICS_4:
    case RESERVED_FOR_FUTURE_STATISTICS_5:
    case RESERVED_FOR_FUTURE_STATISTICS_6:
    case RESERVED_FOR_FUTURE_STATISTICS_7:
    case RESERVED_FOR_FUTURE_STATISTICS_8:
    case RESERVED_FOR_FUTURE_STATISTICS_9:
    case RESERVED_FOR_FUTURE_STATISTICS_10:
        break;
    case DISC_SLIP_IN_MICRO_INCHES_BY_HEAD:
        headerName = "Disc Slip in micro-inches by Head";
        break;
    case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:
        headerName = "Bit Error Rate of Zone 0";
        break;
    case DOS_WRITE_REFRESH_COUNT:
        headerName = "DOS Write Refresh Count";
        break;
    case DVGA_SKIP_WRITE_DETECT_BY_HEAD:
        headerName = "DVGA Skip Write";
        break;
    case RVGA_SKIP_WRITE_DETECT_BY_HEAD:
        headerName = "RVGA Skip Write";
        break;
    case FVGA_SKIP_WRITE_DETECT_BY_HEAD:
        headerName = "FVGA Skip Write";
        break;
    case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:
        headerName = "Skip Write Detect Threshold Exceeded Count";
        break;
    case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "ACFF Sine 1X, value from most recent SMART Summary Frame";
        break;
    case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "ACFF Cosine 1X, value from most recent SMART Summary Frame";
        break;
    case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "PZT Calibration, value from most recent SMART Summary Frame";
        break;
    case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "MR Head Resistance from most recent SMART Summary Frame";
        break;
    case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Number of TMD over last 3 SMART Summary Frame";
        break;
    case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Velocity Observer over last 3 SMART Summary Frame";
        break;
    case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Number of Velocity Observer over last 3 SMART SummaryFrame";
        break;
    case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT percentage of codewords at iteration level";
        break;
    case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT amplitude, averaged across Test Zone";
        break;
    case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT asymmetry, averaged across Test Zone";
        break;
    case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
        headerName = "Resident Glist Entries";
        break;
    case NUMBER_OF_PENDING_ENTRIES:
        headerName = "Resident Pending list Entries";
        break;
    case DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:
        headerName = "DOS Oought To Scan Count";
        break;
    case DOS_NEED_TO_SCAN_COUNT_PER_HEAD:
        headerName = "DOS Needs To Scan Count";
        break;
    case DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:
        headerName = "DOS Write Fault Scan Count";
        break;
    case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
        headerName = "Write Power On Hours from most recent SMART";
        break;
    case RESERVED_FOR_FUTURE_EXPANSION_1:
    case RESERVED_FOR_FUTURE_EXPANSION_2:
    case RESERVED_FOR_FUTURE_EXPANSION_3:
    case RESERVED_FOR_FUTURE_EXPANSION_4:
    case RESERVED_FOR_FUTURE_EXPANSION_5:
    case RESERVED_FOR_FUTURE_EXPANSION_6:
    case RESERVED_FOR_FUTURE_EXPANSION_7:
    case RESERVED_FOR_FUTURE_EXPANSION_8:
    case RESERVED_FOR_FUTURE_EXPANSION_9:
        headerName = "Something is wrong. Please report error SAS FARM 613";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 0";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 1";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 2";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT iterations to cnverge Test Zone 0";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT iterations to cnverge Test Zone 1";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT iterations to cnverge Test Zone 2";
        break;
    case RESERVED_FOR_FUTURE_EXPANSION_10:
    case RESERVED_FOR_FUTURE_EXPANSION_11:
    case RESERVED_FOR_FUTURE_EXPANSION_12:
    case RESERVED_FOR_FUTURE_EXPANSION_13:
    case RESERVED_FOR_FUTURE_EXPANSION_14:
    case RESERVED_FOR_FUTURE_EXPANSION_15:
    case RESERVED_FOR_FUTURE_EXPANSION_16:
    case RESERVED_FOR_FUTURE_EXPANSION_17:
    case RESERVED_FOR_FUTURE_EXPANSION_18:
    case RESERVED_FOR_FUTURE_EXPANSION_19:
        headerName = "Something is wrong. Please report error SAS FARM 614";
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:
        if (!m_alreadySet)
        {
            headerName = "Applied fly height clearance delta per head in thousandths of one Angstrom - Outer";
            m_alreadySet = true;
        }
        else // fix for the firmware issue. FDDRAGON-95
        {
            headerName = "Applied fly height clearance delta per head in thousandths of one Angstrom - Middle";
        }
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
        headerName = "Applied fly height clearance delta per head in thousandths of one Angstrom - Inner";
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
        headerName = "Applied fly height clearance delta per head in thousandths of one Angstrom - Middle";
        break;
    default:
        headerName = "Something is wrong. Please report error SAS FARM 615";
        break;

    }
}
//-----------------------------------------------------------------------------
//
//! \fn create_Serial_Number()
//
//! \brief
//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
//
//  Entry:
//! \param serialNumber - pointer to the serial number, where once constructed, will hold the serial number of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return serialNumber = the string serialNumber
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::create_Serial_Number(std::string *serialNumber, const sDriveInfo * const idInfo)
{
	uint64_t sn = 0;
	uint64_t sn1 = idInfo->serialNumber & 0x00FFFFFFFFFFFFFFLL;
	uint64_t sn2 = idInfo->serialNumber2 & 0x00FFFFFFFFFFFFFFLL;
	byte_Swap_64(&sn1);
	byte_Swap_64(&sn2);
	sn = (sn1 | (sn2 >> 32));
	serialNumber->resize(SERIAL_NUMBER_LEN );
	memset((char*)serialNumber->c_str(),0, SERIAL_NUMBER_LEN );
	strncpy((char *)serialNumber->c_str(), (char*)&sn, SERIAL_NUMBER_LEN);
}
//-----------------------------------------------------------------------------
//
//! \fn create_World_Wide_Name()
//
//! \brief
//!   Description:  takes the two uint64 bit world wide name values and create a string world wide name 
//
//  Entry:
//! \param worldWideName - pointer to the world wide name, where once constructed, will hold the world wide name of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return wordWideName = the string wordWideName
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::create_World_Wide_Name(std::string *worldWideName, const sDriveInfo * const idInfo)
{
	uint64_t wwn = 0;
	wwn = (idInfo->worldWideName & 0x00FFFFFFFFFFFFFFLL) | ((idInfo->worldWideName2 & 0x00FFFFFFFFFFFFFFLL) << 32);
	worldWideName->resize(WORLD_WIDE_NAME_LEN);
	memset((char *)worldWideName->c_str(), 0, WORLD_WIDE_NAME_LEN);
	snprintf((char *)worldWideName->c_str(), WORLD_WIDE_NAME_LEN, "0x%" PRIX64"", wwn);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Firmware_String()
//
//! \brief
//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
//
//  Entry:
//! \param firmwareRev - pointer to the firmware Rev, where once constructed, will hold the firmware Rev of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return firmwareRev = the string firmwareRev
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::create_Firmware_String(std::string *firmwareRev, const sDriveInfo * const idInfo)
{
	uint32_t firm = 0;
	firm = (uint32_t)(idInfo->firmware & 0x00FFFFFFFFFFFFFFLL);
	byte_Swap_32(&firm);
	firmwareRev->resize(FIRMWARE_REV_LEN);
	memset((char *)firmwareRev->c_str(), 0, FIRMWARE_REV_LEN);
	strncpy((char *)firmwareRev->c_str(), (char*)&firm, FIRMWARE_REV_LEN);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Device_Interface_String()
//
//! \brief
//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
//
//  Entry:
//! \param dInterface - pointer to the Devie interface v, where once constructed, will hold the Devie interface of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return dInterface = the string dInterface
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::create_Device_Interface_String(std::string *dInterface, const sDriveInfo * const idInfo)
{
	uint64_t dFace = 0;
	dFace = (idInfo->deviceInterface & 0x00FFFFFFFFFFFFFFLL);
	dInterface->resize(DEVICE_INTERFACE_LEN);
	memset((char *)dInterface->c_str(), 0, DEVICE_INTERFACE_LEN);
	strncpy((char *)dInterface->c_str(), (char*)&dFace, DEVICE_INTERFACE_LEN);

}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sDriveInfo()
//
//! \brief
//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
//
//  Entry:
//! \param sDriveInfo  =  pointer to the drive info structure that holds the information needed, we will need to swap the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sDriveInfo(sDriveInfo *di)
{
    byte_Swap_64(&di->copyNumber);
    byte_Swap_64(&di->deviceBufferSize);
    byte_Swap_64(&di->deviceCapacity);
    byte_Swap_64(&di->deviceInterface);
    byte_Swap_64(&di->factor);
    byte_Swap_64(&di->firmware);
    byte_Swap_64(&di->firmwareRev);
    byte_Swap_64(&di->headLoadEvents);
    byte_Swap_64(&di->heads);
    byte_Swap_64(&di->lsecSize);
    byte_Swap_64(&di->NVC_StatusATPowerOn);
    byte_Swap_64(&di->pageNumber);
    byte_Swap_64(&di->poh);
    byte_Swap_64(&di->powerCycleCount);
    byte_Swap_64(&di->psecSize);
    byte_Swap_64(&di->resetCount);
	byte_Swap_64(&di->rotationRate);
    byte_Swap_64(&di->serialNumber);
    byte_Swap_64(&di->serialNumber2);
    byte_Swap_64(&di->timeAvailable);
    byte_Swap_64(&di->firstTimeStamp);
    byte_Swap_64(&di->lastTimeStamp);
    byte_Swap_64(&di->worldWideName);
	byte_Swap_64(&di->worldWideName2);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn create_Device_Interface_String()
//
//! \brief
//!   Description:  takes the two uint64 bit work load stat and swaps all the bytes 
//
//  Entry:
//! \param sWorkLoadStat  =  pointer to the work load stats and swaps all the bytes
//
//  Exit:
//!   \return true when done
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sWorkLoadStat(sWorkLoadStat *wl)
{
    byte_Swap_64(&wl->copyNumber);
    byte_Swap_64(&wl->logicalSecRead);
    byte_Swap_64(&wl->logicalSecWritten);
    byte_Swap_64(&wl->pageNumber);
    byte_Swap_64(&wl->totalNumberofOtherCMDS);
    byte_Swap_64(&wl->totalRandomReads);
    byte_Swap_64(&wl->totalRandomWrites);
    byte_Swap_64(&wl->totalReadCommands);
    byte_Swap_64(&wl->totalWriteCommands);
    byte_Swap_64(&wl->workloadPercentage);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sErrorStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the error stat
//
//  Entry:
//! \param sErrorStat  =  pointer to the error stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sErrorStat(sErrorStat * es)
{
	byte_Swap_64(&es->totalReadECC);
	byte_Swap_64(&es->totalWriteECC);
	byte_Swap_64(&es->totalReallocations);
    byte_Swap_64(&es->attrIOEDCErrors);
    byte_Swap_64(&es->copyNumber);
    byte_Swap_16(&es->m_pPageHeader.pramCode);
    byte_Swap_64(&es->pageNumber);
    byte_Swap_64(&es->totalFlashLED);
    byte_Swap_64(&es->totalMechanicalFails);
	byte_Swap_64(&es->totalReallocatedCanidates);
    byte_Swap_64(&es->FRUCode);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sEnvironementStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the environment stat
//
//  Entry:
//! \param sEnvironementStat  =  pointer to the environment stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sEnvironmentStat(sEnvironmentStat *es)
{
    byte_Swap_64(&es->copyNumber);
    byte_Swap_64(&es->curentTemp);
    byte_Swap_64(&es->currentMotorPower);
    byte_Swap_64(&es->highestTemp);
    byte_Swap_64(&es->humidity);
    byte_Swap_64(&es->humidityRatio);
    byte_Swap_64(&es->lowestTemp);
    byte_Swap_64(&es->maxTemp);
    byte_Swap_64(&es->minTemp);
    byte_Swap_16(&es->m_pPageHeader.pramCode);
    byte_Swap_64(&es->pageNumber);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sScsiReliabilityStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the reliability stat
//
//  Entry:
//! \param sScsiReliabilityStat  =  pointer to the reliability stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sScsiReliabilityStat(sScsiReliabilityStat *ss)
{
    byte_Swap_64(&ss->altsAfterIDD);
    byte_Swap_64(&ss->altsBeforeIDD);
    byte_Swap_64(&ss->cmdLastIDDTest);
    byte_Swap_64(&ss->copyNumber);
    byte_Swap_64(&ss->diskSlipRecalPerformed);
    byte_Swap_64(&ss->gListAfterIDD);
    byte_Swap_64(&ss->gListBeforIDD);
    byte_Swap_64(&ss->gListReclamed);
	byte_Swap_64(&ss->heliumPressuretThreshold);
	byte_Swap_64(&ss->idleTime);
    byte_Swap_64(&ss->lastIDDTest);
    byte_Swap_16(&ss->m_pPageHeader.pramCode);
	byte_Swap_64(&ss->maxRVAbsuluteMean);
	byte_Swap_64(&ss->microActuatorLockOut);
    byte_Swap_64(&ss->numberDOSScans);
    byte_Swap_64(&ss->numberLBACorrect);
    byte_Swap_64(&ss->numberRAWops);
    byte_Swap_64(&ss->numberValidParitySec);
    byte_Swap_64(&ss->pageNumber);
	byte_Swap_64(&ss->rvAbsuluteMean);
    byte_Swap_64(&ss->scrubsAfterIDD);
    byte_Swap_64(&ss->scrubsBeforeIDD);
    byte_Swap_64(&ss->servoStatus);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sScsiReliabilityStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the reliability stat
//
//  Entry:
//! \param sScsiReliabilityStat  =  pointer to the reliability stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sFarmHeader(sFarmHeader *fh)
{
    byte_Swap_64(&fh->copies);
    byte_Swap_64(&fh->headsSupported);
    byte_Swap_64(&fh->logSize);
    byte_Swap_64(&fh->majorRev);
    byte_Swap_64(&fh->minorRev);
    byte_Swap_64(&fh->pageSize);
    byte_Swap_64(&fh->pagesSupported);
    byte_Swap_64(&fh->signature);
	//m_MinorRev = fh->minorRev & 0x00FFLL;
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Head_Info()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the reliability stat
//
//  Entry:
//! \param phead  =  pointer to the head information
//! \param buffer = the pointer to the buffer data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::get_Head_Info(sHeadInformation *phead, uint8_t *buffer)
{
    memcpy(&phead->headValue, (sHeadInformation *)&buffer[4], (sizeof(uint64_t) * (size_t) m_heads));
	memcpy(&phead->pageHeader, (sScsiPageParameter *)&buffer[0], sizeof(sScsiPageParameter));
    for (uint32_t index = 0; index < m_heads; index++)
    {
        byte_Swap_64(&phead->headValue[index] );
    }
    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn ParseFarmLog()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::ParseFarmLog()
{
    uint64_t offset = 4;															// the first page starts at offset 4                                   
	sFarmFrame *pFarmFrame = new sFarmFrame();										// create the pointer to the union
    if (pBuf == NULL)
    {
        return FAILURE;
    }
    if ((m_pHeader->signature & 0x00FFFFFFFFFFFFFF) == FARMSIGNATURE)				// check the head to see if it has the farm signature else fail
    {
        for (uint32_t index = 0; index <= m_copies; ++index)						// loop for the number of copies. I don't think it's zero base as of now
        {
			sScsiPageParameter *pFarmPageHeader;
			sFarmHeader *pFarmHeader; 
			sDriveInfo *pIdInfo;
			sWorkLoadStat *pworkLoad;
			sErrorStat *pError;
			sEnvironmentStat *pEnvironment;
			sScsiReliabilityStat *pReli;
			sHeadInformation *pHeadInfo = new sHeadInformation();
			sStringIdentifyData *pInfo = new sStringIdentifyData();
            while (offset < m_logSize)
            {
                m_pageParam = (sScsiPageParameter *)&pBuf[offset];										 // get the page params, so we know what the param code is. 
                byte_Swap_16(&m_pageParam->pramCode);
                pFarmFrame->vFramesFound.push_back((eLogPageTypes)m_pageParam->pramCode);                // collect all the log page types in a vector to pump them out at the end
                switch (m_pageParam->pramCode)
                {
                case FARM_HEADER_PARAMETER:
                    pFarmPageHeader = (sScsiPageParameter *)&pBuf[offset];                                      // get the Farm Header information
                    pFarmHeader = (sFarmHeader *)&pBuf[offset + sizeof(sScsiPageParameter)];                    // get the Farm Header information
                    memcpy(&pFarmFrame->farmHeader, pFarmHeader, sizeof(sFarmHeader));
                    offset += (pFarmPageHeader->plen + sizeof(sScsiPageParameter));
                    break;
                case  GENERAL_DRIVE_INFORMATION_PARAMETER:
                    pFarmPageHeader = (sScsiPageParameter *)&pBuf[offset];                                      // get the Farm Header information
                    pIdInfo = (sDriveInfo *)&pBuf[offset ];														// get the id drive information at the time.
                    swap_Bytes_sDriveInfo(pIdInfo);
                    pFarmFrame->driveInfo = *pIdInfo;
                    
                    create_Serial_Number(&pInfo->serialNumber, pIdInfo);										// create the serial number
                    create_World_Wide_Name(&pInfo->worldWideName, pIdInfo);										// create the wwwn
                    create_Firmware_String(&pInfo->firmwareRev, pIdInfo);										// create the firmware string
                    create_Device_Interface_String(&pInfo->deviceInterface, pIdInfo);							// get / create the device interface string

                    memcpy(&pFarmFrame->identStringInfo, pInfo, sizeof(sStringIdentifyData));
                    offset += (pFarmPageHeader->plen + sizeof(sScsiPageParameter));
                    break;
                case  WORKLOAD_STATISTICS_PARAMETER:
                    pFarmPageHeader = (sScsiPageParameter *)&pBuf[offset];                                      // get the Farm Header information
                    pworkLoad = (sWorkLoadStat *)&pBuf[offset ];												// get the work load information
                    swap_Bytes_sWorkLoadStat(pworkLoad);
                    memcpy(&pFarmFrame->workLoadPage, pworkLoad, sizeof(sWorkLoadStat));
                    offset += (pFarmPageHeader->plen + sizeof(sScsiPageParameter));
                    break;
                case ERROR_STATISTICS_PARAMETER:
                    pError = (sErrorStat *)&pBuf[offset];                    // get the error status
                    swap_Bytes_sErrorStat(pError);
                    memcpy(&pFarmFrame->errorPage, pError, sizeof(sErrorStat));
                    offset += (pError->m_pPageHeader.plen + sizeof(sScsiPageParameter));
                    break;
                case ENVIRONMENTAL_STATISTICS_PARAMETER:
                    pEnvironment = (sEnvironmentStat *)&pBuf[offset]; // get the envirmonent information 
                    swap_Bytes_sEnvironmentStat(pEnvironment);
                    memcpy(&pFarmFrame->environmentPage, pEnvironment, sizeof(sEnvironmentStat));
                    offset += (pEnvironment->m_pPageHeader.plen + sizeof(sScsiPageParameter));
                    break;
                case RELIABILITY_STATISTICS_PARAMETER:
                    pReli = (sScsiReliabilityStat *)&pBuf[offset];         // get the Reliabliity stat
                    swap_Bytes_sScsiReliabilityStat(pReli);
                    memcpy(&pFarmFrame->reliPage, pReli, sizeof(sScsiReliabilityStat));
                    offset += (pReli->m_pPageHeader.plen + sizeof(sScsiPageParameter));
                    break;    
                case DISC_SLIP_IN_MICRO_INCHES_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->discSlipPerHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->bitErrorRateByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case DOS_WRITE_REFRESH_COUNT:  
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->dosWriteRefreshCountByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case DVGA_SKIP_WRITE_DETECT_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->dvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case RVGA_SKIP_WRITE_DETECT_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->rvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;        
                case FVGA_SKIP_WRITE_DETECT_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->fvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->skipWriteDectedThresholdExceededByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->acffSine1xValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->acffCosine1xValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->pztCalibrationValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->mrHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->numberOfTMDByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->velocityObserverByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->numberOfVelocityObservedByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2SATPercentagedbyHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAmplituedByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAsymmetryByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentGlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case     NUMBER_OF_PENDING_ENTRIES:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentPlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case    DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->DOSOoughtToScan, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case   DOS_NEED_TO_SCAN_COUNT_PER_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->DOSNeedToScan, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case   DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->DOSWriteFaultScan, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case  WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->writePowerOnHours, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;

                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->appliedFlyHeightByHeadOuter, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->appliedFlyHeightByHeadInner, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->appliedFlyHeightByHeadMiddle, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                    memset(pHeadInfo, 0, sizeof(pHeadInfo));
                    break;
                default:
                    offset += m_logSize;
                    break;
                }
                
            }
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
			delete pHeadInfo;
			delete pInfo;
            pFarmFrame->vFramesFound.clear();                                                 // clear the vector for the next copy
            pFarmFrame = { 0 };
			delete pFarmFrame;
            
        }
        return SUCCESS;
    }

    return FAILURE;
}


//-----------------------------------------------------------------------------
//
//! \fn PrintHeader()
//
//! \brief
//!   Description:  print out the header information all data is done in Json format
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Header(JSONNODE *masterData)
{
    std::string myStr = "";
    JSONNODE *FARMheader = json_new(JSON_NODE);
    sFarmHeader *header = (sFarmHeader *)&pBuf[4];                                                                // pointer to the header to get the signature
#if defined( _DEBUG)
    printf("\tLog Signature   =  0x%" PRIX64" \n", header->signature & 0x00FFFFFFFFFFFFFFLL);                                  //!< Log Signature = 0x00004641524D4552
    printf("\tMajor Revision =   %" PRIu64"  \n", header->majorRev & 0x00FFFFFFFFFFFFFFLL);                                    //!< Log Major rev
    printf("\tMinor Revision =   %" PRIu64"  \n", header->minorRev & 0x00FFFFFFFFFFFFFFLL);                                    //!< minor rev 
    printf("\tPages Supported =   %" PRIu64"  \n", header->pagesSupported & 0x00FFFFFFFFFFFFFFLL);                             //!< number of pages supported
    printf("\tLog Size        =   %" PRIu64"  \n", header->logSize & 0x00FFFFFFFFFFFFFFLL);                                    //!< log size in bytes
    printf("\tPage Size       =   %" PRIu64"  \n", header->pageSize & 0x00FFFFFFFFFFFFFFLL);                                   //!< page size in bytes
    printf("\tHeads Supported =   %" PRIu64"  \n", header->headsSupported & 0x00FFFFFFFFFFFFFFLL);                             //!< Maximum Drive Heads Supported
    printf("\tNumber of Copies=   %" PRIu64"  \n", header->copies & 0x00FFFFFFFFFFFFFLL);                                      //!< Number of Historical Copies
#endif
    json_set_name(FARMheader, "FARM Log");
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Header");

    snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIX64"", check_Status_Strip_Status(header->signature));
	json_push_back(pageInfo, json_new_a("Log Signature", (char*)myStr.c_str() ));
	json_push_back(pageInfo, json_new_i("Major Revision", static_cast<uint32_t>(check_Status_Strip_Status(header->majorRev))));
	json_push_back(pageInfo, json_new_i("Minor Revision", static_cast<uint32_t>(check_Status_Strip_Status(header->minorRev))));
	json_push_back(pageInfo, json_new_i("Pages Supported", static_cast<uint32_t>(check_Status_Strip_Status(header->pagesSupported))));
	json_push_back(pageInfo, json_new_i("Log Size", static_cast<uint32_t>(check_Status_Strip_Status(header->logSize))));
	json_push_back(pageInfo, json_new_i("Page Size", static_cast<uint32_t>(check_Status_Strip_Status(header->pageSize))));
	json_push_back(pageInfo, json_new_i("Heads Supported", static_cast<uint32_t>(check_Status_Strip_Status(header->headsSupported))));
	json_push_back(pageInfo, json_new_i("Number of Copies", static_cast<uint32_t>(check_Status_Strip_Status(header->copies ))));

    json_push_back(FARMheader, pageInfo);
    json_push_back(masterData, FARMheader);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintDriveInformation()
//
//! \brief
//!   Description:  print out the drive information
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------

eReturnValues CSCSI_Farm_Log::print_Drive_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *driveInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nDrive Information From Farm Log copy %d:", page);
    printf("\tDevice Interface:                         %s         \n", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    printf("\tDevice Capcaity in sectors:               %" PRId64" \n", vFarmFrame[page].driveInfo.deviceCapacity & 0x00FFFFFFFFFFFFFFLL);
    printf("\tPhysical Sector size:                     %" PRIX64" \n", vFarmFrame[page].driveInfo.psecSize & 0x00FFFFFFFFFFFFFFLL);									//!< Physical Sector Size in Bytes
    printf("\tLogical Sector Size:                      %" PRIX64" \n", vFarmFrame[page].driveInfo.lsecSize & 0x00FFFFFFFFFFFFFFLL);									//!< Logical Sector Size in Bytes
    printf("\tDevice Buffer Size:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.deviceBufferSize & 0x00FFFFFFFFFFFFFFLL);							//!< Device Buffer Size in Bytes
    printf("\tNumber of heads:                          %" PRId64" \n", vFarmFrame[page].driveInfo.heads & 0x00FFFFFFFFFFFFFFLL);										//!< Number of Heads
    printf("\tDevice form factor:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.factor & 0x00FFFFFFFFFFFFFFLL);										//!< Device Form Factor (ID Word 168)
    printf("\tserial number:                            %s         \n", vFarmFrame[page].identStringInfo.serialNumber.c_str());
    printf("\tworkd wide name:                          %s         \n", vFarmFrame[page].identStringInfo.worldWideName.c_str());
    printf("\tfirmware Rev:                             %s         \n", vFarmFrame[page].identStringInfo.firmwareRev.c_str());											//!< Firmware Revision [0:3]
    printf("\tRotation Rate:                            %" PRIu64" \n", vFarmFrame[page].driveInfo.rotationRate & 0x00FFFFFFFFFFFFFFLL);								//!< Rotational Rate of Device 
    printf("\treserved:								    %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:								    %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved1 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:								    %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved2 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\tPower on Hours:                           %" PRIu64" \n", vFarmFrame[page].driveInfo.poh & 0x00FFFFFFFFFFFFFFLL);											//!< Power-on Hour
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved3 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved4 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\tHead Load Events:                         %" PRIu64" \n", vFarmFrame[page].driveInfo.headLoadEvents & 0x00FFFFFFFFFFFFFFLL);								//!< Head Load Events
    printf("\tPower Cycle count:                        %" PRIu64" \n", vFarmFrame[page].driveInfo.powerCycleCount & 0x00FFFFFFFFFFFFFFLL);								//!< Power Cycle Count
    printf("\tHardware Reset count:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.resetCount & 0x00FFFFFFFFFFFFFFLL);									//!< Hardware Reset Count
    printf("\treserved:									%" PRIu64" \n", vFarmFrame[page].driveInfo.reserved5 & 0x00FFFFFFFFFFFFFFLL);									//!< treserved
    printf("\tNVC Status @ power on:                    %" PRIu64" \n", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn & 0x00FFFFFFFFFFFFFFLL);							//!< NVC Status on Power-on
    printf("\tTime Available to save:                   %" PRIu64" \n", vFarmFrame[page].driveInfo.timeAvailable & 0x00FFFFFFFFFFFFFFLL);								//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    printf("\tTime of latest frame:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.firstTimeStamp & 0x00FFFFFFFFFFFFFFLL);								//!< Timestamp of first SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
    printf("\tTime of latest frame (milliseconds):      %" PRIu64" \n", vFarmFrame[page].driveInfo.lastTimeStamp & 0x00FFFFFFFFFFFFFFLL);								//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1

#endif
    snprintf((char*)myStr.c_str(), BASIC, "Drive Information From Farm Log copy %" PRId32"", page);
    json_set_name(driveInfo, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Drive Information");

	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.serialNumber.c_str());
	json_push_back(pageInfo, json_new_a("Serial Number", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.worldWideName.c_str());
	json_push_back(pageInfo, json_new_a("World Wide Name", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.firmwareRev.c_str());																//!< Firmware Revision [0:3]
	json_push_back(pageInfo, json_new_a("Firmware Rev", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    json_push_back(pageInfo, json_new_a("Device Interface", (char*)myStr.c_str()));
    set_json_64_bit_With_Status(pageInfo, "Device Capacity in Sectors", vFarmFrame[page].driveInfo.deviceCapacity, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame[page].driveInfo.psecSize, false, m_showStatusBits);									//!< Physical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame[page].driveInfo.lsecSize, false, m_showStatusBits);										//!< Logical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame[page].driveInfo.deviceBufferSize, false, m_showStatusBits);								//!< Device Buffer Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame[page].driveInfo.heads, false, m_showStatusBits);											//!< Number of Heads
	if (check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads) != 0)
	{
		m_heads = check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads);
	}
    set_json_64_bit_With_Status(pageInfo, "Device form factor" , vFarmFrame[page].driveInfo.factor, false, m_showStatusBits);										//!< Device Form Factor (ID Word 168)

    set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame[page].driveInfo.rotationRate, false, m_showStatusBits);										//!< Rotational Rate of Device (ID Word 217)
    set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame[page].driveInfo.poh, false, m_showStatusBits);												//!< Power-on Hour
    set_json_64_bit_With_Status(pageInfo, "Head Load Events", vFarmFrame[page].driveInfo.headLoadEvents, false, m_showStatusBits);									//!< Head Load Events
    set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame[page].driveInfo.powerCycleCount, false, m_showStatusBits);								//!< Power Cycle Count
    set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame[page].driveInfo.resetCount, false, m_showStatusBits);									//!< Hardware Reset Count
    set_json_64_bit_With_Status(pageInfo, "NVC Status @ power on", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn, false, m_showStatusBits);						//!< NVC Status on Power-on
    set_json_64_bit_With_Status(pageInfo, "Time Available to save (in 100us)", vFarmFrame[page].driveInfo.timeAvailable, false, m_showStatusBits);					//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
	set_json_64_bit_With_Status(pageInfo, "First Time Stamp (Milliseconds)",vFarmFrame[page].driveInfo.firstTimeStamp, false, m_showStatusBits);					//!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
    set_json_64_bit_With_Status(pageInfo, "Latest Time Stamp (Milliseconds)", vFarmFrame[page].driveInfo.lastTimeStamp, false, m_showStatusBits);					//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds

    json_push_back(driveInfo, pageInfo);
    json_push_back(masterData, driveInfo);
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintWorkLoad()
//
//! \brief
//!   Description:  print out the work load log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_WorkLoad(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *workLoad = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nWork Load From Farm Log copy %d:", page);
    printf("\tRated Workload Percentaged:               %" PRIu64" \n", vFarmFrame[page].workLoadPage.workloadPercentage & 0x00FFFFFFFFFFFFFFLL);         //!< rated Workload Percentage
	printf("\tTotal Number of Read Commands:            %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalReadCommands & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Read Commands
	printf("\tTotal Number of Write Commands:           %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalWriteCommands & 0x00FFFFFFFFFFFFFFLL);         //!< Total Number of Write Commands
	printf("\tTotal Number of Random Read Cmds:         %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalRandomReads & 0x00FFFFFFFFFFFFFFLL);           //!< Total Number of Random Read Commands
	printf("\tTotal Number of Random Write Cmds:        %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalRandomWrites & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Random Write Commands
    printf("\tTotal Number of Other Commands:           %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS & 0x00FFFFFFFFFFFFFFLL);     //!< Total Number Of Other Commands
    printf("\tLogical Sectors Written:                  %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecWritten & 0x00FFFFFFFFFFFFFFLL);          //!< Logical Sectors Written
    printf("\tLogical Sectors Read:                     %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecRead & 0x00FFFFFFFFFFFFFFLL);             //!< Logical Sectors Read
    
#endif

    snprintf((char*)myStr.c_str(), BASIC, "Work Load From Farm Log copy %" PRId32"", page);
    json_set_name(workLoad, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Work Load");
    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentaged",vFarmFrame[page].workLoadPage.workloadPercentage, false, m_showStatusBits);				//!< rated Workload Percentage
	set_json_64_bit_With_Status(pageInfo, "Total Number of Read Commands", vFarmFrame[page].workLoadPage.totalReadCommands, false, m_showStatusBits);			//!< Total Number of Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Write Commands", vFarmFrame[page].workLoadPage.totalWriteCommands, false, m_showStatusBits);			//!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Number of Random Read Cmds", vFarmFrame[page].workLoadPage.totalRandomReads, false, m_showStatusBits);			//!< Total Number of Random Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Random Write Cmds", vFarmFrame[page].workLoadPage.totalRandomWrites, false, m_showStatusBits);		//!< Total Number of Random Write Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Other Commands", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS, false, m_showStatusBits);		//!< Total Number Of Other Commands
	set_json_64_bit_With_Status(pageInfo, "Logical Sectors Written", vFarmFrame[page].workLoadPage.logicalSecWritten, false, m_showStatusBits);					//!< Logical Sectors Written
    set_json_64_bit_With_Status(pageInfo, "Logical Sectors Read", vFarmFrame[page].workLoadPage.logicalSecRead, false, m_showStatusBits);						//!< Logical Sectors Read
    
    json_push_back(workLoad, pageInfo);
    json_push_back(masterData, workLoad);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintErrorInformation()
//
//! \brief
//!   Description:  print out the work load log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Error_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *errorPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nError Information Log From Farm Log copy %d:", page);
    printf("\tUnrecoverable Read Errors:                %" PRIu64" \n", vFarmFrame[page].errorPage.totalReadECC & 0x00FFFFFFFFFFFFFFLL);				//!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors:               %" PRIu64" \n", vFarmFrame[page].errorPage.totalWriteECC & 0x00FFFFFFFFFFFFFFLL);				//!< Number of Unrecoverable Write Errors
    printf("\tNumber of Reallocated Sectors:            %" PRIu64" \n", vFarmFrame[page].errorPage.totalReallocations & 0x00FFFFFFFFFFFFFFLL);			//!< Number of Reallocated Sectors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tNumber of Mechanical Start Failures:      %" PRIu64" \n", vFarmFrame[page].errorPage.totalMechanicalFails & 0x00FFFFFFFFFFFFFFLL);		//!< Number of Mechanical Start Failures
    printf("\tNumber of Reallocated Candidate Sectors:  %" PRIu64" \n", vFarmFrame[page].errorPage.totalReallocatedCanidates & 0x00FFFFFFFFFFFFFFLL);	//!< Number of Reallocated Candidate Sectors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved1 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved2 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved3 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved4 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved5 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tNumber of IOEDC Errors (Raw):             %" PRIu64" \n", vFarmFrame[page].errorPage.attrIOEDCErrors & 0x00FFFFFFFFFFFFFFLL);				//!< Number of IOEDC Errors 
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved6 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved7 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved8 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tTotal Flash LED (Assert) Events:          %" PRIu64" \n", vFarmFrame[page].errorPage.totalFlashLED & 0x00FFFFFFFFFFFFFFLL);				//!< Total Flash LED (Assert) Events
	printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.reserved9 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
	printf("\tFRU code if smart trip from most recent SMART Frame:%" PRIu64" \n", vFarmFrame[page].errorPage.FRUCode & 0x00FFFFFFFFFFFFFFLL);
#endif

    snprintf((char*)myStr.c_str(), BASIC, "Error Information Log From Farm Log copy %" PRId32"", page);
    json_set_name(errorPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Error Information");
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.totalReadECC, false, m_showStatusBits);							//!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.totalWriteECC, false, m_showStatusBits);							//!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors",vFarmFrame[page].errorPage.totalReallocations, false, m_showStatusBits);					//!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of Mechanical Start Failures",vFarmFrame[page].errorPage.totalMechanicalFails, false, m_showStatusBits);			//!< Number of Mechanical Start Failures
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors",vFarmFrame[page].errorPage.totalReallocatedCanidates, false, m_showStatusBits); //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of IOEDC Errors (Raw)",vFarmFrame[page].errorPage.attrIOEDCErrors, false, m_showStatusBits);						//!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events",vFarmFrame[page].errorPage.totalFlashLED, false, m_showStatusBits);						//!< Total Flash LED (Assert) Events
	set_json_64_bit_With_Status(pageInfo, "FRU code if smart trip from most recent SMART Frame", vFarmFrame[page].errorPage.FRUCode, false, m_showStatusBits);		//!< FRU code if smart trip from most recent SMART Frame

    json_push_back(errorPage, pageInfo);
    json_push_back(masterData, errorPage);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintEnviromentInformation()
//
//! \brief
//!   Description:  print out the Envirnment log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Enviroment_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *envPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nEnvironment Information From Farm Log copy %d:", page);

    printf("\tCurrent Temperature:                  %" PRIu64" \n", vFarmFrame[page].environmentPage.curentTemp & 0x00FFFFFFFFFFFFFFLL);			//!< Current Temperature in Celsius
    printf("\tHighest Temperature:                  %" PRIu64" \n", vFarmFrame[page].environmentPage.highestTemp & 0x00FFFFFFFFFFFFFFLL);			//!< Highest Temperature in Celsius
    printf("\tLowest Temperature:                   %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestTemp & 0x00FFFFFFFFFFFFFFLL);			//!< Lowest Temperature
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved1 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved2 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved3 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved4 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved5 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved6 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved7 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tSpecified Max Operating Temperature:  %" PRIu64" \n", vFarmFrame[page].environmentPage.maxTemp & 0x00FFFFFFFFFFFFFFLL);				//!< Specified Max Operating Temperature
    printf("\tSpecified Min Operating Temperature:  %" PRIu64" \n", vFarmFrame[page].environmentPage.minTemp & 0x00FFFFFFFFFFFFFFLL);				//!< Specified Min Operating Temperature
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved8 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                             %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved9 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tCurrent Relative Humidity:            %" PRIu64" \n", vFarmFrame[page].environmentPage.humidity & 0x00FFFFFFFFFFFFFFLL);				//!< Current Relative Humidity (in units of .1%)
    printf("\tHumidity Mixed Ratio:                 %" PRIu64" \n", ((vFarmFrame[page].environmentPage.humidityRatio & 0x00FFFFFFFFFFFFFFLL) / 8)); //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    printf("\tCurrent Motor Power:                  %" PRIu64" \n", vFarmFrame[page].environmentPage.currentMotorPower & 0x00FFFFFFFFFFFFFFLL);		//!< Current Motor Power, value from most recent SMART Summary Frame6
#endif

    snprintf((char*)myStr.c_str(), BASIC, "Environment Information From Farm Log copy %" PRId32"", page);
    json_set_name(envPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Environment");

	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.curentTemp))*1.00);							//!< Current Temperature in Celsius
	set_json_string_With_Status(pageInfo, "Current Temperature (Celsius)", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.curentTemp, m_showStatusBits);
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestTemp))*1.00);						//!< Highest Average Long Term Temperature
	set_json_string_With_Status(pageInfo, "Highest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.highestTemp, m_showStatusBits);		
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestTemp))*1.00);							//!< Lowest Average Long Term Temperature
	set_json_string_With_Status(pageInfo, "Lowest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.lowestTemp, m_showStatusBits);			
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.maxTemp))*1.00);							//!< Specified Max Operating Temperature
	set_json_string_With_Status(pageInfo, "Specified Max Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.maxTemp, m_showStatusBits);					
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.minTemp))*1.00);							//!< Specified Min Operating Temperature
	set_json_string_With_Status(pageInfo, "Specified Min Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.minTemp, m_showStatusBits);					
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidity))*0.1);							//!< Current Relative Humidity (in units of .1%)
	set_json_string_With_Status(pageInfo, "Current Relative Humidity", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidity, m_showStatusBits);
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidityRatio) / 8.0));						//!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
	set_json_string_With_Status(pageInfo, "Humidity Mixed Ratio", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidityRatio, m_showStatusBits);
	set_json_int_With_Status(pageInfo, "Current Motor Power", vFarmFrame[page].environmentPage.currentMotorPower, m_showStatusBits);					    //!< Current Motor Power, value from most recent SMART Summary Frame6
	
    json_push_back(envPage, pageInfo);
    json_push_back(masterData, envPage);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Reli_Information()
//
//! \brief
//!   Description:  print out the Reli log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Reli_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *reliPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nReliability Information From Farm Log copy: %d\n", page);
    printf("\tTimestamp of last IDD test:               %" PRIu64" \n", vFarmFrame[page].reliPage.lastIDDTest & 0x00FFFFFFFFFFFFFFLL);                     //!< Timestamp of last IDD test
    printf("\tSub-command of last IDD test:             %" PRIu64" \n", vFarmFrame[page].reliPage.cmdLastIDDTest & 0x00FFFFFFFFFFFFFFLL);                  //!< Sub-command of last IDD test
    printf("\tNumber of G-List Reclamations:            %" PRIu64" \n", vFarmFrame[page].reliPage.gListReclamed & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of G-List Reclamations 
    printf("\tServo Status:                             %" PRIu64" \n", vFarmFrame[page].reliPage.servoStatus & 0x00FFFFFFFFFFFFFFLL);                     //!< Servo Status (follows standard DST error code definitions)
    printf("\tAlts List Entries Before IDD Scan:        %" PRIu64" \n", vFarmFrame[page].reliPage.altsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Alt List Entries Before IDD Scan
    printf("\tAltz List Entries After IDD Scan:         %" PRIu64" \n", vFarmFrame[page].reliPage.altsAfterIDD & 0x00FFFFFFFFFFFFFFLL);                    //!< Number of Alt List Entries After IDD Scan
    printf("\tResident G-List Entries Before IDD Scan:  %" PRIu64" \n", vFarmFrame[page].reliPage.gListBeforIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Resident G-List Entries Before IDD Scan
    printf("\tResident G-List Entries After IDD Scan:   %" PRIu64" \n", vFarmFrame[page].reliPage.gListAfterIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Resident G-List Entries After IDD Scan
    printf("\tScrubs List Entries Before IDD Scan:      %" PRIu64" \n", vFarmFrame[page].reliPage.scrubsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);                 //!< Number of Scrub List Entries Before IDD Scan
    printf("\tScrubs List Entries After IDD Scan:       %" PRIu64" \n", vFarmFrame[page].reliPage.scrubsAfterIDD & 0x00FFFFFFFFFFFFFFLL);                  //!< Number of Scrub List Entries After IDD Scan
    printf("\tNumber of DOS Scans Performed:            %" PRIu64" \n", vFarmFrame[page].reliPage.numberDOSScans & 0x00FFFFFFFFFFFFFFLL);                  //!< Number of DOS Scans Performed
    printf("\tNumber of LBAs Corrected by ISP:          %" PRIu64" \n", vFarmFrame[page].reliPage.numberLBACorrect & 0x00FFFFFFFFFFFFFFLL);                //!< Number of LBAs Corrected by ISP
    printf("\tNumber of Valid Parity Sectors:           %" PRIu64" \n", vFarmFrame[page].reliPage.numberValidParitySec & 0x00FFFFFFFFFFFFFFLL);            //!< Number of Valid Parity Sectors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved & 0x00FFFFFFFFFFFFFFLL);							//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved1 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved2 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved3 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved4 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved5 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reserved6 & 0x00FFFFFFFFFFFFFFLL);						//!< Reserved
    printf("\tMicro Actuator Lock-out accumulated:      %" PRIu64" \n", vFarmFrame[page].reliPage.microActuatorLockOut & 0x00FFFFFFFFFFFFFFLL);				//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
	printf("\t# of Disc Slip Recalibrations Performed : %" PRIu64" \n", vFarmFrame[page].reliPage.diskSlipRecalPerformed & 0x00FFFFFFFFFFFFFFLL);			//!< Number of disc slip recalibrations performed
	printf("\tHelium Pressure Threshold Trip:           %" PRIu64" \n", vFarmFrame[page].reliPage.heliumPressuretThreshold & 0x00FFFFFFFFFFFFFFLL);			//!< helium Pressure Threshold Trip
	printf("\tRV Absulute Mean:                         %" PRIu64" \n", vFarmFrame[page].reliPage.rvAbsuluteMean & 0x00FFFFFFFFFFFFFFLL);					//!< RV Absulute Mean
	printf("\tMax RV absulute Mean:                     %" PRIu64" \n", vFarmFrame[page].reliPage.maxRVAbsuluteMean & 0x00FFFFFFFFFFFFFFLL);				//!< Max RV absulute Mean
	printf("\tIdle Time value from the most recent SMART Summary Frame:     %" PRIu64" \n", vFarmFrame[page].reliPage.idleTime & 0x00FFFFFFFFFFFFFFLL);		//!< idle Time value from the most recent SMART Summary Frame

#endif

    snprintf((char*)myStr.c_str(), BASIC, "Reliability Information From Farm Log copy: %" PRId32"", page);
    json_set_name(reliPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Reliability");

    set_json_64_bit_With_Status(pageInfo, "Timestamp of last IDD test", vFarmFrame[page].reliPage.lastIDDTest, false, m_showStatusBits);							//!< Timestamp of last IDD test
    set_json_64_bit_With_Status(pageInfo, "Sub-command of last IDD test", vFarmFrame[page].reliPage.cmdLastIDDTest, false, m_showStatusBits);						//!< Sub-command of last IDD test
    set_json_64_bit_With_Status(pageInfo, "Number of G-List Reclamations", vFarmFrame[page].reliPage.gListReclamed, false, m_showStatusBits);						//!< Number of G-List Reclamations 
    set_json_64_bit_With_Status(pageInfo, "Servo Status", vFarmFrame[page].reliPage.servoStatus, false, m_showStatusBits);											//!< Servo Status (follows standard DST error code definitions)
    set_json_64_bit_With_Status(pageInfo, "Alts List Entries Before IDD Scan", vFarmFrame[page].reliPage.altsBeforeIDD, false, m_showStatusBits);					//!< Number of Alt List Entries Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Alts List Entries After IDD Scan", vFarmFrame[page].reliPage.altsAfterIDD, false, m_showStatusBits);						//!< Number of Alt List Entries After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Resident G-List Entries Before IDD Scan", vFarmFrame[page].reliPage.gListBeforIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Resident G-List Entries After IDD Scan", vFarmFrame[page].reliPage.gListAfterIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Scrubs List Entries Before IDD Scan", vFarmFrame[page].reliPage.scrubsBeforeIDD, false, m_showStatusBits);				//!< Number of Scrub List Entries Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Scrubs List Entries After IDD Scan", vFarmFrame[page].reliPage.scrubsAfterIDD, false, m_showStatusBits);					//!< Number of Scrub List Entries After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", vFarmFrame[page].reliPage.numberDOSScans, false, m_showStatusBits);						//!< Number of DOS Scans Performed
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", vFarmFrame[page].reliPage.numberLBACorrect, false, m_showStatusBits);					//!< Number of LBAs Corrected by ISP
    set_json_64_bit_With_Status(pageInfo, "Number of Valid Parity Sectors", vFarmFrame[page].reliPage.numberValidParitySec, false, m_showStatusBits);				//!< Number of Valid Parity Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of RAW Operations", vFarmFrame[page].reliPage.numberRAWops, false, m_showStatusBits);								//!< Number of RAW Operations
    set_json_64_bit_With_Status(pageInfo, "Micro Actuator Lock-out accumulated", vFarmFrame[page].reliPage.microActuatorLockOut, false, m_showStatusBits);			//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
	set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame[page].reliPage.diskSlipRecalPerformed, false, m_showStatusBits);	            //!< Number of disc slip recalibrations performed
	set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Trip", vFarmFrame[page].reliPage.heliumPressuretThreshold, false, m_showStatusBits);			//!< helium Pressure Threshold Trip
	set_json_64_bit_With_Status(pageInfo, "RV Absulute Mean",vFarmFrame[page].reliPage.rvAbsuluteMean, false, m_showStatusBits);									//!< RV Absulute Mean
	set_json_64_bit_With_Status(pageInfo, "Max RV absulute Mean", vFarmFrame[page].reliPage.maxRVAbsuluteMean, false, m_showStatusBits);							//!< Max RV absulute Mean
	set_json_64_bit_With_Status(pageInfo, "Idle Time value from the most recent SMART Summary Frame", vFarmFrame[page].reliPage.idleTime, false, m_showStatusBits);	//!< idle Time value from the most recent SMART Summary Frame

    json_push_back(reliPage, pageInfo);
    json_push_back(masterData, reliPage);

    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Head_Information()
//
//! \brief
//!   Description:  print out the Reli log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Head_Information(eLogPageTypes type, JSONNODE *masterData, uint32_t page)
{
    uint32_t loopCount = 0;
	JSONNODE *headInfoPage = json_new(JSON_NODE);
	std::string myStr = " ";
	myStr.resize(BASIC);
	snprintf((char*)myStr.c_str(), BASIC, "Head Information From Farm Log copy: %" PRId32"", page);
	
	json_set_name(headInfoPage, (char*)myStr.c_str());

    std::string myHeader = " ";
    set_Head_Header(myHeader, type);
    myHeader.resize(BASIC);
    JSONNODE *headPage = json_new(JSON_NODE);
    json_set_name(headPage, (char *)myHeader.c_str());
    switch (type)
    {
    case FARM_HEADER_PARAMETER:
    case GENERAL_DRIVE_INFORMATION_PARAMETER:
    case WORKLOAD_STATISTICS_PARAMETER:
    case ERROR_STATISTICS_PARAMETER:
    case ENVIRONMENTAL_STATISTICS_PARAMETER:
    case RELIABILITY_STATISTICS_PARAMETER:
        break;
    case DISC_SLIP_IN_MICRO_INCHES_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDisc Slip in micro-inches by Head by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].discSlipPerHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Disc Slip in micro-inches

#endif
			uint64_t dsHead = check_Status_Strip_Status(vFarmFrame[page].discSlipPerHead.headValue[loopCount]);
			int16_t whole = M_BytesTo2ByteValue(*((uint8_t*)&dsHead + 5), *((uint8_t*)&dsHead + 4));							// get 5:4 whole part of the float
			uint32_t decimal  = M_BytesTo4ByteValue(*((uint8_t*)&dsHead + 3), *((uint8_t*)&dsHead + 2) , *((uint8_t*)&dsHead + 1), *((uint8_t*)&dsHead + 0));  // get 3:0 for the Deciaml Part of the float
            snprintf((char*)myHeader.c_str(), BASIC, "Disc Slip in micro-inches Head %d", loopCount); // Head count
			snprintf((char*)myStr.c_str(), BASIC, "%0.04f", (std::abs((double)whole) + ((double)decimal / 10000))); //!< Disc Slip in micro-inches by Head
			set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].discSlipPerHead.headValue[loopCount], m_showStatusBits);
        }
        break;
    case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tBit Error Rate of Zone 0 by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].bitErrorRateByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Bit Error Rate of Zone 0 by Drive Head
#endif
			uint64_t beHead = check_Status_Strip_Status(vFarmFrame[page].bitErrorRateByHead.headValue[loopCount]);
			int16_t whole = M_BytesTo2ByteValue(*((uint8_t*)&beHead + 5), *((uint8_t*)&beHead + 4));							// get 5:4 whole part of the float
			uint32_t decimal = M_BytesTo4ByteValue(*((uint8_t*)&beHead + 3), *((uint8_t*)&beHead + 2), *((uint8_t*)&beHead + 1), *((uint8_t*)&beHead + 0));  // get 3:0 for the Deciaml Part of the float
            snprintf((char*)myHeader.c_str(), BASIC, "Bit Error Rate of Zone 0 Head number %d", loopCount); // Head count
			snprintf((char*)myStr.c_str(), BASIC, "%0.04f", ((std::abs((double)whole) + ((double)decimal / 10000)) * -1));  //!< Bit Error Rate of Zone 0 by Drive Head
			set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].bitErrorRateByHead.headValue[loopCount], m_showStatusBits);
        }
        break;
    case DOS_WRITE_REFRESH_COUNT:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDOS Write Refresh Count by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].dosWriteRefreshCountByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< DOS Write Refresh Count
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "DOS Write Refresh Count Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].dosWriteRefreshCountByHead.headValue[loopCount], false, m_showStatusBits);  //!< DOS Write Refresh Count
        }
        break;
    case DVGA_SKIP_WRITE_DETECT_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDVGA Skip Write 0 by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].dvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< DVGA Skip Write
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "VGA Skip Write Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].dvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< DVGA Skip Write
        }
        break;
    case RVGA_SKIP_WRITE_DETECT_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tRVGA Skip Write 0 by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].rvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< RVGA Skip Write
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "RVGA Skip Writ Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].rvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< RVGA Skip Write
        }
        break;
    case FVGA_SKIP_WRITE_DETECT_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tFVGA Skip Write 0 by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].fvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< FVGA Skip Write 
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "FVGA Skip Writ Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].fvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< FVGA Skip Write 
        }
        break;
    case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tSkip Write Detect Threshold Exceeded Count by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].skipWriteDectedThresholdExceededByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Skip Write Detect Threshold Exceeded Count
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Skip Write Detect Threshold Exceeded Count Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].skipWriteDectedThresholdExceededByHead.headValue[loopCount], false, m_showStatusBits);  //!< Skip Write Detect Threshold Exceeded Count
        }
        break;
    case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tACFF Sine 1X, value from most recent SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].acffSine1xValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< ACFF Sine 1X, value from most recent SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "ACFF Sine 1X value from most recent SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].acffSine1xValueByHead.headValue[loopCount], false, m_showStatusBits);  //!< ACFF Sine 1X, value from most recent SMART Summary Frame
        }
        break;
    case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tACFF Cosine 1X, value from most recent SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].acffCosine1xValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< ACFF Cosine 1X, value from most recent SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "ACFF Cosine 1X value from most recent SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].acffCosine1xValueByHead.headValue[loopCount], false, m_showStatusBits);  //!< ACFF Cosine 1X, value from most recent SMART Summary Frame
        }
        break;
    case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tPZT Calibration, value from most recent SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].pztCalibrationValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< PZT Calibration, value from most recent SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "PZT Calibration value from most recent SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].pztCalibrationValueByHead.headValue[loopCount], false, m_showStatusBits);  //!< PZT Calibration, value from most recent SMART SummaryFrame
        }
        break;
    case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tMR Head Resistance from most recent SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< MR Head Resistance from most recent SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "MR Head Resistanc from most recent SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount], false, m_showStatusBits);  //!< MR Head Resistance from most recent SMART Summary Frame
        }
        break;
    case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
                printf("\tNumber of TMD over last 3 SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].numberOfTMDByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Number of TMD over last 3 SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Number of TMD over last 3 SMART Summary Frame Head number %d", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].numberOfTMDByHead.headValue[loopCount], false, m_showStatusBits);  //!< Number of TMD over last 3 SMART Summary Frame
        }
        break;
    case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tVelocity Observer over last 3 SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].velocityObserverByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Velocity Observer over last 3 SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Velocity Observer over last 3 SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].velocityObserverByHead.headValue[loopCount], false, m_showStatusBits); //!< Velocity Observer over last 3 SMART Summary Frame
        }
        break;
    case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tNumber of Velocity Observer over last 3 SMART Summary Frame by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].numberOfVelocityObservedByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Number of Velocity Observer over last 3 SMART Summary Frame
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Number of Velocity Observer over last 3 SMART Summary Frame Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].numberOfVelocityObservedByHead.headValue[loopCount], false, m_showStatusBits); //!< Number of Velocity Observer over last 3 SMART Summary Frame
        }
        break;
    case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT percentage of codewords at iteration level by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2SATPercentagedbyHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT percentage of codewords at iteration level
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT percentage of codewords at iteration level Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2SATPercentagedbyHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT percentage of codewords at iteration level
        }
        break;
    case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT amplitude, averaged across Test Zone by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT amplitude, averaged across Test Zone
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT amplitude Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT amplitude, averaged across Test Zone
        }
        break;
    case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT asymmetry, averaged across Test Zone by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT asymmetry, averaged across Test Zone
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT asymmetry Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT asymmetry, averaged across Test Zone
        }
        break;
    case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tResident Glist Entries by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentGlistEntries.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Resident Glist Entries Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].ResidentGlistEntries.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case NUMBER_OF_PENDING_ENTRIES:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tResident Pending list Entries by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentPlistEntries.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Resident Pending list Entries Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].ResidentPlistEntries.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDOS Oought To Scan Count by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSOoughtToScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "DOS Oought To Scan Count Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSOoughtToScan.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case DOS_NEED_TO_SCAN_COUNT_PER_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDOS Needs To Scan Count by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSNeedToScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "DOS Needs To Scan Count Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSNeedToScan.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tDOS Write Fault Scan Count by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSWriteFaultScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "DOS Write Fault Scan Count Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSWriteFaultScan.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tWrite Power On Hours from most recent SMART by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].writePowerOnHours.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Write Power On Hour Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].writePowerOnHours.headValue[loopCount], false, m_showStatusBits);
        }
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT trimmed mean bits in error by Head %d , by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount], false, m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
        }
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT trimmed mean bits in error by Head %d , by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
        }
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT trimmed mean bits in error by Head %d , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
        }
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT iterations to converge by Head %d , by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 0 Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
        }
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT iterations to converge by Head %d , by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 1 Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
        }
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tCurrent H2SAT iterations to converge by Head %d , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 2 Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
        }
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tApplied fly height clearance delta per head in thousandths of one Angstrom: Outer by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadOuter.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Outer by Head
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head in thousandths of one Angstrom: Outer Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].appliedFlyHeightByHeadOuter.headValue[loopCount], false, m_showStatusBits);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Outer by Head
        }
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tApplied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadInner.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head in thousandths of one Angstrom: Inner Head number %d", loopCount); // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].appliedFlyHeightByHeadInner.headValue[loopCount], false, m_showStatusBits);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head
        }
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
#if defined( _DEBUG)
            printf("\tApplied fly height clearance delta per head in thousandths of one Angstrom: Middle by Head %d:      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadMiddle.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: middle by Head
#endif
            snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head in thousandths of one Angstrom: Middle Head number %d", loopCount);     // Head count
            set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].appliedFlyHeightByHeadMiddle.headValue[loopCount], false, m_showStatusBits);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: middle by Head
        }
        break;
    default:
#if defined( _DEBUG)
        printf("\tSomething went wrong, ERROR \n");  
#endif
        return FAILURE;
        break;
    }
	json_push_back(headInfoPage, headPage);
    json_push_back(masterData, headInfoPage);
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn PrintAllPages()
//
//! \brief
//!   Description:  print out all the pages and the copies of each page
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::print_All_Pages(JSONNODE *masterData)
{
    print_Header(masterData);
    for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
    {

        for (uint32_t pramCode = 0; pramCode < vFarmFrame.at(index).vFramesFound.size(); pramCode++)
        {
            switch (vFarmFrame.at(index).vFramesFound.at(pramCode))
            {
            case FARM_HEADER_PARAMETER:
                // get the Farm Header information;
                break;
            case  GENERAL_DRIVE_INFORMATION_PARAMETER:
                print_Drive_Information(masterData, index);                   // get the id drive information at the time.
                break;
            case  WORKLOAD_STATISTICS_PARAMETER:
                print_WorkLoad(masterData, index);                           // get the work load information
                break;
            case ERROR_STATISTICS_PARAMETER:
                print_Error_Information(masterData, index);                    // get the error status
                break;
            case ENVIRONMENTAL_STATISTICS_PARAMETER:
                print_Enviroment_Information(masterData, index);               // get the envirmonent information 
                break;
            case RELIABILITY_STATISTICS_PARAMETER:
                print_Reli_Information(masterData, index);         // get the Reliabliity stat
                break;
            case DISC_SLIP_IN_MICRO_INCHES_BY_HEAD:
            case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:
            case DOS_WRITE_REFRESH_COUNT:
            case DVGA_SKIP_WRITE_DETECT_BY_HEAD:
            case RVGA_SKIP_WRITE_DETECT_BY_HEAD:
            case FVGA_SKIP_WRITE_DETECT_BY_HEAD:
            case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:
            case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
            case NUMBER_OF_PENDING_ENTRIES:
            case DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:
            case DOS_NEED_TO_SCAN_COUNT_PER_HEAD:
            case DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:
            case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
            case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
            case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
            case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
            case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
            case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
            case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
            case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:
            case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
            case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
            default:
				print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), masterData, index);
				break;
            }
        }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn PrintPage()
//
//! \brief
//!   Description:  print out a copy of a the pages
//
//  Entry:
//! \param page  = the page copy number of the data we want to print.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::print_Page(JSONNODE *masterData, uint32_t page)
{
    if (page <= vFarmFrame.size())
    {
        print_Drive_Information(masterData, page);
        print_WorkLoad(masterData, page);
        print_Error_Information(masterData, page);
        print_Enviroment_Information(masterData, page);
        print_Reli_Information(masterData, page);
        for (uint32_t frame = 0; frame < vFarmFrame.at(page).vFramesFound.size(); frame++)
        {
            print_Head_Information(vFarmFrame.at(page).vFramesFound.at(frame), masterData, page);
        }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn PrintPageWithoutDriveInfo()
//
//! \brief
//!   Description:  print out a copy of a the pages without the drive info page.
//
//  Entry:
//! \param page  = the page copy number of the data we want to print.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page)
{
    if (page <= vFarmFrame.size())
    {
        print_WorkLoad(masterData, page);
        print_Error_Information(masterData, page);
        print_Enviroment_Information(masterData, page);
        print_Reli_Information(masterData, page);
        for (uint32_t frame = 0; frame < vFarmFrame.at(page).vFramesFound.size(); frame++)
        {
            print_Head_Information(vFarmFrame.at(page).vFramesFound.at(frame), masterData, page);
        }
    }
}

