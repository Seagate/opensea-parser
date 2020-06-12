//
// CScsi_Protocol_Specific_Port_Log.h   Definition of Protocol-Specific Port log page for SAS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Protocol_Specific_Port_Log.h   Definition of Protocol-Specific Port log page for SAS
//
#include "CScsi_Protocol_Specific_Port_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiProtocolPortLog()
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
CScsiProtocolPortLog::CScsiProtocolPortLog()
	: pData()
	, m_PSPName("Protocol Port Log")
	, m_PSPStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PSPName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiProtocolPortLog()
//
//! \brief
//!   Description: Class constructor for the CScsiProtocolPortLog
//
//  Entry:
//! \param fileName = the name of the file that is to be read in
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiProtocolPortLog::CScsiProtocolPortLog(uint8_t *buffer, size_t bufferSize)
	:pData()
	, m_PSPName("Protocol Port Log")
	, m_PSPStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(bufferSize)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PSPName.c_str());
	}
    pData = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pData, buffer, bufferSize);
#else
    memcpy_s(pData, bufferSize, buffer, bufferSize);           // copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_PSPStatus = IN_PROGRESS;
	}
	else
	{
		m_PSPStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiProtocolPortLog
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
CScsiProtocolPortLog::~CScsiProtocolPortLog()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Events_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed event data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::process_Events_Data(JSONNODE *eventData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Phy Event Description \n");
#endif
	snprintf((char*)myStr.c_str(), BASIC, "Phy Event Description %" PRId8"",m_Event->eventSource);
	JSONNODE *eventInfo = json_new(JSON_NODE);
	json_set_name(eventInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Event->eventSource);
	json_push_back(eventInfo, json_new_a("Phy Event Source", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Event->event);
	json_push_back(eventInfo, json_new_a("Phy Event", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Event->threshold);
	json_push_back(eventInfo, json_new_a("Peak Value Detector Threshold", (char*)myStr.c_str()));

	json_push_back(eventData, eventInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Device_Type_Field
//
//! \brief
//!   Description: parser out the data for Descriptor Information
//
//  Entry:
//! \param description - string to give the description depending on what the code is
//! \param code - bits 4 -6 of the  attached device type and reason, code holds the bit value to the discription
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::get_Device_Type_Field(std::string *description, uint8_t code)
{
	switch (code)
	{
		case 0x01:
		{
			snprintf((char*)description->c_str(), BASIC, "1 = End device");
			break;
		}
		case 0x02:
		{
			snprintf((char*)description->c_str(), BASIC, "2 = Expander device");
			break;
		}
		case 0x03:
		{
			snprintf((char*)description->c_str(), BASIC, "3 = Expander device compliant with a previous version of the SAS standard");
			break;
		}
		default:
		{
			snprintf((char*)description->c_str(), BASIC, "Reserved");
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Reason_Field
//
//! \brief
//!   Description: parser out the data for the reason
//
//  Entry:
//! \param reason - string to give the reason depending on what the code is
//! \param code - bits 0-3 of the  attached device type and reason, code holds the bit value to the reson
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::get_Reason_Field(std::string *reason, uint8_t code)
{
	switch (code)
	{
		case 0x01:
		{
			snprintf((char*)reason->c_str(), BASIC, "Unknown reason");
			break;
		}
		case 0x02:
		{
			snprintf((char*)reason->c_str(), BASIC, "Power on");
			break;
		}
		case 0x03:
		{
			snprintf((char*)reason->c_str(), BASIC, "Hard reset"); 
			break;
		}
		case 0x04:
		{
			snprintf((char*)reason->c_str(), BASIC, "SMP PHY CONTROL function LINK RESET phy operation"); 
				break;
		}
		case 0x05:
		{
			snprintf((char*)reason->c_str(), BASIC, "After the multiplexing sequence complete"); 
				break;
		}
		case 0x06:
		{
			snprintf((char*)reason->c_str(), BASIC, "I_T nexus loss timer"); 
				break;
		}
		case 0x07:
		{
			snprintf((char*)reason->c_str(), BASIC, "Break Timeout Timer expired");
				break;
		}
		case 0x08:
		{
			snprintf((char*)reason->c_str(), BASIC, "Phy test function stopped"); 
				break;
		}
		case 0x09:
		{
			snprintf((char*)reason->c_str(), BASIC, "Expander device reduced functionality"); 
				break;
		}
		default:
		{
			snprintf((char*)reason->c_str(), BASIC, "Reserved");
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Negotiated_Logical_Link_Rate
//
//! \brief
//!   Description: parser out the data for negotiated link rate Information
//
//  Entry:
//! \param description - string to give the negotiated link rate depending on what the code is
//! \param code - bits 0 - 3 of the  attached Rate, code holds the bit value to the discription
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::get_Negotiated_Logical_Link_Rate(std::string *rate, uint8_t code)
{
	switch (code)
	{
		case 0x09:
		{
			snprintf((char*)rate->c_str(), BASIC, "1.5 GBs");
			break;
		}
		case 0x0a:
		{
			snprintf((char*)rate->c_str(), BASIC, "3.5  to 6 GBs");
			break;
		}
		case 0x0b:
		{
			snprintf((char*)rate->c_str(), BASIC, "6 - 12 GBs");
			break;
		}
		default:
		{
			snprintf((char*)rate->c_str(), BASIC, "Same as the NEGOTIATED LOGICAL LINK RATE field");
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_Descriptor_Information
//
//! \brief
//!   Description: parser out the data for Descriptor Information
//
//  Entry:
//! \param descData - Json node that the information will be added to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::process_Descriptor_Information(JSONNODE *descData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Descriptor Information \n");
#endif

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->phyIdent);
	json_push_back(descData, json_new_a("PHY Identifier", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->phyLength);
	json_push_back(descData, json_new_a("SAS Phy Descriptor Length", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->attached);
	json_push_back(descData, json_new_a("Attached Device Type and Reason", (char*)myStr.c_str()));
	get_Device_Type_Field(&myStr,M_GETBITRANGE(m_Descriptor->attached, 6, 4));
	json_push_back(descData, json_new_a("Attached Device Type Meaning", (char*)myStr.c_str()));
	get_Reason_Field(&myStr, M_GETBITRANGE(m_Descriptor->attached, 3, 0));
	json_push_back(descData, json_new_a("Attached Reason Meaning", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->reason);
	json_push_back(descData, json_new_a("Attached Rate", (char*)myStr.c_str()));
	get_Negotiated_Logical_Link_Rate(&myStr, M_GETBITRANGE(m_Descriptor->reason, 3, 0));
	json_push_back(descData, json_new_a("Attached Rate Meaning", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->initiatorPort);
	json_push_back(descData, json_new_a("Attached Initiator Port", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->targetPort);
	json_push_back(descData, json_new_a("Attached Target Port", (char*)myStr.c_str()));

	byte_Swap_64(&m_Descriptor->address);						// need to byte swap on SAS 64 bit
	byte_Swap_64(&m_Descriptor->attachedAddress);				// need to byte swap on SAS 64 bit
    snprintf((char*)myStr.c_str(), BASIC, "0x%014" PRIx64"", m_Descriptor->address);
    json_push_back(descData, json_new_a("World Wide Name", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%014" PRIx64"", m_Descriptor->attachedAddress);
    json_push_back(descData, json_new_a("Attached Address", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->attachedPhyIdent);
	json_push_back(descData, json_new_a("Attached Phy Identifier", (char*)myStr.c_str()));

	byte_Swap_32(&m_Descriptor->invalidDwordCount);				// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->disparityErrorCount);			// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->synchronization);				// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->phyResetProblem);				// need to byte swap on SAS 
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Descriptor->invalidDwordCount);
	json_push_back(descData, json_new_a("Invalid Dword Count", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Descriptor->disparityErrorCount);
	json_push_back(descData, json_new_a("Running Dispariity Error Count", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Descriptor->synchronization);
	json_push_back(descData, json_new_a("Loss Of DWORD Synchronization", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%08" PRIx32"", m_Descriptor->phyResetProblem);
	json_push_back(descData, json_new_a("Phy Reset Problem", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->phyEventLength);
	json_push_back(descData, json_new_a("Phy Event Descriptor Length", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Descriptor->numberOfEvents);
	json_push_back(descData, json_new_a("Number of Pyh Events", (char*)myStr.c_str()));

}
//-----------------------------------------------------------------------------
//
//! \fn process_List_Information
//
//! \brief
//!   Description: parser out the data for List Information
//
//  Entry:
//! \param listData - Json node that the information will be added to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::process_List_Information(JSONNODE *listData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("List Information \n");
#endif
	
	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_List->paramCode);
	json_push_back(listData, json_new_a("Relative Target Port Identifier", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_List->paramControlByte);
	json_push_back(listData, json_new_a("Relative Target Port Control Byte", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_List->paramLength);
	json_push_back(listData, json_new_a("Relative Target Port Length", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_List->ident);
	json_push_back(listData, json_new_a("Port Identifier", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_List->genCode);
	json_push_back(listData, json_new_a("Generation Code", (char*)myStr.c_str()));
	json_push_back(listData, json_new_i("Number of Phys", m_List->numberOfPhys));
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data
//
//! \brief
//!   Description: parser out the data for the Protocol Port Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiProtocolPortLog::get_Data(JSONNODE *masterData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	eReturnValues retStatus = IN_PROGRESS;

	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Protocol-Specific Port Log Page - 18h");

		for (size_t offset = 0; (offset < m_PageLength && offset < UINT16_MAX);)
		{
			m_List = (sRelativeTargetPort *)&pData[offset];
			// process the List header information
			byte_Swap_16(&m_List->paramCode);
			snprintf((char*)myStr.c_str(), BASIC, "Relative Target Port Information %" PRId16"", m_List->paramCode);
			JSONNODE *listInfo = json_new(JSON_NODE);
			json_set_name(listInfo, (char*)myStr.c_str());
			process_List_Information(listInfo);
			offset += sizeof(sRelativeTargetPort);
			m_Descriptor = (sSASPHYLogDescriptorList *)&pData[offset];

			if((offset + m_Descriptor->phyLength) <= m_bufferLength)
			{
				offset += sizeof(sSASPHYLogDescriptorList);
				// process the descriptor information
				snprintf((char*)myStr.c_str(), BASIC, "Descriptor Information %" PRId8"", m_Descriptor->phyIdent);
				JSONNODE *descInfo = json_new(JSON_NODE);
				json_set_name(descInfo, (char*)myStr.c_str());
				process_Descriptor_Information(descInfo);

				for (uint32_t events = 1; events <= m_Descriptor->numberOfEvents; events++)
				{
					// check the length to the end of the buffer
					if ((offset + m_Descriptor->phyEventLength) < m_bufferLength)
					{
						m_Event = (sPhyEventDescriptor *)&pData[offset];
						offset += m_Descriptor->phyEventLength;
						//process events
						process_Events_Data(descInfo);
					}
					else
					{
						json_push_back(listInfo, descInfo);
						json_push_back(pageInfo, listInfo);
						json_push_back(masterData, pageInfo);
						return BAD_PARAMETER;
					}
				}
				json_push_back(listInfo, descInfo);
			}
			else
			{
				json_push_back(pageInfo, listInfo);
				json_push_back(masterData, pageInfo);
				return BAD_PARAMETER;
			}
			json_push_back(pageInfo, listInfo);
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