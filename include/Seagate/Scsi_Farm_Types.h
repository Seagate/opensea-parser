//
// Scsi_Farm_Types.h   Structures of SCSI FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
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

    enum class eSASLogPageTypes
    {
        FARM_HEADER_PARAMETER = 0x0000,
        GENERAL_DRIVE_INFORMATION_PARAMETER = 0x0001,
        WORKLOAD_STATISTICS_PARAMETER = 0x0002,
        ERROR_STATISTICS_PARAMETER = 0x0003,
        ENVIRONMENTAL_STATISTICS_PARAMETER = 0x0004,
        RELIABILITY_STATISTICS_PARAMETER = 0x0005,
        GENERAL_DRIVE_INFORMATION_06 = 0x0006,
        ENVIRONMENT_STATISTICS_PARAMETER_07 = 0x0007,
        WORKLOAD_STATISTICS_PARAMETER_08 = 0x0008,
        GPES_FIELD_PARAMETER_09 = 0x0009,
        RESERVED_FOR_FUTURE_STATISTICS_5 = 0x000A,
        RESERVED_FOR_FUTURE_STATISTICS_6 = 0x000B,
        RESERVED_FOR_FUTURE_STATISTICS_7 = 0x000C,
        RESERVED_FOR_FUTURE_STATISTICS_8 = 0x000D,
        RESERVED_FOR_FUTURE_STATISTICS_9 = 0x000E,
        RESERVED_FOR_FUTURE_STATISTICS_10 = 0x000F,
        RESERVED_FOR_FUTURE_HEAD_1 = 0x0010,
        RESERVED_FOR_FUTURE_HEAD_2 = 0x0011,
        RESERVED_FOR_FUTURE_HEAD_3 = 0x0012,
        RESERVED_FOR_FUTURE_HEAD_4 = 0x0013,
        RESERVED_FOR_FUTURE_HEAD_5 = 0x0014,
        RESERVED_FOR_FUTURE_HEAD_6 = 0x0015,
        RESERVED_FOR_FUTURE_HEAD_7 = 0x0016,
        RESERVED_FOR_FUTURE_HEAD_8 = 0x0017,
        RESERVED_FOR_FUTURE_HEAD_9 = 0x0018,
        RESERVED_FOR_FUTURE_HEAD_10 = 0x0019,
        MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD = 0x001A,        //0x001A
        RESERVED_FOR_FUTURE_HEAD_11 = 0x001B,
        RESERVED_FOR_FUTURE_HEAD_12 = 0x001C,
        RESERVED_FOR_FUTURE_HEAD_13 = 0x001D,
        CURRENT_H2SAT_PERCENTAGE_OF_CODEWORDS_ITERATION_LEVEL_AVERAGED_ACROSS_TEST_ZONES = 0x001E,
        CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES = 0x001F, //0x001F
        CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES = 0x0020, //0x0020
        NUMBER_OF_RESIDENT_GLIST_ENTRIES = 0x0021,
        NUMBER_OF_PENDING_ENTRIES = 0x0022,
        RESERVED_FOR_FUTURE_HEAD_15 = 0x0023,
        RESERVED_FOR_FUTURE_HEAD_16 = 0x0024,
        RESERVED_FOR_FUTURE_HEAD_17 = 0x0025,
        WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART = 0x0026, 
        RESERVED_FOR_FUTURE_HEAD_18 = 0x0027,
        CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD = 0x0028,
        CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD = 0x0029,
        RESERVED_FOR_FUTURE_HEAD_19 = 0x002A,
        RESERVED_FOR_FUTURE_HEAD_20 = 0x002B,
        RESERVED_FOR_FUTURE_HEAD_21 = 0x002C,
        RESERVED_FOR_FUTURE_HEAD_22 = 0x002D,
        RESERVED_FOR_FUTURE_HEAD_23 = 0x002E,
        RESERVED_FOR_FUTURE_HEAD_24 = 0x002F,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0 = 0x0030,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1 = 0x0031,
        CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2 = 0x0032,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0 = 0x0033,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1 = 0x0034,
        CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2 = 0x0035,
        RESERVED_FOR_FUTURE_HEAD_25 = 0x0036,
        RESERVED_FOR_FUTURE_HEAD_26 = 0x0037,
        RESERVED_FOR_FUTURE_HEAD_27 = 0x0038,
        RESERVED_FOR_FUTURE_HEAD_28 = 0x0039,
        RESERVED_FOR_FUTURE_HEAD_29 = 0x003A,
        RESERVED_FOR_FUTURE_HEAD_30 = 0x003B,
        RESERVED_FOR_FUTURE_HEAD_31 = 0x003C,
        RESERVED_FOR_FUTURE_HEAD_32 = 0x003D,
        RESERVED_FOR_FUTURE_HEAD_33 = 0x003E,
        RESERVED_FOR_FUTURE_HEAD_34 = 0x003F,
        RESERVED_FOR_FUTURE_HEAD_35 = 0x0040,
        RESERVED_FOR_FUTURE_HEAD_36 = 0x0041,
        RESERVED_FOR_FUTURE_HEAD_37 = 0x0042,
        SECOND_MR_HEAD_RESISTANCE = 0x0043,
        RESERVED_FOR_FUTURE_HEAD_38 = 0x0044,
        RESERVED_FOR_FUTURE_HEAD_39 = 0x0045,
        RESERVED_FOR_FUTURE_HEAD_40 = 0x0046,
        RESERVED_FOR_FUTURE_HEAD_41 = 0x0047,
        RESERVED_FOR_FUTURE_HEAD_42 = 0x0048,
        RESERVED_FOR_FUTURE_HEAD_43 = 0x0049,
        RESERVED_FOR_FUTURE_HEAD_44 = 0x004A,
        RESERVED_FOR_FUTURE_HEAD_45 = 0x004B,
        RESERVED_FOR_FUTURE_HEAD_46 = 0x004C,
        RESERVED_FOR_FUTURE_HEAD_47 = 0x004D,
        RESERVED_FOR_FUTURE_HEAD_48 = 0x004E,
        RESERVED_FOR_FUTURE_HEAD_49 = 0x004F,
        LUN_0_ACTUATOR = 0x0050,
        LUN_0_FLASH_LED = 0x0051,
        LUN_REALLOCATION_0 = 0x0052,
        RESERVED_FOR_FUTURE_EXPANSION_42 = 0x0053,
        RESERVED_FOR_FUTURE_EXPANSION_43 = 0x0054,
        RESERVED_FOR_FUTURE_EXPANSION_44 = 0x0055,
        RESERVED_FOR_FUTURE_EXPANSION_45 = 0x0056,
        RESERVED_FOR_FUTURE_EXPANSION_46 = 0x0057,
        RESERVED_FOR_FUTURE_EXPANSION_47 = 0x0058,
        RESERVED_FOR_FUTURE_EXPANSION_48 = 0x0059,
        RESERVED_FOR_FUTURE_EXPANSION_49 = 0x005A,
        RESERVED_FOR_FUTURE_EXPANSION_50 = 0x005B,
        RESERVED_FOR_FUTURE_EXPANSION_51 = 0x005C,
        RESERVED_FOR_FUTURE_EXPANSION_52 = 0x005D,
        RESERVED_FOR_FUTURE_EXPANSION_53 = 0x005E,
        RESERVED_FOR_FUTURE_EXPANSION_54 = 0x005F,
        LUN_1_ACTUATOR = 0x0060,
        LUN_1_FLASH_LED = 0x0061,
        LUN_REALLOCATION_1 = 0x0062,
        RESERVED_FOR_FUTURE_EXPANSION_61 = 0x0063,
        RESERVED_FOR_FUTURE_EXPANSION_62 = 0x0064,
        RESERVED_FOR_FUTURE_EXPANSION_63 = 0x0065,
        RESERVED_FOR_FUTURE_EXPANSION_64 = 0x0066,
        RESERVED_FOR_FUTURE_EXPANSION_65 = 0x0067,
        RESERVED_FOR_FUTURE_EXPANSION_66 = 0x0068,
        RESERVED_FOR_FUTURE_EXPANSION_67 = 0x0069,
        RESERVED_FOR_FUTURE_EXPANSION_68 = 0x006A,
        RESERVED_FOR_FUTURE_EXPANSION_69 = 0x006B,
        RESERVED_FOR_FUTURE_EXPANSION_70 = 0x006C,
        RESERVED_FOR_FUTURE_EXPANSION_71 = 0x006D,
        RESERVED_FOR_FUTURE_EXPANSION_72 = 0x006E,
        RESERVED_FOR_FUTURE_EXPANSION_73 = 0x006F,
        RESERVED_FOR_FUTURE_EXPANSION_74 = 0x0070,
        LUN_2_ACTUATOR = 0x0071,
        LUN_2_FLASH_LED = 0x0072,
        LUN_REALLOCATION_2 = 0x0073,
        RESERVED_FOR_FUTURE_EXPANSION_81 = 0x0074,
        RESERVED_FOR_FUTURE_EXPANSION_82 = 0x0075,
        RESERVED_FOR_FUTURE_EXPANSION_83 = 0x0076,
        RESERVED_FOR_FUTURE_EXPANSION_84 = 0x0077,
        RESERVED_FOR_FUTURE_EXPANSION_85 = 0x0078,
        RESERVED_FOR_FUTURE_EXPANSION_86 = 0x0079,
        RESERVED_FOR_FUTURE_EXPANSION_87 = 0x007A,
        RESERVED_FOR_FUTURE_EXPANSION_88 = 0x007B,
        RESERVED_FOR_FUTURE_EXPANSION_89 = 0x007C,
        RESERVED_FOR_FUTURE_EXPANSION_90 = 0x007D,
        RESERVED_FOR_FUTURE_EXPANSION_91 = 0x007E,
        RESERVED_FOR_FUTURE_EXPANSION_92 = 0x007F,
        RESERVED_FOR_FUTURE_EXPANSION_93 = 0x0080,
        RESERVED_FOR_FUTURE_EXPANSION_94 = 0x0081,
        LUN_3_ACTUATOR = 0x0082,
        LUN_3_FLASH_LED = 0x0083,
        LUN_REALLOCATION_3 = 0x0084,
        RESERVED_FOR_FUTURE_EXPANSION_101 = 0x0085,
        RESERVED_FOR_FUTURE_EXPANSION_102 = 0x0086,
        RESERVED_FOR_FUTURE_EXPANSION_103 = 0x0087,
        RESERVED_FOR_FUTURE_EXPANSION_104 = 0x0088,
        RESERVED_FOR_FUTURE_EXPANSION_105 = 0x0089,
        RESERVED_FOR_FUTURE_EXPANSION_106 = 0x008A,
        RESERVED_FOR_FUTURE_EXPANSION_107 = 0x008B,
        RESERVED_FOR_FUTURE_EXPANSION_108 = 0x008C,
        RESERVED_FOR_FUTURE_EXPANSION_109 = 0x008D,
        RESERVED_FOR_FUTURE_EXPANSION_110 = 0x008E,
        RESERVED_FOR_FUTURE_EXPANSION_111 = 0x008F,
        RESERVED_FOR_FUTURE_EXPANSION_112 = 0x0090,
        RESERVED_FOR_FUTURE_EXPANSION_113 = 0x0091,
        RESERVED_FOR_FUTURE_EXPANSION_114 = 0x0092,
        RESERVED_FOR_FUTURE_EXPANSION_115 = 0x0093,
        RESERVED_FOR_FUTURE_EXPANSION_116 = 0x0094,
        RESERVED_FOR_FUTURE_EXPANSION_117 = 0x0095,
        RESERVED_FOR_FUTURE_EXPANSION_118 = 0x0096,  //150
        RESERVED_FOR_FUTURE_EXPANSION_119 = 0x0097,
        LIFETIME_TERABYTES_WRITTEN = 0x0100,
        RESERVED_FOR_FUTURE_EXPANSION_120 = 0x0101,
        RESERVED_FOR_FUTURE_EXPANSION_121 = 0x0102,
        RESERVED_FOR_FUTURE_EXPANSION_122 = 0x0103,
        RESERVED_FOR_FUTURE_EXPANSION_123 = 0x0104,
        RESERVED_FOR_FUTURE_EXPANSION_124 = 0x0105,
        RESERVED_FOR_FUTURE_EXPANSION_125 = 0x0106,
        UNIQUE_UNRECOVERABLES_SINCE = 0x0107,
        UNIQUE_UNRECOVERALBES_BETWEEN = 0x0108,
        GET_PHYSICAL_ELLEMENT_STATUS = 0x0109,
        RESERVED_FOR_FUTURE_EXPANSION_129 = 0x010A,
        RESERVED_FOR_FUTURE_EXPANSION_130 = 0x010B,
        RESERVED_FOR_FUTURE_EXPANSION_131 = 0x010C,
        RESERVED_FOR_FUTURE_EXPANSION_132 = 0x010D,
        RESERVED_FOR_FUTURE_EXPANSION_133 = 0x010E,
        RESERVED_FOR_FUTURE_EXPANSION_134 = 0x010F,
        RESERVED_FOR_FUTURE_EXPANSION_135 = 0x0110,
        RESERVED_FOR_FUTURE_EXPANSION_136 = 0x0111,
        RESERVED_FOR_FUTURE_EXPANSION_137 = 0x0112,
        RESERVED_FOR_FUTURE_EXPANSION_138 = 0x0113,
        TOTAL_VISIBLE_READ_ERRORS_SEEN = 0x0114,
        LIFETIME_SECTORS_READ = 0x0115,
        MAX_RESERVED_FOR_FUTURE_EXPANSION = 0x0116,
        SENSE_CODE_CLASS_1_0 = 0x0500,
        SENSE_CODE_CLASS_1_1 = 0x0501,
        SENSE_CODE_CLASS_1_2 = 0x0502,
        SENSE_CODE_CLASS_1_3 = 0x0503,
        SENSE_CODE_CLASS_1_4 = 0x0504,
        SENSE_CODE_CLASS_1_5 = 0x0505,
        SENSE_CODE_CLASS_1_6 = 0x0506,
        SENSE_CODE_CLASS_1_7 = 0x0507,
        SENSE_CODE_CLASS_1_8 = 0x0508,
        SENSE_CODE_CLASS_1_9 = 0x0509,
        SENSE_CODE_CLASS_1_A = 0x050A,
        SENSE_CODE_CLASS_1_B = 0x050B,
        SENSE_CODE_CLASS_1_C = 0x050C,
        SENSE_CODE_CLASS_1_D = 0x050D,
        SENSE_CODE_CLASS_1_E = 0x050E,
        SENSE_CODE_CLASS_1_F = 0x050F,
        SENSE_CODE_CLASS_2_0 = 0x0580,
        SENSE_CODE_CLASS_2_1 = 0x0581,
        SENSE_CODE_CLASS_2_2 = 0x0582,
        SENSE_CODE_CLASS_2_3 = 0x0583,
        SENSE_CODE_CLASS_2_4 = 0x0584,
        SENSE_CODE_CLASS_2_5 = 0x0585,
        SENSE_CODE_CLASS_2_6 = 0x0586,
        SENSE_CODE_CLASS_2_7 = 0x0587,
        SENSE_CODE_CLASS_2_8 = 0x0588,
        SENSE_CODE_CLASS_2_9 = 0x0589,
        SENSE_CODE_CLASS_2_A = 0x058A,
        SENSE_CODE_CLASS_2_B = 0x058B,
        SENSE_CODE_CLASS_2_C = 0x058C,
        SENSE_CODE_CLASS_2_D = 0x058D,
        SENSE_CODE_CLASS_2_E = 0x058E,
        SENSE_CODE_CLASS_2_F = 0x058F,
        SENSE_CODE_CLASS_3_0 = 0x0600,
        SENSE_CODE_CLASS_3_1 = 0x0601,
        SENSE_CODE_CLASS_3_2 = 0x0602,
        SENSE_CODE_CLASS_3_3 = 0x0603,
        SENSE_CODE_CLASS_4_0 = 0x0680,
        SENSE_CODE_CLASS_4_1 = 0x0681,
        SENSE_CODE_CLASS_4_2 = 0x0682,
        SENSE_CODE_CLASS_4_3 = 0x0683,
        SENSE_CODE_CLASS_5_0 = 0x0690,
        SENSE_CODE_CLASS_5_1 = 0x0691,
        SENSE_CODE_CLASS_5_2 = 0x0692,
        SENSE_CODE_CLASS_5_3 = 0x0693,

    };

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
        uint64_t            reserved5;								    //!< reserved
        uint64_t            powerCycleCount;                            //!< Power Cycle Count
        uint64_t            resetCount;                                 //!< Hardware Reset Count
        uint64_t            reserved6;                                  //!< reserved
        uint64_t            NVC_StatusATPowerOn;                        //!< NVC Status on Power-on
        uint64_t            timeAvailable;                              //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
        uint64_t            firstTimeStamp;                             //!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
        uint64_t            lastTimeStamp;                              //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds
        uint64_t            dateOfAssembly;                             //!< Date of Assembly in ASCII "YYWW" where YY is the year and WW is the week
        _sScsiDriveInfo() :pageNumber(0), copyNumber(0), serialNumber(0), serialNumber2(0), worldWideName(0), 
            worldWideName2(0), deviceInterface(0), deviceCapacity(0),  psecSize(0), lsecSize(0), deviceBufferSize(0),
            heads(0), factor(0), rotationRate(0), firmware(0), firmwareRev(0), reserved(0), reserved1(0), reserved2(0), 
            poh(0), reserved3(0), reserved4(0), reserved5(0), powerCycleCount(0), resetCount(0), reserved6(0),
            NVC_StatusATPowerOn(0), timeAvailable(0), firstTimeStamp(0), lastTimeStamp(0), dateOfAssembly(0) {};
    }sScsiDriveInfo;


    typedef struct _sSASWorkLoadStat
    {
        uint64_t        pageNumber;                                 //!< Page Number = 2
        uint64_t        copyNumber;                                 //!< Copy Number
        uint64_t        workloadPercentage;                         //!< rated Workload Percentage
        uint64_t        totalReadCommands;                          //!< Total Number of Read Commands
        uint64_t        totalWriteCommands;                         //!< Total Number of Write Commands
        uint64_t        totalRandomReads;                           //!< Total Number of Random Read Commands
        uint64_t        totalRandomWrites;                          //!< Total Number of Random Write Commands
        uint64_t        totalNumberofOtherCMDS;                     //!< Total Number Of Other Commands
        uint64_t        logicalSecWritten;                          //!< Logical Sectors Written
        uint64_t        logicalSecRead;                             //!< Logical Sectors Read
        
        uint64_t        totalReadCmdsFromFrames1;               //!< SCSI - Number of Read commands from 0-3.125% of LBA space for restricted time range.
        uint64_t        totalReadCmdsFromFrames2;               //!< SCSI - Number of Read commands from 3.125-25% of LBA space for restricted time range.
        uint64_t        totalReadCmdsFromFrames3;               //!< SCSI - Number of Read commands from 25-75% of LBA space for restricted time range.
        uint64_t        totalReadCmdsFromFrames4;               //!< SCSI - Number of Read commands from 75-100% of LBA space for restricted time range.
        uint64_t        totalWriteCmdsFromFrames1;              //!< SCSI - Number of Write commands from 0-3.125% of LBA space for restricted time range.
        uint64_t        totalWriteCmdsFromFrames2;              //!< SCSI - Number of Write commands from 3.125-25% of LBA space for restricted time range.
        uint64_t        totalWriteCmdsFromFrames3;              //!< SCSI - Number of Write commands from 25-50% of LBA space for restricted time range.
        uint64_t        totalWriteCmdsFromFrames4;              //!< SCSI - Number of Write commands from 50-100% of LBA space for restricted time range.

        uint64_t        numberofUnalignedWriteCMDS;        //!< SCSI - Total Number of Unaligned Write Commands (added 4.4)

        _sSASWorkLoadStat() : numberofUnalignedWriteCMDS(0), pageNumber(0), copyNumber(0), workloadPercentage(0), totalReadCommands(0), totalWriteCommands(0),
            totalRandomReads(0), totalRandomWrites(0), totalNumberofOtherCMDS(0), logicalSecWritten(0), logicalSecRead(0), 
            totalReadCmdsFromFrames1(0), totalReadCmdsFromFrames2(0),totalReadCmdsFromFrames3(0), totalReadCmdsFromFrames4(0), 
            totalWriteCmdsFromFrames1(0),totalWriteCmdsFromFrames2(0), totalWriteCmdsFromFrames3(0), totalWriteCmdsFromFrames4(0)
            {};

    }sSASWorkLoadStat;

    typedef struct _sScsiWorkLoadStat
    {
        sLogParams          PageHeader;								    //!< pointer the farm header page parameter 
        sSASWorkLoadStat		workLoad;								//!< structure of the work load Stat

    }sScsiWorkLoadStat;


    typedef struct _sScsiErrorFrame
    {
        sLogParams          pPageHeader;								//!< pointer the farm header page parameter
        uint64_t            pageNumber;									//!< Page Number = 3
        uint64_t            copyNumber;									//!< Copy Number
        uint64_t            totalReadECC;								//!< Number of Unrecoverable Read Errors
        uint64_t            totalWriteECC;								//!< Number of Unrecoverable Write Errors
        uint64_t            readRecoveryAtt;							//!< read Recovery Attempts
        uint64_t            reserved1;									//!< Reserved
        uint64_t            totalMechanicalFails;						//!< Number of Mechanical Start Failures
        uint64_t            reserved2;					                //!< reserved
        uint64_t            reserved3;									//!< Reserved
        uint64_t            reserved4;									//!< Reserved
        uint64_t            reserved5;									//!< Reserved
        uint64_t            reserved6;									//!< Reserved
        uint64_t            reserved7;									//!< Reserved
        uint64_t            attrIOEDCErrors;							//!< Number of IOEDC Errors 
        uint64_t            reserved8;							        //!< Reserved
        uint64_t            reserved9;									//!< Reserved
        uint64_t            reserved10;									//!< Reserved
        uint64_t            smartTripParam1;						    //!< SMART Trip Parameter 1   
        uint64_t            smartTripParam2;						    //!< SMART Trip Parameter 2  
        uint64_t            reserved11;									//!< Reserved
        uint64_t            FRUCode;									//!< FRU code if smart trip from most recent SMART Frame (SAS only) 
        uint64_t            portAInvalidDwordCount;                     //!< Invalid DWord Count (Port A)
        uint64_t            portBInvalidDwordCount;                     //!< Invalid DWord Count (Port B)
        uint64_t            portADisparityErrorCount;                   //!< Disparity Error Count (Port A)
        uint64_t            portBDisparityErrorCount;                   //!< Disparity Error Count (Port B)
        uint64_t            portALossDwordSync;                         //!< Loss of DWord Sync (Port A)
        uint64_t            portBLossDwordSync;                         //!< Loss of DWord Sync (Port B)
        uint64_t            portAPhyResetProblem;                       //!< Phy Reset Problem (Port A)
        uint64_t            portBPhyResetProblem;                       //!< Phy Reset Problem (Port B()

        _sScsiErrorFrame() : pageNumber(0), copyNumber(0), totalReadECC(0), totalWriteECC(0), readRecoveryAtt(0), reserved1(0),
            totalMechanicalFails(0), reserved2(0), reserved3(0), reserved4(0), reserved5(0), reserved6(0), attrIOEDCErrors(0),
            reserved7(0), reserved8(0), reserved9(0), reserved10(0), smartTripParam1(0),smartTripParam2(0), 
            reserved11(0), FRUCode(0), portAInvalidDwordCount(0), portBInvalidDwordCount(0),
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
        uint64_t            unloadEvents;							        //!< High Priority Unload Events (Emergency Retracts)  
        uint64_t            reserved19;                                  //!< reserved
        uint64_t            diskSlipRecalPerformed;                      //!< Number of disc slip recalibrations performed
        uint64_t            heliumPressuretThreshold;                    //!< helium Pressure Threshold Trip
        uint64_t            reserved21;                                  //!< reserved
        uint64_t            reserved22;                                  //!< reserved
        uint64_t            reserved23;                                  //!< reserved

        _sScsiReliablility() : pageNumber(0), copyNumber(0), reserved(0), reserved1(0), reserved2(0), reserved3(0),
            reserved4(0), reserved5(0), reserved6(0), reserved7(0), reserved8(0), reserved9(0), reserved10(0), 
            reserved11(0), reserved12(0), numberRAWops(0), cumECCDueToERC(0),reserved13(0), reserved14(0), 
            reserved15(0), reserved16(0), reserved17(0), unloadEvents(0), reserved19(0), diskSlipRecalPerformed(0),
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
        uint64_t            isDepopped;                                  //!< Is drive currently depopped - 1 = depopped, 0 = not depopped
        uint64_t            maxNumAvaliableSectors;                      //!< Max Number of Available Sectors for Reassignment - Value in disc sectors	
        uint64_t            timeToReady;                                 //!< Time to Ready of the last power cycle in milliseconds 
        uint64_t            holdTime;                                    //!< Time the drive is held in staggered spin in milliseconds
        uint64_t            servoSpinUpTime;                             //!< The last servo spin up time in milliseconds
        uint64_t            writeProtect;                                //!< Data Protect Status: 1 = Data Protect, 0 = No Data Protect
        uint64_t            regenHeadMask;                               //!< Regen Head Mask: bitmap where 1 = bad head, 0 = good head
        uint64_t            pohFirst;                                    //!< Power - on Hours of the most recent FARM Time series frame save
        uint64_t            pohSecond;                                   //!< Power - on Hours of the second most recent FARM Time series frame save

        _sGeneralDriveInformationpage06() : pageNumber(0), copyNumber(0), Depop(0),
#if defined __cplusplus && __cplusplus >= 201103L
            productID{ 0 },
#endif
            driveType(0), isDepopped(0), maxNumAvaliableSectors(0), timeToReady(0),
            holdTime(0), servoSpinUpTime(0), writeProtect(0), regenHeadMask(0), pohFirst(0), pohSecond(0) {};
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
        uint64_t            min12vMv;                                   //!< Min voltage in mV for the 12V power supply since the last power cycle or pulling of FARM
        uint64_t            max12vMv;                                   //!< Max voltage in mV for the 12V power supply since the last power cycle or pulling of FARM
        uint64_t            min5vMv;                                    //!< Min voltage in mV for the 5V power supply since the last power cycle or pulling of FARM
        uint64_t            max5vMv;                                    //!< Max voltage in mV for the 5V power supply since the last power cycle or pulling of FARM
        uint64_t            timeSinceLastPowerCycle;                    //!< Time in minutes(rounded down) since the last power cycle or pulling of FARM

        _sScsiEnvironmentStatPage07() : pageNumber(0), copyNumber(0), average12v(0), min12v(0),
            max12v(0), average5v(0), min5v(0), max5v(0), min12vMv(0), max12vMv(0), min5vMv(0), max5vMv(0), timeSinceLastPowerCycle(0) {};
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
        uint64_t            readCmdBin0;                                //!< Count of Read Commands of transfer length bin 0 space for last 3 SMART Summary Frames  
        uint64_t            readCmdBin1;                                //!< Count of Read Commands of transfer length bin 1 space for last 3 SMART Summary Frames  
        uint64_t            readCmdBin2;                                //!< Count of Read Commands of transfer length bin 2 space for last 3 SMART Summary Frames  
        uint64_t            readCmdBin3;                                //!< Count of Read Commands of transfer length bin 3 space for last 3 SMART Summary Frames  
        uint64_t            readCmdBin4;                                //!< Count of Read Commands of transfer length bin 4 space for last 3 SMART Summary Frames  
        uint64_t            readCmdBin5;                                //!< Count of Read Commands of transfer length bin 5 space for last 3 SMART Summary Frames
        uint64_t            readCmdBin6;                                //!< Count of Read Commands of transfer length bin 6 space for last 3 SMART Summary Frames
        uint64_t            readCmdBin7;                                //!< Count of Read Commands of transfer length bin 7 space for last 3 SMART Summary Frames
        uint64_t            writeCmdBin0;                               //!< Count of Write Commands of transfer length bin 0 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin1;                               //!< Count of Write Commands of transfer length bin 1 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin2;                               //!< Count of Write Commands of transfer length bin 2 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin3;                               //!< Count of Write Commands of transfer length bin 3 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin4;                               //!< Count of Write Commands of transfer length bin 4 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin5;                               //!< Count of Write Commands of transfer length bin 5 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin6;                               //!< Count of Write Commands of transfer length bin 6 for last 3 SMART Summary Frames
        uint64_t            writeCmdBin7;                               //!< Count of Write Commands of transfer length bin 7 for last 3 SMART Summary Frames

        _sScsiWorkloadStatPage08() : pageNumber(0), copyNumber(0), countQueDepth1(0), countQueDepth2(0), 
            countQueDepth3_4(0), countQueDepth5_8(0), countQueDepth9_16(0), countQueDepth17_32(0), 
            countQueDepth33_64(0), countQueDepth_gt_64(0), readCmdBin0(0), readCmdBin1(0), readCmdBin2(0),
            readCmdBin3(0), readCmdBin4(0), readCmdBin5(0), readCmdBin6(0), readCmdBin7(0), writeCmdBin0(0),
            writeCmdBin1(0), writeCmdBin2(0), writeCmdBin3(0), writeCmdBin4(0), writeCmdBin5(0), writeCmdBin6(0),
            writeCmdBin7(0) {};
    }sScsiWorkloadStatPage08;

    typedef struct _sScsiGPESData
    {
        sLogParams          pageHeader;                                  //!< header page parameters
        uint64_t            gpesData[ZONECOUNT];                         //!< PARAM [16]  Each Qword Entries carry the following info :
                                                                         //!< -Status and Valid field[63:56]
                                                                         //!< - Reserved[55:48]
                                                                         //!< - Head when GPES changes[47:40]
                                                                         //!< - Timestamp(hours) when GPES changes[39:8]
                                                                         //!< - Get Physical Element Status(GPES) when it changes[7:0]
                                                                         //! 
#if defined __cplusplus && __cplusplus >= 201103L
        _sScsiGPESData() : gpesData{ 0 } {};
#endif
    }sScsiGPESData;

    typedef struct _sHeadInformation
    {
        sLogParams          pageHeader;                                  //!< header page parameters
        uint64_t            headValue[MAX_HEAD_COUNT];                   //!< head information
        
#if defined __cplusplus && __cplusplus >= 201103L
        _sHeadInformation() : headValue{ 0 } {};
#endif
           
    }sHeadInformation;

    typedef struct _sLUNStruct
    {
        sLogParams          pageHeader;                                 //!<  header page parameters
        uint64_t            pageNumber;                                 //!< Page Number = 0x50
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
        uint64_t            reserved20;                                 //!< Reserved
        uint64_t            reserved21;                                 //!< Reserved
        uint64_t            primarySPCovPercentageCMR;                  //!< Primary Super Parity Coverage Percentage CMR
        uint64_t            primarySPCovPercentageSMR;                  //!< Primary Super Parity Coverage Percentage SMR

        _sLUNStruct() : pageNumber(0), copyNumber(0), LUNID(0), headLoadEvents(0),reserved(0), reserved1(0), reserved2(0), 
            reserved3(0),  reserved4(0), reserved5(0), reserved6(0), reserved7(0), reserved8(0), reserved9(0), reserved10(0),
            reserved11(0), dosScansPerformed(0), correctedLBAbyISP(0), reserved12(0), reserved13(0), reserved14(0), reserved15(0),
            lbasCorrectedByParity(0), reserved16(0), reserved17(0), reserved18(0), reserved19(0), 
            reserved20(0), reserved21(0), primarySPCovPercentageCMR(0), primarySPCovPercentageSMR(0) {};
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
        uint64_t            numReallocatedSince;                        //!< 164 - 171 Number of reallocated sectors since the last FARM Time series Frame save
        int64_t             numReallocatedBetween;                      //!< 172 - 179 Number of reallocated sectors between FARM time series Frame N and N - 1
        uint64_t            numCandidateSince;                          //!< 180 - 187 Number of reallocation candidate sectors since the last FARM Time series Frame save
        int64_t             numCandidateBetween;                        //!< 188 - 195 Number of reallocation candidate sectors between FARM time series Frame N and N - 1

        _sActuatorReallocationData() : pageNumber(0), copyNumber(0), actID(0), numberReallocatedSectors(0), numberReallocatedCandidates(0)
#if defined __cplusplus && __cplusplus >= 201103L
            , reallocatedCauses{ 0 }
#endif 
       , numReallocatedSince(0), numReallocatedBetween(0), numCandidateSince(0), numCandidateBetween(0) {};
    }sActReallocationData;
#pragma pack(pop)

    typedef struct _sScsiSenseData
    {
        sLogParams          pageHeader;                                 //!< header page parameters (code 0x0500-0x050F, 0x0580-0x058F, 0x0600-0x0603, 0x0680-0x0683, 0x0690-0x0693)
        uint64_t            senseClass;                                 //!<  Sense Class calculated from the parameter code (code 0x0500-0x050F, 0x0580-0x058F, 0x0600-0x0603, 0x0680-0x0683, 0x0690-0x0693)
        uint64_t            classCount;                                 //!<  Class Count calculated from the parameter code (code 0x0500-0x050F, 0x0580-0x058F, 0x0600-0x0603, 0x0680-0x0683, 0x0690-0x0693)
        uint8_t             statusBits;                                 //!< byte 4:  Valid [7], Support [6], Reserved [5:0]
        uint64_t            cdbOpCode;                                  //!< byte 5:  CDB Operation Code
        uint64_t            errorCode;                                  //!< byte 8:  Error Code
        uint64_t            additionalSenseCode;                        //!< byte 9:  Additional Sense Code
        uint64_t            additionalSenseCodeQualifier;               //!< byte 10: Additional Sense Code Qualifier
        uint64_t            fruCode;                                    //!< byte 11: Field Replaceable Unit Code
        uint64_t            lbaStatus;                                  //!< byte 12: Valid [7], Support [6], Reserved [5:0]
        uint64_t            actuator;                                   //!< byte 13: Actuator [6]
        uint64_t            head;                                       //!< byte 13: Logical Head[0]
        uint64_t            errorLBA;                                   //!< bytes 14-19: Error LBA (48 bit, MSB first)
        uint64_t            powerCycleStatus;                           //!< byte 20: Valid [7], Support [6], Reserved [5:0]
        uint64_t            powerCycle;                                 //!< bytes 22-23: Power Cycle
        uint64_t            timeStamp;                                  //!< bytes 24-27: Time Stamp (seconds)

        _sScsiSenseData() :senseClass(0), classCount(0), statusBits(0), cdbOpCode(0), errorCode(0), additionalSenseCode(0), additionalSenseCodeQualifier(0),
            fruCode(0), lbaStatus(0), actuator(0), head(0), errorLBA(0), powerCycleStatus(0), powerCycle(0), timeStamp(0) {
        };
    }sScsiSenseData;


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
        sScsiGPESData           gpesPage09;                             //!< GPES data page 09
        std::vector<sScsiSenseData> vSenseData;                         //!< sense data entries found
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
        sHeadInformation        gpesHeadData;
        sHeadInformation        mrHeadResistanceByHead;
        sHeadInformation        currentH2SATPercentagedbyHead;
        sHeadInformation        currentH2STAmplituedByHead;
        sHeadInformation        currentH2STAsymmetryByHead;
        sHeadInformation        ResidentGlistEntries;
        sHeadInformation        ResidentPlistEntries;
        sHeadInformation        writePowerOnHours;
        sHeadInformation        cumECCReadRepeat;
        sHeadInformation        cumECCReadUnique;
        sHeadInformation        currentH2STTrimmedbyHeadZone0;
        sHeadInformation        currentH2STTrimmedbyHeadZone1;
        sHeadInformation        currentH2STTrimmedbyHeadZone2;
        sHeadInformation        currentH2STIterationsByHeadZone0;
        sHeadInformation        currentH2STIterationsByHeadZone1;
        sHeadInformation        currentH2STIterationsByHeadZone2;
        sHeadInformation        secondMRHeadResistanceByHead;       
        sHeadInformation        lifeTimeWritten;
        sHeadInformation        uniqueURESince;
        sHeadInformation        uniqueUREBetween;
        sHeadInformation        totalVisibleReadErrorsSeen;
        sHeadInformation        lifetimeSectorsRead;


        std::vector<eSASLogPageTypes>   vFramesFound;

    }sScsiFarmFrame;


}
