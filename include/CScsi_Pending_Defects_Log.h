// CScsi_Pending_Defects_Log.h  Definition for parsing the pending defecs
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Pending_Defects_Log.h  Definition for parsing the pending defecs
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIPENDINGLOG
#define SCSIPENDINGLOG

#pragma pack(push, 1)
	typedef struct _sPendingDefectCountParameter
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field (04h)
		uint32_t		totalPlistCount;					//<! count of the total number of pending list
		_sPendingDefectCountParameter() : paramCode(0), paramControlByte(0), paramLength(0), totalPlistCount(0) {};
	} sPendindDefectCount;
	typedef struct _sPendingDefectInformationParameter
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field (14h)
		uint32_t		defectPOH;							//<! Defect's power on hours
		uint64_t		defectLBA;							//<! Defect's Logical Block Address
		_sPendingDefectInformationParameter() : paramCode(0), paramControlByte(0), paramLength(0), defectPOH(0), defectLBA(0) {};
	} sDefect;
#pragma pack(pop)

	class CScsiPendingDefectsLog
	{
	private:
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_PlistName;				//<! class name	
		eReturnValues				m_PlistStatus;  		    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sPendindDefectCount			*m_PListCountParam;			//<! Parameters to the Plist count
		sDefect						*m_PlistDefect;				//<! structure to the Pending defect

		void get_Supported_And_Subpage_Description(std::string *description);
		void process_PList_Data(JSONNODE *pendingData);
		void process_PList_Count(JSONNODE *pendingCount);
		eReturnValues get_Plist_Data(JSONNODE *masterData);
	public:
		CScsiPendingDefectsLog();
		CScsiPendingDefectsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiPendingDefectsLog();
		virtual eReturnValues get_Log_Status() { return m_PlistStatus; };
		virtual eReturnValues parse_Supported_Log_Pages_Log(JSONNODE *masterData) { return get_Plist_Data(masterData); };

	};
#endif
}
