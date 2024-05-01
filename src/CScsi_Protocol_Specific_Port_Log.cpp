//
// CScsi_Protocol_Specific_Port_Log.h   Definition of Protocol-Specific Port log page for SAS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
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
	, m_PSPStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(0)
	, m_List(NULL)
	, m_Descriptor(NULL)
	, m_Event(NULL)
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
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
	, m_PSPStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(bufferSize)
	, m_List(NULL)
	, m_Descriptor(NULL)
	, m_Event(NULL)
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PSPName.c_str());
	}
    pData = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
    memcpy(pData, buffer, bufferSize);
#else
    memcpy_s(pData, bufferSize, buffer, bufferSize);           // copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_PSPStatus = eReturnValues::IN_PROGRESS;
	}
	else
	{
		m_PSPStatus = eReturnValues::FAILURE;
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
#if defined _DEBUG
	printf("Phy Event Description \n");
#endif
    std::ostringstream temp;
    temp << "Phy Event Description " << std::dec << static_cast<uint16_t>(m_Event->eventSource);
	JSONNODE *eventInfo = json_new(JSON_NODE);
	json_set_name(eventInfo, temp.str().c_str());

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Event->eventSource);
	json_push_back(eventInfo, json_new_a("Phy Event Source", temp.str().c_str()));

    byte_Swap_32(&m_Event->event);              // need to byte swap on SAS 
    byte_Swap_32(&m_Event->threshold);          // need to byte swap on SAS 
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Event->event;
	json_push_back(eventInfo, json_new_a("Phy Event", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Event->threshold;
	json_push_back(eventInfo, json_new_a("Peak Value Detector Threshold", temp.str().c_str()));

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
			description->assign("1 = End device");
			break;
		}
		case 0x02:
		{
            description->assign("2 = Expander device");
			break;
		}
		case 0x03:
		{
            description->assign("3 = Expander device compliant with a previous version of the SAS standard");
			break;
		}
		default:
		{
            description->assign("Reserved");
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
			reason->assign("Unknown reason");
			break;
		}
		case 0x02:
		{
            reason->assign("Power on");
			break;
		}
		case 0x03:
		{
            reason->assign("Hard reset");
			break;
		}
		case 0x04:
		{
            reason->assign("SMP PHY CONTROL function LINK RESET phy operation");
				break;
		}
		case 0x05:
		{
            reason->assign("After the multiplexing sequence complete");
				break;
		}
		case 0x06:
		{
            reason->assign("I_T nexus loss timer");
				break;
		}
		case 0x07:
		{
            reason->assign("Break Timeout Timer expired");
				break;
		}
		case 0x08:
		{
            reason->assign("Phy test function stopped");
				break;
		}
		case 0x09:
		{
            reason->assign("Expander device reduced functionality");
				break;
		}
		default:
		{
            reason->assign("Reserved");
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_Event_Description
//
//! \brief
//!   Description: parser out the data for phy event source 
//
//  Entry:
//! \param source - string to give the event a name
//! \param event - code holds event description
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiProtocolPortLog::process_Event_Description(std::string* source, uint8_t event)
{
	switch (event)
	{
	case 0x00:
	{
		*source = "no event";
		break;
	}
	case 0x01:
	{
		*source = "invalid dwords";
		break;
	}
	case 0x02:
	{
		*source = "running disparity errors";
		break;
	}
	case 0x03:
	{
		*source = "loss of dword sync";
		break;
	}
	case 0x04:
	{
		*source = "phy reset problems";
		break;
	}
	case 0x05:
	{
		*source = "elasticity buffer overflows";
		break;
	}
	case 0x06:
	{
		*source = "received errors";
		break;
	}
	case 0x07:
		*source = "invalid SPL packets";
		break;
	case 0x08:
		*source = "loss of SPL packet sync";
		break;
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f:
	{
		*source = "reserved for phy layer-based phy events";
		break;
	}
	case 0x20:
	{
		*source = "received address frame errors";
		break;
	}
	case 0x21:
	{
		*source = "transmitted abandon-class open_rejects";
		break;
	}
	case 0x22:
	{
		*source = "received abandon-class open_rejects";
		break;
	}
	case 0x23:
	{
		*source = "transmitted retry-class open_rejects";
		break;
	}
	case 0x24:
	{
		*source = "received retry-class open_rejects";
		break;
	}
	case 0x25:
	{
		*source = "received (waiting on partial) apis";
		break;
	}
	case 0x26:
	{
		*source = "received (waiting on connection) apis";
		break;
	}
	case 0x27:
	{
		*source = "transmitted breaks";
		break;
	}
	case 0x28:
	{
		*source = "received breaks";
		break;
	}
	case 0x29:
	{
		*source = "break timeouts";
		break;
	}
	case 0x2a:
	{
		*source = "connections";
		break;
	}
	case 0x2b:
	{
		*source = "peak transmitted pathway blocks";
		break;
	}
	case 0x2c:
	{
		*source = "peak transmitted arbitration wait time";
		break;
	}
	case 0x2d:
	{
		*source = "peak arbitration time";
		break;
	}
	case 0x2e:
	{
		*source = "peak connection time pvd the peak duration, in microseconds, of any connection in which the phy was involved";
		break;
	}
	case 0x2f:
		*source = "persistent connections";
		break;
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:
	case 0x38:
	case 0x39:
	case 0x3a:
	case 0x3b:
	case 0x3c:
	case 0x3d:
	case 0x3e:
	case 0x3f:
	{
		*source = "reserved for sas arbitration-related phy information";
		break;
	}
	case 0x40:
	{
		*source = "transmitted ssp frames";
		break;
	}
	case 0x41:
	{
		*source = "received ssp frames";
		break;
	}
	case 0x42:
	{
		*source = "transmitted ssp frame errors";
		break;
	}
	case 0x43:
	{
		*source = "received ssp frame errors";
		break;
	}
	case 0x44:
	{
		*source = "transmitted credit_blockeds";
		break;
	}
	case 0x45:
	{
		*source = "received credit_blockeds";
		break;
	}
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
	{
		*source = "reserved for ssp-related phy events";
		break;
	}
	case 0x50:
	{
		*source = "transmitted sata frames";
		break;
	}
	case 0x51:
	{
		*source = "received sata frames";
		break;
	}
	case 0x52:
	{
		*source = "sata flow control buffer overflows";
		break;
	}
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f:
	{
		*source = "reserved for stp and sata-related phy events";
		break;
	}
	case 0x60:
	{
		*source = "transmitted smp frames";
		break;
	}
	case 0x61:
	{
		*source = "received smp frames";
		break;
	}
	case 0x62:
	{
		*source = "reserved for smp-related phy events";
		break;
	}
	case 0x63:
	{
		*source = "received smp frame errors";
		break;
	}
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x6a:
	case 0x6b:
	case 0x6c:
	case 0x6d:
	case 0x6e:
	case 0x6f:
	{
		*source = "reserved for smp-related phy events";
		break;
	}
	default:
	{
		*source = "vendor specific";
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
			rate->assign("1.5 GBs");
			break;
		}
		case 0x0a:
		{
            rate->assign("3.5  to 6 GBs");
			break;
		}
		case 0x0b:
		{
            rate->assign("6 - 12 GBs");
			break;
		}
		case 0x0c:
		{
            rate->assign("22.5 GBs");
			break;
		}
		default:
		{
            rate->assign("Same as the NEGOTIATED LOGICAL LINK RATE field");
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
	std::string myStr;
#if defined _DEBUG
	printf("Descriptor Information \n");
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->phyIdent);
	json_push_back(descData, json_new_a("PHY Identifier", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->phyLength);
	json_push_back(descData, json_new_a("SAS Phy Descriptor Length", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->attached);
	json_push_back(descData, json_new_a("Attached Device Type and Reason", temp.str().c_str()));
    temp.str("");temp.clear();
	get_Device_Type_Field(&myStr,M_GETBITRANGE(m_Descriptor->attached, 6, 4));
	json_push_back(descData, json_new_a("Attached Device Type Meaning", myStr.c_str()));
	get_Reason_Field(&myStr, M_GETBITRANGE(m_Descriptor->attached, 3, 0));
	json_push_back(descData, json_new_a("Attached Reason Meaning", myStr.c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->reason);
	json_push_back(descData, json_new_a("Attached Rate", temp.str().c_str()));
	get_Negotiated_Logical_Link_Rate(&myStr, M_GETBITRANGE(m_Descriptor->reason, 3, 0));
	json_push_back(descData, json_new_a("Attached Rate Meaning", myStr.c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->initiatorPort);
	json_push_back(descData, json_new_a("Attached Initiator Port", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->targetPort);
	json_push_back(descData, json_new_a("Attached Target Port", temp.str().c_str()));

	byte_Swap_64(&m_Descriptor->address);						// need to byte swap on SAS 64 bit
	byte_Swap_64(&m_Descriptor->attachedAddress);				// need to byte swap on SAS 64 bit
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(14) << m_Descriptor->address;
    json_push_back(descData, json_new_a("World Wide Name", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(14) << m_Descriptor->attachedAddress;
    json_push_back(descData, json_new_a("Attached Address", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->attachedPhyIdent);
	json_push_back(descData, json_new_a("Attached Phy Identifier", temp.str().c_str()));

	byte_Swap_32(&m_Descriptor->invalidDwordCount);				// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->disparityErrorCount);			// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->synchronization);				// need to byte swap on SAS 
	byte_Swap_32(&m_Descriptor->phyResetProblem);				// need to byte swap on SAS 
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Descriptor->invalidDwordCount;
	json_push_back(descData, json_new_a("Invalid Dword Count", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Descriptor->disparityErrorCount;
	json_push_back(descData, json_new_a("Running Dispariity Error Count", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Descriptor->synchronization;
	json_push_back(descData, json_new_a("Loss Of DWORD Synchronization", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << m_Descriptor->phyResetProblem;
	json_push_back(descData, json_new_a("Phy Reset Problem", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->phyEventLength);
	json_push_back(descData, json_new_a("Phy Event Descriptor Length", temp.str().c_str()));
	temp.str(""); temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Descriptor->numberOfEvents);
	json_push_back(descData, json_new_a("Number of Pyh Events", temp.str().c_str()));

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
#if defined _DEBUG
	printf("List Information \n");
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_List->paramCode;
	json_push_back(listData, json_new_a("Relative Target Port Identifier", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_List->paramControlByte);
	json_push_back(listData, json_new_a("Relative Target Port Control Byte", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_List->paramLength);
	json_push_back(listData, json_new_a("Relative Target Port Length", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_List->ident);
	json_push_back(listData, json_new_a("Port Identifier", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_List->genCode);
	json_push_back(listData, json_new_a("Generation Code", temp.str().c_str()));
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
	eReturnValues retStatus = eReturnValues::IN_PROGRESS;

	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Protocol-Specific Port Log Page - 18h");

		for (size_t offset = 0; (offset < m_PageLength && offset < UINT16_MAX);)
		{
			m_List = reinterpret_cast<sRelativeTargetPort *>(&pData[offset]);
			// process the List header information
			byte_Swap_16(&m_List->paramCode);
            std::ostringstream temp;
            temp << "Relative Target Port Information " << std::dec << m_List->paramCode;
			JSONNODE *listInfo = json_new(JSON_NODE);
			json_set_name(listInfo, temp.str().c_str());
			process_List_Information(listInfo);
			offset += sizeof(sRelativeTargetPort);
			m_Descriptor = reinterpret_cast<sSASPHYLogDescriptorList *>(&pData[offset]);

			if((offset + m_Descriptor->phyLength) <= m_bufferLength)
			{
				offset += sizeof(sSASPHYLogDescriptorList);
				// process the descriptor information
                temp.str("");temp.clear();
                temp << "Descriptor Information " << std::dec << static_cast<uint16_t>(m_Descriptor->phyIdent);
				JSONNODE *descInfo = json_new(JSON_NODE);
				json_set_name(descInfo, temp.str().c_str());
				process_Descriptor_Information(descInfo);

				for (uint32_t events = 1; events <= m_Descriptor->numberOfEvents; events++)
				{
					// check the length to the end of the buffer
					if ((offset + m_Descriptor->phyEventLength) <= m_bufferLength)
					{
						m_Event = reinterpret_cast<sPhyEventDescriptor *>(&pData[offset]);
						offset += m_Descriptor->phyEventLength;
						//process events
						process_Events_Data(descInfo);
					}
					else
					{
						json_push_back(listInfo, descInfo);
						json_push_back(pageInfo, listInfo);
						json_push_back(masterData, pageInfo);
						return eReturnValues::BAD_PARAMETER;
					}
				}
				json_push_back(listInfo, descInfo);
			}
			else
			{
				json_push_back(pageInfo, listInfo);
				json_push_back(masterData, pageInfo);
				return eReturnValues::BAD_PARAMETER;
			}
			json_push_back(pageInfo, listInfo);
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