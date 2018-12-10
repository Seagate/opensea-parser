
// CScsi_Power_Condition_Transitions_Log.h  Definition of Power Condition Transistions Log Page for SAS
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

// \file CScsi_Power_Condition_Transitions_Log.h   Definition of Power Condition Transistions Log Page for SAS
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIPOWERLOG
#define SCSIPOWERLOG


	class CScsiPowerConditiontLog
	{
	private:
#pragma pack(push, 1)
		typedef enum _eTransitionsTypes
		{
			ACTIVE = 0x0001,
			IDLE_A = 0x0002,
			IDLE_B = 0x0003,
			IDLE_C = 0x0004,
			STANDZ = 0x0008,
			STANDY = 0x0009
		}eTransitionsTypes;
		typedef struct _sPowerConditionTransisionsParameters
		{
			uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
			uint8_t			paramControlByte;					//<! binary format list log parameter
			uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
			uint32_t		paramValue;							//<! Parameter Value
		} sPowerParams;

#pragma pack(pop)
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_PowerName;				//<! class name	
		eReturnValues				m_PowerStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sPowerParams				*m_PowerParam;				//<! sturcture for each of the transistions

		void get_Power_Mode_Type(std::string *power, uint16_t code);
		void process_List_Information(JSONNODE *powerData);
		eReturnValues get_Data(JSONNODE *masterData);
	public:
		CScsiPowerConditiontLog();
		CScsiPowerConditiontLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiPowerConditiontLog();
		virtual eReturnValues get_Log_Status() { return m_PowerStatus; };
		virtual eReturnValues parse_Power_Condition_Transitions_Log(JSONNODE *masterData) { return get_Data(masterData); };

	};
#endif
}
