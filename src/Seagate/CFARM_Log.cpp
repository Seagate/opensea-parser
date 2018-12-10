//
// CFARM_Log.cpp   Implementation of class CFARMLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
    : CLog()
    , m_status(IN_PROGRESS)
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
//! \param securityPrintLevel = the level of print 
//! \parma fileName = the name of the file that need to be read in for getting the data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFARMLog::CFARMLog(const std::string & fileName)
    : CLog(fileName)
    , m_status(IN_PROGRESS)
{
    if (m_logStatus == SUCCESS)
    {
        if (m_bufferData != NULL)
        {
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
        m_status = m_logStatus;
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
    if (m_bufferData != NULL)
    {
        if (M_GETBITRANGE(m_bufferData[0], 5, 0) == 0x3D)
        {
            if (m_bufferData[1] == 03)
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
eReturnValues CFARMLog::ParseFarmLog(JSONNODE *masterJson)
{
    eReturnValues retStatus = MEMORY_FAILURE;
    if (m_isScsi)
    {
        CSCSI_Farm_Log *pCFarm;
        pCFarm = new CSCSI_Farm_Log((uint8_t *)m_bufferData, m_size);
        if (pCFarm->get_Log_Status() == SUCCESS)
        {
            retStatus = pCFarm->ParseFarmLog();
            if (retStatus == SUCCESS)
            {
                pCFarm->print_All_Pages(masterJson);
            }
        }
        delete( pCFarm);
    }
    else
    {
        CATA_Farm_Log *pCFarm;
        pCFarm = new CATA_Farm_Log((uint8_t *)m_bufferData, m_size);
        if (pCFarm->get_Log_Status() == IN_PROGRESS)
        {
            retStatus = pCFarm->parse_Farm_Log();
            if (retStatus == IN_PROGRESS)
            {
                pCFarm->print_All_Pages(masterJson);
                retStatus = SUCCESS;
            }
        }
        delete (pCFarm);
    }
    return retStatus;
}