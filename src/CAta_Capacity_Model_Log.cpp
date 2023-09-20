//
// CAta_Capacity_Model_Log.cpp
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

#include "CAta_Capacity_Model_Log.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Cap_Model_Number()
//
//! \brief
//!   Description:  Default Class constructor for the CAta_Cap_Model_Number
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Cap_Model_Number::CAta_Cap_Model_Number()
    : m_name("ATA LBA Status Log")
    , pBuf(NULL)
    , m_logSize(0)
    , m_status(IN_PROGRESS)
    , m_MaxNumber(0)
{
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Cap_Model_Number()
//
//! \brief
//!   Description:  Default Class constructor for the CAta_Cap_Model_Number
//
//  Entry:
//! \parama fileName = the name of the file that we need to open to get the buffer data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Cap_Model_Number::CAta_Cap_Model_Number(const std::string &fileName)
    : m_name("ATA Capacity Model Log")
    , pBuf(NULL)
    , m_logSize(0)
    , m_status(IN_PROGRESS)
    , m_MaxNumber(0)
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            m_logSize = cCLog->get_Size();
            pBuf = new uint8_t[cCLog->get_Size()];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
            memcpy(pBuf, cCLog->get_Buffer(), m_logSize);
#else
            memcpy_s(pBuf, m_logSize, cCLog->get_Buffer(), m_logSize);// copy the buffer data to the class member pBuf
#endif
            m_status = parse_Capacity_Model_Log();
        }
        else
        {
            m_status = FAILURE;
        }
    }
    else
    {
        m_status = cCLog->get_Log_Status();
    }
    delete(cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Cap_Model_Number()
//
//! \brief
//!   Description:  Default Class constructor for the CSeaTreasureLog
//
//  Entry:
//! \parama BufferData = pointer, length and allocation structure to the data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Cap_Model_Number::CAta_Cap_Model_Number(uint8_t *bufferData, size_t logSize)
    : m_name("ATA Capacity Model Log")
    , pBuf(bufferData)
    , m_logSize(logSize)
    , m_status(IN_PROGRESS)
    , m_MaxNumber(0)
{
    if (bufferData != NULL)
    {
        m_status = parse_Capacity_Model_Log();
    }
    else
    {
        m_status = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn ~CAta_Cap_Model_Number()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param pData  pointer to the buffer data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_Cap_Model_Number::~CAta_Cap_Model_Number()
{
    if (!m_logList.empty())
    {
        m_logList.clear();
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Number_Of_CM_Descriptors
//
//! \brief
//!   Description: get the number of Capative and Model number Descrptors
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_Cap_Model_Number::get_Number_Of_CM_Descriptors()
{
    uint32_t offset = 0;
    m_MaxNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    // check the compiled endianness of the os  if we need to byte swap or not
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_64(&m_MaxNumber);
    }
    if (m_MaxNumber <= 0)
        return BAD_PARAMETER;  // should be some data in the log
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn parse_Capacity_Model_Log
//
//! \brief
//!   Description: parse the Capacity Model number log
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_Cap_Model_Number::parse_Capacity_Model_Log()
{
    eReturnValues ret = SUCCESS;
    ret = get_Number_Of_CM_Descriptors();             // get first page of information
    if (ret == SUCCESS)
    {
        // loop through the pages, each page is 512 bytes
        for (uint32_t offset = 8; offset <= m_logSize; offset += sizeof(sCapacityModelDescriptor))
        {

            if (m_logSize != 0)
            {
                sCapacityModelDescriptor* logDetails = new sCapacityModelDescriptor;
                // copy the data from the buffer to the structure
                memcpy(logDetails, reinterpret_cast<sCapacityModelDescriptor*>(&pBuf[offset]), sizeof(sCapacityModelDescriptor));
                if (get_Compiled_Endianness() != OPENSEA_LITTLE_ENDIAN)
                {
                    byte_Swap_64(&logDetails->capat);
                }
                // now convert the data into what we are going to print out or add to json
                sCreatedDescriptorInformation* logInfo = new sCreatedDescriptorInformation;
                logInfo->maxAddress = M_GETBITRANGE(logDetails->capat,47,0);
                logInfo->modelNumber.clear();
                logInfo->modelNumber.assign(reinterpret_cast<const char*>(&logDetails->modelField), MODEL_NUMBER_LEN + 1);
                byte_swap_std_string(logInfo->modelNumber);
                remove_trailing_whitespace_std_string(logInfo->modelNumber);
                
                
                if (logInfo != nullptr)
                    m_logList.push_back(*logInfo);
            }
            if (m_logList.size() == m_MaxNumber)
                break;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Capacity_Model_Log
//
//! \brief
//!   Description: print out the Max capacity and the model number for that capacity
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_Cap_Model_Number::print_Capacity_Model_Log(JSONNODE *masterData)
{
    JSONNODE *dirInfo = json_new(JSON_NODE);
    json_set_name(dirInfo, "Caqpacity Model Number Log");



    std::vector<sCreatedDescriptorInformation>::iterator logItr = m_logList.begin();
    for (; logItr != m_logList.end(); ++logItr)
    {
        sCreatedDescriptorInformation logDetail = *logItr;
        json_push_back(dirInfo, json_new_i("Max LBA", logDetail.maxAddress));
        json_push_back(dirInfo, json_new_a("Model Number", logDetail.modelNumber.c_str()));   

    }

    json_push_back(masterData, dirInfo);

    return SUCCESS;
}