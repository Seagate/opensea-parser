//
// CAta_Ext_DST_Log.h
//
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

// \file CAta_Ext_DST_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include "common.h"
#include "CLog.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef ATAEXTDST
#define ATAEXTDST

    class CAta_Ext_DST_Log 
    {
    protected:
        enum m_DST_Enum { DST_HAS_NOT_BEEN_RUN, DST_HAS_BEEN_RUN, DST_INCOMPLETE, DST_HAS_FAILED, DST_HAS_TIMEDOUT };
        std::string                 m_name;                                         //!< name of the class
		uint8_t						*pData;											//!< pointer the the data
        size_t						m_logSize;                                      //!< log size in bytes                             
        eReturnValues               m_status;                                       //!< holds the status so 

        void Get_Status_Meaning(std::string &meaning,uint8_t status);
        eReturnValues parse_Ext_Self_Test_Log(JSONNODE *masterData);

    public:
        CAta_Ext_DST_Log(const std::string &fileName, JSONNODE *masterData);
        CAta_Ext_DST_Log(uint8_t *pBufferData, JSONNODE *masterData);
        ~CAta_Ext_DST_Log();
        eReturnValues get_Status(){ return m_status; };

    };
#endif //ATAEXTDST
}
