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
	:bufferData()
	, m_LogSize(0)
	, m_status(IN_PROGRESS)
	, m_isScsi(false)
	, m_shwoStatus(false)
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
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_LogSize = cCLog->get_Size();
			bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
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
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_LogSize = cCLog->get_Size();
			bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
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
    if (bufferData != NULL)
    {
        delete [] bufferData;
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
        if (M_GETBITRANGE(bufferData[0], 5, 0) == 0x3D)
        {
            if (bufferData[1] == 03)
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
        pCFarm = new CSCSI_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
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
        pCFarm = new CATA_Farm_Log((uint8_t *)bufferData, m_LogSize, m_shwoStatus);
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
