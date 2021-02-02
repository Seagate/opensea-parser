//
// CFARM_Log.cpp   Implementation of class CFARMLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CFARM_Log.cpp  Implementation of class CFARMLog
//
#include "CFARM_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CATA_Farm_Log::CATA_Farm_Log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::CFARMLog()
    :bufferData()
    , m_LogSize(0)
    , m_status(IN_PROGRESS)
    , m_isScsi(false)
    , m_shwoStatus(false)
    , m_bufferdelete(false)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFARMLog::CFARMLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
 
//! \parma fileName = the name of the file that need to be read in for getting the data
//! \param showStatus = if true then show the status bits
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::CFARMLog(const std::string & fileName,bool showStatus)
    :bufferData()
    , m_LogSize(0)
    , m_status(IN_PROGRESS)
    , m_isScsi(false)
    , m_shwoStatus(showStatus)
    , m_bufferdelete(true)
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            m_LogSize = cCLog->get_Size();
            bufferData = new uint8_t[m_LogSize];                                // new a buffer to the point                
#ifndef _WIN64
            memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
            memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
            m_isScsi = is_Device_Scsi();

            m_status = IN_PROGRESS;
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
    delete (cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn CFARMLog::CFARMLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//! \parma fileName = the name of the file that need to be read in for getting the data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::CFARMLog(const std::string & fileName)
    :bufferData()
    , m_LogSize(0)
    , m_status(IN_PROGRESS)
    , m_isScsi(false)
    , m_shwoStatus(false)
    , m_bufferdelete(true)
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            m_LogSize = cCLog->get_Size();
            bufferData = new uint8_t[m_LogSize];                                // new a buffer to the point                
#ifndef _WIN64
            memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
            memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
            m_isScsi = is_Device_Scsi();

            m_status = IN_PROGRESS;
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
    delete (cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn CFARMLog::CFARMLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//! \param bufferData = pointer to the buffer data.
//! \param bufferSize = the size of the buffer
//! \param showStatus = true to show the status bits of the farmlog
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::CFARMLog(uint8_t *bufferData, size_t bufferSize, bool showStatus)
    :bufferData(bufferData)
    , m_LogSize(bufferSize)
    , m_status(IN_PROGRESS)
    , m_isScsi(false)
    , m_shwoStatus(showStatus)
    , m_bufferdelete(false)
{
    if (bufferData != NULL)
    {
        m_isScsi = is_Device_Scsi();
        m_status = IN_PROGRESS;
}
    else
    {

        m_status = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CATA_Farm_Log::CATA_Farm_Log()
//
//! \brief
//!   Description: Class Deconstructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::~CFARMLog()
{
    if (m_bufferdelete)
    {
        if (bufferData != NULL)
        {
            delete[] bufferData;
        }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn is_Device_Scsi()
//
//! \brief
//!   Description:  check the first part of the buffer to see if it's a scsi log 
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
bool CFARMLog::is_Device_Scsi()
{
    if (bufferData != NULL)
    {
        if (M_GETBITRANGE(bufferData[0], 5, 0) == 0x3D )
        {
            if (bufferData[1] == 03 || bufferData[1] == 04)
            {
                return true;
            }
        }
    }
            return false;
}

//-----------------------------------------------------------------------------
//
//! \fn ParseFarmLog()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CFARMLog::parse_Device_Farm_Log(JSONNODE *masterJson)
{
    eReturnValues retStatus = MEMORY_FAILURE;
    if (m_isScsi)
    {
        CSCSI_Farm_Log *pCFarm;
        pCFarm = new CSCSI_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
        if (pCFarm->get_Log_Status() == SUCCESS)
        {
            pCFarm->print_All_Pages(masterJson);
            retStatus = SUCCESS;
        }
        else
        {
            retStatus = pCFarm->get_Log_Status() ;
        }
        delete( pCFarm);
    }
    else
    {
        CATA_Farm_Log *pCFarm;
        pCFarm = new CATA_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
        if (pCFarm->get_Log_Status() == IN_PROGRESS)
        {
            try
            {
                retStatus = pCFarm->parse_Farm_Log();
                if (retStatus == IN_PROGRESS)
                {
                    pCFarm->print_All_Pages(masterJson);
                    retStatus = SUCCESS;
                }
            }
            catch (...)
            {
                delete (pCFarm);
                return MEMORY_FAILURE;
            }
            
        }
        delete (pCFarm);
    }
    return retStatus;
}


//-----------------------------------------------------------------------------
//
//! \fn parse_Device_Farm_Log_And_Analyze()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return Helth of the drive
//
//---------------------------------------------------------------------------
eAnalyzeStatus CFARMLog::parse_Device_Farm_Log_And_Analyze()
{
    eReturnValues retStatus = MEMORY_FAILURE;
    eAnalyzeStatus anzStatus = UNDEFINED;
    if (m_isScsi)
    {
        CSCSI_Farm_Log *pCFarm;
        pCFarm = new CSCSI_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
        if (pCFarm->get_Log_Status() == SUCCESS)
        {
            // No support for SAS yet
            anzStatus = UNDEFINED;
        }
        else
        {
            retStatus = pCFarm->get_Log_Status();
        }
        delete(pCFarm);
    }
    else
    {
        CATA_Farm_Log *pCFarm;
        pCFarm = new CATA_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
        if (pCFarm->get_Log_Status() == IN_PROGRESS)
        {
            try
            {
                retStatus = pCFarm->parse_Farm_Log();
                if (retStatus == IN_PROGRESS)
                {
                    // Analyze the Log
                    anzStatus = pCFarm->parse_and_Analyze_Farm_Log();
                }
            }
            catch (...)
            {
                delete (pCFarm);
                return UNDEFINED;
            }

        }
        delete (pCFarm);
    }
    return anzStatus;
}


//-----------------------------------------------------------------------------
//
//! \fn print_Drive_Health()
//
//! \brief
//!   Description:  print out the drive health information
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param analyzeStatus  = Drive health info. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMLog::print_Drive_Health(JSONNODE *masterData, eAnalyzeStatus  analyzeStatus)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *pageInfo = json_new(JSON_NODE);

    snprintf((char*)myStr.c_str(), BASIC, "Drive Health Information ");
    json_set_name(pageInfo, (char*)myStr.c_str());

    /*
     * Get Drive health status.
     */
        
    switch (analyzeStatus)
    {
    case NTF:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "NTF"));
    }
    break;
    case PRESSURE:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "PRESSURE"));
    }
    break;
    case FW_ASSERTION:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "FW_ASSERTION"));
    }
    break;
    case DATA_PATH:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "DATA_PATH"));
    }
    break;
    case SEEK_HARD_ERROR:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "SEEK_HARD_ERROR"));
    }
    break;
    case UNRECOVERABLE_SERVO:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "UNRECOVERABLE_SERVO"));
    }
    break;
    case UNRECOVERABLE_WRITE:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "UNRECOVERABLE_WRITE"));
    }
    break;
    case UNRECOVERABLE_READ_GLIST:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "UNRECOVERABLE_READ_GLIST"));
    }
    break;
    case UNDEFINED:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "UNDEFINED_ERROR"));
    }
    break;
    default:
    {
        json_push_back(pageInfo, json_new_a("Drive Health", "UNDEFINED Error"));
    }
    break;
    }


    json_push_back(masterData, pageInfo);

    return SUCCESS;
}
