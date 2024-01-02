// CScsi_Background_Scan_Log.h  Definition of Background Scan Log Page
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Background_Scan_Log.h  Definition of Background Scan Log Page
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include <vector>
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSISCANLOG
#define SCSISCANLOG
#pragma pack(push, 1)
	typedef struct _sBackgroundScanStatusParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		uint32_t		timeStamp;						    //<! Time Stamp (Minutes)
		uint8_t			reserved;
		uint8_t			status;								//<! background scan status
		uint16_t		scansPerformed;						//<! number of background scans performed
		uint16_t		mediumProgress;						//<! background medium scan progress
		uint16_t		mediumPerformed;					//<! background medium scan performed
		_sBackgroundScanStatusParameters() : paramCode(0), paramControlByte(0), paramLength(0), timeStamp(0), reserved(0), status(0), \
			scansPerformed(0), mediumProgress(0), mediumPerformed(0) {};
	} sScanStatusParams;
	typedef struct _sBackgroundScanEncountersParameters
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
		uint32_t		powerOnMinutes;						//<! Accumulated Power on Minutes
		uint8_t			status;								//<! Reassign status, Sense Key
		uint8_t			senseCode;							//<! additional sense code
		uint8_t			codeQualifier;						//<! additional sense code qualifier
		uint16_t		vendorSpecific;						//<! vendor Specific
		uint16_t		vendorSpecific1;					//<! vendor Specific
		uint8_t			vendorSpecific2;					//<! vendor Specific
		uint64_t		LBA;								//<! Logical Block Address
		_sBackgroundScanEncountersParameters() : paramCode(0), paramControlByte(0), paramLength(0), powerOnMinutes(0), status(0), senseCode(0), \
			codeQualifier(0), vendorSpecific(0), vendorSpecific1(0), vendorSpecific2(0), LBA(0) {};
	} sScanFindingsParams;

    typedef struct _sBackgroundScanParamHeader // For other paramcodes
    {
        uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
        uint8_t			paramControlByte;					//<! binary format list log parameter
        uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
    }sBackgroundScanParamHeader;

#pragma pack(pop)
	class CScsiScanLog
	{
	private:
	protected:
		std::vector<uint8_t>        v_Data;                     //!< vector for holding the buffer data
		std::string					m_ScanName;					//<! class name	
		eReturnValues				m_ScanStatus;			    //<! status of the class
		uint16_t					m_PageLength;				//<! length of the page
		size_t						m_bufferLength;			    //<! length of the buffer from reading in the log
		sScanStatusParams			*m_ScanParam;				//<! scan status parameters
		sScanFindingsParams			*m_defect;					//<! finding of defect structure
        sBackgroundScanParamHeader  *m_ParamHeader;             //<! for other param codes

		void get_Scan_Defect_Status_Description(std::string *defect);
		void get_Scan_Status_Description(std::string *scan);
		void process_Defect_Data(JSONNODE *defectData);
		void process_Scan_Status_Data(JSONNODE *scanData);
        void process_other_param_data(JSONNODE *scanData, size_t offset);
		eReturnValues get_Scan_Data(JSONNODE *masterData);
	public:
		CScsiScanLog();
		CScsiScanLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
		virtual ~CScsiScanLog();
		virtual eReturnValues get_Log_Status() { return m_ScanStatus; };
		virtual eReturnValues parse_Background_Scan_Log(JSONNODE *masterData) { return get_Scan_Data(masterData); };

	};
#endif
}
