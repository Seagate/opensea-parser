// CScsi_Pending_Defects_Log..cpp  Definition for parsing the pending defecs
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Pending_Defects_Log..cpp  Definition for parsing the pending defecs
//
#include "CScsi_Pending_Defects_Log.h"


using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiPendingDefectsLog()
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
CScsiPendingDefectsLog::CScsiPendingDefectsLog()
	: pData()
	, m_PlistName("Pending Defect Log")
	, m_PlistStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(0)
	, m_PListCountParam()
	, m_PlistDefect()
	, m_count(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PlistName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiPendingDefectsLog()
//
//! \brief
//!   Description: Class constructor for the supported log pages
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiPendingDefectsLog::CScsiPendingDefectsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(NULL)
	, m_PlistName("Pending Defect Log")
	, m_PlistStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_PListCountParam()
	, m_PlistDefect()
	, m_count(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PlistName.c_str());
	}
    pData = new uint8_t[pageLength];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pData, buffer, pageLength);
#else
    memcpy_s(pData, pageLength, buffer, pageLength);// copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_PlistStatus = IN_PROGRESS;
	}
	else
	{
		m_PlistStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiPendingDefectsLog
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
CScsiPendingDefectsLog::~CScsiPendingDefectsLog()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_PList_Data
//
//! \brief
//!   Description: parser out the data for each Pending defect in the list
//
//  Entry:
//! \param SupportData - Json node that parsed Supported pages
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiPendingDefectsLog::process_PList_Data(JSONNODE *pendingData, uint32_t offset)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	byte_Swap_16(&m_PlistDefect->paramCode);
	byte_Swap_32(&m_PlistDefect->defectPOH);
	byte_Swap_64(&m_PlistDefect->defectLBA);
	if (g_dataformat == PREPYTHON_DATA)
	{
		JSONNODE* data = json_new(JSON_NODE);

		json_push_back(data, json_new_a("name", "pending_defect"));
		JSONNODE* label = json_new(JSON_NODE);
		json_set_name(label, "labels");
		snprintf(&*myStr.begin(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", 0x15, 0x01, m_PlistDefect->paramCode,offset);
		json_push_back(label, json_new_a("metric_source", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "%" PRIu32"", m_PlistDefect->defectPOH);
		json_push_back(label, json_new_a("power_on_hours", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "0x%08" PRIx64"", m_PlistDefect->defectLBA);
		json_push_back(label, json_new_a("lba", &*myStr.begin()));
		json_push_back(label, json_new_a("units", "index"));
		json_push_back(data, label);
		json_push_back(data, json_new_i("value", m_count));
		json_push_back(pendingData, data);

	}
	else
	{

#if defined _DEBUG
		printf("Pending Defect Log  \n");
#endif
		
		snprintf(&*myStr.begin(), BASIC, "Pending Defect number %" PRIi16"", m_PlistDefect->paramCode);
		JSONNODE* pListInfo = json_new(JSON_NODE);
		json_set_name(pListInfo, &*myStr.begin());

		snprintf(&*myStr.begin(), BASIC, "0x%04" PRIx16"", m_PlistDefect->paramCode);
		json_push_back(pListInfo, json_new_a("Pending Defect Code", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_PlistDefect->paramControlByte);
		json_push_back(pListInfo, json_new_a("Pending DefectControl Byte ", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_PlistDefect->paramLength);
		json_push_back(pListInfo, json_new_a("Pending Defect Length ", &*myStr.begin()));
		json_push_back(pListInfo, json_new_i("Pending Defect Power On Hours", static_cast<uint32_t>(m_PlistDefect->defectPOH)));

		set_json_64bit(pListInfo, "Pending Defect LBA", m_PlistDefect->defectLBA, true);

		json_push_back(pendingData, pListInfo);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_PList_Count
//
//! \brief
//!   Description: parser out the data for the count of the Pending Defect list
//
//  Entry:
//! \param pendingCount - Json node that parsed Supported pages
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiPendingDefectsLog::process_PList_Count(JSONNODE *pendingCount)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	byte_Swap_16(&m_PListCountParam->paramCode);
	byte_Swap_32(&m_PListCountParam->totalPlistCount);
	if (g_dataformat == PREPYTHON_DATA)
	{
#if defined _DEBUG
		printf("Pending Defect Count \n");
#endif
		prePython_int(pendingCount, "pending_defect", NULL, "count", m_PListCountParam->totalPlistCount, 0x15, 0x01, m_PListCountParam->paramCode, 0);
	}
	else
	{
#if defined _DEBUG
		printf("Pending Defect Count \n");
#endif
		JSONNODE* countInfo = json_new(JSON_NODE);
		json_set_name(countInfo, "Pending Defect count");

		snprintf(&*myStr.begin(), BASIC, "0x%04" PRIx16"", m_PListCountParam->paramCode);
		json_push_back(countInfo, json_new_a("Pending Defect Counter Code", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_PListCountParam->paramControlByte);
		json_push_back(countInfo, json_new_a("Pending Defect Counter Control Byte ", &*myStr.begin()));
		snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_PListCountParam->paramLength);
		json_push_back(countInfo, json_new_a("Pending Defect Counter Length ", &*myStr.begin()));
		json_push_back(countInfo, json_new_i("Pending Defect Count", static_cast<uint32_t>(m_PListCountParam->totalPlistCount)));

		json_push_back(pendingCount, countInfo);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_Log_Data
//
//! \brief
//!   Description: parser out the data for tLog
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiPendingDefectsLog::get_Plist_Data(JSONNODE *masterData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		myStr = "Pending Defect Log - 15h";
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, &*myStr.begin());
		m_PListCountParam = (sPendindDefectCount *)pData;
		process_PList_Count(pageInfo);
		if ((size_t)m_PageLength > sizeof(sPendindDefectCount))    // for when there is zero count defects
		{
			for (uint32_t offset = sizeof(sPendindDefectCount); offset < (size_t)m_PageLength; )
			{
				if (offset < (m_bufferLength - sizeof(sLogPageStruct)))
				{
					m_PlistDefect = (sDefect *)&pData[offset];
					offset += sizeof(sDefect);
					m_count++;    // defect found add one to the count
					process_PList_Data(pageInfo,offset);
				}
				else
				{
					json_push_back(masterData, pageInfo);
					return BAD_PARAMETER;
				}
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
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_Log_Data
//
//! \brief
//!   Description: parser out the data for tLog
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiPendingDefectsLog::get_PrePython_Plist_Data(JSONNODE* masterData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		m_PListCountParam = (sPendindDefectCount*)pData;
		process_PList_Count(masterData);
		if ((size_t)m_PageLength > sizeof(sPendindDefectCount))    // for when there is zero count defects
		{
			for (uint32_t offset = sizeof(sPendindDefectCount); offset < (size_t)m_PageLength; )
			{
				if (offset < (m_bufferLength - sizeof(sLogPageStruct)))
				{
					m_PlistDefect = (sDefect*)&pData[offset];
					m_count++;    // defect found add one to the count
					process_PList_Data(masterData,offset);
					offset += sizeof(sDefect);
				}
				else
				{
					return BAD_PARAMETER;
				}
			}
		}
		retStatus = SUCCESS;
	}
	else
	{
		retStatus = MEMORY_FAILURE;
	}
	return retStatus;
}