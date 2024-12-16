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
	, m_bufferData()
    , m_logStatus(eReturnValues::UNKNOWN)
    , m_log(M_NULLPTR)
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
	, m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
    , m_log()
{
    m_log = new eFileParams();
    m_log->fileName = fileName;
    get_Log();
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
    , m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
    , m_log()
{
    m_log = new eFileParams();
    m_log->fileName = fileName;
    if (useV_Buff)
    {
        read_In_Log();
    }
    else
    {
        get_Log();
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
	, m_bufferData()
    , m_logStatus(eReturnValues::IN_PROGRESS)
{
    m_log = new eFileParams();
    m_log->secure->fileSize = logSize;
    //get_CLog(pBuf, logSize);
    read_In_Buffer();
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
    if (m_bufferData != M_NULLPTR) {
        safe_free(&m_bufferData);
    }
    if (m_log != M_NULLPTR)
    {
        free_Secure_File_Info(&m_log->secure);
    }
    delete m_log;
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
eReturnValues CLog::get_Log()
{
    eReturnValues retStatus = eReturnValues::SUCCESS;
    m_log->secure = secure_Open_File(m_log->fileName.c_str(),"rb", M_NULLPTR, M_NULLPTR, M_NULLPTR);

    if (m_log->secure->error == eSecureFileError::SEC_FILE_SUCCESS)
    {
        m_bufferData = static_cast<char*>(safe_calloc(m_log->secure->fileSize, sizeof(char)));

        if (m_log->secure->fileSize != 0 && m_logStatus != eReturnValues::FILE_OPEN_ERROR)
        {
            retStatus = read_In_Buffer();
            m_log->secure->error = secure_Close_File(m_log->secure);
            if (m_log->secure->error != eSecureFileError::SEC_FILE_SUCCESS)
            {
                retStatus = eReturnValues::FILE_OPEN_ERROR;
            }
            m_logStatus = retStatus;
        }
    }
    else
    {
        free_Secure_File_Info(&m_log->secure);   // also use on the closing of the file    eSecureFileError secure_Close_File(secureFileInfo* fileInfo);
        retStatus = eReturnValues::FILE_OPEN_ERROR;
        m_logStatus = retStatus;
    }
    return retStatus;
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
    if (m_logStatus == eReturnValues::IN_PROGRESS)
    {
        if (secure_Read_File(m_log->secure, pData, m_log->secure->fileSize, 
            sizeof(char), m_log->secure->fileSize / sizeof(char), 0) != eSecureFileError::SEC_FILE_SUCCESS)
        {
            m_logStatus = eReturnValues::FILE_OPEN_ERROR;
            return eReturnValues::FILE_OPEN_ERROR;
        }
    }
    if (eVerbosityLevels::VERBOSITY_DEFAULT < g_verbosity)
    {
        printf("\nLoadbinbuf read %zd bytes into buffer.\n", m_log->secure->fileSize);
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
    m_log->secure = secure_Open_File(m_log->fileName.c_str(), "rb", M_NULLPTR, M_NULLPTR, M_NULLPTR);
    if (m_log->secure->error == eSecureFileError::SEC_FILE_SUCCESS)
    {
        //set the size of the buffer
        m_bufferData = static_cast<char*>(safe_calloc(m_log->secure->fileSize, sizeof(char)));
        // now we need to read in the buffer 
        if (m_log->secure->fileSize != 0 && m_logStatus != eReturnValues::FILE_OPEN_ERROR)
        {
            //only allow reading as a binary file
            if (secure_Read_File(m_log->secure, m_bufferData, m_log->secure->fileSize,
                sizeof(char), m_log->secure->fileSize / sizeof(char), 0) != eSecureFileError::SEC_FILE_SUCCESS)
            {
                m_logStatus = eReturnValues::INSECURE_PATH;
            }
            else
            {
                m_logStatus = eReturnValues::SUCCESS;
            }

            if (m_bufferData != M_NULLPTR)
            {
                v_Buff.insert(v_Buff.end(), &m_bufferData[0], &m_bufferData[m_log->secure->fileSize]);
                if (v_Buff.size() == 0)
                {
                    m_logStatus = eReturnValues::INVALID_LENGTH;
                }
                else
                {
                    m_logStatus = eReturnValues::SUCCESS;
                }
            }

            safe_free(&m_bufferData);
        }
    }
    else
    {
        free_Secure_File_Info(&m_log->secure);   // also use on the closing of the file    eSecureFileError secure_Close_File(secureFileInfo* fileInfo);
        m_logStatus = eReturnValues::FILE_OPEN_ERROR;
    }
}
