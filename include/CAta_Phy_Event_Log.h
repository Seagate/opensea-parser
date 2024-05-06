//
// CAta_Phy_Event_Log.h
//
// Do NOT modify or remove this copyright and confidentiality notice.
//
// Copyright 2015 - 2024 Seagate Technology LLC.
//
// The code contained herein is CONFIDENTIAL to Seagate Technology LLC
// and may be covered under one or more Non-Disclosure Agreements.
// All or portions are also trade secret.
// Any use, modification, duplication, derivation, distribution or disclosure
// of this code, for any reason, not expressly authorized is prohibited.
// All other rights are expressly reserved by Seagate Technology LLC.
//
// *****************************************************************************

// \file CAta_Phy_Event_Log.h
// \brief Defines the function calls and structures for parsing Seagate logs
#pragma once
#include <vector>
#include <string>
#include "common.h"
#include "libjson.h"
#include "CLog.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef ATAPHYEVENT
#define ATAPHYEVENT

    class CAtaPhyEventLog
    {
    private:
        std::string                 m_name;                                         //!< name of the class
        size_t                      m_logSize;                                      //!< log size in bytes 
        eReturnValues               m_status;                                       //!< holds the status
        std::vector<uint8_t>        v_Buff;                                         //!< vector for holding the buffer data
    public:
        CAtaPhyEventLog();
        explicit CAtaPhyEventLog(std::string& filename, JSONNODE* masterData);
        explicit CAtaPhyEventLog(uint8_t* bufferData, size_t logSize);
        virtual ~CAtaPhyEventLog();
    };
#endif  //ATAPHYEVENT
}