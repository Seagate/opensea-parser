// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file clog.cpp   Implementation of CLog class
// 

#include "CLog.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CLog()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//
//  Exit:
//
//---------------------------------------------------------------------------
CLog::CLog()
    : m_name("CLog")
    , m_fileName("")
    , m_size(0)
	, m_bufferData()
    , m_logStatus(eReturnValues::UNKNOWN)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CLog()
//
//! \brief
//!   Description: Class constructor needs the filename for opening and getting the data to a buffer
//
//  Entry:
// \param fileName = the namve fo the file that we should be opening
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CLog::CLog(const std::string &fileName)
    :m_name("CLog")
    , m_fileName(fileName)
    , m_size(0)
	, m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
{
    get_CLog();
}
//-----------------------------------------------------------------------------
//
//! \fn CLog()
//
//! \brief
//!   Description: Class constructor needs the filename for opening and getting the data to a buffer
//
//  Entry:
// \param fileName = the namve fo the file that we should be opening
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CLog::CLog(const std::string& fileName,bool useV_Buff)
    :m_name("CLog")
    , m_fileName(fileName)
    , m_size(0)
    , m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
{
    if (useV_Buff)
    {
        read_In_Log();
    }
    else
    {
        get_CLog();
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CLog()
//
//! \brief
//!   Description: Class constructor needs the filename for opening and getting the data to a buffer
//
//  Entry:
// \param pBuf = a pointer to a buffer where the data will be copied to
// \param logSize = the size of the buffer
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CLog::CLog(const uint8_t * pBuf, size_t logSize)
	:m_name("CLog")
	, m_fileName("")
    , m_size(logSize)
	, m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
{
    get_CLog(pBuf, logSize);
}
//-----------------------------------------------------------------------------
//
//! \fn CLog::~CLog()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CLog::~CLog()
{
    if (m_bufferData != NULL) {
        safe_Free(m_bufferData);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_CLog()
//
//! \brief
//!   Description: it will open the file and check that we can open it and then get the size of the file
//!                calls the ReadInBuffer() function for acctual reading in 
//
//  Entry:
//! \param masterData = Main Json node where all the data should be added to
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
void CLog::get_CLog()
{
    m_logStatus = eReturnValues::IN_PROGRESS;
    std::ifstream fb(m_fileName.c_str(), std::ios::in | std::ios::binary);
    if (fb.is_open())
    {
        fb.seekg(0, std::ios::end);
        m_size = static_cast<size_t>(fb.tellg());
        fb.seekg(0, std::ios::beg);			//set back to beginning of the file now that we know the size
        fb.close();
    }
    else
    {
        m_logStatus = eReturnValues::FILE_OPEN_ERROR;
    }
    m_bufferData = static_cast<char*>(calloc(m_size, sizeof(char)));

    if (m_size != 0 && m_logStatus != eReturnValues::FILE_OPEN_ERROR)
    {
        m_logStatus = read_In_Buffer();
    }
}

//-----------------------------------------------------------------------------
//
//! \fn get_CLog()
//
//! \brief
//!   Description: Function to read in the buffer to internal buffer data. 
//
//  Entry:
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
void CLog::get_CLog(const uint8_t * pBuf, size_t logSize)
{
	m_logStatus = eReturnValues::FILE_OPEN_ERROR;

	if (pBuf != NULL)
	{
		m_bufferData = static_cast<char*>(calloc(logSize, sizeof(char)));
		if (m_bufferData)
		{
#ifndef __STDC_SECURE_LIB__ 
            memcpy(m_bufferData, pBuf, logSize);
#else
			memcpy_s(m_bufferData, logSize, pBuf, logSize);
#endif
			m_logStatus = eReturnValues::IN_PROGRESS;
		}			
	}
}

//-----------------------------------------------------------------------------
//
//! \fn read_In_Buffer()
//
//! \brief
//!   Description: it will open the file and reads in the data to the buffer 
//
//  Entry:
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CLog::read_In_Buffer()
{
    if (m_logStatus == eReturnValues::FILE_OPEN_ERROR)
    {
        return eReturnValues::FILE_OPEN_ERROR;
    }
    char * pData = &m_bufferData[0];
    std::fstream logFile(m_fileName.c_str(), std::ios::in | std::ios::binary);//only allow reading as a binary file
    if (logFile.is_open())
    {
        logFile.read(pData, static_cast<std::streamsize>(m_size));
        logFile.close();
    }
    else
    {
        m_logStatus = eReturnValues::FILE_OPEN_ERROR;
        return eReturnValues::FILE_OPEN_ERROR;
    }

    if (eVerbosityLevels::VERBOSITY_DEFAULT < g_verbosity)
    {
        printf("\nLoadbinbuf read %zd bytes into buffer.\n", m_size);
    }

    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn read_In_Log()
//
//! \brief
//!   Description: it will open the file and reads in the log not as a binary file
//
//  Entry:
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
void CLog::read_In_Log()
{
    m_logStatus = eReturnValues::IN_PROGRESS;

    //open the file and see what the size is first
    std::ifstream fb(m_fileName.c_str(), std::ios::in);
    if (fb.is_open())
    {
        fb.seekg(0, std::ios::end);
        m_size = static_cast<size_t>(fb.tellg());
        fb.seekg(0, std::ios::beg);			//set back to beginning of the file now that we know the size
        fb.close();
    }
    else
    {
        m_logStatus = eReturnValues::FILE_OPEN_ERROR;
    }
    //set the size of the buffer
    m_bufferData = static_cast<char*>(calloc(m_size, sizeof(char)));

    // now we need to read in the buffer 
    if (m_size != 0 && m_logStatus != eReturnValues::FILE_OPEN_ERROR)
    {
        char* pData = &m_bufferData[0];
        std::fstream logFile(m_fileName.c_str(), std::ios::in | std::ios::binary);//only allow reading as a binary file
        if (logFile.is_open())
        {
            logFile.read(pData, static_cast<std::streamsize>(m_size));
            logFile.close();

            if (m_bufferData != NULL)
            {
                v_Buff.insert(v_Buff.end(), &m_bufferData[0], &m_bufferData[m_size]);
            }

            m_logStatus = eReturnValues::SUCCESS;
        }
        else
        {
            m_logStatus = eReturnValues::FILE_OPEN_ERROR;

        }
        if (m_bufferData != NULL) {
            safe_Free(m_bufferData);
        }
    }


}
