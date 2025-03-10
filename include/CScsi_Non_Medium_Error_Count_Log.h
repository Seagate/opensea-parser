// CScsi_Non_Medium_Error_Count_Log.h  Definition of the Non-Medium Error Count Log
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

// \file CScsi_Non_Medium_Error_Count_Log.h  Definition of the Non-Medium Error Count Log provides for counting the occurrences of recoverable error events other than
//   write, read, or verify failures.
#pragma once
#include <string>
#include "common_types.h"
#include "bit_manip.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIMEDIUMLOG
#define SCSIMEDIUMLOG

#define NONMEDIUMERRORLOG 0x6
#define NONMEDIUMERRORSUBPAGE 0x00

	class CScsiNonMediumErrorCountLog
	{
	private:

	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_NMECName;					//<! class name	
		eReturnValues				m_NMECStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint64_t					m_Value;					//<! the total count of errors
		sLogParams*					m_CountErrors;				//<! params for the error count log


		void process_Non_Medium_Error_Count_Data(JSONNODE *countData);
		eReturnValues get_Non_Medium_Error_Count_Data(JSONNODE *masterData);

	public:
		CScsiNonMediumErrorCountLog();
		CScsiNonMediumErrorCountLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiNonMediumErrorCountLog();
		virtual eReturnValues get_Log_Status() { return m_NMECStatus; };
		virtual eReturnValues parse_Non_Medium_Error_Count_Log(JSONNODE *masterData) 
		{ 
			return get_Non_Medium_Error_Count_Data(masterData);
		};

	};
#endif
}