//
// CAta_NCQ_Command_Error_Log.h   parser the NCQ Command Error log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************//
// \file CAta_NCQ_Command_Error_Log.h   
// \brief
#pragma once
#include "common.h"
#include "CLog.h"
#include "libjson.h"


namespace opensea_parser {
#ifndef ATANCQCMDERROR
#define ATANCQCMDERROR

#define MAX_QUEUE           32
#pragma pack(push, 1)
	typedef struct _sNCQError
	{
		uint8_t         NCQbit;         //!< ncq bit name
		uint8_t         reserved;
		uint8_t         status;
		uint8_t         error;
		uint8_t         lba1;
		uint8_t         lba2;
		uint8_t         lba3;
		uint8_t         device;
		uint8_t         lba4;
		uint8_t         lba5;
		uint8_t         lba6;
		uint8_t         reserved1;
		uint16_t        count;
		uint8_t         senseKey;
		uint8_t         senseCodeField;
		uint8_t         senseCodeQualifier;
		uint64_t        finalLBA;
	}sNCQError;
	typedef struct _sNCQWriteErrors
	{
		uint16_t        writePointer1;              //!< first 16 of 48 
		uint16_t        writePointer2;              //!< second 16 of 48
		uint16_t        writePointer3;              //!< third 16 of 48
	}sNCQWriteErrors;


#pragma pack(pop)
    class CAta_NCQ_Command_Error_Log 
    {
    private:
        uint32_t                writeValid;                 //!< write pointer valid field
        std::string             m_name;                                 //!< name of the class
        eReturnValues           m_status;                               //!< the status of the class  
        sNCQError               *ncqError;                              //!< pointer to the structure
        sNCQWriteErrors         *ncqWrite;                              //!< pointer for when the buffer supports the extra write infromation
        uint8_t                 *pBuf;                                  //!< Creation of the Buffer.
        uint64_t                m_LBA;                                  //!< the LBA for the command
        sNCQWriteErrors         writeErrors[MAX_QUEUE];                 //!< array of all the write errors
        uint64_t                fullWriteErrors[MAX_QUEUE];             //!< array of all the write errors in a unint64_t so we can print them correctly

        bool get_Bit_Name_Info(JSONNODE *NCQInfo);
        bool create_LBA();
    public:
        CAta_NCQ_Command_Error_Log();
        CAta_NCQ_Command_Error_Log(const std::string & fileName);
        CAta_NCQ_Command_Error_Log(uint8_t *buffer);
        ~CAta_NCQ_Command_Error_Log();
		eReturnValues get_NCQ_Command_Error_Log_Status() { return m_status; };
        eReturnValues get_NCQ_Command_Error_Log(JSONNODE *masterData);
    };
#endif  //ATANCQCMDERROR
}