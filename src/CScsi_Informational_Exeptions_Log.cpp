//
// CScsi_Informational_Exeptions_Log.cpp  Definition of the Informational Exceptions Log page provides a place for reporting detail about exceptions.
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Informational_Exeptions_Log.cpp  Definition of the Informational Exceptions Log page provides a place for reporting detail about exceptions.
//
#include "CScsi_Informational_Exeptions_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiInformationalExeptionsLog()
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
CScsiInformationalExeptionsLog::CScsiInformationalExeptionsLog()
	: pData()
	, m_infoName("Informational Exceptions Log")
	, m_infoStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_Exeptions()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_infoName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiInformationalExeptionsLog()
//
//! \brief
//!   Description: Class constructor for the Cache Statistics
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiInformationalExeptionsLog::CScsiInformationalExeptionsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(NULL)
	, m_infoName("Informational Exceptions Log")
	, m_infoStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_Exeptions()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_infoName.c_str());
	}
    pData = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pData, buffer, bufferSize);
#else
    memcpy_s(pData, bufferSize, buffer, bufferSize);// copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_infoStatus = IN_PROGRESS;
	}
	else
	{
		m_infoStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiInformationalExeptionsLog
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
CScsiInformationalExeptionsLog::~CScsiInformationalExeptionsLog()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Informational_Exceptions_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed Informational Exceptions Log data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiInformationalExeptionsLog::process_Informational_Exceptions_Data(JSONNODE *exeptionData, uint16_t count)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined _DEBUG
	printf("Informational Exceptions Log \n");
#endif
	snprintf((char*)myStr.c_str(), BASIC, "Informational Exception %" PRId16"", count);
	JSONNODE *exeptionInfo = json_new(JSON_NODE);
	json_set_name(exeptionInfo, (char*)myStr.c_str());
	byte_Swap_16(&m_Exeptions->paramCode);
	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_Exeptions->paramCode);
	json_push_back(exeptionInfo, json_new_a("Informational Exception Parameter Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Exeptions->paramControlByte);
	json_push_back(exeptionInfo, json_new_a("Informational Exception Control Byte ", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Exeptions->paramLength);
	json_push_back(exeptionInfo, json_new_a("Informational Exception Length ", (char*)myStr.c_str()));

	json_push_back(exeptionInfo, json_new_i("Informational Exception Additional Sense Code", static_cast<uint32_t>(m_Exeptions->senseCode)));
	json_push_back(exeptionInfo, json_new_i("Informational Exception Additional Sense Code Qualifier", static_cast<uint32_t>(m_Exeptions->senseCodeQualifier)));
	json_push_back(exeptionInfo, json_new_i("Most Recent Temperature Reading", static_cast<uint32_t>(m_Exeptions->temp)));
	json_push_back(exeptionInfo, json_new_i("Vendor Hda Temperature Trip Point", static_cast<uint32_t>(m_Exeptions->tempLimit)));
	json_push_back(exeptionInfo, json_new_i("Maximum Temperature", static_cast<uint32_t>(m_Exeptions->maxTemp)));
	json_push_back(exeptionInfo, json_new_i("Vendor specific First", static_cast<uint32_t>(m_Exeptions->vendor1)));
	json_push_back(exeptionInfo, json_new_i("Vendor specific Second", static_cast<uint32_t>(m_Exeptions->vendor2)));
	json_push_back(exeptionInfo, json_new_i("Vendor specific Third", static_cast<uint32_t>(m_Exeptions->vendor3)));

	json_push_back(exeptionData, exeptionInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Informational_Exceptions_Data
//
//! \brief
//!   Description: parser out the data for the Informational Exceptions Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiInformationalExeptionsLog::get_Informational_Exceptions_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Informational Exceptions Log - 2fh length 0x0f");
		uint16_t number = 0;
		for (size_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				number++;
				m_Exeptions = (sExeptionsParams *)&pData[offset];
				offset += sizeof(sExeptionsParams);
				process_Informational_Exceptions_Data(pageInfo, number);
			}
			else
			{
				json_push_back(masterData, pageInfo);
				return BAD_PARAMETER;
			}

		}

		json_push_back(masterData, pageInfo);
		retStatus = SUCCESS;
	}
	else
	{
		retStatus = MEMORY_FAILURE;
	}
	return retStatus;
}