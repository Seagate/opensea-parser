//
// CScsiLog.h   Definition of Base class CScsiLog
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

// \file CScsi_Log.h  Definition of Base class CScsiLog
#pragma once
#include "CLog.h"

namespace opensea_parser {
#ifndef SCSILOG
#define SCSILOG

    class CScsiLog 
	{
	private:
    protected:
		uint8_t						* bufferData;				//<! pointer to the data from the log
		size_t						m_LogSize;					//<! log size
        std::string					m_name;						//<! class name	
		eReturnValues				m_ScsiStatus;			    //<! status of the class
        sLogPageStruct*				m_Page;						//<! page code for the log lpage format


		eReturnValues get_Log_Parsed(JSONNODE *masterData);
	public:
		CScsiLog();
        CScsiLog(const std::string fileName, JSONNODE *masterData);
		virtual ~CScsiLog();
        virtual eReturnValues get_Log_Status(){ return m_ScsiStatus; };
		inline int get_Page_Code() const { return m_Page->pageCode; }
		inline int get_Page_Length() const { return m_Page->pageLength; }

	};
#endif
}
