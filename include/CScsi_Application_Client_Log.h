// CScsi_Application_Client_Log.h  Definition of Application Client Log where clients store information
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2023 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

#define MAX_PARTITION 64 //As per the spec, 
#define APP_CLIENT_DATA_LEN 252 //header(4 bytes) + data (252 bytes) = 256 bytes

#pragma pack(push, 1)
	typedef struct _sApplicationClientParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
        uint8_t         *data;                              //<! pointer to the data in the buffer
        _sApplicationClientParameters() 
        {
            paramCode = 0;
            paramControlByte = 0;
            paramLength = 0;
            data = NULL;
        }
        _sApplicationClientParameters(uint8_t* buffer)
        {
#define byte2 2  
#define byte3 3
#define byte4 4
            paramCode = *(reinterpret_cast<uint16_t*>(buffer));
            paramControlByte = static_cast<uint8_t>(buffer[byte2]);
            paramLength = static_cast<uint8_t>(buffer[byte3]);
            data = static_cast<uint8_t*>(&buffer[byte4]);
        }

	} sApplicationParams;
#pragma pack(pop)

	class CScsiApplicationLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_ApplicationName;			//<! class name	
		eReturnValues				m_ApplicationStatus;		//<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
        size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sApplicationParams			*m_App;						//<! Application client structure 


		void process_Client_Data(JSONNODE *appData);
		eReturnValues get_Client_Data(JSONNODE *masterData);
	public:
		CScsiApplicationLog();
		CScsiApplicationLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiApplicationLog();
		virtual eReturnValues get_Log_Status() { return m_ApplicationStatus; };
		virtual eReturnValues parse_Application_Client_Log(JSONNODE *masterData) 
		{
			return get_Client_Data(masterData);
		};

	};
#endif
}
