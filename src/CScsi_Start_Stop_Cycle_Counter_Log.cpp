//
// CScsi_Start_Stop_Counter_Log.cpp  Implementation of CScsi start stop cycle counter Log class
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Start_Stop_Counter_Log.cpp Implementation of CScsi start stop cycle counter Log class
//
#include "CScsi_Start_Stop_Cycle_Counter_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop()
//
//! \brief
//!   Description: Default Class constructor 
//
//  Entry:
// \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiStartStop::CScsiStartStop()
	: pData(NULL)
    , m_SSName("Start Stop Log")
	, m_StartStatus(IN_PROGRESS)
	, m_PageLength(0)
    , m_SubPage(0)
	, m_Page(0)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop()
//
//! \brief
//!   Description: Class constructor for the CScsiStartStop
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param masterData - the pointer to the json node that holds all of the json data
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiStartStop::CScsiStartStop(uint8_t * buffer, size_t bufferSize, JSONNODE *masterData)
	: pData(NULL)
    , m_SSName("Start Stop Log")
	, m_StartStatus(IN_PROGRESS)
	, m_PageLength(0)
    , m_SubPage(0)
	, m_Page(0)
{
	if (buffer != NULL)
	{
        pData = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef _WIN64
        memcpy(pData, buffer, bufferSize);
#else
        memcpy_s(pData, bufferSize, buffer, bufferSize);// copy the buffer data to the class member pBuf
#endif
        if (pData != NULL)
        {
            if (bufferSize >= sizeof(sStartStopStruct))				// check for invaid log size < need to add in the size of the log page header
            {
                m_Page = (sStartStopStruct *)pData;				// set a buffer to the point to the log page info
                m_StartStatus = parse_Start_Stop_Log(masterData);
            }
            else
            {
                m_StartStatus = static_cast<eReturnValues>(INVALID_LENGTH);
            }
        }
        else
        {
            m_StartStatus = MEMORY_FAILURE;
        }
	}
	else
	{
		m_StartStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiStartStop::~CScsiStartStop()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn parse_Start_Stop_Log
//
//! \brief
//!   Description:parse the start stop log into json data, Calling needed function to parse each parameter 
//
//  Entry:
//! \param masterData - the master json node pointer
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::parse_Start_Stop_Log(JSONNODE *masterData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	eReturnValues status = IN_PROGRESS;
	eReturnValues retStatus = IN_PROGRESS;
	JSONNODE *pageInfo = json_new(JSON_NODE);
	json_set_name(pageInfo, "Scsi Start Stop Cycle Counter Log - Eh");
	byte_Swap_16(&m_Page->manufatureParamCode);
	if (manufactureDate == m_Page->manufatureParamCode)
	{
		retStatus =  week_Year_Print(pageInfo, m_Page->manufatureParamCode, m_Page->paramLength1, m_Page->paramControlByte1, m_Page->year, m_Page->week, "Date of Manufacture", "Year of Manufacture", "Week of Manufacture");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	byte_Swap_16(&m_Page->accountParamCode);
	if (accountingDate == m_Page->accountParamCode)
	{
		retStatus = week_Year_Print(pageInfo, m_Page->accountParamCode, m_Page->paramLength2, m_Page->paramControlByte2, m_Page->accYear, m_Page->accWeek, "Accounting Data","Accounting Year", "Accounting Week");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	byte_Swap_16(&m_Page->specCycleParamCode);
	if (specLifetime == m_Page->specCycleParamCode)
	{
		retStatus = get_Count(pageInfo, m_Page->specCycleParamCode, m_Page->paramLength3, m_Page->paramControlByte3, m_Page->specLifeTime,"Specified Cycle Count", "Specified Cycle Count Over Device Lifetime");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	byte_Swap_16(&m_Page->AccumulatedParamCode);
	if (accumulated == m_Page->AccumulatedParamCode)
	{
		retStatus = get_Count(pageInfo, m_Page->AccumulatedParamCode, m_Page->paramLength4, m_Page->paramControlByte4, m_Page->accumulatedCycles, "Accumulated Start-Stop Count", "Accumulated Start-Stop Over Device Lifetime");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	byte_Swap_16(&m_Page->loadUnloadParamCode);
	if (loadUnload == m_Page->loadUnloadParamCode)
	{
		retStatus = get_Count(pageInfo, m_Page->loadUnloadParamCode, m_Page->paramLength5, m_Page->paramControlByte5, m_Page->loadUnloadCount, "Load - Unload Count", "Specified Load-Unload Count Over Device Lifetime");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	byte_Swap_16(&m_Page->accLoadUnloadParamCode);
	if (accumulatedLU == m_Page->accLoadUnloadParamCode)
	{
		retStatus = get_Count(pageInfo, m_Page->accLoadUnloadParamCode, m_Page->paramLength6, m_Page->paramControlByte6, m_Page->accloadUnloadCount, "Accumulated Load - Unload Count", "Specified Load-Unload Count Over Device Lifetime");
	}
	else
	{
		retStatus = BAD_PARAMETER;
	}
	json_push_back(masterData, pageInfo);

	if (retStatus == BAD_PARAMETER || retStatus ==  IN_PROGRESS)
	{
		status = BAD_PARAMETER;
	}
	else
	{
		status = SUCCESS;
	}
	return status;
}

//-----------------------------------------------------------------------------
//
//! \fn week_Year_Print
//
//! \brief
//!   Description:parse out the manufaturing and Accounting Date parameter of the log
//
//  Entry:
//! \param manData - node to add the manufacturing data to
//! \param param - param code 
//! \param paramlength - the length of the parm information
//! \param paramConByte - the control byte data
//! \param year - the binary data for the year
//! \param week - the binary data for the week
//! \param strHeader - the string data for the header of the json data
//! \param strYear - the string for the year
//! \param strWeek - the string for the week
//
//  Exit:
//!   \return eReturnParserValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::week_Year_Print(JSONNODE *data,uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t year, uint16_t week,const std::string strHeader, const std::string strYear, const std::string strWeek)
{
#define YEARSIZE 4
#define WEEKSIZE 2
	std::string myStr = "";
	myStr.resize(BASIC);

	JSONNODE *dateInfo = json_new(JSON_NODE);
	json_set_name(dateInfo, (char *)strHeader.c_str());
	
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", param);
        json_push_back(dateInfo, json_new_a("Parameter Code", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", paramlength);
        json_push_back(dateInfo, json_new_a("Parameter Length", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", paramConByte);
        json_push_back(dateInfo, json_new_a("Control Byte", (char*)myStr.c_str()));
    }
	myStr.resize(YEARSIZE);
	memset((char*)myStr.c_str(), 0, YEARSIZE);
    if (year != 0x20202020)
    {
        strncpy((char *)myStr.c_str(), (char*)&year, YEARSIZE);
    }
    else
    {
        myStr = "0000";
    }
	json_push_back(dateInfo, json_new_a((char *)strYear.c_str(), (char*)myStr.c_str()));
	myStr.resize(WEEKSIZE);
    if (week != 0x2020)
    {
        strncpy((char *)myStr.c_str(), (char*)&week, WEEKSIZE);
    }
    else
    {
        myStr = "00";
    }
	json_push_back(dateInfo, json_new_a((char *)strWeek.c_str(), (char*)myStr.c_str()));

	json_push_back(data, dateInfo);
	return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Count
//
//! \brief
//!   Description:parse out the cycle count over the lifetime of the device
//
//  Entry:
//! \param countData - node to add the cycle count data to
//
//  Exit:
//!   \return eReturnParserValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::get_Count(JSONNODE *countData, uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t count, const std::string strHeader, const std::string strCount)
{
	std::string myStr = "";
	myStr.resize(BASIC);

	JSONNODE *countInfo = json_new(JSON_NODE);
	json_set_name(countInfo, (char *)strHeader.c_str());

    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", param);
        json_push_back(countInfo, json_new_a("Parameter Code", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", paramlength);
        json_push_back(countInfo, json_new_a("Parameter Length", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", paramConByte);
        json_push_back(countInfo, json_new_a("Control Byte", (char*)myStr.c_str()));
    }
	byte_Swap_32(&count);
	json_push_back(countInfo, json_new_i((char *)strCount.c_str(),  count )); 

	json_push_back(countData, countInfo);
	return SUCCESS;
}