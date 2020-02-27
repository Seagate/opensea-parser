//
// CScsi_Format_Status_Log.cpp  Format Status log page reports information about the most recent successful format operation
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Format_Status_Log.cpp  Format Status log page reports information about the most recent successful format operation
//
#include "CScsi_Format_Status_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog()
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
CScsiFormatStatusLog::CScsiFormatStatusLog()
	: pData()
	, m_FormatName("Format Status Log")
	, m_FormatStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_Value(0)
	, m_Format()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_FormatName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog()
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
CScsiFormatStatusLog::CScsiFormatStatusLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(buffer)
	, m_FormatName("Format Status Log")
	, m_FormatStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_Value(0)
	, m_Format()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_FormatName.c_str());
	}
	if (buffer != NULL)
	{
		m_FormatStatus = IN_PROGRESS;
	}
	else
	{
		m_FormatStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog
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
CScsiFormatStatusLog::~CScsiFormatStatusLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Format_Log_Value_Description
//
//! \brief
//!   Description: parser out the value information
//
//  Entry:
//! \param formatData - string to give the value string into
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::get_Format_Log_Value_Description(std::string *valueData)
{
	switch (m_Format->paramCode)
	{
	case 0x0000:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Format Data Out count");
		break;
	}
	case 0x0001:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Grown Defects During Certification count");
		break;
	}
	case 0x0002:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Total Blocks Reassigned During Format count");
		break;
	}
	case 0x0003:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Total New Blocks Reassigned count");
		break;
	}
	case 0x0004:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Power On Minutes Since Format count");
		break;
	}
	default:
	{
		snprintf((char*)valueData->c_str(), BASIC, "Vendor Specific 0x%04" PRIx16"", m_Format->paramCode);
		break;
	}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Format_Log_Header_Description
//
//! \brief
//!   Description: parser out the header information
//
//  Entry:
//! \param formatData - string to give the header string into
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::get_Format_Log_Header_Description(std::string *headerData)
{
	switch (m_Format->paramCode)
	{
		case 0x0000:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Format Data Out");
			break;
		}
		case 0x0001:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Grown Defects During Certification");
			break;
		}
		case 0x0002:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Total Blocks Reassigned During Format");
			break;
		}
		case 0x0003:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Total New Blocks Reassigned");
			break;
		}
		case 0x0004:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Power On Minutes Since Format");
			break;
		}
		default:
		{
			snprintf((char*)headerData->c_str(), BASIC, "Vendor Specific 0x%04" PRIx16"", m_Format->paramCode);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_Cache_Event_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed cache data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::get_Format_Status_Descriptions(JSONNODE *formatData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Format Status Log\n");
#endif
	byte_Swap_16(&m_Format->paramCode);
	get_Format_Log_Header_Description(&myStr);
	//snprintf((char*)myStr.c_str(), BASIC, "Cache Statistics Description %" PRId16"", m_Format->paramCode);
	JSONNODE *formatInfo = json_new(JSON_NODE);
	json_set_name(formatInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_Format->paramCode);
	json_push_back(formatInfo, json_new_a("Format Status Parameter Code", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramControlByte);
	json_push_back(formatInfo, json_new_a("Format Status Control Byte ", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramLength);
	json_push_back(formatInfo, json_new_a("Format Status Length ", (char*)myStr.c_str()));
	get_Format_Log_Value_Description(&myStr);
	if (m_Format->paramLength == 8 || m_Value > UINT32_MAX)
	{
		set_json_64bit(formatInfo, (char*)myStr.c_str(), m_Value, false);
	}
	else
	{
		json_push_back(formatInfo, json_new_i((char*)myStr.c_str(), static_cast<uint32_t>(m_Value)));
	}

	json_push_back(formatData, formatInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Format_Status_Data
//
//! \brief
//!   Description: parser out the data for the Format Status Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiFormatStatusLog::get_Format_Status_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		for (size_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				m_Format = (sFormatParams *)&pData[offset];
				offset += sizeof(sFormatParams);
				switch (m_Format->paramLength)
				{
				case 1:
				{
					if ((offset + m_Format->paramLength) < m_bufferLength)
					{
						m_Value = pData[offset];
						offset += m_Format->paramLength;
					}
					else
					{
						//json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 2:
				{
					if ((offset + m_Format->paramLength) < m_bufferLength)
					{
						m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
						offset += m_Format->paramLength;
					}
					else
					{
						//json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 4:
				{
					if ((offset + m_Format->paramLength) < m_bufferLength)
					{
						m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
						offset += m_Format->paramLength;
					}
					else
					{
						//json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 8:
				{
					if ((offset + m_Format->paramLength) < m_bufferLength)
					{
						m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
						offset += m_Format->paramLength;
					}
					else
					{
						//json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				default:
				{
					//json_push_back(masterData, pageInfo);
					return BAD_PARAMETER;
					break;
				}
				}
                JSONNODE *pageInfo = json_new(JSON_NODE);
                json_set_name(pageInfo, "Format Status Log - 8h");
				get_Format_Status_Descriptions(pageInfo);
                json_push_back(masterData, pageInfo);
			}
			else
			{
				//json_push_back(masterData, pageInfo);
				return BAD_PARAMETER;
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