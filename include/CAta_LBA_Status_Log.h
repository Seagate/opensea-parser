//
// CAta_LBA_status_log.h
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

// \file CAta_LBA_status_log.h
// \brief Defines the function 

#include <vector>

#include "common.h"
#include "libjson.h"
#include "CLog.h"

namespace opensea_parser {
#ifndef LBASTATUS
#define LBASTATUS

#define MAXDESCRIPTORCOUNT 30

    class CAta_LBA_Status
    {
    private:
    protected:
#pragma pack(push, 1)   
        typedef struct _sDescriptor
        {
            uint64_t                        start;                              //!< starting Logical block address
            uint32_t                        numberOfBlocks;                     //!< Number of log pages
            uint16_t                        status;                             //!< LBA range status
            uint16_t                        reserved;                           //!< reserved

            _sDescriptor() : start(0), numberOfBlocks(0), status(0), reserved(0) {};

        } sDescriptor;
        typedef struct _sLBAPageInfo
        {
            uint64_t                        firstLBA;                           //!< The FIRST LBA field contains the LBA in LBA Status Descriptor
            uint64_t                        lastLBA;                            //!< the last LBA in the range
            sDescriptor                     descriptor[MAXDESCRIPTORCOUNT];     //!< descriptor full of information on the LBA
            _sLBAPageInfo() : firstLBA(0), lastLBA(0), descriptor() {};

        }sLBAPageInfo;
#pragma pack(pop)

        std::string                         m_name;                             //!< name of the class
        uint8_t                             *pBuf;                              //!< pointer data structure
        size_t                              m_logSize;                          //!< log size
        eReturnValues                       m_status;                           //!< holds the status fo the class
        uint64_t                            m_MaxNumber;                        //!< the number of descriptors in the log

        std::vector<sLBAPageInfo>           m_logList;

    public:
        CAta_LBA_Status();
        explicit CAta_LBA_Status(const std::string &fileName);
        CAta_LBA_Status(uint8_t *bufferData, size_t logSize);
        virtual ~CAta_LBA_Status();
        eReturnValues get_LBA_Status() { return m_status; };
        eReturnValues get_Number_Of_Descriptors();
        eReturnValues parse_LBA_Status_Log();
        eReturnValues print_LBA_Status_Log(JSONNODE* masterData);

    };
#endif // ! LBASTATUS
}
