// CScsi_Informational_Exeptions_Log.h  Definition of the Informational Exceptions Log page provides a place for reporting detail about exceptions.
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Informational_Exeptions_Log.h  Definition of the Informational Exceptions Log page provides a place for reporting detail about exceptions.
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIINFOLOG
#define SCSIINFOLOG

	class CScsiInformationalExeptionsLog
	{
	private:
#pragma pack(push, 1)
		typedef struct _sInfoExeptionsParameters
		{
			uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
			uint8_t			paramControlByte;					//<! binary format list log parameter
			uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
			uint8_t			senseCode;							//<! INFORMATIONAL EXCEPTION ADDITIONAL SENSE CODE
			uint8_t			senseCodeQualifier;					//<! INFORMATIONAL EXCEPTION ADDITIONAL SENSE CODE QUALIFIER
			uint8_t			temp;								//<! MOST RECENT TEMPERATURE READING
			uint8_t			tempLimit;							//<! VENDOR HDA TEMPERATURE TRIP POINT
			uint8_t			maxTemp;							//<! MAXIMUM TEMPERATURE
			uint8_t			vendor1;							//<! Vendor specific
			uint8_t			vendor2;							//<! Vendor specific
			uint8_t			vendor3;							//<! Vendor specific

			_sInfoExeptionsParameters (): paramCode(0), paramControlByte(0), paramLength(0), senseCode(0), senseCodeQualifier(0), \
				temp(0), tempLimit(0), maxTemp(0), vendor1(0), vendor2(0), vendor3(0) {};
		} sExeptionsParams;

#pragma pack(pop)
	protected:
		uint8_t						*pData;						//<! pointer to the data
		std::string					m_infoName;					//<! class name	
		eReturnValues				m_infoStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sExeptionsParams			*m_Exeptions;				//<! Information Exeptions structure 

		void process_Informational_Exceptions_Data(JSONNODE *exeptionData, uint16_t count);
		eReturnValues get_Informational_Exceptions_Data(JSONNODE *masterData);
	public:
		CScsiInformationalExeptionsLog();
		CScsiInformationalExeptionsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiInformationalExeptionsLog();
		virtual eReturnValues get_Log_Status() { return m_infoStatus; };
		virtual eReturnValues get_Informational_Exceptions(JSONNODE *masterData) { return get_Informational_Exceptions_Data(masterData); };

	};
#endif
}
