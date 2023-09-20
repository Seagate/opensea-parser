//
// CAta_Capacity_Model_Log.h
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

// \file CAta_Capacity_Model_Log.h
// \brief Defines the function 

#include <vector>

#include "common.h"
#include "libjson.h"
#include "CLog.h"

namespace opensea_parser {
#ifndef CAPACITY
#define CAPACITY


    class CAta_Cap_Model_Number
    {
    private:
    protected:
#pragma pack(push, 1)   

        typedef struct _sCapacityModelDescriptor
        {
            uint64_t                        capat;                              //!< The FIRST LBA field contains the LBA in LBA Status Descriptor
            uint8_t                         modelField[40];                     //!< the last LBA in the range
            _sCapacityModelDescriptor() : capat(0)
#if defined __cplusplus && __cplusplus >= 201103L
                , modelField{ 0 } 
#endif
            {};

        }sCapacityModelDescriptor;

        typedef struct _sCreated_Descriptor_Information
        {
            uint64_t                        maxAddress;
            std::string                     modelNumber;
            _sCreated_Descriptor_Information() : maxAddress(0), modelNumber() {};
        }sCreatedDescriptorInformation;
#pragma pack(pop)

        std::string                         m_name;                             //!< name of the class
        uint8_t                             *pBuf;                              //!< pointer data structure
        size_t                              m_logSize;                          //!< log size
        eReturnValues                       m_status;                           //!< holds the status fo the class
        uint64_t                            m_MaxNumber;                        //!< the number of descriptors in the log

        std::vector<sCreatedDescriptorInformation>           m_logList;              //!< vector of capatity and model numbers

    public:
        CAta_Cap_Model_Number();
        explicit CAta_Cap_Model_Number(const std::string &fileName);
        CAta_Cap_Model_Number(uint8_t *bufferData, size_t logSize);
        virtual ~CAta_Cap_Model_Number();
        eReturnValues get_Cap_Model_Status() { return m_status; }
        eReturnValues get_Number_Of_CM_Descriptors();
        eReturnValues parse_Capacity_Model_Log();
        eReturnValues print_Capacity_Model_Log(JSONNODE* masterData);

    };
#endif // ! CAPACITY
}
