// CScsi_Application_Client_Log.h  Definition of Application Client Log where clients store information
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Application_Client_Log.h  Definition of Application Client Log where clients store information
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIAPPLICATIONLOG
#define SCSIAPPLICATIONLOG

	class CScsiApplicationLog
	{
	private:
#pragma pack(push, 1)
		typedef struct _sApplicationClientParameters
		{
			uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
			uint8_t			paramControlByte;					//<! binary format list log parameter
			uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
			uint8_t			data[252];
			_sApplicationClientParameters() : paramCode(0), paramControlByte(0), paramLength(0), data{ {0} } {};
		} sApplicationParams;

#pragma pack(pop)
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_ApplicationName;			//<! class name	
		eReturnValues				m_ApplicationStatus;		//<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sApplicationParams			*m_App;						//<! Application client structure 

		void process_Client_Data(JSONNODE *clientData);
		eReturnValues get_Client_Data(JSONNODE *masterData);
	public:
		CScsiApplicationLog();
		CScsiApplicationLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiApplicationLog();
		virtual eReturnValues get_Log_Status() { return m_ApplicationStatus; };
		virtual eReturnValues parse_Application_Client_Log(JSONNODE *masterData) { return get_Client_Data(masterData); };

	};
#endif
}