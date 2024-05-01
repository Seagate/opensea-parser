//
// CScsi_Environmental_Logs.h   Definition of Base class Temperature logs
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

// \file CScsi_Environmental_Logs.h  Definition of the Temperature Classes 
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "CScsi_Temperature_Log.h"

namespace opensea_parser {
#ifndef SCSIENVIRONMENTLOG
#define SCSIENVIRONMENTLOG

	class CScsiEnvironmentLog : virtual public CScsiTemperatureLog
	{
	private:

	protected:
		sLogPageStruct				*m_Page;					//<! page code for the log lpage format
		std::string					m_EvnName;					//<! class name	
		eReturnValues				m_EnvStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! byte swapped length of the page
		uint8_t						m_SubPage;					//<! passed in- this is subpage that tell us what log to parse as.

		eReturnValues figureout_What_Log_To_Parsed(JSONNODE *masterData);
	public:
		CScsiEnvironmentLog();
		CScsiEnvironmentLog(uint8_t *bufferData, size_t bufferSize, uint8_t subPage, JSONNODE *masterData);
		virtual ~CScsiEnvironmentLog();
		virtual eReturnValues get_Log_Status() { return m_EnvStatus; };

	};
#endif
}
