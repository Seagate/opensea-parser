//
// CScsi_Start_Stop_Cycle_Counter_Log.h   Definition of Start Stop counter log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Start_Stop_Cycle_Counter_Log.h  Definition of Start Stop counter log 
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSISTARTSTOPLOG
#define SCSISTARTSTOPLOG
#pragma pack(push, 1)
	typedef enum _eSSMLogParams
	{
		manufactureDate = 0x0001,
		accountingDate = 0x0002,
		specLifetime = 0x0003,
		accumulated = 0x0004,
		loadUnload = 0x0005,
		accumulatedLU = 0x0006,
	}eSSMLogParams;
	typedef struct _sStartStopStruct
	{
		uint16_t			manufatureParamCode;			//<! Data of Manufacture parameter code for the log 
		uint8_t				paramControlByte1;				//<! param code for the log page format
		uint8_t				paramLength1;					//<! this is different from size, see SCSI SPC Spec. 
		uint32_t			year;							//<! Year of manufacture
		uint16_t			week;							//<! Week of manufature
		uint16_t			accountParamCode;				//<! Accounting Data parameter code for the log
		uint8_t				paramControlByte2;				//<! param code for the log page format
		uint8_t				paramLength2;					//<! this is different from size, see SCSI SPC Spec.
		uint32_t			accYear;						//<! Accounting Year of manufacture
		uint16_t			accWeek;						//<! Accounting Week of manufature
		uint16_t			specCycleParamCode;				//<! Specified Cycle count of parameter code for the log 
		uint8_t				paramControlByte3;				//<! param code for the log page format
		uint8_t				paramLength3;					//<! this is different from size, see SCSI SPC Spec.
		uint32_t			specLifeTime;					//<! Specified cycle count over device lifetime
		uint16_t			AccumulatedParamCode;			//<! Accumulated start-stop cycles parameter code for the log 
		uint8_t				paramControlByte4;				//<! param code for the log page format
		uint8_t				paramLength4;					//<! this is different from size, see SCSI SPC Spec. 
		uint32_t			accumulatedCycles;				//<! Accumulated start-stop cycle
		uint16_t			loadUnloadParamCode;			//<! Load Unload Count parameter code for the log 
		uint8_t				paramControlByte5;				//<! param code for the log page format
		uint8_t				paramLength5;					//<! this is different from size, see SCSI SPC Spec. 
		uint32_t			loadUnloadCount;				//<! Load Unload Count
		uint16_t			accLoadUnloadParamCode;			//<! Accumulated Load Unload Count parameter code for the log 
		uint8_t				paramControlByte6;				//<! param code for the log page format
		uint8_t				paramLength6;					//<! this is different from size, see SCSI SPC Spec. 
		uint32_t			accloadUnloadCount;				//<! AccumulatedLoad Unload Count

	}sStartStopStruct;
#pragma pack(pop)

	class CScsiStartStop 
	{
	private:
	protected:
        uint8_t                     *pData;                     //<! pointer to the data buffer
		std::string					m_SSName;					//<! class name	
		eReturnValues				m_StartStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! byte swapped length of the page
		uint8_t						m_SubPage;					//<! passed in- this is subpage that tell us what log to parse as.
        sStartStopStruct			*m_Page;					//<! page code for the log format
	

		eReturnValues parse_Start_Stop_Log(JSONNODE *masterData);
		eReturnValues week_Year_Print(JSONNODE *Data, uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t year, uint16_t week, const std::string strHeader,const std::string strYear, const std::string strWeek);
		eReturnValues get_Count(JSONNODE *countData, uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t count, const std::string strHeader, const std::string strCount);
	public:
		CScsiStartStop();
		CScsiStartStop(uint8_t *bufferData, size_t bufferSize, JSONNODE *masterData);
		virtual ~CScsiStartStop();
		virtual eReturnValues get_Log_Status() { return m_StartStatus; };

	};
#endif
}
