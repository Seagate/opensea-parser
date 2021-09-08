//
// CScsi_Self_Test_Results_Log.h  Definition of DST Results log 
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

// \file CScsi_Self_Test_Results_Log.h  Definition of DST Results log 
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIDSTLOG
#define SCSIDSTLOG

#define DST_TESTS  20
#pragma pack(push, 1)
	enum m_DST_Enum
	{
		DST_COMPLETED_WITHOUT_ERROR = 0x00,
		DST_BACKGROUND_ABORTED = 0x01,
		DST_ABORTED = 0x02,
		DST_UNKNOWN_ERROR = 0x03,
		DST_FAILURE_UNKNOWN_SEGMENT = 0x04,
		DST_FAILURE_FIRST_SEGMENT = 0x05,
		DST_FAILURE_SECOND_SEGMENT = 0x06,
		DST_FAILURE_CHECK_NUMBER_FOR_SEGMENT = 0x07,
        DST_FAILURE_HANDLING_DAMAGE = 0x08,
        DST_FAILURE_SUSPECTED_HANDLING_DAMAGE =0x09,   
		DST_IN_PROGRESS = 0x0f
	};
	enum m_DST_Test_Type
	{
		DST_NOT_RUN,
		DST_BACKGROUND_SHORT,
		DST_BACKGROUND_EXTENDED,
		DST_RESERVED,
		DST_ABORT_BACKGROUND,
		DST_FORGROUND_SHORT,
		DST_FORGROUND_EXTENDED,
		DST_RESERVED2,
	};
	typedef struct _sSelfTest
	{
		uint16_t			paramCode;						//<! parameter code for the log for each self test  
		uint8_t				paramControlByte;				//<! param code for the log page format
		uint8_t				paramLength;					//<! param length.
		uint8_t				stCode;							//<! self test code and results
		uint8_t				stNumber;						//!< self test number
		uint16_t			accPOH;							//!< Accumulated Power on Hours
		uint64_t			address;						//!< address of firmst Failure
		uint8_t				senseKey;						//!< Sense Key
		uint8_t				addSenseCode;					//!< Additional Sense code
		uint8_t				addSenseCodeQualifier;			//!< Additional Sense code
		uint8_t				vendor;							//!< Vendor Specific
	}sSelfTest;
#pragma pack(pop)

	class CScsi_DST_Results
	{
	private:

	protected:
		std::string					m_DSTName;					//<! class name	
		eReturnValues				m_DSTStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! byte swapped length of the page
		uint8_t						m_SubPage;					//<! passed in- this is subpage that tell us what log to parse as.
        sSelfTest					*m_DST;						//<! single dst results

		eReturnValues get_Self_Test_Log(uint8_t * buffer, size_t bufferSize, JSONNODE *masterJson);
		eReturnValues get_PrePython_Self_Test_Log(uint8_t* buffer, size_t bufferSize, JSONNODE* masterJson);
		void print_Self_Test_Log(JSONNODE *dstNode, uint16_t run,uint32_t offset);
		void get_Self_Test_Results_String(std::string & meaning, uint8_t result);
		void get_DST_PrePython_Results_String(std::string& meaning, uint8_t result);
		void byte_Swap_Self_Test();
	public:
		CScsi_DST_Results();
		CScsi_DST_Results(uint8_t *bufferData, size_t bufferSize, JSONNODE *masterJson);
		virtual ~CScsi_DST_Results();
		virtual eReturnValues get_Log_Status() { return m_DSTStatus; };
		virtual eReturnValues parse_Self_Test_Log(uint8_t* buffer, size_t bufferSize, JSONNODE* masterJson)
		{
			if (g_dataformat == PREPYTHON_DATA)
			{
				return get_PrePython_Self_Test_Log(buffer, bufferSize, masterJson);
			}
			else
			{
				return get_Self_Test_Log(buffer, bufferSize, masterJson);
			}
		};
	};
#endif
}
