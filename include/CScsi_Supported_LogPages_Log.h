// CScsi_Supported_LogPages_Log.h  Definition for parsing the supported log pages
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

// \file CScsi_Supported_LogPages_Log.h Definition for parsing the supported log pages 
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSISUPPORTLOG
#define SCSISUPPORTLOG

	class CScsiSupportedLog
	{
	private:

	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_SupportedName;			//<! class name	
		eReturnValues				m_SupportedStatus;		    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		uint8_t						m_SubPage;					//<! subpage data
		uint8_t						m_Page;						//<! Page number
		bool						m_ShowSubPage;				//<! flag to show the subpage information
        bool                        m_ShowSupportedPagesOnce;   //<! set to true and then once we should this page set to false so we don't show it more then once

		void get_Supported_And_Subpage_Description(std::string *description);
		void process_Supported_Data(JSONNODE *supportData);
		eReturnValues get_Supported_Log_Data(JSONNODE *masterData);
	public:
		CScsiSupportedLog();
		CScsiSupportedLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength, bool subPage);
		virtual ~CScsiSupportedLog();
		virtual eReturnValues get_Log_Status() { return m_SupportedStatus; };
		virtual eReturnValues parse_Supported_Log_Pages_Log(JSONNODE *masterData) { return get_Supported_Log_Data(masterData); };

	};
#endif
}