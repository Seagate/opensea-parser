//
// CFARM_Log.h   Definition of class CFarm_Log
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

// \file CFARM_Log.h  Definition of class CFarm_Log
#pragma once
#include "CLog.h"
#include <string>
#include "common.h"
#include "libjson.h"
#include "CFARM_Combine.h"

namespace opensea_parser {
#ifndef FARMCLASS
#define FARMCLASS

    class CFARMLog : public CFarm_Combine
    {
    protected:
        eReturnValues               m_FARMstatus;                            //!< status of the class
        bool                        m_isCombo;                           //!< true if the log is a FARM combo log
        bool                        m_bufferdelete;

    public:
        CFARMLog();
        CFARMLog(const std::string & fileName,bool showStatus);
        explicit CFARMLog(const std::string & fileName);
        CFARMLog(uint8_t *farmbufferData, size_t bufferSize, bool showStatus);
        virtual ~CFARMLog();
		eReturnValues get_FARM_Status() { return m_FARMstatus; };
        eReturnValues parse_Device_Farm_Log( JSONNODE *masterData);

    };
#endif
}
