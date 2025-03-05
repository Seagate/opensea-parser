// CScsi_Background_Operation_Log.h  Definition of Background Operation log page reports parameters that are specific to background operations.
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

// \file CScsi_Background_Operation_Log.h  Definition of Background Operation log page reports parameters that are specific to background operations.
#pragma once
#include <string>
#include "common_types.h"
#include "bit_manip.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIOPERATIONLOG
#define SCSIOPERATIONLOG
#pragma pack(push, 1)
	typedef struct _sBackgroundOperationParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		uint8_t			bo_Status;							//<! Background Operation Status
		uint8_t			reserved;							//<! reserved
		uint16_t		reserved1;							//<! reserved
		_sBackgroundOperationParameters() : paramCode(0), paramControlByte(0), paramLength(0), bo_Status(0), reserved(0), reserved1(0) {};
	} sOperationParams;

#pragma pack(pop)
	class CScsiOperationLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_OperationName;			//<! class name	
		eReturnValues				m_OperationsStatus;			//<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sOperationParams			*m_Operation;				//<! operation structure 

		void get_Background_Operations_status(std::string *status);
		void process_Background_Operations_Data(JSONNODE *operationData,uint32_t offset);
		eReturnValues get_Background_Operations_Data(JSONNODE *masterData);
	public:
		CScsiOperationLog();
		CScsiOperationLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiOperationLog();
		virtual eReturnValues get_Log_Status() { return m_OperationsStatus; };
		virtual eReturnValues parse_Background_Operationss_Log(JSONNODE *masterData) { return get_Background_Operations_Data(masterData); };

	};
#endif
}

