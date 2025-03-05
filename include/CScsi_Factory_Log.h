// CScsi_Factory_Log.h  Definition of Factory Log page for SAS
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

// \file CScsi_Factory_Log.h  Definition of Factory Log page for SAS
#pragma once
#include <string>
#include "common_types.h"
#include "bit_manip.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIFACTORYLOG
#define SCSIFACTORYLOG

	class CScsiFactoryLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_FactoryName;				//<! class name	
		eReturnValues				m_FactoryStatus;		    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint64_t					m_Value;					//<! Parameter Value
		sLogParams					*m_factory;					//<! factory structure 

		void process_Factorty_Data(JSONNODE *factoryData);
		eReturnValues get_Factory_Data(JSONNODE *masterData);
	public:
		CScsiFactoryLog();
		CScsiFactoryLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiFactoryLog();
		virtual eReturnValues get_Log_Status() { return m_FactoryStatus; };
		virtual eReturnValues parse_Factory_Log(JSONNODE *masterData) { return get_Factory_Data(masterData); };

	};
#endif
}
