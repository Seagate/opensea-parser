//
// CScsi_Informational_Exeptions_Log.cpp  Definition of the Informational Exceptions Log page provides a place for reporting detail about exceptions.
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
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
void CScsiInformationalExeptionsLog::process_Informational_Exceptions_Data(JSONNODE *exeptionData, uint16_t count, uint16_t offset)
{
	std::string myStr = "";
    std::ostringstream temp;
	myStr.resize(BASIC);
#if defined _DEBUG
	printf("Informational Exceptions Log \n");
#endif

	if (g_dataformat == PREPYTHON_DATA)
	{
		JSONNODE* data = json_new(JSON_NODE);
		json_push_back(data, json_new_a("name", "environment_temperature"));
		JSONNODE* label = json_new(JSON_NODE);
		json_set_name(label, "labels");
        std::ostringstream temp;
        temp << "scsi-log-page:0x" << std::hex << std::setfill('0') << INFORMATIONAL_EXCEPTIONS << "," << 0 << ":0x" << m_Exeptions->paramCode << ":" << offset;
		json_push_back(label, json_new_a("metric_source", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCode);
		json_push_back(label, json_new_a("scsi_asc", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCodeQualifier);
		json_push_back(label, json_new_a("scsi_ascq", temp.str().c_str()));
		json_push_back(label, json_new_a("stat_type", "most recent temperature"));
		json_push_back(label, json_new_a("units", "celsius"));
		json_push_back(data, label);
		json_push_back(data, json_new_f("value", static_cast<double>(m_Exeptions->temp)));
		json_push_back(exeptionData, data);

		JSONNODE* trip = json_new(JSON_NODE);
		json_push_back(trip, json_new_a("name", "environment_temperature"));
		JSONNODE* label1 = json_new(JSON_NODE);
		json_set_name(label1, "labels");
        temp.clear();
        temp << "scsi-log-page:0x" << std::hex << std::setfill('0') << INFORMATIONAL_EXCEPTIONS << "," << 0 << ":0x" << m_Exeptions->paramCode << ":" << offset;
		json_push_back(label1, json_new_a("metric_source", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCode);
		json_push_back(label1, json_new_a("scsi_asc", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCodeQualifier);
		json_push_back(label1, json_new_a("scsi_ascq", temp.str().c_str()));
		json_push_back(label1, json_new_a("stat_type", "trip point temperature"));
		json_push_back(label1, json_new_a("units", "celsius"));
		json_push_back(trip, label1);
		json_push_back(trip, json_new_f("value", static_cast<double>(m_Exeptions->tempLimit)));
		json_push_back(exeptionData, trip);

		JSONNODE* max = json_new(JSON_NODE);
		json_push_back(max, json_new_a("name", "environment_temperature"));
		JSONNODE* label2 = json_new(JSON_NODE);
		json_set_name(label2, "labels");
        temp.clear();
        temp << "scsi-log-page:0x" << std::hex << std::setfill('0') << INFORMATIONAL_EXCEPTIONS << "," << 0 << ":0x" << m_Exeptions->paramCode << ":" << offset;
		json_push_back(label2, json_new_a("metric_source", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCode);
		json_push_back(label2, json_new_a("scsi_asc", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->senseCodeQualifier);
		json_push_back(label2, json_new_a("scsi_ascq", temp.str().c_str()));
		json_push_back(label2, json_new_a("stat_type", "maximum temperature"));
		json_push_back(label2, json_new_a("units", "celsius"));
		json_push_back(max, label2);
		json_push_back(max, json_new_f("value", static_cast<double>(m_Exeptions->maxTemp)));
		json_push_back(exeptionData, max);

	}
	else
	{
        temp.clear();
        temp << "Informational Exception " << std::dec << count;
		JSONNODE* exeptionInfo = json_new(JSON_NODE);
		json_set_name(exeptionInfo, temp.str().c_str());
		byte_Swap_16(&m_Exeptions->paramCode);
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(4) << m_Exeptions->paramCode;
		json_push_back(exeptionInfo, json_new_a("Informational Exception Parameter Code", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->paramControlByte);
		json_push_back(exeptionInfo, json_new_a("Informational Exception Control Byte ", temp.str().c_str()));
        temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Exeptions->paramLength);
		json_push_back(exeptionInfo, json_new_a("Informational Exception Length ", temp.str().c_str()));

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
		JSONNODE* pageInfo;
		if (g_dataformat == PREPYTHON_DATA)
		{
			pageInfo = masterData;
		}
		else
		{
			pageInfo = json_new(JSON_NODE);
			json_set_name(pageInfo, "Informational Exceptions Log - 2fh length 0x0f");
		}
		uint16_t number = 0;
		for (uint16_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				number++;
				m_Exeptions = (sExeptionsParams *)&pData[offset];
				process_Informational_Exceptions_Data(pageInfo, number, offset);
				offset += sizeof(sExeptionsParams);
			}
			else
			{
				if (g_dataformat != PREPYTHON_DATA)
				{
					json_push_back(masterData, pageInfo);
				}
				return BAD_PARAMETER;
			}

		}
		if (g_dataformat != PREPYTHON_DATA)
		{
			json_push_back(masterData, pageInfo);
		}
		retStatus = SUCCESS;
	}
	else
	{
		retStatus = MEMORY_FAILURE;
	}
	return retStatus;
}