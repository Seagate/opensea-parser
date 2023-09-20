//
// CAta_LBA_status_log.cpp
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

#include "CAta_LBA_status_log.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn   CAta_LBA_Status()
//
//! \brief
//!   Description:  Default Class constructor for the CAta_LBA_Status
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_LBA_Status::CAta_LBA_Status()
    : m_name("ATA LBA Status Log")
    , pBuf(NULL)
    , m_logSize(0)
    , m_status(IN_PROGRESS)
    , m_MaxNumber(0)
{
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_LBA_Status()
//
//! \brief
//!   Description:  Default Class constructor for the CAta_LBA_Status
//
//  Entry:
//! \parama fileName = the name of the file that we need to open to get the buffer data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_LBA_Status::CAta_LBA_Status(const std::string &fileName)
    : m_name("ATA LBA Status Log")
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
            m_status = parse_LBA_Status_Log();
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
//! \fn   CAta_LBA_Status()
//
//! \brief
//!   Description:  Default Class constructor for the CAta_LBA_Status
//
//  Entry:
//! \parama BufferData = pointer, length and allocation structure to the data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_LBA_Status::CAta_LBA_Status(uint8_t *bufferData, size_t logSize)
    : m_name("ATA LBA Status Log")
    , pBuf(bufferData)
    , m_logSize(logSize)
    , m_status(IN_PROGRESS)
    , m_MaxNumber(0)
{
    if (bufferData != NULL)
    {
        m_status = parse_LBA_Status_Log();
    }
    else
    {
        m_status = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn ~CAta_LBA_Status()
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
CAta_LBA_Status::~CAta_LBA_Status()
{
    if (!m_logList.empty())
    {
        m_logList.clear();
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Number_Of_Descriptors
//
//! \brief
//!   Description: get the number of descriptors in the log
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_LBA_Status::get_Number_Of_Descriptors()
{
    uint32_t offset = 0;
    m_MaxNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    // check the compiled endianness of the os  if we need to byte swap or not
    if (get_Compiled_Endianness() != OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_64(&m_MaxNumber);
    }
    if (m_MaxNumber <= 0)
        return BAD_PARAMETER;  // should be some data in the log
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn parse_LBA_Status_Log
//
//! \brief
//!   Description: parse the LBA Status log
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_LBA_Status::parse_LBA_Status_Log()
{
    eReturnValues ret = SUCCESS;
    ret = get_Number_Of_Descriptors();             // get first page of information
    if (ret == SUCCESS)
    {
        // loop through the pages, each page is 512 bytes
        for (uint32_t offset = 512; offset < m_logSize; offset += 512)
        {

            if (m_logSize != 0)
            {
                sLBAPageInfo *logDetails = new sLBAPageInfo;
                memcpy(logDetails, reinterpret_cast<sLBAPageInfo*>(&pBuf[offset]), sizeof(sLBAPageInfo));
                if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
                {
                    byte_Swap_64(&logDetails->firstLBA);
                    byte_Swap_64(&logDetails->lastLBA);
                    // need to byte swap the descriptor ????  TO DO  if needed
                }
                m_logList.push_back(*logDetails);
            }
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------
//
//! \fn print_LBA_Status_Log
//
//! \brief
//!   Description: print out the LBA Status log information
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_LBA_Status::print_LBA_Status_Log(JSONNODE *masterData)
{
    JSONNODE *dirInfo = json_new(JSON_NODE);
    json_set_name(dirInfo, "LBA Status Log");



    std::vector<sLBAPageInfo>::iterator logItr = m_logList.begin();
    for (; logItr != m_logList.end(); ++logItr)
    {
        sLBAPageInfo logDetail = *logItr;
        std::ostringstream temp, temp2;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(logDetail.firstLBA);
        json_push_back(dirInfo, json_new_a("First LBA", temp.str().c_str()));
        temp.str("");
        temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(logDetail.lastLBA);
        json_push_back(dirInfo, json_new_a("Last LBA", temp.str().c_str()));

        temp.str(""); temp.clear();      
        for (uint16_t count = 0; count <= MAXDESCRIPTORCOUNT; count++)
        {
            temp.str(""); temp.clear();
            JSONNODE* Descriptor = json_new(JSON_NODE);
            temp << "Descriptor " << std::dec << count;
            json_set_name(Descriptor, temp.str().c_str());

            if (logDetail.descriptor[count].status & BIT3)
            {
                json_push_back(Descriptor, json_new_b("LBA range is read only", TRUE));
            }
            else
            {
                json_push_back(Descriptor, json_new_b("LBA range is read only", FALSE));
            }
            if (logDetail.descriptor[count].status & BIT2)
            {
                json_push_back(Descriptor, json_new_b("LBA range is offline", TRUE));
            }
            else
            {
                json_push_back(Descriptor, json_new_b("LBA range is offline", FALSE));
            }
            if (logDetail.descriptor[count].status & BIT1)
            {
                json_push_back(Descriptor, json_new_b("LBA accessibility is not reported", TRUE));
            }
            else
            {
                json_push_back(Descriptor, json_new_b("LBA accessibility is not reported", FALSE));
            }
            if (logDetail.descriptor[count].status & BIT0)
            {
                json_push_back(Descriptor, json_new_b("Trim status bit", TRUE));
            }
            else
            {
                json_push_back(Descriptor, json_new_b("Trim status bit", FALSE));
            }
            json_push_back(dirInfo, Descriptor);
        }
    }

    json_push_back(masterData, dirInfo);

    return SUCCESS;
}