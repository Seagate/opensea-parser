//
// Scsi_Farm_Types.h   Structures of SCSI FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
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


namespace opensea_parser {
#pragma pack(push,1)
    typedef enum _eSASLogPageTypes
    {
        FARM_HEADER_PARAMETER,
        GENERAL_DRIVE_INFORMATION_PARAMETER,
        WORKLOAD_STATISTICS_PARAMETER,
        ERROR_STATISTICS_PARAMETER,
        ENVIRONMENTAL_STATISTICS_PARAMETER,
        RELIABILITY_STATISTICS_PARAMETER,
        GENERAL_DRIVE_INFORMATION_06,
        ENVIRONMENT_STATISTICS_PAMATER_07,
        WORKLOAD_STATISTICS_PAMATER_08,
        RESERVED_FOR_FUTURE_STATISTICS_4,
        RESERVED_FOR_FUTURE_STATISTICS_5,
        RESERVED_FOR_FUTURE_STATISTICS_6,
        RESERVED_FOR_FUTURE_STATISTICS_7,
        RESERVED_FOR_FUTURE_STATISTICS_8,
        RESERVED_FOR_FUTURE_STATISTICS_9,
        RESERVED_FOR_FUTURE_STATISTICS_10,
        RESERVED_FOR_FUTURE_HEAD_1,
        RESERVED_FOR_FUTURE_HEAD_2,
        RESERVED_FOR_FUTURE_HEAD_3,
        RESERVED_FOR_FUTURE_HEAD_4,
        RESERVED_FOR_FUTURE_HEAD_5,
        RESERVED_FOR_FUTURE_HEAD_6,
        RESERVED_FOR_FUTURE_HEAD_7,
        RESERVED_FOR_FUTURE_HEAD_8,
        RESERVED_FOR_FUTURE_HEAD_9,
        RESERVED_FOR_FUTURE_HEAD_10,
        MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD,  //0x001A
        RESERVED_FOR_FUTURE_HEAD_11,
        RESERVED_FOR_FUTURE_HEAD_12,
        RESERVED_FOR_FUTURE_HEAD_13,
        RESERVED_FOR_FUTURE_HEAD_14,
        CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES, //0x001F
        CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES, //0x0020
        NUMBER_OF_RESIDENT_GLIST_ENTRIES,
        NUMBER_OF_PENDING_ENTRIES,
        RESERVED_FOR_FUTURE_HEAD_15,
        RESERVED_FOR_FUTURE_HEAD_16,
        RESERVED_FOR_FUTURE_HEAD_17,
        WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART, //0x0036
        RESERVED_FOR_FUTURE_HEAD_18,
        CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD,
        CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD,
        RESERVED_FOR_FUTURE_HEAD_19,
        RESERVED_FOR_FUTURE_HEAD_20,
        RESERVED_FOR_FUTURE_HEAD_21,
        RESERVED_FOR_FUTURE_HEAD_22,
        RESERVED_FOR_FUTURE_HEAD_23,
        RESERVED_FOR_FUTURE_HEAD_24,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2,
        RESERVED_FOR_FUTURE_HEAD_25,
        RESERVED_FOR_FUTURE_HEAD_26,
        RESERVED_FOR_FUTURE_HEAD_27,
        RESERVED_FOR_FUTURE_HEAD_28,
        RESERVED_FOR_FUTURE_HEAD_29,
        RESERVED_FOR_FUTURE_HEAD_30,
        RESERVED_FOR_FUTURE_HEAD_31,
        RESERVED_FOR_FUTURE_HEAD_32,
        RESERVED_FOR_FUTURE_HEAD_33,
        RESERVED_FOR_FUTURE_HEAD_34,
        RESERVED_FOR_FUTURE_HEAD_35,
        RESERVED_FOR_FUTURE_HEAD_36,
        RESERVED_FOR_FUTURE_HEAD_37,
        SECOND_MR_HEAD_RESISTANCE,
        RESERVED_FOR_FUTURE_HEAD_38,
        RESERVED_FOR_FUTURE_HEAD_39,
        RESERVED_FOR_FUTURE_HEAD_40,
        RESERVED_FOR_FUTURE_HEAD_41,
        RESERVED_FOR_FUTURE_HEAD_42,
        RESERVED_FOR_FUTURE_HEAD_43,
        RESERVED_FOR_FUTURE_HEAD_44,
        RESERVED_FOR_FUTURE_HEAD_45,
        RESERVED_FOR_FUTURE_HEAD_46,
        RESERVED_FOR_FUTURE_HEAD_47,
        RESERVED_FOR_FUTURE_HEAD_48,
        RESERVED_FOR_FUTURE_HEAD_49,
        LUN_0_ACTUATOR,
        LUN_0_FLASH_LED,
        LUN_REALLOCATION_0,
        RESERVED_FOR_FUTURE_EXPANSION_42,
        RESERVED_FOR_FUTURE_EXPANSION_43,
        RESERVED_FOR_FUTURE_EXPANSION_44,
        RESERVED_FOR_FUTURE_EXPANSION_45,
        RESERVED_FOR_FUTURE_EXPANSION_46,
        RESERVED_FOR_FUTURE_EXPANSION_47,
        RESERVED_FOR_FUTURE_EXPANSION_48,
        RESERVED_FOR_FUTURE_EXPANSION_49,
        RESERVED_FOR_FUTURE_EXPANSION_50,
        RESERVED_FOR_FUTURE_EXPANSION_51,
        RESERVED_FOR_FUTURE_EXPANSION_52,
        RESERVED_FOR_FUTURE_EXPANSION_53,
        RESERVED_FOR_FUTURE_EXPANSION_54,
        LUN_1_ACTUATOR,
        LUN_1_FLASH_LED,
        LUN_REALLOCATION_1,
        RESERVED_FOR_FUTURE_EXPANSION_61,
        RESERVED_FOR_FUTURE_EXPANSION_62,
        RESERVED_FOR_FUTURE_EXPANSION_63,
        RESERVED_FOR_FUTURE_EXPANSION_64,
        RESERVED_FOR_FUTURE_EXPANSION_65,
        RESERVED_FOR_FUTURE_EXPANSION_66,
        RESERVED_FOR_FUTURE_EXPANSION_67,
        RESERVED_FOR_FUTURE_EXPANSION_68,
        RESERVED_FOR_FUTURE_EXPANSION_69,
        RESERVED_FOR_FUTURE_EXPANSION_70,
        RESERVED_FOR_FUTURE_EXPANSION_71,
        RESERVED_FOR_FUTURE_EXPANSION_72,
        RESERVED_FOR_FUTURE_EXPANSION_73,
        RESERVED_FOR_FUTURE_EXPANSION_74,
        LUN_2_ACTUATOR,
        LUN_2_FLASH_LED,
        LUN_REALLOCATION_2,
        RESERVED_FOR_FUTURE_EXPANSION_81,
        RESERVED_FOR_FUTURE_EXPANSION_82,
        RESERVED_FOR_FUTURE_EXPANSION_83,
        RESERVED_FOR_FUTURE_EXPANSION_84,
        RESERVED_FOR_FUTURE_EXPANSION_85,
        RESERVED_FOR_FUTURE_EXPANSION_86,
        RESERVED_FOR_FUTURE_EXPANSION_87,
        RESERVED_FOR_FUTURE_EXPANSION_88,
        RESERVED_FOR_FUTURE_EXPANSION_89,
        RESERVED_FOR_FUTURE_EXPANSION_90,
        RESERVED_FOR_FUTURE_EXPANSION_91,
        RESERVED_FOR_FUTURE_EXPANSION_92,
        RESERVED_FOR_FUTURE_EXPANSION_93,
        RESERVED_FOR_FUTURE_EXPANSION_94,
        LUN_3_ACTUATOR,
        LUN_3_FLASH_LED,
        LUN_REALLOCATION_3,
        RESERVED_FOR_FUTURE_EXPANSION_101,
        RESERVED_FOR_FUTURE_EXPANSION_102,
        RESERVED_FOR_FUTURE_EXPANSION_103,
        RESERVED_FOR_FUTURE_EXPANSION_104,
        RESERVED_FOR_FUTURE_EXPANSION_105,
        RESERVED_FOR_FUTURE_EXPANSION_106,
        RESERVED_FOR_FUTURE_EXPANSION_107,
        RESERVED_FOR_FUTURE_EXPANSION_108,
        RESERVED_FOR_FUTURE_EXPANSION_109,
        RESERVED_FOR_FUTURE_EXPANSION_110,
        RESERVED_FOR_FUTURE_EXPANSION_111,
        RESERVED_FOR_FUTURE_EXPANSION_112,
        RESERVED_FOR_FUTURE_EXPANSION_113,
        RESERVED_FOR_FUTURE_EXPANSION_114,
        RESERVED_FOR_FUTURE_EXPANSION,
    }eSASLogPageTypes;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct _sScsiFarmHeader
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
        sFarmHeader			farmHeader;									//!< structure of the farm header
    }sScsiFarmHeader;

    typedef struct _sScsiDriveInfo
    {
        sLogParams          pPageHeader;
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
        union {
            uint64_t            headLoadEvents;                         //!< Head Load Events
            uint64_t            reserved5;								//!< reserved
        };

        uint64_t            powerCycleCount;                            //!< Power Cycle Count
        uint64_t            resetCount;                                 //!< Hardware Reset Count
        uint64_t            reserved6;                                  //!< reserved
        uint64_t            NVC_StatusATPowerOn;                        //!< NVC Status on Power-on
        uint64_t            timeAvailable;                              //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
        uint64_t            firstTimeStamp;                             //!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
        uint64_t            lastTimeStamp;                              //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds
        uint64_t            dateOfAssembly;                             //!< Date of Assembly in ASCII “YYWW” where YY is the year and WW is the calendar week
        _sScsiDriveInfo() :pageNumber(0), copyNumber(0), serialNumber(0), serialNumber2(0), worldWideName(0), 
            worldWideName2(0), deviceInterface(0), deviceCapacity(0),  psecSize(0), lsecSize(0), deviceBufferSize(0),
            heads(0), factor(0), rotationRate(0), firmware(0), firmwareRev(0), reserved(0), reserved1(0), reserved2(0), 
            poh(0), reserved3(0), reserved4(0), headLoadEvents(0), powerCycleCount(0), resetCount(0), reserved6(0), 
            NVC_StatusATPowerOn(0), timeAvailable(0), firstTimeStamp(0), lastTimeStamp(0), dateOfAssembly(0) {};
    }sScsiDriveInfo;

    typedef struct _sScsiWorkLoadStat
    {
        sLogParams          PageHeader;								//!< pointer the farm header page parameter 
        sWorkLoadStat		workLoad;									//!< structure of the work load Stat

    }sScsiWorkLoadStat;

    typedef struct _sScsiErrorFrame
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
        uint64_t            pageNumber;									//!< Page Number = 3
        uint64_t            copyNumber;									//!< Copy Number
        uint64_t            totalReadECC;								//!< Number of Unrecoverable Read Errors
        uint64_t            totalWriteECC;								//!< Number of Unrecoverable Write Errors
        uint64_t            reserved;							        //!< reserved
        uint64_t            reserved1;									//!< Reserved
        uint64_t            totalMechanicalFails;						//!< Number of Mechanical Start Failures
        uint64_t            reserved2;					                //!< reserved
        uint64_t            reserved3;									//!< Reserved
        uint64_t            reserved4;									//!< Reserved
        uint64_t            reserved5;									//!< Reserved
        uint64_t            reserved6;									//!< Reserved
        uint64_t            reserved7;									//!< Reserved
        uint64_t            reserved8;							        //!< Reserved
        uint64_t            reserved9;									//!< Reserved
        uint64_t            reserved10;									//!< Reserved
        uint64_t            reserved11;									//!< Reserved
        uint64_t            reserved12;								    //!< reserved
        uint64_t            reserved13;									//!< Reserved
        uint64_t            reserved14;									//!< Reserved
        uint64_t            FRUCode;									//!< FRU code if smart trip from most recent SMART Frame (SAS only) 
        uint64_t            portAInvalidDwordCount;                     //!< Invalid DWord Count (Port A)
        uint64_t            portBInvalidDwordCount;                     //!< Invalid DWord Count (Port B)
        uint64_t            portADisparityErrorCount;                   //!< Disparity Error Count (Port A)
        uint64_t            portBDisparityErrorCount;                   //!< Disparity Error Count (Port B)
        uint64_t            portALossDwordSync;                         //!< Loss of DWord Sync (Port A)
        uint64_t            portBLossDwordSync;                         //!< Loss of DWord Sync (Port B)
        uint64_t            portAPhyResetProblem;                       //!< Phy Reset Problem (Port A)
        uint64_t            portBPhyResetProblem;                       //!< Phy Reset Problem (Port B()

        _sScsiErrorFrame() : pageNumber(0), copyNumber(0), totalReadECC(0), totalWriteECC(0), reserved(0), reserved1(0),
            totalMechanicalFails(0), reserved2(0), reserved3(0), reserved4(0), reserved5(0), reserved6(0),
            reserved7(0), reserved8(0), reserved9(0), reserved10(0), reserved11(0), reserved12(0),
             reserved13(0), reserved14(0), FRUCode(0), portAInvalidDwordCount(0), portBInvalidDwordCount(0),
            portADisparityErrorCount(0), portBDisparityErrorCount(0), portALossDwordSync(0),
            portBLossDwordSync(0), portAPhyResetProblem(0), portBPhyResetProblem(0) {};
    }sScsiErrorFrame;

    typedef struct _sScsiEnvironmentStat
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
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
        uint64_t            reserved10;								    //!< Reserved
        uint64_t            currentMotorPower;							//!< Current Motor Power, value from most recent SMART Summary Frame6
        uint64_t            average12v;                                 //!< 12V Power Average(mw) - Highest of the three summary frames
        uint64_t            min12v;                                     //!< 12V Power Min(mw) - Lowest of last 3 SMART summary frames
        uint64_t            max12v;                                     //!< 12V Power Max(mw) - Highest of last 3 SMART summary frames
        uint64_t            average5v;                                  //!< 5V Power Average (mw) - Highest of the last 3 SMART summary frames
        uint64_t            min5v;                                      //!< 5V Power Min(mw) - Lowest of last 3 SMART summary frames
        uint64_t            max5v;                                      //!< 5V Power Max(mw) - Highest of last 3 SMART summary frames

        _sScsiEnvironmentStat() : pageNumber(0), copyNumber(0), curentTemp(0), highestTemp(0), lowestTemp(0), reserved(0), 
            reserved1(0), reserved2(0), reserved3(0), reserved4(0), reserved5(0), reserved6(0), reserved7(0), maxTemp(0), minTemp(0),
            reserved8(0), reserved9(0), humidity(0), reserved10(0), currentMotorPower(0), average12v(0), min12v(0), max12v(0), 
            average5v(0), min5v(0), max5v(0) {};
    }sScsiEnvironmentStat;

    typedef struct _sScsiReliablility
    {
        sLogParams          pPageHeader;								 //!< pointer the farm header page parameter
        uint64_t            pageNumber;                                  //!< Page Number = 5
        uint64_t            copyNumber;                                  //!< Copy Number
        uint64_t            reserved;									 //!< Reserved
        uint64_t            reserved1;									 //!< Reserved
        uint64_t            reserved2;									 //!< Reserved 
        uint64_t            reserved3;									 //!< Reserved
        uint64_t            reserved4;									 //!< Reserved
        uint64_t            reserved5;									 //!< Reserved
        uint64_t            reserved6;									 //!< Reserved
        uint64_t            reserved7;									 //!< Reserved
        uint64_t            reserved8;									 //!< Reserved
        uint64_t            reserved9;									 //!< Reserved
        uint64_t            reserved10;									 //!< Reserved
        uint64_t            reserved11;									 //!< Reserved
        uint64_t            reserved12;									 //!< Reserved
        uint64_t            numberRAWops;                                //!< Number of RAW Operations
        uint64_t            cumECCDueToERC;							     //!< Cumulative Lifetime Unrecoverable Read errors due to Error Recovery Control (e.g. ERC timeout)
        uint64_t            reserved13;									 //!< Reserved
        uint64_t            reserved14;									 //!< Reserved
        uint64_t            reserved15;									 //!< Reserved
        uint64_t            reserved16;									 //!< Reserved
        uint64_t            reserved17;									 //!< Reserved
        uint64_t            reserved18;									 //!< Reserved
        uint64_t            reserved19;                                  //!< reserved
        uint64_t            reserved20;                                  //!< reserved
        uint64_t            heliumPressuretThreshold;                    //!< helium Pressure Threshold Trip
        uint64_t            reserved21;                                  //!< reserved
        uint64_t            reserved22;                                  //!< reserved
        uint64_t            reserved23;                                  //!< reserved

        _sScsiReliablility() : pageNumber(0), copyNumber(0), reserved(0), reserved1(0), reserved2(0), reserved3(0),
            reserved4(0), reserved5(0), reserved6(0), reserved7(0), reserved8(0), reserved9(0), reserved10(0), 
            reserved11(0), reserved12(0), numberRAWops(0), cumECCDueToERC(0),reserved13(0), reserved14(0), 
            reserved15(0), reserved16(0), reserved17(0), reserved18(0), reserved19(0), reserved20(0), 
            heliumPressuretThreshold(0), reserved21(0), reserved22(0), reserved23(0) {};

    }sScsiReliablility;

    typedef struct _sGeneralDriveInformationpage06
    {
        sLogParams          pPageHeader;								 //!< pointer the farm header page parameter
        uint64_t            pageNumber;                                  //!< Page Number = 6
        uint64_t            copyNumber;                                  //!< Copy Number
        uint64_t            Depop;                                       //!< Depopulation Head Mask
        uint64_t            productID[4];                                //!< Model Number [3 0]   Product ID
        uint64_t            driveType;                                   //!< Drive Recording Type
        uint64_t            isDepopped;                                  //!< Is drive currently depopped – 1 = depopped, 0 = not depopped
        uint64_t            maxNumAvaliableSectors;                      //!< Max Number of Available Sectors for Reassignment – Value in disc sectors	
        uint64_t            timeToReady;                                 //!< Time to Ready of the last power cycle in milliseconds 
        uint64_t            holdTime;                                    //!< Time the drive is held in staggered spin in milliseconds
        uint64_t            servoSpinUpTime;                             //!< The last servo spin up time in milliseconds

        _sGeneralDriveInformationpage06() : pageNumber(0), copyNumber(0), Depop(0),
#if defined __cplusplus && __cplusplus >= 201103L
            productID{ 0 },
#endif
            driveType(0), isDepopped(0), maxNumAvaliableSectors(0), timeToReady(0),
            holdTime(0), servoSpinUpTime(0) {};
    }sGeneralDriveInfoPage06;

    typedef struct _sScsiEnvironmentStatPage07
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
        uint64_t            pageNumber;									//!< Page Number = 7
        uint64_t            copyNumber;									//!< Copy Number
        uint64_t            average12v;                                 //!< 12V Power Average(mw) - Highest of the three summary frames
        uint64_t            min12v;                                     //!< 12V Power Min(mw) - Lowest of last 3 SMART summary frames
        uint64_t            max12v;                                     //!< 12V Power Max(mw) - Highest of last 3 SMART summary frames
        uint64_t            average5v;                                  //!< 5V Power Average (mw) - Highest of the last 3 SMART summary frames
        uint64_t            min5v;                                      //!< 5V Power Min(mw) - Lowest of last 3 SMART summary frames
        uint64_t            max5v;                                      //!< 5V Power Max(mw) - Highest of last 3 SMART summary frames

        _sScsiEnvironmentStatPage07() : pageNumber(0), copyNumber(0), average12v(0), min12v(0),
            max12v(0), average5v(0), min5v(0), max5v(0) {};
    }sScsiEnvStatPage07;

    typedef struct _sScsiWorkloadStatPage08
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
        uint64_t            pageNumber;									//!< Page Number = 8
        uint64_t            copyNumber;									//!< Copy Number
        uint64_t            countQueDepth1;                             //!< Count of Queue Depth =1 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth2;                             //!< Count of Queue Depth =2 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth3_4;                           //!< Count of Queue Depth 3-4 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth5_8;                           //!< Count of Queue Depth 5-8 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth9_16;                          //!< Count of Queue Depth 9-16 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth17_32;                         //!< Count of Queue Depth 17-32 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth33_64;                         //!< Count of Queue Depth 33-64 at 30s intervals for last 3 SMART Summary Frames
        uint64_t            countQueDepth_gt_64;                        //!< Count of Queue Depth greater than 64 at 30s intervals for last 3 SMART Summary Frames

        _sScsiWorkloadStatPage08() : pageNumber(0), copyNumber(0), countQueDepth1(0), countQueDepth2(0), 
            countQueDepth3_4(0), countQueDepth5_8(0), countQueDepth9_16(0), countQueDepth17_32(0), 
            countQueDepth33_64(0), countQueDepth_gt_64(0) {};
    }sScsiWorkloadStatPage08;

    typedef struct _sHeadInformation
    {
        sLogParams          pageHeader;                                  //!<  header page parameters
        uint64_t            headValue[MAX_HEAD_COUNT];                   //!< head information
        
#if defined __cplusplus && __cplusplus >= 201103L
        _sHeadInformation() : headValue{ 0 } {};
#endif
           
    }sHeadInformation;

    typedef struct _sLUNStruct
    {
        sLogParams          pageHeader;                                 //!<  header page parameters
        uint64_t            pageNumber;                                 //!< Page Number = 5
        uint64_t            copyNumber;                                 //!< Copy Number
        uint64_t            LUNID;                                      //!< LUN ID
        uint64_t            headLoadEvents;                             //!< Head Load Events
        uint64_t            reserved;                                   //!< Reserved
        uint64_t            reserved1;                                  //!< Reserved
        uint64_t            reserved2;                                  //!< Reserved
        uint64_t            reserved3;                                  //!< Reserved
        uint64_t            reserved4;                                  //!< Reserved
        uint64_t            reserved5;                                  //!< Reserved
        uint64_t            reserved6;                                  //!< Reserved
        uint64_t            reserved7;                                  //!< Reserved
        uint64_t            reserved8;                                  //!< Reserved
        uint64_t            reserved9;                                  //!< Reserved
        uint64_t            reserved10;                                 //!< Reserved
        uint64_t            reserved11;                                 //!< Reserved
        uint64_t            dosScansPerformed;                          //!< Number of DOS Scans Performed
        uint64_t            correctedLBAbyISP;                          //!< Number of LBAs Corrected by ISP
        uint64_t            reserved12;                                 //!< Reserved
        uint64_t            reserved13;                                 //!< Reserved
        uint64_t            reserved14;                                 //!< Reserved
        uint64_t            reserved15;                                 //!< Reserved
        uint64_t            lbasCorrectedByParity;                      //!< total valid parity sectors
        uint64_t            reserved16;                                 //!< Reserved
        uint64_t            reserved17;                                 //!< Reserved
        uint64_t            reserved18;                                 //!< Reserved
        uint64_t            reserved19;                                 //!< Reserved
        uint64_t            reserved20;                                 //!< WReserved
        uint64_t            reserved21;                                 //!< Reserved
        uint64_t            primarySPCovPercentage;                     //!< Primary Super Parity Coverage Percentage
        uint64_t            primarySPCovPercentageSMR;                  //!< Primary Super Parity Coverage Percentage SMR

        _sLUNStruct() : pageNumber(0), copyNumber(0), LUNID(0), headLoadEvents(0),reserved(0), reserved1(0), reserved2(0), 
            reserved3(0),  reserved4(0), reserved5(0), reserved6(0), reserved7(0), reserved8(0), reserved9(0), reserved10(0),
            reserved11(0), dosScansPerformed(0), correctedLBAbyISP(0), reserved12(0), reserved13(0), reserved14(0), reserved15(0),
            lbasCorrectedByParity(0), reserved16(0), reserved17(0), reserved18(0), reserved19(0), 
            reserved20(0), reserved21(0), primarySPCovPercentage(0), primarySPCovPercentageSMR(0) {};
    }sLUNStruct;

    typedef struct _sActuatorFLEDInfo
    {
        sLogParams          pageHeader;                                 //!<  header page parameters
        uint64_t            pageNumber;                                 //!< Page Number = 0x0051,0x0061,0x0071,0x0081,
        uint64_t            copyNumber;                                 //!< Copy Number
        uint64_t            actID;                                      //!< Actuator ID
        uint64_t            totalFLEDEvents;                            //!< Total Flash LED (Assert) Events
        uint64_t            index;                                      //!< index of the FLED array
        uint64_t            flashLEDArray[8];                           //!< Info on the last 8 Flash LED events Wrapping array. 
        uint64_t            timestampForLED[8];                         //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array 
        uint64_t            powerCycleOfLED[8];                        //<! Power Cycle of the last 8 Flash LED (assert) Events, wrapping array 

        _sActuatorFLEDInfo() : pageNumber(0), copyNumber(0), actID(0), totalFLEDEvents(0), index(0)
#if defined __cplusplus && __cplusplus >= 201103L
            ,flashLEDArray{ 0 }, timestampForLED{ 0 }, powerCycleOfLED{ 0 }
#endif 
        {};
    }sActuatorFLEDInfo;

    typedef struct _sActuatorReallocationData
    {
        sLogParams          pageHeader;                                 //!<  header page parameters
        uint64_t            pageNumber;                                 //!< Page Number = 0x0052,0x0062,0x0072,0x0082,
        uint64_t            copyNumber;                                 //!< Copy Number
        uint64_t            actID;                                      //!< Actuator ID
        uint64_t            numberReallocatedSectors;                   //!< Number of Reallocated Sectors
        uint64_t            numberReallocatedCandidates;                //!< Number of Reallocated Candidate Sectors
        uint64_t            reallocatedCauses[15];                      //!< Reallocated sectors by cause, see below.  This is a 15 element array, each element is 8 bytes

        _sActuatorReallocationData() : pageNumber(0), copyNumber(0), actID(0), numberReallocatedSectors(0), numberReallocatedCandidates(0)
#if defined __cplusplus && __cplusplus >= 201103L
            , reallocatedCauses{ 0 }
#endif 
        {};
    }sActReallocationData;
#pragma pack(pop)
    typedef struct _sScsiFarmFrame
    {
        sScsiFarmHeader         farmHeader;                             //!< Farm header 
        sScsiDriveInfo          driveInfo;                              //!< drive information structure 
        sStringIdentifyData     identStringInfo;                        //!< string information 
        sScsiWorkLoadStat       workLoadPage;                           //!< work load page
        sScsiErrorFrame         errorPage;                              //!< error page information
        sScsiEnvironmentStat    environmentPage;                        //!< environment page
        sScsiReliablility       reliPage;                               //!< reliability data
        sGeneralDriveInfoPage06 gDPage06;                               //!< Gerneral Drive Information Page 06
        sScsiEnvStatPage07      envStatPage07;                          //!< Environment Stat Page 07
        sScsiWorkloadStatPage08 workloadStatPage08;                     //!< Workload Stat Page 08
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
        sHeadInformation        cumECCReadRepeat;
        sHeadInformation        cumECCReadUnique;
        sHeadInformation        totalLaserFieldAdjustIterations;
        sHeadInformation        totalReaderWriteerOffsetIterationsPerformed;
        sHeadInformation        pre_lfaZone_0;
        sHeadInformation        pre_lfaZone_1;
        sHeadInformation        pre_lfaZone_2;
        sHeadInformation        zeroPercentShift;
        sHeadInformation        currentH2STTrimmedbyHeadZone0;
        sHeadInformation        currentH2STTrimmedbyHeadZone1;
        sHeadInformation        currentH2STTrimmedbyHeadZone2;
        sHeadInformation        currentH2STIterationsByHeadZone0;
        sHeadInformation        currentH2STIterationsByHeadZone1;
        sHeadInformation        currentH2STIterationsByHeadZone2;
        sHeadInformation        laser_operatingZone_0;
        sHeadInformation        laser_operatingZone_1;
        sHeadInformation        laserOperatingZone_2;
        sHeadInformation        postLFAOptimalBERZone_0;
        sHeadInformation        postLFAOptimalBERZone_1;
        sHeadInformation        postLFAOptimalBERZone_2;
        sHeadInformation        microJogOffsetZone_0;
        sHeadInformation        microJogOffsetZone_1;
        sHeadInformation        microJogOffsetZone_2;
        sHeadInformation        zeroPercentShiftZone_1;
        sHeadInformation        appliedFlyHeightByHeadOuter;
        sHeadInformation        appliedFlyHeightByHeadInner;
        sHeadInformation        appliedFlyHeightByHeadMiddle;
        sHeadInformation        secondMRHeadResistanceByHead;
        sHeadInformation        fafhMeasurementStatus;                  //!< FAFH Measurement Status, bitwise OR across all diameters per head
        sHeadInformation        fafhRelativeApmlitude;                  //!< FAFH HF / LF Relative Amplitude in tenths, maximum value across all 3 zones per head
        sHeadInformation        fafh_bit_error_rate_0;                  //!< FAFH Bit Error Rate, write then read BER on reserved tracks Diameter 0: Outer
        sHeadInformation        fafh_bit_error_rate_1;                  //!< FAFH Bit Error Rate, write then read BER on reserved tracks Diameter 1 : Outer
        sHeadInformation        fafh_bit_error_rate_2;                  //!< FAFH Bit Error Rate, write then read BER on reserved tracks Diameter 2 : Outer
        sHeadInformation        fafhLowFrequency_0;                     //!< FAFH Low Frequency Passive Clearance in ADC counts Diameter 0 : outer
        sHeadInformation        fafhLowFrequency_1;                     //!< FAFH Low Frequency Passive Clearance in ADC counts Diameter 1 : outer
        sHeadInformation        fafhLowFrequency_2;                     //!< FAFH Low Frequency Passive Clearance in ADC counts Diameter 2 : outer
        sHeadInformation        fafhHighFrequency_0;                    //!< FAFH High Frequency Passive Clearance in ADC counts Diameter 0 : outer
        sHeadInformation        fafhHighFrequency_1;                    //!< FAFH High Frequency Passive Clearance in ADC counts Diameter 1 : outer
        sHeadInformation        fafhHighFrequency_2;                    //!< FAFH High Frequency Passive Clearance in ADC counts Diameter 2 : outer
        sHeadInformation        zeroPercentShiftZone_2;
        sLUNStruct              vLUN50;
        sActuatorFLEDInfo       fled51;
        sActReallocationData    reall52;
        sLUNStruct              vLUN60;
        sActuatorFLEDInfo       fled61;
        sActReallocationData    reall62;
        sLUNStruct              vLUN70;
        sActuatorFLEDInfo       fled71;
        sActReallocationData    reall72;
        sLUNStruct              vLUN80;
        sActuatorFLEDInfo       fled81;
        sActReallocationData    reall82;

        std::vector<eSASLogPageTypes>   vFramesFound;

        _sScsiFarmFrame() : vFramesFound(FARM_HEADER_PARAMETER) {};

    }sScsiFarmFrame;


}
