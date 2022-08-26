// CScsi_Logical_Block_Provisioning_Log.h  Definition of Logical Block Provisioning Log page
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Logical_Block_Provisioning_Log.h  Definition of Logical Block Provisioning Log page reports the logical block provisioning status 
//   of the logical
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSILBALOG
#define SCSILBALOG
#pragma pack(push, 1)
	typedef struct _sLogical_Block_ProvisioningParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		uint32_t		resourceCount;						//<! The RESOURCE COUNT field indicates an estimate of the number of Available Provisioning resources
		uint32_t		reserved;							//<! Remander of the reserved area
		_sLogical_Block_ProvisioningParameters() : paramCode(0), paramControlByte(0), paramLength(0), resourceCount(0), reserved(0) {};
	} sLBA;
#pragma pack(pop)

	class CScsiLBAProvisionLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_LBAName;					//<! class name	
		eReturnValues				m_LBAStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sLBA						*m_Provision;				//<! structure for most of the parameters in the log

		void get_Resource_Percentage(std::string *percentStr);
		void get_LBA_Provision_Parameter_Description(std::string *lbaStr);
		void process_LBA_Provision_Data(JSONNODE *lbaData);
		eReturnValues get_LBA_Data(JSONNODE *masterData);
	public:
		CScsiLBAProvisionLog();
		CScsiLBAProvisionLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiLBAProvisionLog();
		virtual eReturnValues get_Log_Status() { return m_LBAStatus; };
		virtual eReturnValues parse_LBA_Provision_Log(JSONNODE *masterData) { return get_LBA_Data(masterData); };

	};
#endif
}
