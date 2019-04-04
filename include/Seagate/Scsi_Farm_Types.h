//
// Scsi_Farm_Types.h   Structures of SCSI FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Scsi_Farm_Types.h  
#pragma once

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"

#pragma pack(push, 1)
typedef enum _eLogPageTypes
{
	FARM_HEADER_PARAMETER,
	GENERAL_DRIVE_INFORMATION_PARAMETER,
	WORKLOAD_STATISTICS_PARAMETER,
	ERROR_STATISTICS_PARAMETER,
	ENVIRONMENTAL_STATISTICS_PARAMETER,
	RELIABILITY_STATISTICS_PARAMETER,
	RESERVED_FOR_FUTURE_STATISTICS_1,
	RESERVED_FOR_FUTURE_STATISTICS_2,
	RESERVED_FOR_FUTURE_STATISTICS_3,
	RESERVED_FOR_FUTURE_STATISTICS_4,
	RESERVED_FOR_FUTURE_STATISTICS_5,
	RESERVED_FOR_FUTURE_STATISTICS_6,
	RESERVED_FOR_FUTURE_STATISTICS_7,
	RESERVED_FOR_FUTURE_STATISTICS_8,
	RESERVED_FOR_FUTURE_STATISTICS_9,
	RESERVED_FOR_FUTURE_STATISTICS_10,
	DISC_SLIP_IN_MICRO_INCHES_BY_HEAD,
	BIT_ERROR_RATE_OF_ZONE_0_BY_DRIVE_HEAD,
	DOS_WRITE_REFRESH_COUNT,
	DVGA_SKIP_WRITE_DETECT_BY_HEAD,
	RVGA_SKIP_WRITE_DETECT_BY_HEAD,
	FVGA_SKIP_WRITE_DETECT_BY_HEAD,
	SKIP_WRITE_DETECT_THRESHOLD_EXCEEDED_COUNT_BY_HEAD,
	ACFF_SINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD,
	ACFF_COSINE_1X_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD,
	PZT_CALIBRATION_VALUE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD,
	MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD,
	NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD,
	VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD,
	NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD,
	CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_AT_ITERATION_LEVEL_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES,
	CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES,
	CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES,
	NUMBER_OF_RESIDENT_GLIST_ENTRIES,
	NUMBER_OF_PENDING_ENTRIES,
	DOS_OUGHT_TO_SCAN_COUNT_PER_HEAD,
	DOS_NEED_TO_SCAN_COUNT_PER_HEAD,
	DOS_WRITE_FAULT_SCAN_COUNT_PER_HEAD,
	WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART,
	DOS_WRITE_COUNT_THRESHOLD_PER_HEAD,
	RESERVED_FOR_FUTURE_EXPANSION_2,
	RESERVED_FOR_FUTURE_EXPANSION_3,
	RESERVED_FOR_FUTURE_EXPANSION_4,
	RESERVED_FOR_FUTURE_EXPANSION_5,
	RESERVED_FOR_FUTURE_EXPANSION_6,
	RESERVED_FOR_FUTURE_EXPANSION_7,
	RESERVED_FOR_FUTURE_EXPANSION_8,
	RESERVED_FOR_FUTURE_EXPANSION_9,
	CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0,
	CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1,
	CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2,
	CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0,
	CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1,
	CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2,
	RESERVED_FOR_FUTURE_EXPANSION_10,
	RESERVED_FOR_FUTURE_EXPANSION_11,
	RESERVED_FOR_FUTURE_EXPANSION_12,
	RESERVED_FOR_FUTURE_EXPANSION_13,
	RESERVED_FOR_FUTURE_EXPANSION_14,
	RESERVED_FOR_FUTURE_EXPANSION_15,
	RESERVED_FOR_FUTURE_EXPANSION_16,
	RESERVED_FOR_FUTURE_EXPANSION_17,
	RESERVED_FOR_FUTURE_EXPANSION_18,
	RESERVED_FOR_FUTURE_EXPANSION_19,
	APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER,
	APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_INNER,
	APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_MIDDLE,
	RESERVED_FOR_FUTURE_EXPANSION,
}eLogPageTypes;

typedef struct _sScsiLogParameter
{
	uint8_t             pageCode;                                   //!< page code of the Logs Parameters
	uint8_t             subpage;                                    //!< sub page code for the log Parameters
	uint16_t            length;                                     //!< legnth of the sub page code
	_sScsiLogParameter() :pageCode(0), subpage(0), length(0) {};
}sScsiLogParameter;

typedef struct _sScsiPageParameter
{
	uint16_t            pramCode;                                  //!< paramaeter code 
	uint8_t             pramControl;                               //!< parameter control byte
	uint8_t             plen;                                      //!< parameter length
	_sScsiPageParameter() :pramCode(0), pramControl(0), plen(0) {};
}sScsiPageParameter;

typedef struct _sScsiFarmHeader
{
	sScsiPageParameter  pPageHeader;								//!< pointer the farm header page parameter
	sFarmHeader			farmHeader;									//!< structure of the farm header
}sScsiFarmHeader;

typedef struct _sScsiDriveInfo
{
	sScsiPageParameter  pPageHeader;								//!< pointer the farm header page parameter
	uint64_t            pageNumber;
	uint64_t            copyNumber;
	uint64_t            serialNumber;
	uint64_t            serialNumber2;
	uint64_t            worldWideName;
	uint64_t            worldWideName2;
	uint64_t            deviceInterface;
	uint64_t            deviceCapacity;                             //!< 48-bit Device Capacity
	uint64_t            psecSize;                                   //!< Physical Sector Size in Bytes
	uint64_t            lsecSize;                                   //!< Logical Sector Size in Bytes
	uint64_t            deviceBufferSize;                           //!< Device Buffer Size in Bytes
	uint64_t            heads;                                      //!< Number of Heads
	uint64_t            factor;                                     //!< Device Form Factor 
	uint64_t            rotationRate;                               //!< Rotational Rate of Device 
	uint64_t            firmware;
	uint64_t            firmwareRev;
	uint64_t            reserved;                                   //!< reserved
	uint64_t            reserved1;									//!< reserved
	uint64_t            reserved2;									//!< reserved
	uint64_t            poh;                                        //!< Power-on Hours
	uint64_t            reserved3;									//!< reserved
	uint64_t            reserved4;									//!< reserved
	uint64_t            headLoadEvents;                             //!< Head Load Events
	uint64_t            powerCycleCount;                            //!< Power Cycle Count
	uint64_t            resetCount;                                 //!< Hardware Reset Count
	uint64_t            reserved5;                                  //!< reserved
	uint64_t            NVC_StatusATPowerOn;                        //!< NVC Status on Power-on
	uint64_t            timeAvailable;                              //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
	uint64_t            firstTimeStamp;                             //!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
	uint64_t            lastTimeStamp;                              //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds
	_sScsiDriveInfo() :pageNumber(0), copyNumber(0), serialNumber(0), serialNumber2(0), worldWideName(0), worldWideName2(0), deviceInterface(0), deviceCapacity(0), \
		psecSize(0), lsecSize(0), deviceBufferSize(0), heads(0), factor(0), rotationRate(0), firmware(0), firmwareRev(0), reserved(0), reserved1(0), reserved2(0), poh(0), reserved3(0), \
		reserved4(0), headLoadEvents(0), powerCycleCount(0), resetCount(0), reserved5(0), NVC_StatusATPowerOn(0), timeAvailable(0), firstTimeStamp(0), lastTimeStamp(0) {};
}sScsiDriveInfo;

typedef struct _sScsiWorkLoadStat
{
	sScsiPageParameter  pPageHeader;								//!< pointer the farm header page parameter
	sWorkLoadStat		workLoad;									//!< structure of the work load Stat
}sScsiWorkLoadStat;

typedef struct _sScsiErrorStat
{
	sScsiPageParameter  pPageHeader;								//!< pointer the farm header page parameter
	uint64_t            pageNumber;									//!< Page Number = 3
	uint64_t            copyNumber;									//!< Copy Number
	uint64_t            totalReadECC;								//!< Number of Unrecoverable Read Errors
	uint64_t            totalWriteECC;								//!< Number of Unrecoverable Write Errors
	uint64_t            totalReallocations;							//!< Number of Reallocated Sectors
	uint64_t            reserved;									//!< Reserved
	uint64_t            totalMechanicalFails;						//!< Number of Mechanical Start Failures
	uint64_t            totalReallocatedCanidates;					//!< Number of Reallocated Candidate Sectors
	uint64_t            reserved1;									//!< Reserved
	uint64_t            reserved2;									//!< Reserved
	uint64_t            reserved3;									//!< Reserved
	uint64_t            reserved4;									//!< Reserved
	uint64_t            reserved5;									//!< Reserved
	uint64_t            attrIOEDCErrors;							//!< Number of IOEDC Errors 
	uint64_t            reserved6;									//!< Reserved
	uint64_t            reserved7;									//!< Reserved
	uint64_t            reserved8;									//!< Reserved
	uint64_t            totalFlashLED;								//!< Total Flash LED (Assert) Events
	uint64_t            reserved9;									//!< Reserved
	uint64_t            FRUCode;									//!< FRU code if smart trip from most recent SMART Frame (SAS only) 
}sScsiErrorStat;

typedef struct _sScsiEnvironmentStat
{
	sScsiPageParameter  pPageHeader;								//!< pointer the farm header page parameter
	uint64_t            pageNumber;									//!< Page Number = 4
	uint64_t            copyNumber;									//!< Copy Number
	uint64_t            curentTemp;									//!< Current Temperature in Celsius
	uint64_t            highestTemp;								//!< Highest Temperature in Celsius
	uint64_t            lowestTemp;									//!< Lowest Temperature
	uint64_t            reserved;									//!< Reserved
	uint64_t            reserved1;									//!< Reserved
	uint64_t            reserved2;									//!< Reserved
	uint64_t            reserved3;									//!< Reserved
	uint64_t            reserved4;									//!< Reserved
	uint64_t            reserved5;									//!< Reserved
	uint64_t            reserved6;									//!< Reserved
	uint64_t            reserved7;									//!< Reserved
	uint64_t            maxTemp;									//!< Specified Max Operating Temperature
	uint64_t            minTemp;									//!< Specified Min Operating Temperature
	uint64_t            reserved8;									//!< Reserved
	uint64_t            reserved9;									//!< Reserved
	uint64_t            humidity;									//!< Current Relative Humidity (in units of .1%)
	uint64_t            humidityRatio;								//!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
	uint64_t            currentMotorPower;							//!< Current Motor Power, value from most recent SMART Summary Frame6
}sScsiEnvironmentStat;

typedef struct _sScsiReliabilityStat
{
	sScsiPageParameter  pPageHeader;								 //!< pointer the farm header page parameter
	uint64_t            pageNumber;                                  //!< Page Number = 5
	uint64_t            copyNumber;                                  //!< Copy Number
	uint64_t            lastIDDTest;                                 //!< Timestamp of last IDD test
	uint64_t            cmdLastIDDTest;                              //!< Sub-command of last IDD test
	uint64_t            gListReclamed;                               //!< Number of G-List Reclamations 
	uint64_t            servoStatus;                                 //!< Servo Status (follows standard DST error code definitions)
	uint64_t            altsBeforeIDD;                               //!< Number of Alt List Entries Before IDD Scan
	uint64_t            altsAfterIDD;                                //!< Number of Alt List Entries After IDD Scan
	uint64_t            gListBeforIDD;                               //!< Number of Resident G-List Entries Before IDD Scan
	uint64_t            gListAfterIDD;                               //!< Number of Resident G-List Entries After IDD Scan
	uint64_t            scrubsBeforeIDD;                             //!< Number of Scrub List Entries Before IDD Scan
	uint64_t            scrubsAfterIDD;                              //!< Number of Scrub List Entries After IDD Scan
	uint64_t            numberDOSScans;                              //!< Number of DOS Scans Performed
	uint64_t            numberLBACorrect;                            //!< Number of LBAs Corrected by ISP
	uint64_t            numberValidParitySec;                        //!< Number of Valid Parity Sectors
	uint64_t            numberRAWops;                                //!< Number of RAW Operations
	uint64_t            reserved;									 //!< Reserved
	uint64_t            reserved1;									 //!< Reserved
	uint64_t            reserved2;									 //!< Reserved
	uint64_t            reserved3;									 //!< Reserved
	uint64_t            reserved4;									 //!< Reserved
	uint64_t            reserved5;									 //!< Reserved
	uint64_t            reserved6;									 //!< Reserved
	uint64_t            microActuatorLockOut;                        //!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
	uint64_t            diskSlipRecalPerformed;                      //!< Number of disc slip recalibrations performed
	uint64_t            heliumPressuretThreshold;                    //!< helium Pressure Threshold Trip
	uint64_t            rvAbsuluteMean;                              //!< RV Absulute Mean
	uint64_t            maxRVAbsuluteMean;                           //!< Max RV absulute Mean
	uint64_t            idleTime;                                    //!< idle Time value from the most recent SMART Summary Frame
}sScsiReliabilityStat;

typedef struct _sHeadInformation
{
	sScsiPageParameter  pageHeader;                                  //!<  header page parameters
	uint64_t            headValue[MAX_HEAD_COUNT];                   //!< head information
}sHeadInformation;

typedef struct _sScsiFarmFrame
{
	sScsiFarmHeader         farmHeader;                             //!< Farm header 
	sScsiDriveInfo          driveInfo;                              //!< drive information structure 
	sStringIdentifyData     identStringInfo;                        //!< string information 
	sScsiWorkLoadStat       workLoadPage;                           //!< work load page
	sScsiErrorStat          errorPage;                              //!< error page information
	sScsiEnvironmentStat    environmentPage;                        //!< environment page
	sScsiReliabilityStat    reliPage;                               //!< reliability data
	sHeadInformation        discSlipPerHead;
	sHeadInformation        bitErrorRateByHead;
	sHeadInformation        dosWriteRefreshCountByHead;
	sHeadInformation        dvgaSkipWriteDetectByHead;
	sHeadInformation        rvgaSkipWriteDetectByHead;
	sHeadInformation        fvgaSkipWriteDetectByHead;
	sHeadInformation        skipWriteDectedThresholdExceededByHead;
	sHeadInformation        acffSine1xValueByHead;
	sHeadInformation        acffCosine1xValueByHead;
	sHeadInformation        pztCalibrationValueByHead;
	sHeadInformation        mrHeadResistanceByHead;
	sHeadInformation        numberOfTMDByHead;
	sHeadInformation        velocityObserverByHead;
	sHeadInformation        numberOfVelocityObservedByHead;
	sHeadInformation        currentH2SATPercentagedbyHead;
	sHeadInformation        currentH2STAmplituedByHead;
	sHeadInformation        currentH2STAsymmetryByHead;
	sHeadInformation        ResidentGlistEntries;
	sHeadInformation        ResidentPlistEntries;
	sHeadInformation        DOSOoughtToScan;
	sHeadInformation        DOSNeedToScan;
	sHeadInformation        DOSWriteFaultScan;
	sHeadInformation        writePowerOnHours;
	sHeadInformation		dosWriteCount; 
	sHeadInformation        currentH2STTrimmedbyHeadZone0;
	sHeadInformation        currentH2STTrimmedbyHeadZone1;
	sHeadInformation        currentH2STTrimmedbyHeadZone2;
	sHeadInformation        currentH2STIterationsByHeadZone0;
	sHeadInformation        currentH2STIterationsByHeadZone1;
	sHeadInformation        currentH2STIterationsByHeadZone2;
	sHeadInformation        appliedFlyHeightByHeadOuter;
	sHeadInformation        appliedFlyHeightByHeadInner;
	sHeadInformation        appliedFlyHeightByHeadMiddle;
	std::vector<eLogPageTypes>   vFramesFound;
}sScsiFarmFrame;

#pragma pack(pop)