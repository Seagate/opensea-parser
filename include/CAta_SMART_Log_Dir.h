//
// CAta_SMART_Log_Dir.h
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CAta_SMART_Log_Dir.h
// \brief Defines the function calls and structures for pulling Seagate logs

#include <vector>

#include "common.h"
#include "libjson.h"
#include "CLog.h"

namespace opensea_parser {

    class CAta_SMART_Log_Dir
    {
    private:
    protected:
#pragma pack(push, 1)   
        typedef struct _sLogDetailStructure
        {
            uint8_t                    logAddress;                              //!< Log Address
            uint16_t                   numberOfPages;                           //!< Number of log pages
        } sLogDetailStructure;
#pragma pack(pop)

        std::string                         m_name;                             //!< name of the class
        uint8_t                             *pData;                             //!< pointer data structure
        size_t                              m_logSize;                          //!< log size
        eReturnValues                       m_status;                           //!< holds the status fo the class
        bool                                m_freeBuffer;
        bool                                m_hasHostSpecific;
        bool                                m_hasVendorSpecific;
        std::vector<sLogDetailStructure>    m_logDetailList;

        eReturnValues parse_SMART_Log_Dir();
        bool is_Host_Specific_Log(uint8_t logAddress);
        bool is_Vendor_Specific_Log(uint8_t logAddress);

    public:
        CAta_SMART_Log_Dir();
        CAta_SMART_Log_Dir(const std::string &fileName);
        CAta_SMART_Log_Dir(uint8_t *bufferData, size_t logSize);
        virtual ~CAta_SMART_Log_Dir();
        virtual eReturnValues get_SMART_Log_Dir_Status() { return m_status; };
        virtual eReturnValues print_SMART_Log_Dir(JSONNODE *masterData);
    };
}
