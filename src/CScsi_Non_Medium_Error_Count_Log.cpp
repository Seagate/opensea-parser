//
// CScsi_Non_Medium_Error_Count_Log.cpp Definition of the Non-Medium Error Count Log
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Non_Medium_Error_Count_Log.cpp   Definition of the Non-Medium Error Count Log provides for counting the occurrences of recoverable error events other than
//   write, read, or verify failures.
//
#include "CScsi_Non_Medium_Error_Count_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiNonMediumErrorCountLog()
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
CScsiNonMediumErrorCountLog::CScsiNonMediumErrorCountLog()
	: pData()
	, m_NMECName("Non-Medium Error Count Log")
	, m_NMECStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_Value(0)
	, m_CountErrors()
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_NMECName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiNonMediumErrorCountLog()
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
CScsiNonMediumErrorCountLog::CScsiNonMediumErrorCountLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(buffer)
	, m_NMECName("Non-Medium Error Count Log")
	, m_NMECStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_Value(0)
	, m_CountErrors()
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_NMECName.c_str());
	}
	if (buffer != M_NULLPTR)
	{
		m_NMECStatus = eReturnValues::IN_PROGRESS;
	}
	else
	{
		m_NMECStatus = eReturnValues::FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiNonMediumErrorCountLog
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
CScsiNonMediumErrorCountLog::~CScsiNonMediumErrorCountLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn process_Non_Medium_Error_Count_Data
//
//! \brief
//!   Description: parser out the data for Non-Medium Error Count
//
//  Entry:
//! \param eventData - Json node that parsed 
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiNonMediumErrorCountLog::process_Non_Medium_Error_Count_Data(JSONNODE* countData)
{
#if defined _DEBUG
	printf("Non-Medium Error Count Log Parameters\n");
#endif
	byte_Swap_16(&m_CountErrors->paramCode);
    std::ostringstream temp;
    temp << "Non-Medium Error Count Log Parameters 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_CountErrors->paramCode;
	JSONNODE* cacheInfo = json_new(JSON_NODE);
	json_set_name(cacheInfo, temp.str().c_str());
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_CountErrors->paramCode;
	json_push_back(cacheInfo, json_new_a("Non-Medium Error Count Parameter Code", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_CountErrors->paramControlByte);
	json_push_back(cacheInfo, json_new_a("Non-Medium Error Count Control Byte ", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_CountErrors->paramLength);
	json_push_back(cacheInfo, json_new_a("Non-Medium Error CountLength ", temp.str().c_str()));
	if (m_CountErrors->paramLength == 8 || m_Value > UINT32_MAX)
	{
		set_json_64bit(cacheInfo, "Non-Medium Error Count", m_Value, false);
	}
	else
	{
		json_push_back(cacheInfo, json_new_i("Non-Medium Error Count", static_cast<json_int_t>(m_Value)));
	}

	json_push_back(countData, cacheInfo);

}
//-----------------------------------------------------------------------------
//
//! \fn get_Non_Medium_Error_Count_Data
//
//! \brief
//!   Description: parser out the data for the Non-Medium Error Count Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiNonMediumErrorCountLog::get_Non_Medium_Error_Count_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = eReturnValues::IN_PROGRESS;
	if (pData != M_NULLPTR)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Non-Medium Error Count Log - 6h");
		for (uint32_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
			    m_CountErrors = reinterpret_cast<sLogParams*>(&pData[offset]);
			    offset += PARAMSIZE;
				switch (m_CountErrors->paramLength)
				{
				case 1:
				{
					if ((offset + m_CountErrors->paramLength) <= static_cast<uint32_t>(m_bufferLength))
					{
						m_Value = pData[offset];
						offset += m_CountErrors->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return eReturnValues::BAD_PARAMETER;
					}
					break;
				}
				case 2:
				{
					if ((offset + m_CountErrors->paramLength) <= static_cast<uint32_t>(m_bufferLength))
					{
						m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
						offset += m_CountErrors->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return eReturnValues::BAD_PARAMETER;
					}
					break;
				}
				case 4:
				{
					if ((offset + m_CountErrors->paramLength) <= static_cast<uint32_t>(m_bufferLength))
					{
						m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
						offset += m_CountErrors->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return eReturnValues::BAD_PARAMETER;
					}
					break;
				}
				case 8:
				{
					if ((offset + m_CountErrors->paramLength) <= static_cast<uint32_t>(m_bufferLength))
					{
						m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
						offset += m_CountErrors->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return eReturnValues::BAD_PARAMETER;
					}
					break;
				}
				default:
				{
					json_push_back(masterData, pageInfo);
					return eReturnValues::BAD_PARAMETER;
				}
				}
				process_Non_Medium_Error_Count_Data(pageInfo);
			}
			else
			{
				json_push_back(masterData, pageInfo);
				return eReturnValues::BAD_PARAMETER;
			}
		}
		json_push_back(masterData, pageInfo);
		retStatus = eReturnValues::SUCCESS;
	}
	else
	{
		retStatus = eReturnValues::MEMORY_FAILURE;
	}
	return retStatus;
}
