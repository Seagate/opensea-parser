// CScsi_Temperature_Log.h   Definition of Temperature log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Temperature_Log.h   Definition of Temperature log
#pragma once
#ifdef max
#undef max
#endif
#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <cstddef>
#include <stdexcept>
#include "common_types.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSITEMPLOG
#define SCSITEMPLOG
#pragma pack(push, 1)
	typedef struct _sTempLogPageStruct
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is described in 5.2.2.2.2, and shall be set as shown in table 352 for the Temperature log parameter.
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field is described in 5.2.2.2.2, and shall be set as shown in table 352 for the Temperature log parameter.
		uint8_t			reserved;
		uint8_t			temp;								//<! The TEMPERATURE field indicates the temperature of the SCSI target device in degrees Celsius at the time the LOG SENSE command
	}sTempLogPageStruct;
#pragma pack(pop)

	class CScsiTemperatureLog
	{
	private:
	protected:
		std::vector<uint8_t>        v_Buff;                     //<! vector for holding the buffer data
		size_t						m_pDataSize;                //<! the size of the file that will be opened
		sTempLogPageStruct			*m_Page;					//<! page code for the log lpage format
		std::string					m_TempName;					//<! class name	
		eReturnValues				m_TempStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page

		void get_Temp(JSONNODE *tempData);
		eReturnValues get_Data(JSONNODE *masterData);
		inline bool safe_add_size_t(size_t a, size_t b, size_t& result) {
			const size_t max_val = (std::numeric_limits<size_t>::max)(); // works on Win & Linux
			if (a > max_val - b) {
				return false; // overflow
			}
			result = a + b;
			return true;
		}

		inline bool has_enough_data(size_t param, size_t tempSize, size_t structSize, size_t dataSize) {
			size_t sum1;
			if (!safe_add_size_t(param, tempSize, sum1)) return false;
			if (sum1 >= dataSize) return false;

			size_t sum2;
			if (!safe_add_size_t(param, (2 * tempSize) + structSize, sum2)) return false;
			return sum2 <= dataSize;
		}
	public:
		CScsiTemperatureLog();
		explicit CScsiTemperatureLog(uint8_t * buffer, size_t bufferSize);
		virtual ~CScsiTemperatureLog();
		virtual void set_Temp_Page_Length(uint16_t length) { m_PageLength = length; };
		virtual eReturnValues get_Log_Status() { return m_TempStatus; };
		virtual eReturnValues parse_Temp_Log(JSONNODE *masterData) { return get_Data(masterData); };
	};
#endif
}
