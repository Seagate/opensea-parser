// CScsi_Protocol_Specific_Port_Log.h   Definition of Protocol-Specific Port log page for SAS
//
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
#pragma once
#include <string>
#include "common_types.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIPORTLOG
#define SCSIPORTLOG

#pragma pack(push, 1)
	typedef struct _sRelativeTargetPort
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		uint8_t			ident;								//<! The PROTOCOL IDENTIFIER field is defined
		uint8_t			reserved;
		uint8_t			genCode;							//<! GENERATION CODE field
		uint8_t			numberOfPhys;						//<! The NUMBER OF PHYS field contains the number of phys in the SAS target port
	} sRelativeTargetPort;

	typedef struct _sSASPHYLogDescriptorList
	{
		uint8_t			reserved;
		uint8_t			phyIdent;							//<! Phy Identifier
		uint8_t			reserved1;
		uint8_t			phyLength;							//<! SAS PHY LOG DESCRIPTOR LENGTH
		uint8_t			attached;							//<! ATTACHED DEVICE TYPE and ATTACHED REASON
		uint8_t			reason;								//<! Reason and Negotiated logical link rate
		uint8_t			initiatorPort;						//<! initator port
		uint8_t			targetPort;							//<! attached target port
		uint64_t		address;							//<! SAS Address
		uint64_t		attachedAddress;					//<! attached SAS Address
		uint8_t			attachedPhyIdent;					//<! The ATTACHED PHY IDENTIFIER field contains the phy identifier of the attached SAS phy in the attached SAS device.
		uint32_t		reserved2;
		uint16_t		reserved3;
		uint8_t			reserved4;
		uint32_t		invalidDwordCount;					//<! Invalid DWORD count
		uint32_t		disparityErrorCount;				//<! Running Disparity Error Count
		uint32_t		synchronization;					//<! loss of DWORD Synchronization
		uint32_t		phyResetProblem;					//<! PHY Reset Problem
		uint16_t		reserved5;
		uint8_t			phyEventLength;						//<! PHY event Descriptor Lenght
		uint8_t			numberOfEvents;						//<! Number of PHY event Descriptors
	} sSASPHYLogDescriptorList;

	typedef struct _sPhyEventDescriptor
	{
		uint16_t		reserved;
		uint8_t			reserved1;
		uint8_t			eventSource;						//<! PHY Event Source
		uint32_t		event;								//<! the PHY EVENT
		uint32_t		threshold;							//<! Peak value Detector Threshold
	} sPhyEventDescriptor;
#pragma pack(pop)

	class CScsiProtocolPortLog
	{
	private:

	protected:
		uint8_t						* pData;					//<! pointer to the data
		std::string					m_PSPName;					//<! class name	
		eReturnValues				m_PSPStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sRelativeTargetPort			*m_List;					//<! Relative Target Port Identifier list 
		sSASPHYLogDescriptorList	*m_Descriptor;				//<! pointer to the top of the phy descripter list structure
		sPhyEventDescriptor			*m_Event;					//<! pointer to the first event in the descripter list

		eReturnValues get_Data(JSONNODE *masterData);
		void get_Device_Type_Field(std::string *description, uint8_t code);
		void get_Reason_Field(std::string *reason, uint8_t code);
		void process_Event_Description(std::string* source, uint8_t event);
		void get_Negotiated_Logical_Link_Rate(std::string* reason, uint8_t code);

		void process_Events_Data(JSONNODE *eventData);
		void process_Descriptor_Information(JSONNODE *descData);
		void process_List_Information(JSONNODE *listData);
	public:
		CScsiProtocolPortLog();
		CScsiProtocolPortLog(uint8_t * buffer, size_t bufferSize);
		virtual ~CScsiProtocolPortLog();
		virtual eReturnValues get_Log_Status() { return m_PSPStatus; };
		virtual void set_PSP_Page_Length(uint16_t page) { m_PageLength = page; byte_Swap_16(&m_PageLength); };
        virtual void set_PSP_Page_Length_NoSwap(uint16_t page) { m_PageLength = page; };
		virtual eReturnValues parse_Protocol_Port_Log(JSONNODE *masterData) { return get_Data(masterData); };
	};
#endif
}
