//
// CScsi_Farm_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Now supporting 3.7 version of the FARM LOG
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
    , m_MinorRev(0)
    , pBuf()
    , m_status(IN_PROGRESS)                                
	, m_logParam()   
	, m_pageParam()
        , m_pDriveInfo() 
	, m_alreadySet(false)          
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
    , m_MinorRev(0)
    , pBuf()
    , m_status(IN_PROGRESS)                                
	, m_logParam()
	, m_pageParam()
    , m_pDriveInfo() 
	, m_alreadySet(false)          
	, m_showStatusBits(showStatus)
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
		if (init_Header_Data() == SUCCESS)							// init the data for getting the log
        {
            m_status = parse_Farm_Log();
        }
        else 
        {
            m_status =FAILURE;
        }
    }
    else
    {
        m_status = FAILURE;
    }
    delete[] pBuf;
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

		m_pHeader = (sScsiFarmHeader *)&pBuf[4];
		swap_Bytes_sFarmHeader(m_pHeader);											// swap all the bytes for the header
		m_totalPages = M_DoubleWord0(m_pHeader->farmHeader.pagesSupported);			// get the total pages
		m_pageSize = M_DoubleWord0(m_pHeader->farmHeader.pageSize);					// get the page size
		if (check_For_Active_Status(&m_pHeader->farmHeader.headsSupported))			// the the number of heads if supported
		{
			if ((m_pHeader->farmHeader.headsSupported & 0x00FFFFFFFFFFFFFFLL) > 0)
			{
				m_heads = M_DoubleWord0(m_pHeader->farmHeader.headsSupported);
				m_MaxHeads = M_DoubleWord0(m_pHeader->farmHeader.headsSupported);
			}
		}
		m_copies = M_DoubleWord0(m_pHeader->farmHeader.copies);						// finish up with the number of copies (not supported "YET" in SAS)
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
    case DOS_WRITE_COUNT_THRESHOLD_PER_HEAD:
		headerName = "DOS Write Count Threshold per Head";
		break;
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
            headerName = "Applied fly height clearance delta per head - Outer";
            m_alreadySet = true;
        }
        else // fix for the firmware issue. FDDRAGON-95
        {
            headerName = "Applied fly height clearance delta per head - Middle";
        }
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
        headerName = "Applied fly height clearance delta per head - Inner";
        break;
    case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
        headerName = "Applied fly height clearance delta per head - Middle";
        break;
    case RESERVED_FOR_FUTURE_EXPANSION:
        headerName = "Future Expansion";
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
void CSCSI_Farm_Log::create_Serial_Number(std::string &serialNumber, const sScsiDriveInfo * const idInfo)
{
	uint64_t sn = 0;
	uint64_t sn1 = idInfo->serialNumber & 0x00FFFFFFFFFFFFFFLL;
	uint64_t sn2 = idInfo->serialNumber2 & 0x00FFFFFFFFFFFFFFLL;
	byte_Swap_64(&sn1);
	byte_Swap_64(&sn2);
	sn = (sn1 | (sn2 >> 32));
	serialNumber.resize(SERIAL_NUMBER_LEN );
	memset((char*)serialNumber.c_str(),0, SERIAL_NUMBER_LEN );
	strncpy((char *)serialNumber.c_str(), (char*)&sn, SERIAL_NUMBER_LEN);
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
void CSCSI_Farm_Log::create_World_Wide_Name(std::string &worldWideName, const sScsiDriveInfo * const idInfo)
{
	uint64_t wwn = 0;
	wwn = (idInfo->worldWideName & 0x00FFFFFFFFFFFFFFLL) | ((idInfo->worldWideName2 & 0x00FFFFFFFFFFFFFFLL) << 32);
	worldWideName.resize(WORLD_WIDE_NAME_LEN);
	memset((char *)worldWideName.c_str(), 0, WORLD_WIDE_NAME_LEN);
	snprintf((char *)worldWideName.c_str(), WORLD_WIDE_NAME_LEN, "0x%" PRIX64"", wwn);
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
void CSCSI_Farm_Log::create_Firmware_String(std::string &firmwareRev, const sScsiDriveInfo * const idInfo)
{
	uint32_t firm = 0;
	firm = (uint32_t)(idInfo->firmware & 0x00FFFFFFFFFFFFFFLL);
	byte_Swap_32(&firm);
	firmwareRev.resize(FIRMWARE_REV_LEN);
	memset((char *)firmwareRev.c_str(), 0, FIRMWARE_REV_LEN);
	strncpy((char *)firmwareRev.c_str(), (char*)&firm, FIRMWARE_REV_LEN);
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
void CSCSI_Farm_Log::create_Device_Interface_String(std::string &dInterface, const sScsiDriveInfo * const idInfo)
{
	uint64_t dFace = 0;
	dFace = (idInfo->deviceInterface & 0x00FFFFFFFFFFFFFFLL);
	dInterface.resize(DEVICE_INTERFACE_LEN);
	memset((char *)dInterface.c_str(), 0, DEVICE_INTERFACE_LEN);
	strncpy((char *)dInterface.c_str(), (char*)&dFace, DEVICE_INTERFACE_LEN);

}
//-----------------------------------------------------------------------------
//
//! \fn create_Model_Number_String()
//
//! \brief
//!   Description:  fill in the model number of the drive 
//
//  Entry:
//! \param model - pointer to the  model number of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::create_Model_Number_String(std::string &model, const sScsiDriveInfo * const idInfo)
{
	memset((char *)model.c_str(), 0, DEVICE_INTERFACE_LEN);
	strncpy((char *)model.c_str(),"12345678", DEVICE_INTERFACE_LEN);
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
bool CSCSI_Farm_Log::swap_Bytes_sDriveInfo(sScsiDriveInfo *di)
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
bool CSCSI_Farm_Log::swap_Bytes_sWorkLoadStat(sScsiWorkLoadStat *wl)
{
    byte_Swap_64(&wl->workLoad.copyNumber);
    byte_Swap_64(&wl->workLoad.logicalSecRead);
    byte_Swap_64(&wl->workLoad.logicalSecWritten);
    byte_Swap_64(&wl->workLoad.pageNumber);
    byte_Swap_64(&wl->workLoad.totalNumberofOtherCMDS);
    byte_Swap_64(&wl->workLoad.totalRandomReads);
    byte_Swap_64(&wl->workLoad.totalRandomWrites);
    byte_Swap_64(&wl->workLoad.totalReadCommands);
    byte_Swap_64(&wl->workLoad.totalWriteCommands);
    byte_Swap_64(&wl->workLoad.workloadPercentage);
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
bool CSCSI_Farm_Log::swap_Bytes_sErrorStat(sScsiErrorStat * es)
{
    byte_Swap_64(&es->totalReadECC);
    byte_Swap_64(&es->totalWriteECC);
    byte_Swap_64(&es->totalReallocations);
    byte_Swap_64(&es->attrIOEDCErrors);
    byte_Swap_64(&es->copyNumber);
    byte_Swap_16(&es->pPageHeader.pramCode);
    byte_Swap_64(&es->pageNumber);
    byte_Swap_64(&es->totalFlashLED);
    byte_Swap_64(&es->totalMechanicalFails);
    byte_Swap_64(&es->totalReallocatedCanidates);
    byte_Swap_64(&es->FRUCode);
    byte_Swap_64(&es->parity);
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
bool CSCSI_Farm_Log::swap_Bytes_sEnvironmentStat(sScsiEnvironmentStat *es)
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
    byte_Swap_16(&es->pPageHeader.pramCode);
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
    byte_Swap_16(&ss->pPageHeader.pramCode);
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
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the farm header
//
//  Entry:
//! \param sScsiReliabilityStat  =  pointer to the reliability stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sFarmHeader(sScsiFarmHeader *fh)
{
    byte_Swap_64(&fh->farmHeader.copies);
    byte_Swap_64(&fh->farmHeader.headsSupported);
    byte_Swap_64(&fh->farmHeader.logSize);
    byte_Swap_64(&fh->farmHeader.majorRev);
    byte_Swap_64(&fh->farmHeader.minorRev);
    byte_Swap_64(&fh->farmHeader.pageSize);
    byte_Swap_64(&fh->farmHeader.pagesSupported);
    byte_Swap_64(&fh->farmHeader.signature);
	//m_MinorRev = fh->minorRev & 0x00FFLL;
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sScsiReliabilityStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the LUN Structure
//
//  Entry:
//! \param sLUNStruct  =  pointer to the LUN Structure
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sLUNStruct(sLUNStruct *LUN)
{
    byte_Swap_64(&LUN->correctedLBAbyISP);
    byte_Swap_64(&LUN->dosScansPerformed);
    byte_Swap_64(&LUN->headLoadEvents);
    byte_Swap_64(&LUN->idleTime);
    byte_Swap_64(&LUN->LUNID);
    byte_Swap_64(&LUN->maxRVabsolue);
    byte_Swap_64(&LUN->paritySectors);
    byte_Swap_64(&LUN->reallocatedCandidates);
    byte_Swap_64(&LUN->reallocatedSectors);
    byte_Swap_64(&LUN->reclamedGlist);
    byte_Swap_64(&LUN->residentReallocatedAfterIDD);
    byte_Swap_64(&LUN->residentReallocatedBeforeIDD);
    byte_Swap_64(&LUN->RVabsolue);
    byte_Swap_64(&LUN->servoStatus);
    byte_Swap_64(&LUN->slippedSectorsAfterIDD);
    byte_Swap_64(&LUN->slippedSectorsBeforeIDD);
    byte_Swap_64(&LUN->subCmdOfIDD);
    byte_Swap_64(&LUN->successScrubbedAfterIDD);
    byte_Swap_64(&LUN->successScrubbedBeforeIDD);
    byte_Swap_64(&LUN->timeStampOfIDD);
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Head_Info()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for heads
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
    memcpy(phead->headValue, (sHeadInformation *)&buffer[4], (sizeof(uint64_t) * (size_t) m_heads));
    memcpy(&phead->pageHeader, (sScsiPageParameter *)&pBuf[0], sizeof(sScsiPageParameter));
    for (uint32_t index = 0; index < m_heads; index++)
    {
        byte_Swap_64(&phead->headValue[index] );
    }
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
//! \param pLUN  =  pointer to the LUN structure
//! \param buffer = the pointer to the buffer data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::get_LUN_Info(sLUNStruct *pLUN, uint8_t *buffer)
{
    memcpy(pLUN, (sLUNStruct *)&buffer[4], sizeof(sLUNStruct) );
    memcpy(&pLUN->pageHeader, (sScsiPageParameter *)&buffer[0], sizeof(sScsiPageParameter));
    swap_Bytes_sLUNStruct(pLUN);
}
//-----------------------------------------------------------------------------
//
//! \fn parse_Farm_Log()
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
eReturnValues CSCSI_Farm_Log::parse_Farm_Log()
{
    uint64_t offset = 4;                                                                    // the first page starts at offset 4                                   
    bool headerAlreadyFound = false;                                                        // set to false, for files that are missing data
    sScsiFarmFrame *pFarmFrame = new sScsiFarmFrame();								       	// create the pointer to the union

    if (pBuf == NULL)
    {
        return FAILURE;
    }
    uint64_t signature = m_pHeader->farmHeader.signature & 0x00FFFFFFFFFFFFFFLL;
    if (signature != FARMSIGNATURE || signature == 0x00FFFFFFFFFFFFFF)
    {
        return VALIDATION_FAILURE;
    }
    if (signature == FARMSIGNATURE)				// check the head to see if it has the farm signature else fail
    {
        
        for (uint32_t index = 0; index <= m_copies; ++index)						// loop for the number of copies. I don't think it's zero base as of now
        {
            if (pFarmFrame->vFramesFound.size() > 1)
            {
                pFarmFrame->vFramesFound.clear();                                                 // clear the vector for the next copy
            }

            while (offset < m_logSize)
            {
                m_pageParam = (sScsiPageParameter *)&pBuf[offset];										 // get the page params, so we know what the param code is. 
                byte_Swap_16(&m_pageParam->pramCode);
                if (!headerAlreadyFound || (m_pageParam->pramCode != 0x0000 && m_pageParam->pramCode < 0x004F))
                {
                    pFarmFrame->vFramesFound.push_back((eLogPageTypes)m_pageParam->pramCode);                // collect all the log page types in a vector to pump them out at the end
                }
                switch (m_pageParam->pramCode)
                {
                    
                case FARM_HEADER_PARAMETER:
                    {
                        if (headerAlreadyFound == false)                                    // check to see if we have already found the header
                        {
                            m_pHeader = (sScsiFarmHeader *)&pBuf[offset];                    // get the Farm Header information
                            memcpy((sScsiFarmHeader *)&pFarmFrame->farmHeader, m_pHeader, sizeof(sScsiFarmHeader));
                            offset += (m_pageParam->plen + sizeof(sScsiPageParameter));
                            headerAlreadyFound = true;                                      // set the header to true so we will not look at the data a second time
                        }
                        else
                        {
                            offset += (sizeof(sScsiPageParameter));
                        }
                    }
                    break; 
 
                case  GENERAL_DRIVE_INFORMATION_PARAMETER:
                    {
                        m_pDriveInfo = (sScsiDriveInfo *)&pBuf[offset ];														// get the id drive information at the time.
                        swap_Bytes_sDriveInfo(m_pDriveInfo);
                        memcpy(&pFarmFrame->driveInfo,m_pDriveInfo, sizeof(sScsiDriveInfo));

                        create_Serial_Number(pFarmFrame->identStringInfo.serialNumber, m_pDriveInfo);										// create the serial number
                        create_World_Wide_Name(pFarmFrame->identStringInfo.worldWideName, m_pDriveInfo);										// create the wwwn
                        create_Firmware_String(pFarmFrame->identStringInfo.firmwareRev, m_pDriveInfo);										// create the firmware string
                        create_Device_Interface_String(pFarmFrame->identStringInfo.deviceInterface, m_pDriveInfo);							// get / create the device interface string
                        create_Model_Number_String(pFarmFrame->identStringInfo.modelNumber,  m_pDriveInfo);

                        offset += (m_pageParam->plen + sizeof(sScsiPageParameter));
                    }
                    break;
                   
                case  WORKLOAD_STATISTICS_PARAMETER:
                    {
                       
                        sScsiWorkLoadStat *pworkLoad = NULL; 										// get the work load information
                        pworkLoad = (sScsiWorkLoadStat *)&pBuf[offset ];
                        swap_Bytes_sWorkLoadStat(pworkLoad);
                        memcpy((sScsiWorkLoadStat *)&pFarmFrame->workLoadPage, pworkLoad, sizeof(sScsiWorkLoadStat));
                        offset += (m_pageParam->plen + sizeof(sScsiPageParameter));
                    }
                    break;
                   
                case ERROR_STATISTICS_PARAMETER:   
                    {
                        sScsiErrorStat *pError ;                                                     // get the error status
                        pError = (sScsiErrorStat *)&pBuf[offset];
                        swap_Bytes_sErrorStat(pError);
                        memcpy((sScsiErrorStat *)&pFarmFrame->errorPage,pError, sizeof(sScsiErrorStat));
                        offset += (pError->pPageHeader.plen + sizeof(sScsiPageParameter));
                    }
                    break;
                    
                case ENVIRONMENTAL_STATISTICS_PARAMETER:     
                    {
                        sScsiEnvironmentStat *pEnvironment;                            // get the envirmonent information 
                        pEnvironment = (sScsiEnvironmentStat *)&pBuf[offset];
                        swap_Bytes_sEnvironmentStat(pEnvironment);
                        memcpy((sScsiEnvironmentStat *)&pFarmFrame->environmentPage, pEnvironment, sizeof(sScsiEnvironmentStat));
                        offset += (pEnvironment->pPageHeader.plen + sizeof(sScsiPageParameter)); 
                    }
                    break;
                   
                case RELIABILITY_STATISTICS_PARAMETER:    
                    {
                        sScsiReliabilityStat *pReli;                                              // get the Reliabliity stat
                        pReli =  (sScsiReliabilityStat *)&pBuf[offset];
                        swap_Bytes_sScsiReliabilityStat(pReli);
                        memcpy((sScsiReliabilityStat *)&pFarmFrame->reliPage, pReli, sizeof(sScsiReliabilityStat));
                        offset += (pReli->pPageHeader.plen + sizeof(sScsiPageParameter));
                    }
                    break; 
                    
                case DISC_SLIP_IN_MICRO_INCHES_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->discSlipPerHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->bitErrorRateByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case DOS_WRITE_REFRESH_COUNT:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->dosWriteRefreshCountByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case DVGA_SKIP_WRITE_DETECT_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->dvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter)); 
                        delete pHeadInfo;  
                    }
                    break;
                case RVGA_SKIP_WRITE_DETECT_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->rvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter)); 
                        delete pHeadInfo;  
                    }
                    break;        
                case FVGA_SKIP_WRITE_DETECT_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->fvgaSkipWriteDetectByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->skipWriteDectedThresholdExceededByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation(); 
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->acffSine1xValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->acffCosine1xValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->pztCalibrationValueByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation(); 
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->mrHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:  
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->numberOfTMDByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->velocityObserverByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->numberOfVelocityObservedByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:  
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2SATPercentagedbyHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation(); 
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STAmplituedByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STAsymmetryByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case NUMBER_OF_RESIDENT_GLIST_ENTRIES:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->ResidentGlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case     NUMBER_OF_PENDING_ENTRIES:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->ResidentPlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter)); 
                        delete pHeadInfo;  
                    }
                    break;
                case    DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->DOSOoughtToScan, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case   DOS_NEED_TO_SCAN_COUNT_PER_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation(); 
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->DOSNeedToScan, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case   DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->DOSWriteFaultScan, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case  WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->writePowerOnHours, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case DOS_WRITE_COUNT_THRESHOLD_PER_HEAD:     
                    {
                         sHeadInformation *pHeadInfo = new sHeadInformation();
                         get_Head_Info(pHeadInfo, &pBuf[offset]);
                         memcpy(&pFarmFrame->dosWriteCount, pHeadInfo, sizeof(*pHeadInfo));
                         offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                         delete pHeadInfo; 
                    }
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:    
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter)); 
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STIterationsByHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:  
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STIterationsByHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation(); 
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->currentH2STIterationsByHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:   
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->appliedFlyHeightByHeadOuter, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;  
                    }
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:    
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->appliedFlyHeightByHeadInner, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;
                    }
                    break;
                case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:     
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->appliedFlyHeightByHeadMiddle, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo; 
                    }
                    break; 
                case SECOND_MR_HEAD_RESISTANCE:
                    {
                        sHeadInformation *pHeadInfo = new sHeadInformation();
                        get_Head_Info(pHeadInfo, &pBuf[offset]);
                        memcpy(&pFarmFrame->secondMRHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                        offset += (pHeadInfo->pageHeader.plen + sizeof(sScsiPageParameter));
                        delete pHeadInfo;
                    }
                    break;
                case LUN_0_ACTUATOR:
                    {
                        sLUNStruct *pLUNInfo = new sLUNStruct;
                        memcpy(pLUNInfo,&pBuf[offset],sizeof(sLUNStruct));
                        //pLUNInfo = (sLUNStruct *)&pBuf[offset];
                        swap_Bytes_sLUNStruct(pLUNInfo);
                        pFarmFrame->vLUN.push_back(*pLUNInfo);
                        offset += (pLUNInfo->pageHeader.plen + sizeof(sLUNStruct));

                        delete pLUNInfo;
                    }
                break;
                case LUN_1_ACTUATOR:
                    {
                        sLUNStruct *pLUNInfo = new sLUNStruct();
                        memcpy(pLUNInfo, &pBuf[offset], sizeof(sLUNStruct));
                        //pLUNInfo = (sLUNStruct *)&pBuf[offset];
                        swap_Bytes_sLUNStruct(pLUNInfo);
                        pFarmFrame->vLUN.push_back(*pLUNInfo);
                        offset += (pLUNInfo->pageHeader.plen + sizeof(sLUNStruct));
                        delete pLUNInfo;
                    }
                break;
                case LUN_2_ACTUATOR:
                    {
                        sLUNStruct *pLUNInfo = new sLUNStruct();
                        memcpy(pLUNInfo, &pBuf[offset], sizeof(sLUNStruct));
                        //pLUNInfo = (sLUNStruct *)&pBuf[offset];;
                        swap_Bytes_sLUNStruct(pLUNInfo);
                        pFarmFrame->vLUN.push_back(*pLUNInfo);
                        offset += (pLUNInfo->pageHeader.plen + sizeof(sLUNStruct));
                        delete pLUNInfo;
                    }
                break;
                case LUN_3_ACTUATOR:
                    {
                        sLUNStruct *pLUNInfo = new sLUNStruct();
                        memcpy(pLUNInfo, &pBuf[offset], sizeof(sLUNStruct));
                        //pLUNInfo = (sLUNStruct *)&pBuf[offset];
                        swap_Bytes_sLUNStruct(pLUNInfo);
                        pFarmFrame->vLUN.push_back(*pLUNInfo);
                        offset += (pLUNInfo->pageHeader.plen + sizeof(sLUNStruct));
                        delete pLUNInfo;
                    }
                    break;
                default:
                    offset += m_logSize;
                    break;
               }  
            }
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
        }
	    delete pFarmFrame;	
        return SUCCESS;
    }
    delete pFarmFrame;
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
	uint32_t page = 0;
    std::string myStr = "";
    JSONNODE *pageInfo = json_new(JSON_NODE);
    //sScsiFarmHeader *header = (sScsiFarmHeader *)&pBuf[4];                                                                // pointer to the header to get the signature
#if defined( _DEBUG)
    printf("\tLog Signature   =  0x%" PRIX64" \n", vFarmFrame[page].farmHeader.farmHeader.signature );                                  //!< Log Signature = 0x00004641524D4552
    printf("\tMajor Revision =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.majorRev & 0x00FFFFFFFFFFFFFFLL);                                    //!< Log Major rev
    printf("\tMinor Revision =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.minorRev & 0x00FFFFFFFFFFFFFFLL);                                    //!< minor rev 
    printf("\tPages Supported =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.pagesSupported & 0x00FFFFFFFFFFFFFFLL);                             //!< number of pages supported
    printf("\tLog Size        =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.logSize & 0x00FFFFFFFFFFFFFFLL);                                    //!< log size in bytes
    printf("\tPage Size       =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.pageSize & 0x00FFFFFFFFFFFFFFLL);                                   //!< page size in bytes
    printf("\tHeads Supported =   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.headsSupported & 0x00FFFFFFFFFFFFFFLL);                             //!< Maximum Drive Heads Supported
    printf("\tNumber of Copies=   %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.copies & 0x00FFFFFFFFFFFFFLL);                                      //!< Number of Historical Copies
#endif
    json_set_name(pageInfo, "FARM Log Header");

    snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIX64"", check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.signature));
	json_push_back(pageInfo, json_new_a("Log Signature", (char*)myStr.c_str() ));
	json_push_back(pageInfo, json_new_i("Major Revision", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.majorRev))));
	json_push_back(pageInfo, json_new_i("Minor Revision", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.minorRev))));
	json_push_back(pageInfo, json_new_i("Pages Supported", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.pagesSupported))));
	json_push_back(pageInfo, json_new_i("Log Size", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.logSize))));
	json_push_back(pageInfo, json_new_i("Page Size", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.pageSize))));
	json_push_back(pageInfo, json_new_i("Heads Supported", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.headsSupported))));
	json_push_back(pageInfo, json_new_i("Number of Copies", static_cast<uint32_t>(check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.copies ))));

    json_push_back(masterData, pageInfo);

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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        printf("\nDrive Informatio From FACTORY page\n");
    }
    else
    {
        printf("\nDrive Information From Farm Log copy %d:\n", page);
    }
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
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved1 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved2 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\tPower on Hours:                           %" PRIu64" \n", vFarmFrame[page].driveInfo.poh & 0x00FFFFFFFFFFFFFFLL);											//!< Power-on Hour
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved3 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved4 & 0x00FFFFFFFFFFFFFFLL);									//!< reserved
    printf("\tHead Load Events:                         %" PRIu64" \n", vFarmFrame[page].driveInfo.headLoadEvents & 0x00FFFFFFFFFFFFFFLL);								//!< Head Load Events
    printf("\tPower Cycle count:                        %" PRIu64" \n", vFarmFrame[page].driveInfo.powerCycleCount & 0x00FFFFFFFFFFFFFFLL);								//!< Power Cycle Count
    printf("\tHardware Reset count:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.resetCount & 0x00FFFFFFFFFFFFFFLL);									//!< Hardware Reset Count
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved5 & 0x00FFFFFFFFFFFFFFLL);									//!< treserved
    printf("\tNVC Status @ power on:                    %" PRIu64" \n", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn & 0x00FFFFFFFFFFFFFFLL);							//!< NVC Status on Power-on
    printf("\tTime Available to save:                   %" PRIu64" \n", vFarmFrame[page].driveInfo.timeAvailable & 0x00FFFFFFFFFFFFFFLL);								//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    printf("\tTime of latest frame:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.firstTimeStamp & 0x00FFFFFFFFFFFFFFLL);								//!< Timestamp of first SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
    printf("\tTime of latest frame (milliseconds):      %" PRIu64" \n", vFarmFrame[page].driveInfo.lastTimeStamp & 0x00FFFFFFFFFFFFFFLL);								//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1

#endif
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "Drive Informatio From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "Drive Information From Farm Log copy %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.serialNumber.c_str());
	json_push_back(pageInfo, json_new_a("Serial Number", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.worldWideName.c_str());
	json_push_back(pageInfo, json_new_a("World Wide Name", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.firmwareRev.c_str());																//!< Firmware Revision [0:3]
	json_push_back(pageInfo, json_new_a("Firmware Rev", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    json_push_back(pageInfo, json_new_a("Device Interface", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "%llu", vFarmFrame[page].driveInfo.deviceCapacity & 0x00FFFFFFFFFFFFFFLL);
    set_json_string_With_Status(pageInfo, "Device Capacity in Sectors", (char*)myStr.c_str(), vFarmFrame[page].driveInfo.deviceCapacity, m_showStatusBits);
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

    json_push_back(masterData, pageInfo);
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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    if (vFarmFrame[page].workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        printf("\nWork Load From FACTORY page");
    }
    else
    {
        printf("\nWork Load From Farm Log copy %d: \n", page);
    }
    printf("\tRated Workload Percentaged:               %llu  \n", vFarmFrame[page].workLoadPage.workLoad.workloadPercentage & 0x00FFFFFFFFFFFFFFLL);         //!< rated Workload Percentage
	printf("\tTotal Number of Read Commands:            %llu  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCommands & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Read Commands
	printf("\tTotal Number of Write Commands:           %llu  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCommands & 0x00FFFFFFFFFFFFFFLL);         //!< Total Number of Write Commands
	printf("\tTotal Number of Random Read Cmds:         %llu \n", vFarmFrame[page].workLoadPage.workLoad.totalRandomReads & 0x00FFFFFFFFFFFFFFLL);           //!< Total Number of Random Read Commands
	printf("\tTotal Number of Random Write Cmds:        %llu  \n", vFarmFrame[page].workLoadPage.workLoad.totalRandomWrites & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Random Write Commands
    printf("\tTotal Number of Other Commands:           %llu  \n", vFarmFrame[page].workLoadPage.workLoad.totalNumberofOtherCMDS & 0x00FFFFFFFFFFFFFFLL);     //!< Total Number Of Other Commands
    printf("\tLogical Sectors Written:                  %llu  \n", vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten & 0x00FFFFFFFFFFFFFFLL);          //!< Logical Sectors Written
    printf("\tLogical Sectors Read:                     %llu \n", vFarmFrame[page].workLoadPage.workLoad.logicalSecRead & 0x00FFFFFFFFFFFFFFLL);             //!< Logical Sectors Read
    printf("\tTotal Read Commands by Radius:            %llu \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames1 & 0x00FFFFFFFFFFFFFFLL);             //!< Total Read Commands by Radius from last 3 SMART Summary Frames 
    printf("\tTotal Write Commands by Radius:           %llu \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames1 & 0x00FFFFFFFFFFFFFFLL);             //!< Total Write Commands by Radius from last 3 SMART Summary Frames 
#endif
    if (vFarmFrame[page].workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "Work Load From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "Work Load From Farm Log copy %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());
    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentaged",vFarmFrame[page].workLoadPage.workLoad.workloadPercentage, false, m_showStatusBits);				//!< rated Workload Percentage
	set_json_64_bit_With_Status(pageInfo, "Total Number of Read Commands", vFarmFrame[page].workLoadPage.workLoad.totalReadCommands, false, m_showStatusBits);			//!< Total Number of Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Write Commands", vFarmFrame[page].workLoadPage.workLoad.totalWriteCommands, false, m_showStatusBits);			//!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Number of Random Read Cmds", vFarmFrame[page].workLoadPage.workLoad.totalRandomReads, false, m_showStatusBits);			//!< Total Number of Random Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Random Write Cmds", vFarmFrame[page].workLoadPage.workLoad.totalRandomWrites, false, m_showStatusBits);		//!< Total Number of Random Write Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Other Commands", vFarmFrame[page].workLoadPage.workLoad.totalNumberofOtherCMDS, false, m_showStatusBits);		//!< Total Number Of Other Commands
    snprintf((char*)myStr.c_str(), BASIC, "%llu", vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten & 0x00FFFFFFFFFFFFFFLL);
    set_json_string_With_Status(pageInfo, "Logical Sectors Written", (char*)myStr.c_str(), vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten, m_showStatusBits);					//!< Logical Sectors Written
    snprintf((char*)myStr.c_str(), BASIC, "%llu", vFarmFrame[page].workLoadPage.workLoad.logicalSecRead & 0x00FFFFFFFFFFFFFFLL);
    set_json_string_With_Status(pageInfo, "Logical Sectors Read", (char*)myStr.c_str(), vFarmFrame[page].workLoadPage.workLoad.logicalSecRead, m_showStatusBits);						//!< Logical Sectors Read
    set_json_64_bit_With_Status(pageInfo, "Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames1, false, m_showStatusBits);		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames2, false, m_showStatusBits);		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
    set_json_64_bit_With_Status(pageInfo, "Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames1, false, m_showStatusBits);	//!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames2, false, m_showStatusBits);	//!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
    set_json_64_bit_With_Status(pageInfo, "Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames 

    json_push_back(masterData, pageInfo);

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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    if (vFarmFrame[page].workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        printf("\nError Information From FACTORY page \n");
    }
    else
    {
        printf("\nError Information Log From Farm Log copy %d: \n", page);
    }
    printf("\tUnrecoverable Read Errors:                %llu \n", vFarmFrame[page].errorPage.totalReadECC & 0x00FFFFFFFFFFFFFFLL);				//!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors:               %llu \n", vFarmFrame[page].errorPage.totalWriteECC & 0x00FFFFFFFFFFFFFFLL);				//!< Number of Unrecoverable Write Errors
    printf("\tNumber of Reallocated Sectors:            %llu \n", vFarmFrame[page].errorPage.totalReallocations & 0x00FFFFFFFFFFFFFFLL);			//!< Number of Reallocated Sectors
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tNumber of Mechanical Start Failures:      %llu \n", vFarmFrame[page].errorPage.totalMechanicalFails & 0x00FFFFFFFFFFFFFFLL);		//!< Number of Mechanical Start Failures
    printf("\tNumber of Reallocated Candidate Sectors:  %llu \n", vFarmFrame[page].errorPage.totalReallocatedCanidates & 0x00FFFFFFFFFFFFFFLL);	//!< Number of Reallocated Candidate Sectors
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved1 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved2 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved3 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved4 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved5 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tNumber of IOEDC Errors (Raw):             %llu \n", vFarmFrame[page].errorPage.attrIOEDCErrors & 0x00FFFFFFFFFFFFFFLL);				//!< Number of IOEDC Errors 
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved6 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved7 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved8 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tTotal Flash LED (Assert) Events:          %llu \n", vFarmFrame[page].errorPage.totalFlashLED & 0x00FFFFFFFFFFFFFFLL);				//!< Total Flash LED (Assert) Events
    printf("\tReserved:                                 %llu \n", vFarmFrame[page].errorPage.reserved9 & 0x00FFFFFFFFFFFFFFLL);					//!< Reserved
    printf("\tFRU code if smart trip from most recent SMART Frame:%llu \n", vFarmFrame[page].errorPage.FRUCode & 0x00FFFFFFFFFFFFFFLL);
    printf("\tSuper Parity on the Fly Recovery          %llu \n", vFarmFrame[page].errorPage.parity & 0x00FFFFFFFFFFFFFFLL);
#endif
    if (vFarmFrame[page].errorPage.copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "Error Information From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "Error Information Log From Farm Log copy %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.totalReadECC, false, m_showStatusBits);							//!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.totalWriteECC, false, m_showStatusBits);							//!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors",vFarmFrame[page].errorPage.totalReallocations, false, m_showStatusBits);					//!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of Mechanical Start Failures",vFarmFrame[page].errorPage.totalMechanicalFails, false, m_showStatusBits);			//!< Number of Mechanical Start Failures
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors",vFarmFrame[page].errorPage.totalReallocatedCanidates, false, m_showStatusBits); //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of IOEDC Errors (Raw)",vFarmFrame[page].errorPage.attrIOEDCErrors, false, m_showStatusBits);						//!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events",vFarmFrame[page].errorPage.totalFlashLED, false, m_showStatusBits);						//!< Total Flash LED (Assert) Events
    set_json_64_bit_With_Status(pageInfo, "FRU code if smart trip from most recent SMART Frame", vFarmFrame[page].errorPage.FRUCode, false, m_showStatusBits);		//!< FRU code if smart trip from most recent SMART Frame
    set_json_64_bit_With_Status(pageInfo, "Super Parity on the Fly Recovery", vFarmFrame[page].errorPage.parity, false, m_showStatusBits);                      //!< Super Parity on the Fly Recovery

    
    json_push_back(masterData, pageInfo);

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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        printf( "Environment Information From FACTORY page \n");
    }
    else
    {
        printf("\nEnvironment Information From Farm Log copy %d: \n", page);
    }

    printf("\tCurrent Temperature:                      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.curentTemp)*.10));			        //!< Current Temperature in Celsius
    printf("\tHighest Temperature:                      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.highestTemp)*.10));			        //!< Highest Temperature in Celsius
    printf("\tLowest Temperature:                       %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.lowestTemp)*.10));			        //!< Lowest Temperature
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved1 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved2 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved3 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved4 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved5 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved6 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved7 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tSpecified Max Operating Temperature:      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.maxTemp) * 1.00));				    //!< Specified Max Operating Temperature
    printf("\tSpecified Min Operating Temperature:      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.minTemp) * 1.00));				    //!< Specified Min Operating Temperature
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved8 & 0x00FFFFFFFFFFFFFFLL);				//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved9 & 0x00FFFFFFFFFFFFFFLL);	            //!< Reserved
    printf("\tCurrent Relative Humidity:                %" PRId32".%" PRId32"   \n", M_DoubleWord1(vFarmFrame[page].environmentPage.humidity & 0x00FFFFFFFFFFFFFFLL), M_DoubleWord0(vFarmFrame[page].environmentPage.humidity & 0x00FFFFFFFFFFFFFFLL));		//!< Current Relative Humidity (in units of .1%)
    printf("\tHumidity Mixed Ratio:                     %0.02f     \n", static_cast<float>((vFarmFrame[page].environmentPage.humidityRatio & 0x00FFFFFFFFFFFFFFLL) / 8.0)); //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    printf("\tCurrent Motor Power:                      %" PRIu16" \n", (M_Word0(vFarmFrame[page].environmentPage.currentMotorPower)));		        //!< Current Motor Power, value from most recent SMART Summary Frame6
#endif
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "Environment Information From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "Environment Information From Farm Log copy %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.curentTemp))*.10));							//!< Current Temperature in Celsius
	set_json_string_With_Status(pageInfo, "Current Temperature (Celsius)", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.curentTemp, m_showStatusBits);
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestTemp))*.10));						//!< Highest Average Long Term Temperature
	set_json_string_With_Status(pageInfo, "Highest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.highestTemp, m_showStatusBits);		
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestTemp))*.10));							//!< Lowest Average Long Term Temperature
	set_json_string_With_Status(pageInfo, "Lowest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.lowestTemp, m_showStatusBits);			
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.maxTemp))*1.00));							//!< Specified Max Operating Temperature
	set_json_string_With_Status(pageInfo, "Specified Max Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.maxTemp, m_showStatusBits);					
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.minTemp))*1.00));							//!< Specified Min Operating Temperature
	set_json_string_With_Status(pageInfo, "Specified Min Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.minTemp, m_showStatusBits);					
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.humidity)*0.1));							//!< Current Relative Humidity (in units of .1%)
	set_json_string_With_Status(pageInfo, "Current Relative Humidity", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidity, m_showStatusBits);
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.humidityRatio) / 8.0));						//!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
	set_json_string_With_Status(pageInfo, "Humidity Mixed Ratio", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidityRatio, m_showStatusBits);
    snprintf((char*)myStr.c_str(), BASIC, "%" PRIu16"", M_Word0(vFarmFrame[page].environmentPage.currentMotorPower));
    set_json_string_With_Status(pageInfo, "Current Motor Power", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.currentMotorPower, m_showStatusBits);					    //!< Current Motor Power, value from most recent SMART Summary Frame6
	
    json_push_back(masterData, pageInfo);

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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        printf( "Reliability Information From FACTORY page");
    }
    else
    {
        printf("\nReliability Information From Farm Log copy: %d\n", page);
    }
    printf("\tTimestamp of last IDD test:               %" PRIu64" \n", vFarmFrame[page].reliPage.lastIDDTest & 0x00FFFFFFFFFFFFFFLL);                     //!< Timestamp of last IDD test
    printf("\tSub-command of last IDD test:             %" PRIu64" \n", vFarmFrame[page].reliPage.cmdLastIDDTest & 0x00FFFFFFFFFFFFFFLL);                  //!< Sub-command of last IDD test
    printf("\tNumber of Reclamations Sectors:           %" PRIu64" \n", vFarmFrame[page].reliPage.gListReclamed & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of G-List Reclamations 
    printf("\tServo Status:                             %" PRIu64" \n", vFarmFrame[page].reliPage.servoStatus & 0x00FFFFFFFFFFFFFFLL);                     //!< Servo Status (follows standard DST error code definitions)
    printf("\tNumber of Slipped Secotrs Before IDD Scan:%" PRIu64" \n", vFarmFrame[page].reliPage.altsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Alt List Entries Before IDD Scan
    printf("\tNumber of Slipped Secotrs After IDD Scan: %" PRIu64" \n", vFarmFrame[page].reliPage.altsAfterIDD & 0x00FFFFFFFFFFFFFFLL);                    //!< Number of Alt List Entries After IDD Scan
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
    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "Reliability Information From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "Reliability Information From Farm Log copy: %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());
 
    set_json_64_bit_With_Status(pageInfo, "Timestamp of last IDD test", vFarmFrame[page].reliPage.lastIDDTest, false, m_showStatusBits);							//!< Timestamp of last IDD test
    set_json_64_bit_With_Status(pageInfo, "Sub-command of last IDD test", vFarmFrame[page].reliPage.cmdLastIDDTest, false, m_showStatusBits);						//!< Sub-command of last IDD test
    set_json_64_bit_With_Status(pageInfo, "Number of Reclamations Sectors", vFarmFrame[page].reliPage.gListReclamed, false, m_showStatusBits);						//!< Number of G-List Reclamations 
    set_json_64_bit_With_Status(pageInfo, "Servo Status", vFarmFrame[page].reliPage.servoStatus, false, m_showStatusBits);											//!< Servo Status (follows standard DST error code definitions)
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Secotrs Before IDD Scan", vFarmFrame[page].reliPage.altsBeforeIDD, false, m_showStatusBits);					//!< Number of Alt List Entries Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Secotrs After IDD Scan", vFarmFrame[page].reliPage.altsAfterIDD, false, m_showStatusBits);						//!< Number of Alt List Entries After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors Before IDD Scan Before IDD Scan", vFarmFrame[page].reliPage.gListBeforIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors Before IDD Scans After IDD Scan", vFarmFrame[page].reliPage.gListAfterIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries After IDD Scan
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

    json_push_back(masterData, pageInfo);

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
	std::string myStr = " ";
	myStr.resize(BASIC);

    std::string myHeader = " ";
    myHeader.resize(BASIC);
    if (type == NULL)
    {
        return FAILURE;
    }
    if (type != RESERVED_FOR_FUTURE_EXPANSION_10 && type != RESERVED_FOR_FUTURE_EXPANSION_11 && type != RESERVED_FOR_FUTURE_EXPANSION_12 &&
        type != RESERVED_FOR_FUTURE_EXPANSION_13 && type != RESERVED_FOR_FUTURE_EXPANSION_14 && type != RESERVED_FOR_FUTURE_EXPANSION_15 &&
        type != RESERVED_FOR_FUTURE_EXPANSION_16 && type != RESERVED_FOR_FUTURE_EXPANSION_17 && type != RESERVED_FOR_FUTURE_EXPANSION_18 &&
        type != RESERVED_FOR_FUTURE_EXPANSION_19 && type != RESERVED_FOR_FUTURE_EXPANSION)
    {
        set_Head_Header(myHeader, type);
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
                uint64_t dsHead = check_Status_Strip_Status(vFarmFrame[page].discSlipPerHead.headValue[loopCount]);
                int16_t whole = M_WordInt2(dsHead);							// get 5:4 whole part of the float
                double decimal = static_cast<double>(M_DoubleWordInt0(dsHead));                   // get 3:0 for the Deciaml Part of the float
#if defined( _DEBUG)
                printf("\tDisc Slip in micro-inches by Head by Head %" PRIu32":  raw 0x%" PRIx64", calculated %" PRIi16".%04.0f (debug) \n" \
                    , loopCount, vFarmFrame[page].discSlipPerHead.headValue[loopCount], whole, decimal);  //!< Disc Slip in micro-inches
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Disc Slip in micro-inches Head %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%" PRIi16".%04.0f", whole,decimal);                                      //!< Disc Slip in micro-inches by Head
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].discSlipPerHead.headValue[loopCount], m_showStatusBits);
            }
            break;
        case BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {

                uint64_t beHead = check_Status_Strip_Status(vFarmFrame[page].bitErrorRateByHead.headValue[loopCount]);
                int16_t whole = M_WordInt2(beHead);							// get 5:4 whole part of the float
                double decimal = static_cast<double>(M_DoubleWordInt0(beHead));                     // get 3:0 for the Deciaml Part of the float
#if defined( _DEBUG)
                printf("\tBit Error Rate of Zone 0 by Head %" PRIu32":  raw 0x%" PRIx64", %" PRIi16".%04.0f \n", loopCount, vFarmFrame[page].bitErrorRateByHead.headValue[loopCount], whole, decimal);  //!< Bit Error Rate of Zone 0 by Drive Head
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Bit Error Rate of Zone 0 Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%" PRIi16".%04.0f", whole, decimal);                                     //!< Bit Error Rate of Zone 0 by Drive Head
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].bitErrorRateByHead.headValue[loopCount], m_showStatusBits);
            }
            break;
        case DOS_WRITE_REFRESH_COUNT:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDOS Write Refresh Count by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].dosWriteRefreshCountByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< DOS Write Refresh Count
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "DOS Write Refresh Count Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].dosWriteRefreshCountByHead.headValue[loopCount], false, m_showStatusBits);  //!< DOS Write Refresh Count
            }
            break;
        case DVGA_SKIP_WRITE_DETECT_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDVGA Skip Write 0 by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].dvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< DVGA Skip Write
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "VGA Skip Write Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].dvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< DVGA Skip Write
            }
            break;
        case RVGA_SKIP_WRITE_DETECT_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tRVGA Skip Write 0 by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].rvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< RVGA Skip Write
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "RVGA Skip Writ Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].rvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< RVGA Skip Write
            }
            break;
        case FVGA_SKIP_WRITE_DETECT_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tFVGA Skip Write 0 by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].fvgaSkipWriteDetectByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< FVGA Skip Write 
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "FVGA Skip Writ Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].fvgaSkipWriteDetectByHead.headValue[loopCount], false, m_showStatusBits); //!< FVGA Skip Write 
            }
            break;
        case SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tSkip Write Detect Threshold Exceeded Count by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].skipWriteDectedThresholdExceededByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Skip Write Detect Threshold Exceeded Count
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Skip Write Detect Threshold Exceeded Count Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].skipWriteDectedThresholdExceededByHead.headValue[loopCount], false, m_showStatusBits);  //!< Skip Write Detect Threshold Exceeded Count
            }
            break;
        case ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tACFF Sine 1X, value from most recent SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].acffSine1xValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< ACFF Sine 1X, value from most recent SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "ACFF Sine 1X value from most recent SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%" PRIi8"", static_cast<int8_t>(check_for_signed_int(M_Byte0(check_Status_Strip_Status(vFarmFrame[page].acffSine1xValueByHead.headValue[loopCount])), 8)));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].acffSine1xValueByHead.headValue[loopCount], m_showStatusBits);  //!< ACFF Sine 1X, value from most recent SMART Summary Frame
            }
            break;
        case ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tACFF Cosine 1X, value from most recent SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].acffCosine1xValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< ACFF Cosine 1X, value from most recent SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "ACFF Cosine 1X value from most recent SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%" PRIi8"", (static_cast<int8_t>(check_for_signed_int(M_Byte0(check_Status_Strip_Status(vFarmFrame[page].acffCosine1xValueByHead.headValue[loopCount])), 8)) * 16));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].acffCosine1xValueByHead.headValue[loopCount], m_showStatusBits);  //!< ACFF Cosine 1X, value from most recent SMART Summary Frame
            }
            break;
        case PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tPZT Calibration, value from most recent SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].pztCalibrationValueByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< PZT Calibration, value from most recent SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "PZT Calibration value from most recent SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].pztCalibrationValueByHead.headValue[loopCount], false, m_showStatusBits);  //!< PZT Calibration, value from most recent SMART SummaryFrame
            }
            break;
        case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tMR Head Resistance from most recent SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< MR Head Resistance from most recent SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "MR Head Resistanc from most recent SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount], false, m_showStatusBits);  //!< MR Head Resistance from most recent SMART Summary Frame
            }
            break;
        case NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tNumber of TMD over last 3 SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].numberOfTMDByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Number of TMD over last 3 SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Number of TMD over last 3 SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].numberOfTMDByHead.headValue[loopCount], false, m_showStatusBits);  //!< Number of TMD over last 3 SMART Summary Frame
            }
            break;
        case VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tVelocity Observer over last 3 SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].velocityObserverByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Velocity Observer over last 3 SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Velocity Observer over last 3 SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].velocityObserverByHead.headValue[loopCount], false, m_showStatusBits); //!< Velocity Observer over last 3 SMART Summary Frame
            }
            break;
        case NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tNumber of Velocity Observer over last 3 SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].numberOfVelocityObservedByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Number of Velocity Observer over last 3 SMART Summary Frame
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Number of Velocity Observer over last 3 SMART Summary Frame Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].numberOfVelocityObservedByHead.headValue[loopCount], false, m_showStatusBits); //!< Number of Velocity Observer over last 3 SMART Summary Frame
            }
            break;
        case CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT percentage of codewords at iteration level by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2SATPercentagedbyHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT percentage of codewords at iteration level
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT percentage of codewords at iteration level Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2SATPercentagedbyHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT percentage of codewords at iteration level
            }
            break;
        case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT amplitude, averaged across Test Zone by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT amplitude, averaged across Test Zone
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT amplitude Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT amplitude, averaged across Test Zone
            }
            break;
        case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT asymmetry, averaged across Test Zone by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT asymmetry, averaged across Test Zone
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT asymmetry Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT asymmetry, averaged across Test Zone
            }
            break;
        case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tResident Glist Entries by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentGlistEntries.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Resident Glist Entries Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].ResidentGlistEntries.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case NUMBER_OF_PENDING_ENTRIES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tResident Pending list Entries by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentPlistEntries.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Resident Pending list Entries Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].ResidentPlistEntries.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDOS Oought To Scan Count by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSOoughtToScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "DOS Oought To Scan Count Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSOoughtToScan.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case DOS_NEED_TO_SCAN_COUNT_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDOS Needs To Scan Count by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSNeedToScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "DOS Needs To Scan Count Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSNeedToScan.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDOS Write Fault Scan Count by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].DOSWriteFaultScan.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "DOS Write Fault Scan Count Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].DOSWriteFaultScan.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tWrite Power On Hours from most recent SMART by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].writePowerOnHours.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Write Power On Hour Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].writePowerOnHours.headValue[loopCount], false, m_showStatusBits);
                //snprintf((char*)myStr.c_str(), BASIC, "%0.04f", (check_for_signed_int(M_Word0(check_Status_Strip_Status(vFarmFrame[page].writePowerOnHours.headValue[loopCount])), 16) /216000));
                //set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].writePowerOnHours.headValue[loopCount], m_showStatusBits);
            }
            break;
        case DOS_WRITE_COUNT_THRESHOLD_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tDOS Write Count Threshold per head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].dosWriteCount.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "DOS Write Count Threshold Head number %" PRIu32"", loopCount); // Head count
                set_json_64_bit_With_Status(headPage, (char*)myHeader.c_str(), vFarmFrame[page].dosWriteCount.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32":  by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount], m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
            }
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32": by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount], m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
            }
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32" , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount], m_showStatusBits);//!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 0 Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 1 Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge Test Zone 2 Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount])*.10));
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount], m_showStatusBits); //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
            }
            break;
        case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tApplied fly height clearance delta per head: Outer by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadOuter.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Outer by Head
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head: Outer Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].appliedFlyHeightByHeadOuter.headValue[loopCount])*.1));   //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Outer by Head
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].appliedFlyHeightByHeadOuter.headValue[loopCount], m_showStatusBits);
            }
            break;
        case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tApplied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadInner.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head: Inner Head number %" PRIu32"", loopCount); // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].appliedFlyHeightByHeadInner.headValue[loopCount])*.1));   //!< Applied fly height clearance delta per head in thousandths of one Angstrom: Inner by Head
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].appliedFlyHeightByHeadInner.headValue[loopCount], m_showStatusBits);
            }
            break;
        case APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tApplied fly height clearance delta per head: Middle by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].appliedFlyHeightByHeadMiddle.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Applied fly height clearance delta per head in thousandths of one Angstrom: middle by Head
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Applied fly height clearance delta per head: Middle Head number %" PRIu32"", loopCount);     // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].appliedFlyHeightByHeadMiddle.headValue[loopCount]) *.1));   //!< Applied fly height clearance delta per head in thousandths of one Angstrom:middle by Head
                set_json_string_With_Status(headPage, (char*)myHeader.c_str(), (char*)myStr.c_str(), vFarmFrame[page].appliedFlyHeightByHeadMiddle.headValue[loopCount], m_showStatusBits);
            }
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
#if defined( _DEBUG)
            printf("\tSomething went wrong, ERROR \n");
#endif
            return FAILURE;
            break;
        case SECOND_MR_HEAD_RESISTANCE:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined( _DEBUG)
                printf("\tSecond MR Head Resistance from most recent SMART Summary Frame by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].secondMRHeadResistanceByHead.headValue[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< Second MR Head Resistance
#endif
                snprintf((char*)myHeader.c_str(), BASIC, "Second MR Head Resistance from most recent SMART Summary Frame by Head %" PRIu32"", loopCount);     // Head count
                snprintf((char*)myStr.c_str(), BASIC, "%0.02f", static_cast<float>(M_WordInt0(vFarmFrame[page].secondMRHeadResistanceByHead.headValue[loopCount]) *.1));   //!< Second MR Head Resistance
            }
            break;
        case RESERVED_FOR_FUTURE_EXPANSION_20:
        case RESERVED_FOR_FUTURE_EXPANSION_21:
        case RESERVED_FOR_FUTURE_EXPANSION_22:
        case RESERVED_FOR_FUTURE_EXPANSION_23:
        case RESERVED_FOR_FUTURE_EXPANSION_24:
        case RESERVED_FOR_FUTURE_EXPANSION_25:
        case RESERVED_FOR_FUTURE_EXPANSION_26:
        case RESERVED_FOR_FUTURE_EXPANSION_27:
        case RESERVED_FOR_FUTURE_EXPANSION_28:
        case RESERVED_FOR_FUTURE_EXPANSION_29:
        case RESERVED_FOR_FUTURE_EXPANSION_30:
        case RESERVED_FOR_FUTURE_EXPANSION_31:
        case LUN_0_ACTUATOR:
        case RESERVED_FOR_FUTURE_EXPANSION_40:
        case RESERVED_FOR_FUTURE_EXPANSION_41:
        case RESERVED_FOR_FUTURE_EXPANSION_42:
        case RESERVED_FOR_FUTURE_EXPANSION_43:
        case RESERVED_FOR_FUTURE_EXPANSION_44:
        case RESERVED_FOR_FUTURE_EXPANSION_45:
        case RESERVED_FOR_FUTURE_EXPANSION_46:
        case RESERVED_FOR_FUTURE_EXPANSION_47:
        case RESERVED_FOR_FUTURE_EXPANSION_48:
        case RESERVED_FOR_FUTURE_EXPANSION_49:
        case RESERVED_FOR_FUTURE_EXPANSION_50:
        case RESERVED_FOR_FUTURE_EXPANSION_51:
        case RESERVED_FOR_FUTURE_EXPANSION_52:
        case RESERVED_FOR_FUTURE_EXPANSION_53:
        case RESERVED_FOR_FUTURE_EXPANSION_54:
        case LUN_1_ACTUATOR:
        case RESERVED_FOR_FUTURE_EXPANSION_60:
        case RESERVED_FOR_FUTURE_EXPANSION_61:
        case RESERVED_FOR_FUTURE_EXPANSION_62:
        case RESERVED_FOR_FUTURE_EXPANSION_63:
        case RESERVED_FOR_FUTURE_EXPANSION_64:
        case RESERVED_FOR_FUTURE_EXPANSION_65:
        case RESERVED_FOR_FUTURE_EXPANSION_66:
        case RESERVED_FOR_FUTURE_EXPANSION_67:
        case RESERVED_FOR_FUTURE_EXPANSION_68:
        case RESERVED_FOR_FUTURE_EXPANSION_69:
        case RESERVED_FOR_FUTURE_EXPANSION_70:
        case RESERVED_FOR_FUTURE_EXPANSION_71:
        case RESERVED_FOR_FUTURE_EXPANSION_72:
        case RESERVED_FOR_FUTURE_EXPANSION_73:
        case RESERVED_FOR_FUTURE_EXPANSION_74:
        case LUN_2_ACTUATOR:
        case RESERVED_FOR_FUTURE_EXPANSION_80:
        case RESERVED_FOR_FUTURE_EXPANSION_81:
        case RESERVED_FOR_FUTURE_EXPANSION_82:
        case RESERVED_FOR_FUTURE_EXPANSION_83:
        case RESERVED_FOR_FUTURE_EXPANSION_84:
        case RESERVED_FOR_FUTURE_EXPANSION_85:
        case RESERVED_FOR_FUTURE_EXPANSION_86:
        case RESERVED_FOR_FUTURE_EXPANSION_87:
        case RESERVED_FOR_FUTURE_EXPANSION_88:
        case RESERVED_FOR_FUTURE_EXPANSION_89:
        case RESERVED_FOR_FUTURE_EXPANSION_90:
        case RESERVED_FOR_FUTURE_EXPANSION_91:
        case RESERVED_FOR_FUTURE_EXPANSION_92:
        case RESERVED_FOR_FUTURE_EXPANSION_93:
        case RESERVED_FOR_FUTURE_EXPANSION_94:
        case LUN_3_ACTUATOR:
        case RESERVED_FOR_FUTURE_EXPANSION_100:
        case RESERVED_FOR_FUTURE_EXPANSION_101:
        case RESERVED_FOR_FUTURE_EXPANSION_102:
        case RESERVED_FOR_FUTURE_EXPANSION_103:
        case RESERVED_FOR_FUTURE_EXPANSION_104:
        case RESERVED_FOR_FUTURE_EXPANSION_105:
        case RESERVED_FOR_FUTURE_EXPANSION_106:
        case RESERVED_FOR_FUTURE_EXPANSION_107:
        case RESERVED_FOR_FUTURE_EXPANSION_108:
        case RESERVED_FOR_FUTURE_EXPANSION_109:
        case RESERVED_FOR_FUTURE_EXPANSION_110:
        case RESERVED_FOR_FUTURE_EXPANSION_111:
        case RESERVED_FOR_FUTURE_EXPANSION_112:
        case RESERVED_FOR_FUTURE_EXPANSION_113:
        case RESERVED_FOR_FUTURE_EXPANSION_114:
        case RESERVED_FOR_FUTURE_EXPANSION:
            break;
        default:
#if defined( _DEBUG)
            printf("\tSomething went wrong, ERROR \n");
#endif
            return FAILURE;
            break;
        }

        json_push_back(masterData, headPage);
        return SUCCESS;
    }

    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_LUN_Acutator_Information()
//
//! \brief
//!   Description:  print out the LUN Acutator information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_Information(JSONNODE *masterData, uint32_t page, uint32_t index)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *pageInfo = json_new(JSON_NODE);

    sLUNStruct *pLUN;
    pLUN = &vFarmFrame[page].vLUN.at(index);

#if defined( _DEBUG)
    if (pLUN->copyNumber == FACTORYCOPY)
    {
        printf("LUN Actuator Information From FACTORY page");
    }
    else
    {
        printf("\nLUN Actuator Information From Farm Log copy: %" PRIu32"\n", page);
    }
    printf("\tPage Number:                                  %" PRIu64" \n", pLUN->pageNumber & 0x00FFFFFFFFFFFFFFLL);                   //!< Page Number 
    printf("\tCopy Number:                                  %" PRIu64" \n", pLUN->copyNumber & 0x00FFFFFFFFFFFFFFLL);                   //!< Copy Number 
    printf("\tLUN ID:                                       %" PRIu64" \n", pLUN->LUNID & 0x00FFFFFFFFFFFFFFLL);                        //!< LUN ID  
    printf("\tHead Load Events:                             %" PRIu64" \n", pLUN->headLoadEvents & 0x00FFFFFFFFFFFFFFLL);               //!< Head Load Events 
    printf("\tNumber of Reallocated Sectors:                %" PRIu64" \n", pLUN->reallocatedSectors & 0x00FFFFFFFFFFFFFFLL);           //!< Number of Reallocated Sectors 
    printf("\tNumber of Reallocated Candidate Sectors:      %" PRIu64" \n", pLUN->reallocatedCandidates & 0x00FFFFFFFFFFFFFFLL);        //!< Number of Reallocated Candidate Sectors 
    printf("\tTimeStamp of last IDD test:                   %" PRIu64" \n", pLUN->timeStampOfIDD & 0x00FFFFFFFFFFFFFFLL);               //!< Timestamp of last IDD test 
    printf("\tSub-Command of Last IDD Test:                 %" PRIu64" \n", pLUN->subCmdOfIDD & 0x00FFFFFFFFFFFFFFLL);                  //!< Sub-command of last IDD test 
    printf("\tNumber of G-List reclamations:                %" PRIu64" \n", pLUN->reclamedGlist & 0x00FFFFFFFFFFFFFFLL);				//!< Number of G-list reclamations 
    printf("\tServo Status:                                 %" PRIu64" \n", pLUN->servoStatus & 0x00FFFFFFFFFFFFFFLL);					//!< Servo Status 
    printf("\tNumber of Slipped Sectors Before IDD Scan:    %" PRIu64" \n", pLUN->slippedSectorsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);      //!< Number of Slipped Sectors Before IDD Scan 
    printf("\tNumber of Slipped Sectors After IDD Scan:     %" PRIu64" \n", pLUN->slippedSectorsAfterIDD & 0x00FFFFFFFFFFFFFFLL);       //!< Number of Slipped Sectors After IDD Scan 
    printf("\tResident Reallocated Sectors Before IDD Scan: %" PRIu64" \n", pLUN->residentReallocatedBeforeIDD & 0x00FFFFFFFFFFFFFFLL); //!< Number of Resident Reallocated Sectors Before IDD Scan 
    printf("\tResident Reallocated Sectors After IDD Scan:  %" PRIu64" \n", pLUN->residentReallocatedAfterIDD & 0x00FFFFFFFFFFFFFFLL);  //!< Number of Resident Reallocated Sectors After IDD Scan  
    printf("\tSuccessfully Scrubbed Sectors Before IDD Scan:%" PRIu64" \n", pLUN->successScrubbedBeforeIDD & 0x00FFFFFFFFFFFFFFLL);     //!< Number of Successfully Scrubbed Sectors Before IDD Scan 
    printf("\tSuccessfully Scrubbed Sectors After IDD Scan: %" PRIu64" \n", pLUN->successScrubbedAfterIDD & 0x00FFFFFFFFFFFFFFLL);      //!< Number of Successfully Scrubbed Sectors After IDD Scan 
    printf("\tNumber of DOS Scans Performed:                %" PRIu64" \n", pLUN->dosScansPerformed & 0x00FFFFFFFFFFFFFFLL);            //!< Number of DOS Scans Performed
    printf("\tNumber of LBAs Corrected by ISP:              %" PRIu64" \n", pLUN->correctedLBAbyISP & 0x00FFFFFFFFFFFFFFLL);            //!< Number of LBAs Corrected by ISP
    printf("\tNumber of Valid Parity Sectors:               %" PRIu64" \n", pLUN->paritySectors & 0x00FFFFFFFFFFFFFFLL);                //!< Number of Valid Parity Sectors
    printf("\tRV Absulute Mean:                             %" PRIu64" \n", pLUN->RVabsolue & 0x00FFFFFFFFFFFFFFLL);					//!< RV Absulute Mean
    printf("\tMax RV absulute Mean:                         %" PRIu64" \n", pLUN->maxRVabsolue & 0x00FFFFFFFFFFFFFFLL);				    //!< Max RV absulute Mean
    printf("\tIdle Time:                                    %" PRIu64" \n", pLUN->idleTime & 0x00FFFFFFFFFFFFFFLL);		                //!< idle Time value from the most recent SMART Summary Frame

#endif
    if (pLUN->copyNumber == FACTORYCOPY)
    {
        snprintf((char*)myStr.c_str(), BASIC, "LUN Actuator Information From FACTORY page");
    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "LUN Actuator Information From Farm Log copy: %" PRId32"", page);
    }
    json_set_name(pageInfo, (char*)myStr.c_str());

    set_json_64_bit_With_Status(pageInfo, "Page Number", pLUN->pageNumber, false, m_showStatusBits);							                        //!< Page Number 
    set_json_64_bit_With_Status(pageInfo, "Copy Number ", pLUN->copyNumber, false, m_showStatusBits);						                            //!< Copy Number 
    set_json_64_bit_With_Status(pageInfo, "LUN ID", pLUN->LUNID, false, m_showStatusBits);						                                        //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Head Load Events", pLUN->headLoadEvents, false, m_showStatusBits);											//!< Head Load Events 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", pLUN->reallocatedSectors, false, m_showStatusBits);					        //!< Number of Reallocated Sectors 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", pLUN->reallocatedSectors, false, m_showStatusBits);					        //!< Number of Reallocated Candidate Sectors  
    set_json_64_bit_With_Status(pageInfo, "TimeStamp of last IDD test", pLUN->timeStampOfIDD, false, m_showStatusBits);						            //!< Timestamp of last IDD test  
    set_json_64_bit_With_Status(pageInfo, "Sub-Command of Last IDD Test", pLUN->subCmdOfIDD, false, m_showStatusBits);				                    //!< Sub-command of last IDD test 
    set_json_64_bit_With_Status(pageInfo, "Number of G-List reclamations", pLUN->reclamedGlist, false, m_showStatusBits);				                //!< Number of G-list reclamations  
    set_json_64_bit_With_Status(pageInfo, "Servo Status", pLUN->servoStatus, false, m_showStatusBits);				                                    //!< Servo Status  
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors Before IDD Scan", pLUN->slippedSectorsBeforeIDD, false, m_showStatusBits);			//!< Number of Slipped Sectors Before IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors After IDD Scan", pLUN->slippedSectorsAfterIDD, false, m_showStatusBits);			//!< Number of Slipped Sectors After IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Resident Reallocated Sectors Before IDD Scan", pLUN->residentReallocatedBeforeIDD, false, m_showStatusBits);	//!< Number of Resident Reallocated Sectors Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Resident Reallocated Sectors After IDD Scan", pLUN->residentReallocatedAfterIDD, false, m_showStatusBits);	//!< Number of Resident Reallocated Sectors After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors Before IDD Scan", pLUN->successScrubbedBeforeIDD, false, m_showStatusBits);	//!< Number of Successfully Scrubbed Sectors Before IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors After IDD Scan", pLUN->successScrubbedAfterIDD, false, m_showStatusBits);		//!< Number of Successfully Scrubbed Sectors After IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", pLUN->dosScansPerformed, false, m_showStatusBits);	                        //!< Number of DOS Scans Performed 
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", pLUN->correctedLBAbyISP, false, m_showStatusBits);                         //!< Number of LBAs Corrected by ISP  
    set_json_64_bit_With_Status(pageInfo, "Number of Valid Parity Sectors", pLUN->paritySectors, false, m_showStatusBits);                              //!< Number of Valid Parity Sectors  
    set_json_64_bit_With_Status(pageInfo, "RV Absulute Mean", pLUN->RVabsolue, false, m_showStatusBits);									            //!< RV Absulute Mean
    set_json_64_bit_With_Status(pageInfo, "Max RV absulute Mean", pLUN->maxRVabsolue, false, m_showStatusBits);							                //!< Max RV absulute Mean
    set_json_64_bit_With_Status(pageInfo, "Idle Time", pLUN->idleTime, false, m_showStatusBits);	                                                    //!< idle Time value from the most recent SMART Summary Frame

    json_push_back(masterData, pageInfo);

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
    if (vFarmFrame.size() > 0)
    {
        print_Header(masterData);
        for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
        {

            for (uint32_t pramCode = 0; pramCode < vFarmFrame.at(index).vFramesFound.size(); pramCode++)
            {
                switch (vFarmFrame.at(index).vFramesFound.at(pramCode))
                {
                case FARM_HEADER_PARAMETER:
                    //get the Farm Header information;
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
                case SECOND_MR_HEAD_RESISTANCE:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), masterData, index);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_20:
                case RESERVED_FOR_FUTURE_EXPANSION_21:
                case RESERVED_FOR_FUTURE_EXPANSION_22:
                case RESERVED_FOR_FUTURE_EXPANSION_23:
                case RESERVED_FOR_FUTURE_EXPANSION_24:
                case RESERVED_FOR_FUTURE_EXPANSION_25:
                case RESERVED_FOR_FUTURE_EXPANSION_26:
                case RESERVED_FOR_FUTURE_EXPANSION_27:
                case RESERVED_FOR_FUTURE_EXPANSION_28:
                case RESERVED_FOR_FUTURE_EXPANSION_29:
                case RESERVED_FOR_FUTURE_EXPANSION_30:
                case RESERVED_FOR_FUTURE_EXPANSION_31:
                    break;
                case LUN_0_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, 0);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_40:
                case RESERVED_FOR_FUTURE_EXPANSION_41:
                case RESERVED_FOR_FUTURE_EXPANSION_42:
                case RESERVED_FOR_FUTURE_EXPANSION_43:
                case RESERVED_FOR_FUTURE_EXPANSION_44:
                case RESERVED_FOR_FUTURE_EXPANSION_45:
                case RESERVED_FOR_FUTURE_EXPANSION_46:
                case RESERVED_FOR_FUTURE_EXPANSION_47:
                case RESERVED_FOR_FUTURE_EXPANSION_48:
                case RESERVED_FOR_FUTURE_EXPANSION_49:
                case RESERVED_FOR_FUTURE_EXPANSION_50:
                case RESERVED_FOR_FUTURE_EXPANSION_51:
                case RESERVED_FOR_FUTURE_EXPANSION_52:
                case RESERVED_FOR_FUTURE_EXPANSION_53:
                case RESERVED_FOR_FUTURE_EXPANSION_54:
                    break;
                case LUN_1_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, 1);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_60:
                case RESERVED_FOR_FUTURE_EXPANSION_61:
                case RESERVED_FOR_FUTURE_EXPANSION_62:
                case RESERVED_FOR_FUTURE_EXPANSION_63:
                case RESERVED_FOR_FUTURE_EXPANSION_64:
                case RESERVED_FOR_FUTURE_EXPANSION_65:
                case RESERVED_FOR_FUTURE_EXPANSION_66:
                case RESERVED_FOR_FUTURE_EXPANSION_67:
                case RESERVED_FOR_FUTURE_EXPANSION_68:
                case RESERVED_FOR_FUTURE_EXPANSION_69:
                case RESERVED_FOR_FUTURE_EXPANSION_70:
                case RESERVED_FOR_FUTURE_EXPANSION_71:
                case RESERVED_FOR_FUTURE_EXPANSION_72:
                case RESERVED_FOR_FUTURE_EXPANSION_73:
                case RESERVED_FOR_FUTURE_EXPANSION_74:
                    break;
                case LUN_2_ACTUATOR:
                    print_LUN_Actuator_Information(masterData,index, 2);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_80:
                case RESERVED_FOR_FUTURE_EXPANSION_81:
                case RESERVED_FOR_FUTURE_EXPANSION_82:
                case RESERVED_FOR_FUTURE_EXPANSION_83:
                case RESERVED_FOR_FUTURE_EXPANSION_84:
                case RESERVED_FOR_FUTURE_EXPANSION_85:
                case RESERVED_FOR_FUTURE_EXPANSION_86:
                case RESERVED_FOR_FUTURE_EXPANSION_87:
                case RESERVED_FOR_FUTURE_EXPANSION_88:
                case RESERVED_FOR_FUTURE_EXPANSION_89:
                case RESERVED_FOR_FUTURE_EXPANSION_90:
                case RESERVED_FOR_FUTURE_EXPANSION_91:
                case RESERVED_FOR_FUTURE_EXPANSION_92:
                case RESERVED_FOR_FUTURE_EXPANSION_93:
                case RESERVED_FOR_FUTURE_EXPANSION_94:
                    break;
                case LUN_3_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index,3);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_100:
                case RESERVED_FOR_FUTURE_EXPANSION_101:
                case RESERVED_FOR_FUTURE_EXPANSION_102:
                case RESERVED_FOR_FUTURE_EXPANSION_103:
                case RESERVED_FOR_FUTURE_EXPANSION_104:
                case RESERVED_FOR_FUTURE_EXPANSION_105:
                case RESERVED_FOR_FUTURE_EXPANSION_106:
                case RESERVED_FOR_FUTURE_EXPANSION_107:
                case RESERVED_FOR_FUTURE_EXPANSION_108:
                case RESERVED_FOR_FUTURE_EXPANSION_109:
                case RESERVED_FOR_FUTURE_EXPANSION_110:
                case RESERVED_FOR_FUTURE_EXPANSION_111:
                case RESERVED_FOR_FUTURE_EXPANSION_112:
                case RESERVED_FOR_FUTURE_EXPANSION_113:
                case RESERVED_FOR_FUTURE_EXPANSION_114:
                case RESERVED_FOR_FUTURE_EXPANSION:
                    break;
                default:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), masterData, index);
                    break;
                }
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
    if (vFarmFrame.size() > 0)
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
}
//-----------------------------------------------------------------------------
//
//! \fn print_Page_One_Node()
//
//! \brief
//!   Description:  print out a copy of a the pages, all under one Node
//
//  Entry:
//! \param masterData  =  master data to the json node
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::print_Page_One_Node(JSONNODE *masterData)
{
    if (vFarmFrame.size() > 0)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo,"FARM Log");
        print_All_Pages(pageInfo);
        json_push_back(masterData, pageInfo);
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
    if (vFarmFrame.size() > 0)
    {
        if (page <= vFarmFrame.size())
        {
            print_WorkLoad(masterData, page);
            print_Error_Information(masterData, page);
            print_Enviroment_Information(masterData, page);
            print_Reli_Information(masterData, page);
            JSONNODE *headInfoPage = json_new(JSON_NODE);
            std::string myStr = " ";
            myStr.resize(BASIC);
            snprintf((char*)myStr.c_str(), BASIC, "Head Information From Farm Log copy: %" PRId32"", page);

            json_set_name(headInfoPage, (char*)myStr.c_str());
            for (uint32_t frame = 0; frame < vFarmFrame.at(page).vFramesFound.size(); frame++)
            {
                print_Head_Information(vFarmFrame.at(page).vFramesFound.at(frame), masterData, page);
            }
            json_push_back(masterData, headInfoPage);
        }
    }
}

