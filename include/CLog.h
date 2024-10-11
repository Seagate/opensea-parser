//
// CLog.h   Definition of Base class CLog
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CLog.h   Definition of Base class CLog
//

#pragma once
#include <string>
#include "common_types.h"
#include "string_utils.h"
#include "secure_file.h"
#include "bit_manip.h"
#include "memory_safety.h"
#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef CLOGCLASS
#define CLOGCLASS
    typedef struct _eFileParams
    {
        std::string fileName;
        std::string filePath;
        secureFileInfo* secure;

    } eFileParams;

	class CLog
	{
	private:
    protected:
		std::string             m_name;                         //!< name of the class
        char                    *m_bufferData;                  //!< the buffer that the file will be read into
        eReturnValues           m_logStatus;                    //!< the log status.
        eFileParams*            m_log;
        std::vector<uint8_t>    v_Buff;                         //!< vector for holding the buffer data

	public:
		CLog();
        explicit CLog(const std::string& fileName);
        CLog(const std::string& fileName, bool useV_Buff);
		CLog(const uint8_t * pBuf, size_t logSize);
        virtual ~CLog();

        void get_CLog();
		void get_CLog(const uint8_t * pBuf, size_t logSize);
        inline eReturnValues read_In_Buffer();
        void read_In_Log();
        inline std::string get_Name() const { return m_name; }
        inline std::string get_File_Name() const { return m_log->fileName; }
        inline size_t get_Size() const { return m_log->secure->fileSize; }
        inline uint8_t *get_Buffer(){ return reinterpret_cast<uint8_t *>(m_bufferData); };
        inline uint8_t *get_Buffer_Offset(uint32_t offset){ return reinterpret_cast<uint8_t*>(&m_bufferData[offset]); };
        inline bool get_vBuffer(std::vector<uint8_t>& buff)
        {
            buff = v_Buff;
            if (buff.size() == 0)
            {
                return false;
            }
            return true;
        };
        char * get_log() { return m_bufferData; };
        inline eReturnValues get_Log_Status(){ return m_logStatus; };
        inline void set_Log_Status(eReturnValues status){ m_logStatus = status; };
	};
#endif  // end CLOGCLASS
}