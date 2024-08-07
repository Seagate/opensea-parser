//
// CAta_Ext_Comprehensive_Log.h
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

// \file CAta_Ext_Comprehensive_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include "common_types.h"
#include "bit_manip.h"
#include "CLog.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef ATAEXTCOMPLOG
#define ATAEXTCOMPLOG

    class CExtComp 
    {
    private:
    protected:			
		uint8_t								* pData;													//<! pointer to the buffer
		size_t								m_logSize;													//!< size of the log. 
        std::string                         m_name;                                                     //!< name of the class
        eReturnValues                       m_status;                                                   //!< holds the status of the class
        bool                                m_fileName;                                                   //!< set to true if the file name is set

		
		eReturnValues parse_Ext_Comp_Structure(uint32_t structNumber, uint32_t sector, JSONNODE *structureData);
		eReturnValues get_State_Meaning(std::string *stateMeaning, uint8_t state);
		eReturnValues parse_Ext_Comp_Log(JSONNODE *masterData);
    public:
        CExtComp();
		CExtComp(uint8_t *buffer, size_t logSize, JSONNODE *masterData);
        CExtComp(const std::string &fileName, JSONNODE *masterData);
        virtual ~CExtComp();

        eReturnValues get_EC_Status(){ return m_status; };

    };
#endif  // ATAEXTCOMPLOG
}