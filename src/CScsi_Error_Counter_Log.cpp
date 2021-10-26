//
// CScsi_Error_Counter_Log.cpp  Definition of Error Counter for READ WRITE VERIFY ERRORS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Error_Counter_Log.cpp  Definition of Error Counter for READ WRITE VERIFY ERRORS
//
#include "CScsi_Error_Counter_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog()
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
CScsiErrorCounterLog::CScsiErrorCounterLog()
	: pData()
	, m_ErrorName("Error Counter Log")
	, m_ErrorStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(0)
	, m_ErrorValue(0)
	, m_Error()
	, m_pageType(WRITE)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ErrorName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog()
//
//! \brief
//!   Description: Class constructor for the Error Counter for READ WRITE VERIFY ERRORS
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//! \param type - need to know the type of the page   READ or WRITE or VERIFY
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiErrorCounterLog::CScsiErrorCounterLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength, uint8_t type)
	: pData(buffer)
	, m_ErrorName("Error Counter Log")
	, m_ErrorStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_ErrorValue(0)
	, m_Error()
	, m_pageType(type)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ErrorName.c_str());
	}
	if (buffer != NULL)
	{
		if (m_pageType == WRITE || m_pageType == READ || m_pageType == VERIFY)
		{
			m_ErrorStatus = IN_PROGRESS;
		}
		else
		{
			m_ErrorStatus = BAD_PARAMETER;
		}
	}
	else
	{
		m_ErrorStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog
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
CScsiErrorCounterLog::~CScsiErrorCounterLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn append_Error_Log_Page_Number
//
//! \brief
//!   Description: appends the Error Log Page Number so people can search on the number
//
//  Entry:
//! \param *typeStr - pointer to the string that we will be creating
//! \param main - this is the main header string that we will be adding the master string to
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::append_Error_Log_Page_Number(std::string *typeStr, std::string main)
{
    switch (m_pageType)
    {
    case 0x02:
    {
        snprintf(&*typeStr->begin(), BASIC, "%s - %s", &*main.begin(), "02h");// WRITE
        break;
    }
    case 0x03:
    {
        snprintf(&*typeStr->begin(), BASIC, "%s - %s", &*main.begin(), "03h"); // READ
        break;
    }
    case 0x05:
    {
        snprintf(&*typeStr->begin(), BASIC, "%s - %s", &*main.begin(), "05h"); // VERIFY
        break;
    }
    default:
        snprintf(&*typeStr->begin(), BASIC, "%s - %s", &*main.begin(), "03h");  ///READ
        break;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn set_Master_String
//
//! \brief
//!   Description: creates the master string for the header information
//
//  Entry:
//! \param *typeStr - pointer to the string that we will be creating
//! \param main - this is the main header string that we will be adding the master string to
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::set_Master_String(std::string *typeStr, std::string main)
{
	switch (m_pageType)
	{
		case 0x02:
		{
			snprintf(&*typeStr->begin(), BASIC, "%s %s", "WRITE", &*main.begin());
			break;
		}
		case 0x03:
		{
			snprintf(&*typeStr->begin(), BASIC, "%s %s", "READ", &*main.begin());
			break;
		}
		case 0x05:
		{
			snprintf(&*typeStr->begin(), BASIC, "%s %s", "VERIFY", &*main.begin());
			break;
		}
		default:
			snprintf(&*typeStr->begin(), BASIC, "%s %s", "READ", &*main.begin());
			break;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Error_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for Error Parameter Information
//
//  Entry:
//! \param description - string to give the Error Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiErrorCounterLog::get_Error_Parameter_Code_Description(std::string *error)
{
    bool descriptionFound = false;
	switch (m_Error->paramCode)
	{
		case 0x0000:
		{
			snprintf(&*error->begin(), BASIC, "Corrected Without Substantial Delay");
            descriptionFound = true;
			break;
		}
		case 0x0001:
		{
			snprintf(&*error->begin(), BASIC, "Corrected With Possible Delay");
            descriptionFound = true;
			break;
		}
		case 0x0002:
		{
			snprintf(&*error->begin(), BASIC, "Number of Errors that are Corrected by Applying Retries");
            descriptionFound = true;
			break;
		}
		case 0x0003:
		{
			snprintf(&*error->begin(), BASIC, "Total Number of Errors Corrected");
            descriptionFound = true;
			break;
		}
		case 0x0004:
		{
			snprintf(&*error->begin(), BASIC, "Total Times Correction Algorithm Processed");
            descriptionFound = true;
			break;
		}
		case 0x0005:
		{
			snprintf(&*error->begin(), BASIC, "Total Bytes Processed");
            descriptionFound = true;
			break;
		}
		case 0x0006:
		{
			snprintf(&*error->begin(), BASIC, "Total Uncorrected Errors");
            descriptionFound = true;
			break;
		}
		default:
		{
			snprintf(&*error->begin(), BASIC, "Vendor Specific 0x%04" PRIx16"", m_Error->paramCode);
			break;
		}
	}
    return descriptionFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Error_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed error counter data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::process_Error_Data(JSONNODE *errorData)
{
    bool descriptionFound = false;
	std::string myStr = "";
	myStr.resize(BASIC);
	std::string myHeader = "";
	myHeader.resize(BASIC);
#if defined _DEBUG
	printf("Error Counter Log  \n");
#endif
	byte_Swap_16(&m_Error->paramCode);
    descriptionFound = get_Error_Parameter_Code_Description(&myHeader);
    if (m_ErrorValue != 0)
    {
        set_Master_String(&myStr, myHeader);
        JSONNODE *errorInfo = json_new(JSON_NODE);
        json_set_name(errorInfo, &*myStr.begin());
        snprintf(&*myStr.begin(), BASIC, "0x%04" PRIx16"", m_Error->paramCode);
        json_push_back(errorInfo, json_new_a("Error Counter Code", &*myStr.begin()));
        if (!descriptionFound)
        {
            
            snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_Error->paramControlByte);
            json_push_back(errorInfo, json_new_a("Error Counter Control Byte ", &*myStr.begin()));
            snprintf(&*myStr.begin(), BASIC, "0x%02" PRIx8"", m_Error->paramLength);
            json_push_back(errorInfo, json_new_a("Error Counter Length ", &*myStr.begin()));
        }
        if (m_Error->paramLength == 8 || m_ErrorValue > UINT32_MAX)
        {
            set_json_64bit(errorInfo, "Error Count", m_ErrorValue, false);
        }
        else
        {           
            if (M_GETBITRANGE(m_ErrorValue, 33, 15) == 0)
            {
                json_push_back(errorInfo, json_new_i("Error Count", static_cast<uint32_t>(m_ErrorValue)));
            }

            else
            {
                std::string printStr;
                snprintf(&*printStr.begin(), BASIC, "%" PRIu32"",  static_cast<uint32_t>(m_ErrorValue));
                json_push_back(errorInfo, json_new_a(&*myStr.begin(), &*printStr.begin()));
            }
        }

        json_push_back(errorData, errorInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Error_Counter_Data
//
//! \brief
//!   Description: parser out the data for the error counter log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiErrorCounterLog::get_Error_Counter_Data(JSONNODE *masterData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
    std::string headerStr = "";
    headerStr.resize(BASIC);
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		set_Master_String(&headerStr, "Error Counter Log");
        append_Error_Log_Page_Number(&myStr, headerStr);
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, &*myStr.begin());

		for (size_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				m_Error = (sErrorParams *)&pData[offset];
				offset += sizeof(sErrorParams);
				switch (m_Error->paramLength)
				{
				case 1:
				{
					if ((offset + m_Error->paramLength) < m_bufferLength)
					{
						m_ErrorValue = pData[offset];
						offset += m_Error->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 2:
				{
					if ((offset + m_Error->paramLength) < m_bufferLength)
					{
						m_ErrorValue = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
						offset += m_Error->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 4:
				{
					if ((offset + m_Error->paramLength) < m_bufferLength)
					{
						m_ErrorValue = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
						offset += m_Error->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				case 8:
				{
					if ((offset + m_Error->paramLength) < m_bufferLength)
					{
						m_ErrorValue = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
						offset += m_Error->paramLength;
					}
					else
					{
						json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
					break;
				}
				default:
				{
					json_push_back(masterData, pageInfo);
					return BAD_PARAMETER;
					break;
				}
				}
				process_Error_Data(pageInfo);
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