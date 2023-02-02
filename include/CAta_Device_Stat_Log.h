//
// CAta_Device_Stat_Log.h
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

// \file CAta_Device_Stat_Log.h
// \brief Defines the function calls and structures for parsing Seagate logs
#pragma once
#include "common.h"
#include "CLog.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef ATADEVICESTAT
#define ATADEVICESTAT
#pragma pack(push, 1)
	typedef struct _sStatusResponse
	{
		uint16_t    formatVersion;                                  //!< Status Response format version number.                         0 - 1 
		uint16_t    SCTversion;                                     //!< Manufacturer�s vendor specific implementation version number   2 - 3
		uint16_t    SCTspec;                                        //!< Highest level of SCT Technical Report supported                4 - 5
		uint32_t    statusFlag;                                     //!< Bit 0: Segment Initialized Flag.                               6 - 9
		uint8_t     driveStatus;                                    //!< drive status                                                   10
		uint8_t     reserved1[3];                                   //!< reserved                                                       11 - 13
		uint16_t    extenededStatusCode;                            //!< Status of last SCT command issued. FFFFh if SCT command        14 - 15 
		uint16_t    actionCode;                                     //!< Action code of last SCT command issued.                        16 - 17
		uint16_t    functionCode;                                   //!< Function code of last SCT command issued.                      18 - 19
		uint8_t     reserved2[20];                                  //!< reserved                                                       20 - 39
		uint64_t    lba;                                            //!< lba                                                            40 - 47
		uint8_t     reserved3[154];                                 //!< reserved                                                       48 - 199
		uint8_t     temp;                                           //!< Current drive HDA temperature in degrees Celsius.              200
		uint8_t     reserved4;                                      //!< reserved                                                       201
		uint8_t     maxTemp;                                        //!< Current drive HDA temperature in degrees Celsius.              202
		uint8_t     reserved5;                                      //!< reserved                                                       203
		uint8_t     lifeMaxTemp;                                    //!< Current drive HDA temperature in degrees Celsius.              204
		uint8_t     reserved6;                                      //!< reserved                                                       205
		uint8_t     reserved7[307];                                 //!< to the end                                                     206 - 511   
		_sStatusResponse() : formatVersion(0), SCTversion(0), SCTspec(0), statusFlag(0), driveStatus(0),
#if defined _WIN32  || (__cplusplus && __cplusplus >= 201103L)
			reserved1{ 0 },
#endif
			extenededStatusCode(0), actionCode(0), functionCode(0), 
#if defined _WIN32  || (__cplusplus && __cplusplus >= 201103L)
			reserved2{ 0 },
#endif
			lba(0),
#if defined _WIN32  || (__cplusplus && __cplusplus >= 201103L)
			reserved3{ 0 }, 
#endif
			temp(0), reserved4(), maxTemp(0), reserved5(0), lifeMaxTemp(0), reserved6(0)
#if defined _WIN32  || (__cplusplus && __cplusplus >= 201103L)
			//CLANG++ does not like this and the best I can find on the web is that this is a C++11 addition for initializing all elements of an array to zero - TJE
			, reserved7{ 0 }
#endif
		{};
	}sStatusResponse;
	typedef struct _sHeader
	{
		uint16_t   RevNum;
		uint8_t   LogPageNum;
		uint8_t   Reserved[5];
        _sHeader() :RevNum(0), LogPageNum(0), Reserved() {};   // removed the Reserved init, because of clang issue with setting it zero's
	}sHeader;
	typedef struct _sLogPage01
	{
		sHeader		header;									//!< header for the General Statistics Log 01h
		uint64_t	lifeTimePWRResets;						//!< Lifetime Power on Resets
		uint64_t	poh;									//!< Power on Hours
		uint64_t	logSectWritten;							//!< Logical Sectors written
		uint64_t	numberOfWrites;							//!< Number of Write Commands
		uint64_t	logSectRead;							//!< Logical Sectors Read
		uint64_t	numberOfReads;							//!< Number of Read Commands
		uint64_t	date;									//!< Date and Time Timestamp
		uint64_t	pendingErrorCount;						//!< Pending Error Count
		uint64_t	workLoad;								//!< Workload Utilized
		uint64_t	utilRate;								//!< Utilization Usage Rate
		uint64_t	resourceAval;							//!< Resource Avablity
		uint64_t	randomWriteResourcesUsed;				//!< Random Write Resources Used
		_sLogPage01() : header(), lifeTimePWRResets(0), poh(0), logSectWritten(0), numberOfWrites(0), logSectRead(0), numberOfReads(0), date(0), pendingErrorCount(0), \
			workLoad(0), utilRate(0), resourceAval(0), randomWriteResourcesUsed(0) {};
	}sLogPage01;
	typedef struct _sDEVICELOGFOUR
	{
		sHeader		header;									//!< header for the Generail Error Statistics Log
		uint64_t	numberReportedECC;						//!< Number of Reported Uncorrectable Errors
		uint64_t	resets;									//!< Number of Resets Between Command Acceptance and Command Completion
		uint64_t	statusChanged;							//!< Physical Element Status Changed
		_sDEVICELOGFOUR() : header(), numberReportedECC(0), resets(0), statusChanged(0) {};
	}sDeviceLog04;
	typedef struct _DEVICELOGTHREE
	{
		uint32_t SpdPoh;									//!< Spindle Motor Power-on Hours
		uint32_t HeadFlyHour;								//!< Head Flying Hours
		uint32_t HeadLoadEvent;								//!< Head Load Events
		uint32_t ReLogicalSec;								//!< Number of Reallocated Logical Sectors
		uint32_t ReadRecAtmp;								//!< Read Recovery Attempts 
		uint32_t MeStartFail;								//!< Number of Mechanical Start Failures
		uint32_t ReCandidate;								//!< Number of Reallocation Candidate Logical Sectors
		uint32_t UnloadEvent;								//!< Number of High Priority Unload Events
		_DEVICELOGTHREE() :SpdPoh(0), HeadFlyHour(0), HeadLoadEvent(0), ReLogicalSec(0), ReadRecAtmp(0), MeStartFail(0), ReCandidate(0), UnloadEvent(0) {};
	}sDeviceLog3;

	typedef struct _DEVICELOGSIX
	{
		uint32_t HwReset;
		uint32_t ASREvent;
		uint32_t CRCError;
		_DEVICELOGSIX() : HwReset(0), ASREvent(0), CRCError(0) {};
	}sDeviceLog6;
#pragma pack(pop)


    class CSAtaDevicStatisticsTempLogs 
    {
    protected:
        std::string                         m_name;                                                     //!< name of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
		size_t								m_logSize;													//!< size fo the log
        eReturnValues						m_status;                                                   //!< holds the status of the class 
		size_t								m_dataSize;													//!< data size read in form the clog
        JSONNODE							*JsonData;                                                  //!< json master data
    public:
        CSAtaDevicStatisticsTempLogs();
        CSAtaDevicStatisticsTempLogs(uint8_t *buffer,JSONNODE *masterData);
        CSAtaDevicStatisticsTempLogs(const std::string &fileName, JSONNODE *masterData);
        virtual ~CSAtaDevicStatisticsTempLogs();
        eReturnValues parse_SCT_Temp_Log();
        eReturnValues get_Status(){ return m_status; };
    };

    class CAtaDeviceStatisticsLogs 
    {
    protected:
        std::string                         m_name;                                                     //!< name of the class
		eReturnValues                       m_status;                                                   //!< holds the status of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
        size_t                              m_deviceLogSize;                                            //!< Log size 
        sStatusResponse                     m_Response;                                                 //!< status response

	eReturnValues ParseSCTDeviceStatLog(JSONNODE *masterData);
        bool isBit63Set(uint64_t *value);
        bool isBit62Set(uint64_t *value);
        bool isBit61Set(uint64_t *value);
        bool isBit60Set(uint64_t *value);
        bool isBit59Set(uint64_t *value);
        void DeviceStatFlag(uint64_t *value, JSONNODE *masterData);
        uint8_t  CheckStatusAndValid_8(uint64_t *value);
        int8_t  CheckStatusAndValidSigned_8(uint64_t *value);
        uint32_t CheckStatusAndValid_32(uint64_t *value);
        void logPage00(uint64_t *value);
        void logPage01(uint64_t *value, JSONNODE *masterData);
        void logPage02(uint64_t *value, JSONNODE *masterData);
        void logPage03(uint64_t *value, JSONNODE *masterData);
        void logPage04(uint64_t *value, JSONNODE *masterData);
        void logPage05(uint64_t *value, JSONNODE *masterData);
        void logPage06(uint64_t *value, JSONNODE *masterData);
        void logPage07(uint64_t *value, JSONNODE *masterData);
		
    public:
        CAtaDeviceStatisticsLogs();
        CAtaDeviceStatisticsLogs(const std::string &fileName, JSONNODE *masterData);
        CAtaDeviceStatisticsLogs(uint32_t logSize, JSONNODE *masterData, uint8_t *buffer);
        virtual ~CAtaDeviceStatisticsLogs();
        eReturnValues get_Device_Stat_Status(){ return m_status; };
    };
#endif  //ATADEVICESTAT

}
