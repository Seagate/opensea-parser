// CScsi_Cache_Statistics_Log.h  Definition of Cache Statistics page for SAS
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

// \file CScsi_Cache_Statistics_Log.h  Definition of Cache Statistics page for SAS
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSICACHELOG
#define SCSICACHELOG
#pragma pack(push, 1)
	typedef struct _sCacheParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		_sCacheParameters() : paramCode(0), paramControlByte(0), paramLength(0) {};
	} sCacheParams;

#pragma pack(pop)

	class CScsiCacheLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_CacheName;				//<! class name	
		eReturnValues				m_CacheStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint64_t					m_Value;					//<! Parameter Value
		sCacheParams				*m_cache;					//<! cache structure 

		bool get_Cache_Parameter_Code_Description(std::string *cache);
		void process_Cache_Event_Data(JSONNODE *cacheData);
		eReturnValues get_Cache_Data(JSONNODE *masterData);
	public:
		CScsiCacheLog();
		CScsiCacheLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiCacheLog();
		virtual eReturnValues get_Log_Status() { return m_CacheStatus; };
		virtual eReturnValues parse_Cache_Statistics_Log(JSONNODE *masterData) { return get_Cache_Data(masterData); };

	};
#endif
}
