//
// CFARM_Log.h   Definition of class CFarm_Log
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

// \file CFARM_Log.h  Definition of class CFarm_Log
#pragma once
#include "CLog.h"
#include <string>
#include "common.h"
#include "libjson.h"
#include "CAta_Farm_Log.h"
#include "CScsi_Farm_Log.h"

namespace opensea_parser {
#ifndef FARMCLASS
#define FARMCLASS

    class CFARMLog 
    {
    protected:
		uint8_t						* bufferData;						 //!< pointer to the buffer	
		size_t						m_LogSize;							 //!< size of the log
        eReturnValues               m_status;                            //!< status of the class
        bool                        m_isScsi;                            //!< true if the log is Scsi

        bool is_Device_Scsi();											 //<! Function for finding out if the binary is for Scsi or not
    public:
        CFARMLog();
        CFARMLog(const std::string & fileName);
        virtual ~CFARMLog();
        eReturnValues ParseFarmLog( JSONNODE *masterData);

    };
#endif
}
