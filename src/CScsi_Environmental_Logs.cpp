//
// CScsi_Environmental_Logs.cpp  Implementation of CScsi Environmental Log class
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Environmental_Logs.cpp Implementation of CScsi Environmental Log class
//
#include "CScsi_Environmental_Logs.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiEnvironmentLog()
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
CScsiEnvironmentLog::CScsiEnvironmentLog()
	: CScsiTemperatureLog()
	, m_EvnName("Environmnetal Log")
	, m_EnvStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_SubPage(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_EvnName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiEnvironmentLog()
//
//! \brief
//!   Description: Class constructor for the CScsiTemperatureLog
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param masterData - the pointer to the json node that holds all of the json data
//! \param subPage - holds the sub page information so we know which temp log to parse.
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiEnvironmentLog::CScsiEnvironmentLog(uint8_t *bufferData, size_t bufferSize, uint8_t subPage, JSONNODE *masterData)
	:CScsiTemperatureLog(&bufferData[4], bufferSize)
	, m_EvnName("Environmnetal Log")
	, m_EnvStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_SubPage(subPage)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_EvnName.c_str());
	}
	if (bufferData != NULL)
	{
		pData = bufferData;
		m_Page = (sLogPageStruct *)bufferData;				// set a buffer to the point to the log page info
		m_PageLength = m_Page->pageLength;
		//byte_Swap_16(&m_PageLength);						// get the length of the page and byte swap it
		m_EnvStatus = figureout_What_Log_To_Parsed(masterData);					// init the data for getting the log  
	}
	else
	{
		m_EnvStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiEnvironmentLog
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
CScsiEnvironmentLog::~CScsiEnvironmentLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn figureout_What_Log_To_Parsed
//
//! \brief
//!   Description: we now know it's an environment log. Now which one is it. and then get it parssed
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiEnvironmentLog::figureout_What_Log_To_Parsed(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (m_SubPage == 0x00)			// Temperature 
	{
		set_Temp_Page_Length(m_PageLength);
		retStatus = parse_Temp_Log(masterData);
	}
	else if (m_SubPage == 0x01)       // Environmental Reporting  
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Temperature Log Page Not Supported");
		json_push_back(masterData, pageInfo);
	}
	else if (m_SubPage == 0x02)		// Environmental Limits
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Temperature Log Page Not Supported");
		json_push_back(masterData, pageInfo);
	}
	return retStatus;
}