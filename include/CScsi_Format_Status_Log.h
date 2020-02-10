// CScsi_Format_Status_Log.h  Format Status log page reports information about the most recent successful format operation
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Format_Status_Log.h  Format Status log page reports information about the most recent successful format operation
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIFORMATLOG
#define SCSIFORMATLOG

#pragma pack(push, 1)
	typedef struct _sFormatStatusParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		_sFormatStatusParameters() : paramCode(0), paramControlByte(0), paramLength(0) {};
	} sFormatParams;

#pragma pack(pop)

	class CScsiFormatStatusLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_FormatName;				//<! class name	
		eReturnValues				m_FormatStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint64_t					m_Value;					//<! Parameter Value
		sFormatParams				*m_Format;					//<! format status structure 

		void get_Format_Log_Value_Description(std::string *valueData);
		void get_Format_Log_Header_Description(std::string *headerData);
		void get_Format_Status_Descriptions(JSONNODE *formatData);
		eReturnValues get_Format_Status_Data(JSONNODE *masterData);
	public:
		CScsiFormatStatusLog();
		CScsiFormatStatusLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiFormatStatusLog();
		virtual eReturnValues get_Log_Status() { return m_FormatStatus; };
		virtual eReturnValues parse_Format_Status_Log(JSONNODE *masterData) { return get_Format_Status_Data(masterData); };

	};
#endif
}
