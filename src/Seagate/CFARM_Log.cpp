//
// CFARM_Log.cpp   Implementation of class CFARMLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
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
//! \fn CFARMLog::CFARMLog()
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
	:m_FARMstatus(eReturnValues::IN_PROGRESS)
	, bufferData()
    , m_bufferdelete(false)
	, m_LogSize(0)
	, m_showStatusBytes(false)
	, m_showStatic(false)
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
CFARMLog::CFARMLog(const std::string& fileName, bool showStatus, bool showStatic)
	: m_FARMstatus(eReturnValues::IN_PROGRESS)
	, bufferData()
    , m_bufferdelete(true)
	, m_LogSize(0)
	, m_showStatusBytes(showStatus)
	, m_showStatic(showStatic)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_LogSize = cCLog->get_Size();
			bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
			memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
			memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
			m_FARMstatus = eReturnValues::IN_PROGRESS;
		}
		else
		{

			m_FARMstatus = eReturnValues::FAILURE;
		}
	}
	else
	{
		m_FARMstatus = cCLog->get_Log_Status();
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
	:m_FARMstatus(eReturnValues::IN_PROGRESS)
	, bufferData()
    , m_bufferdelete(true)
	, m_LogSize(0)
	, m_showStatusBytes(false)
	, m_showStatic(false)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_LogSize = cCLog->get_Size();
			bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
			memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
			memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
			m_FARMstatus = eReturnValues::IN_PROGRESS;
		}
		else
		{

			m_FARMstatus = eReturnValues::FAILURE;
		}
	}
	else
	{
		m_FARMstatus = cCLog->get_Log_Status();
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
CFARMLog::CFARMLog(uint8_t *farmbufferData, size_t bufferSize, bool showStatus, bool showStatic)
	: m_FARMstatus(eReturnValues::IN_PROGRESS)
	, bufferData(farmbufferData)
    , m_bufferdelete(false)
	, m_LogSize(bufferSize)
	, m_showStatusBytes(showStatus)
	, m_showStatic(showStatic)
{
	if (farmbufferData != NULL)
	{
		m_FARMstatus = eReturnValues::IN_PROGRESS;
}
	else
	{

		m_FARMstatus = eReturnValues::FAILURE;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CFARMLog::CFARMLog()
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
//! \fn ParseFarmLog()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return eReturnValues::SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CFARMLog::parse_Device_Farm_Log(JSONNODE *masterJson)
{
	eReturnValues retStatus = eReturnValues::SUCCESS; 

	if (is_Device_Scsi(bufferData[0], bufferData[1]))
	{
		uint8_t subpage = bufferData[1];
		CSCSI_Farm_Log* pCFarm;
		pCFarm = new CSCSI_Farm_Log(bufferData, m_LogSize, subpage, false, m_showStatusBytes, m_showStatic);
		if (pCFarm->get_Log_Status() == eReturnValues::SUCCESS)
		{
			try
			{
				pCFarm->print_All_Pages(masterJson);
				retStatus = eReturnValues::SUCCESS;
			}
			catch (...)
			{
				delete (pCFarm);
				return eReturnValues::MEMORY_FAILURE;
			}
		}
		else
		{
			retStatus = pCFarm->get_Log_Status();
		}
		delete(pCFarm);
	}
	else
	{
		CATA_Farm_Log* pCFarm;
		pCFarm = new CATA_Farm_Log(bufferData, m_LogSize, m_showStatusBytes, m_showStatic);
		if (pCFarm->get_Log_Status() == eReturnValues::IN_PROGRESS)
		{
			try
			{
				retStatus = pCFarm->parse_Farm_Log();
				if (retStatus == eReturnValues::IN_PROGRESS)
				{
					pCFarm->print_All_Pages(masterJson);
					retStatus = eReturnValues::SUCCESS;
				}
			}
			catch (...)
			{
				delete (pCFarm);
				return eReturnValues::MEMORY_FAILURE;
			}

		}
		else
		{
			retStatus = pCFarm->get_Log_Status();
		}
		delete (pCFarm);
	}
    return retStatus;
}
