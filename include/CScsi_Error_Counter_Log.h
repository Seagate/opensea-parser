// CScsi_Error_Counter_Log.h  Definition of Error Counter for READ WRITE VERIFY ERRORS
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

// \file CScsi_Error_Counter_Log.h  Definition of Error Counter for READ WRITE VERIFY ERRORS
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIERRORLOG
#define SCSIERRORLOG

#define WRITE  0x02
#define READ   0x03
#define VERIFY 0x05
#pragma pack(push, 1)
	typedef struct _sErrorParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		_sErrorParameters() : paramCode(0), paramControlByte(0), paramLength(0) {};
	} sErrorParams;

#pragma pack(pop)
	class CScsiErrorCounterLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_ErrorName;				//<! class name	
		eReturnValues				m_ErrorStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint64_t					m_ErrorValue;				//<! Parameter Error Value
		sErrorParams				*m_Error;					//<! Error counter structure 
		uint8_t						m_pageType;					//<! need to know if it's a write read or verify log

        void append_Error_Log_Page_Number(std::string *typeStr, std::string main);
		void set_Master_String(std::string *typeStr, std::string main);
		bool get_Error_Parameter_Code_Description(std::string *error);
		void process_Error_Data(JSONNODE *errorData);
		eReturnValues get_Error_Counter_Data(JSONNODE *masterData);
	public:
		CScsiErrorCounterLog();
		CScsiErrorCounterLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength, uint8_t type);
		virtual ~CScsiErrorCounterLog();
		virtual eReturnValues get_Log_Status() { return m_ErrorStatus; };
		virtual eReturnValues parse_Error_Counter_Log(JSONNODE *masterData) { return get_Error_Counter_Data(masterData); };

	};
#endif
}
