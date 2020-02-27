//
// CAta_Power_Conditions_Log.h
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CAta_Power_Conditions_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include "common.h"
#include "CLog.h"
#include "libjson.h"

namespace opensea_parser {
#ifndef ATAPOWERCOND
#define ATAPOWERCOND
#pragma pack(push,1)

	typedef struct _sPowerConditionFlags
	{
		bool powerSupportBit;
		bool powerSaveableBit;
		bool powerChangeableBit;
		bool defaultTimerBit;
		bool savedTimmerBit;
		bool currentTimmerBit;
		bool holdPowerConditionBit;
		bool reserved;
	}sPowerConditionFlags;

	typedef struct _sPowerLogDescriptor
	{
		uint8_t					reserved8;									//!< Power Condition log descriptor reserved byte
		uint8_t					bitFlags;									//!< Power Condition Flags for each bit.
		uint16_t				reserved16;									//!< Power Condition log descriptor reserved byte
		uint32_t				defaultTimerSetting;						//!< The DEFAULT TIMER SETTING field is set at the time of manufacture.
		uint32_t				savedTimerSetting;							//!< The SAVED TIMER SETTING field is valid if the POWER CONDITION SUPPORTED bit set to 1.
		uint32_t				currentTimerSetting;						//!< The CURRENT TIMER SETTING field contains the minimum time that the device shall wait after command completion.
		uint32_t				normalRecoveryTime;							//!< The NOMINAL RECOVERY TIME TO PM0:ACTIVE field contains the nominal time required to transition from this power.
		uint32_t				minimumTimerSetting;						//!< The MINIMUM TIMER SETTING field contains the minimum timer value allowed by the Set Power Condition Timer.
		uint32_t				maximumTimerSetting;						//!< The MAXIMUM TIMER SETTING field contains the maximum timer value allowed by the Set Power Condition Timer
		uint32_t				reserved32;									//!< Power Condition log descriptor reserved byte
	}sPowerLogDescriptor;

#pragma pack(pop)

	class CAtaPowerConditionsLog 
	{
	protected:
		uint8_t                *m_powerConditionLog;
		uint8_t                *m_powerFlags;
		tDataPtr			    Buffer;
		eReturnValues		    m_status;
		JSONNODE               *m_myJSON;
		sPowerLogDescriptor    *m_idleAPowerConditions, *m_idleBPowerConditions, *m_idleCPowerConditions, *m_standbyYPowerConditions, *m_standbyZPowerConditions;
		sPowerConditionFlags   *m_conditionFlags;
		sPowerConditionFlags    conditionFlags;
	public:
		CAtaPowerConditionsLog();
		CAtaPowerConditionsLog(std::string filename);
        CAtaPowerConditionsLog(tDataPtr pData, JSONNODE *masterData);
		virtual ~CAtaPowerConditionsLog();
		eReturnValues get_Power_Condition_Flags(uint8_t readFlags);
		eReturnValues get_Power_Condition_Log();
		eReturnValues printPowerConditionLog(JSONNODE *masterData);
		void printPowerConditionFlag(JSONNODE *masterData);
		eReturnValues printPowerLogDescriptor(JSONNODE *masterData, sPowerLogDescriptor *logDescriptor);
        eReturnValues get_Power_Status(){ return m_status; };
	};
#endif //ATAPOWERCOND

}