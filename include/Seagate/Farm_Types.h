//
// Farm_Types.h   Structures of FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Farm_Types.h  
#pragma once

#include <inttypes.h>

#define MAX_HEAD_COUNT			24
#define FARMSIGNATURE           0x00004641524D4552
#define MAJORVERSION2           2
#define MAJORVERSION3           3
#define MAJORVERSION4           4
#define FACTORYCOPY             0x00464143544f5259
#define FARMEMPTYSIGNATURE      0x00FFFFFFFFFFFFFF
#define FARMPADDINGSIGNATURE    0x0000000000000000
#define DATE_YEAR_DATE_SIZE     2
#define FLASH_EVENTS            8
#define REALLOCATIONEVENTS      15
#define PRINTABLE_MODEL_NUMBER  12
// FARM Combine definitions ascii check values
#define FARM_CURRENT            0x4641524d20202020                      //!< "FARM    "
#define FARM_FACTORY            0x464143544f525920                      //!< "FACTORY "
#define FARM_TIME               0x4641524d54494d45                      //<! "FARMTIME"
#define FARM_STICKY             0x4641524d5354434b                      //<! "FARMSTCK"
#define FARM_WLDTR              0x574f524b4c445443                      //<! "WORKLDTR"
#define FARM_SAVE               0x4641524d53415645                      //<! "FARMSAVE"
#define FARM_LONG_SAVE          0x4641524d4c4f4e47                      //<! "FARMLONG"


#define ASSERT_UNKNOWN                         0x00
#define MICROPROCESSOR_FAILED                  0x08
#define DRAM_FAILED_POWERUP_OR_WRAM_FAIL       0x0B
#define SCC_FAILED_POWERUP_DIAGNOSTICS         0x0C
#define FW_DOES_NOT_MATCH_THE_SCC_VERSION      0x0D
#define UNIMPLEMENTED_OPCODE_INTERRUPT         0x0E
#define POWER_UP_XOR_FAILURE_FOR_FIBER_CH      0x10
#define EEPROM_VERIFY_ERROR_EVEN_BYTE          0x12
#define EEPROM_ERASE_ERROR_EVEN_BYTE           0x13
#define DOWNLOAD_TPM_FAILED_0                  0x14
#define DOWNLOAD_TPM_FAILED_1                  0x15
#define DOWNLOAD_TPM_FAILED_2                  0x16
#define DOWNLOAD_TPM_FAILED_3                  0x17
#define DOWNLOAD_TPM_FAILED_4                  0x18
#define DOWNLOAD_TPM_FAILED_5                  0x19
#define DOWNLOAD_TPM_FAILED_6                  0x1A
#define DOWNLOAD_TPM_FAILED_7                  0x1B
#define DOWNLOAD_TPM_FAILED_8                  0x1C
#define DOWNLOAD_VOLTAGE_FAULT                 0x1D
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_0    0x30
#define FLASH_LOOKING_FOR_MEMORY_RANGE_ERROR   0x31
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_1    0x32
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_2    0x33
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_3    0x34
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_4    0x35
#define FAILS_WRITING_ARRAY_DATA_TO_FLASH_5    0x36
#define ALU_BUFFER_PARITY_ERROR                0x40
#define PREFETCH_TCM_ECC_ERROR                 0x41
#define ERROR_INJECTION_ASSERT                 0x42
#define DRAM_CONFIGURATION_PROCESS_FAILED      0x44
#define FDE_BUS_PARITY_ERROR                   0x45
#define PREFETCH_VECTOR_OR_STACK_POINTER_OUT   0x47
#define ERROR_IN_WRITING_TO_READ_CHIP          0x4C
#define IER_STACK_OVERFLOW                     0x4D
#define IER_STACK_UNDERFLOW                    0x4E
#define IER_STACK_NOT_EMPTY_ON_ENTRY_TO_SLEEP  0x4F
#define IRAW_HAD_MISCOMPARE                    0x5F
#define UNDEFINED_INSTRUCTION                  0x67
#define LOGGING_SAVE_FAILED_EXCEEDED_ALLOCATED 0x77
#define CANT_FIND_BACKPLANE_DATA_RATE          0x81
#define CONTROLLER_I_TCM_DOUBLE_BIT_ECC_ERROR  0x90
#define CONTROLLER_D_TCM_DOUBLE_BIT_ECC_ERROR  0x91
#define SERVO_I_TCM_DOUBLE_BIT_ECC_ERROR       0x92
#define SERVO_D_TCM_DOUBLE_BIT_ECC_ERROR       0x93
#define CDPRAM_UNRECOVERABLE_ERROR             0x94
#define SDPRAM_UNRECOVERABLE_ERROR             0x95
#define TCM_CRC_RESULT_IS_NON_ZERO             0x9D
#define SWI_ASSERT_FLASH_CODE_BOOT             0xBB
#define SWI_ASSERT_FLASH_CODE_NQNR             0xBC
#define SWI_ASSERT_FLASH_CODE_DISC             0xBD
#define REMOTE_ASSERT                          0xC3
#define DRAM_INTEGRITY_FAILURE                 0xC4
#define CLOCK_FAILURE                          0xC5
#define ASSERT_FLASH_CODE                      0xCC
#define ENSURE_FLASH_CODE                      0xCD
#define REQUIRE_FLASH_CODE                     0xCE
#define SMART_FLASH_CODE                       0xCF
#define SCSI_UNEXEPCTED_INTERRUPT              0xD1
#define SCSI_TIMEOUT                           0xD2
#define ILLEGAL_STATUS_CODE                    0xD3
#define SCSI_UNDER_OVER_RUN_OCCURRED           0xD4
#define UNEXPECTED_STATUS                      0xD5
#define DIVIDE_BY_ZERO_INTERRUPT               0xDD
#define DATA_ABORT_CACHE_ECC_ERROR             0xE0
#define DATA_ABORT_TCM_ECC_ERROR               0xE1
#define ABORT_INTERRUPT                        0xEE
#define SELF_SEEK_FAILURE                      0xF0
#define CONTROLLER_NUKED_BY_FDE                0xF1
#define FLASH_IOEDC_PARITY_ERROR               0xF4
#define SERIAL_PORT_DUMP_MODE                  0xF5


// Frame Type Identification  0x0 - 0xF
#define CURRENT_FRAME                           0x00
#define TIME_SERIES_FRAME                       0x01
#define LONG_TERM_FRAME                         0x02
#define GLIST_DISC_ENTRIES_FRAME                0x03
#define FIRST_UNRECOVERED_READ_ERROR_FRAME      0x04
#define TENTH_UNRECOVERED_READ_ERROR_FRAME      0x05
#define FIRST_FATAL_CTO_FRAME                   0x06
#define BEFORE_CFW_UPDATE_FRAME                 0x07
#define TEMP_EXCEDED_FRAME                      0x08
#define RESERVED_FRAME                          0x09
#define FACTORY_COPY_FRAME                      0x0A
#define FARM_LOG_DISC_COPY_FRAME                0x0B  //sata only
#define IDD_FARM_LOG_DISC_FRAME                 0x0C  //sata only
#define FARM_DRAM_COPY_FARME                    0x0D  //sata only
#define FARM_DRAM_COPY_DIAG_FRAME               0x0E  
#define FARM_UDS_COPY_FRAME                     0x0F
#define FARM_EMPTY_FRAME                        0xff

typedef enum _eReallocationCauses
{
    HOST_READ_GENERIC,
    HOST_READ_UNCORRECTABLE,
    HOST_READ_RAW,
    HOST_WRITE_GENERIC,
    HOST_WRITE_UNCORRECTABLE,
    HOST_WRITE_RAW,
    BACKGROUND_READ_GENERIC,
    BACKGROUND_READ_RELIABILITY,
    BACKGROUND_READ_RECOVERY,
    BACKGROUND_READ_HOST_SELF_TEST,
    BACKGROUND_WRITE_GENERIC,
    BACKGROUND_WRITE_RELIABILITY,
    BACKGROUND_WRITE_RECOVERY,
    BACKGROUND_WRITE_HOST_SELF_TEST,
    SERVO_WEDGE,
}eReallocationCauses;


typedef struct _sStringIdentifyData
{
    std::string serialNumber;									//!< seiral number of the drive
    std::string worldWideName;									//!< World Wide Name of the device
    std::string deviceInterface;								//!< Device Interface 
    std::string firmwareRev;									//!< Firmware Rev
    std::string modelNumber;                                    //!< Model Number (4.3 and newer will have this)
}sStringIdentifyData;

#pragma pack(push, 1)
typedef struct _sFarmHeader
{
    uint64_t        signature;                                  //!< Log Signature = 0x00004641524D4552
    uint64_t        majorRev;                                   //!< Log Major rev
    uint64_t        minorRev;                                   //!< minor rev 
    uint64_t        pagesSupported;                             //!< number of pages supported
    uint64_t        logSize;                                    //!< log size in bytes
    uint64_t        pageSize;                                   //!< page size in bytes
    uint64_t        headsSupported;                             //!< Maximum Drive Heads Supported
    uint64_t        copies;                                     //!< Number of Historical Copies
    uint64_t        reasonForFrameCapture;                      //!< Reason for Frame Capture (added 4.14)
    _sFarmHeader() : signature(0), majorRev(0), minorRev(0), pagesSupported(0), logSize(0), pageSize(0), headsSupported(0), copies(0), reasonForFrameCapture(0) {};
}sFarmHeader;

typedef struct _sWorkLoadStat
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
    union
    {
        uint64_t        dither;                                 //!< ATA - Number of dither events during current power cycle (added 3.4)
        uint64_t        totalReadCmdsFromFrames1;               //!< SCSI - Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        ditherRandom;                           //!< ATA - Number of times dither was held off during random workloads during current power cycle(added 3.4)
        uint64_t        totalReadCmdsFromFrames2;               //!< SCSI - Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        ditherSequential;                       //!< ATA - Number of times dither was held off during sequential workloads during current power cycle(added 3.4)
        uint64_t        totalReadCmdsFromFrames3;               //!< SCSI - Number of Read commands from 25-75% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds1;                      //!< ATA - Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalReadCmdsFromFrames4;               //!< SCSI - Number of Read commands from 75-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds2;                      //!< ATA - Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames1;              //!< SCSI - Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds3;                      //!< ATA - Number of Read commands from 25-75% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames2;              //!< SCSI - Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds4;                      //!< ATA - Number of Read commands from 75-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames3;              //!< SCSI - Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfWriteCmds1;                     //!< ATA - Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames4;              //!< SCSI - Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union 
    {
        uint64_t        numberOfWriteCmds2;                         //!< ATA - Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferSmall;                      //!< SCSI - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };
    union
    {
        uint64_t        numberOfWriteCmds3;                         //!< ATA - Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferMid1;                      //!< SCSI - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
    };
    union
    {
        uint64_t        numberOfWriteCmds4;                         //!< ATA - Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferMid2;                      //!< SCSI - Number of read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    };
    union 
    {
        uint64_t        numReadTransferLarge;                      //!< SCSI - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames
        uint64_t        numReadTransferSmallATA;                      //!< ATA - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };
    
    union 
    {
        uint64_t        numWriteTransferSmall;                      //!< SCSI - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
        uint64_t        numReadTransferMid1ATA;                      //!< ATA - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames    
    };
    union 
    {
        uint64_t        numWriteTransferMid1;                       //!< SCSI - Number of Write Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
        uint64_t        numReadTransferMid2ATA;                      //!< ATA - Number of Read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    };
    union 
    {
        uint64_t        numWriteTransferMid2;                       //!< SCSI - Number of Write Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
        uint64_t        numReadTransferLargeATA;                      //!< ATA - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames    };
    };
    union 
    {
        uint64_t        numWriteTransferLarge;                      //!< SCSI - Number of Write Commands of transfer length > 2MB for last 3 SMART Summary Frames
        uint64_t        numWriteTransferSmallATA;                      //!< ATA - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };

    uint64_t        numWriteTransferMid1ATA;                      //!< ATA - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
    uint64_t        numWriteTransferMid2ATA;                      //!< ATA - Number of Read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    uint64_t        numWriteTransferLargeATA;                     //!< ATA - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames 
    uint64_t        cntQueueDepth1;                               //!< ATA - Count of Queue Depth =1 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth2;                               //!< ATA - Count of Queue Depth =2 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth3to4;                            //!< ATA - Count of Queue Depth =3-4 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth5to8;                            //!< ATA - Count of Queue Depth =5-8 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth9to16;                           //!< ATA - Count of Queue Depth =9-16 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth17to32;                          //!< ATA - Count of Queue Depth =17-32 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth33to64;                          //!< ATA - Count of Queue Depth =33-64 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepthmorethan64;                      //!< ATA - Count of Queue Depth >64 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        numDithEvtAct1;                               //!< ATA - Number of dither events during current power cycle, Actuator 1
    uint64_t        numRandWLDitherHoldOffAct1;                   //!< ATA - Number of times dither was held off during random workloads during current power cycle, Actuator 1
    uint64_t        numSequentialWLDitherHoldOffAct1;             //!< ATA - Number of times dither was held off during sequential workloads during current power cycle, Actuator 1
    uint64_t        hotWriteStatistics;                           //!< ATA - Hot write statistics: including metadata and 2400-band/region workload rating data


    _sWorkLoadStat() : pageNumber(0), copyNumber(0), workloadPercentage(0), totalReadCommands(0), totalWriteCommands(0), totalRandomReads(0), totalRandomWrites(0), 
        totalNumberofOtherCMDS(0), logicalSecWritten(0), logicalSecRead(0), dither(0), ditherRandom(0), ditherSequential(0), numberOfReadCmds1(0), numberOfReadCmds2(0), 
        numberOfReadCmds3(0), numberOfReadCmds4(0), numberOfWriteCmds1(0), numberOfWriteCmds2(0), numberOfWriteCmds3(0), numberOfWriteCmds4(0),
        numReadTransferLarge(0), numWriteTransferSmall(0),  numWriteTransferMid1(0), numWriteTransferMid2(0), numWriteTransferLarge(0), 
        numWriteTransferMid1ATA(0), numWriteTransferMid2ATA(0), numWriteTransferLargeATA(0), cntQueueDepth1(0), cntQueueDepth2(0),
        cntQueueDepth3to4(0), cntQueueDepth5to8(0), cntQueueDepth9to16(0), cntQueueDepth17to32(0), cntQueueDepth33to64(0),
        cntQueueDepthmorethan64(0), numDithEvtAct1(0), numRandWLDitherHoldOffAct1(0), numSequentialWLDitherHoldOffAct1(0), hotWriteStatistics(0) {};
}sWorkLoadStat;

typedef struct _sComboDataSet
{
    uint64_t        dataSetType;                                //!< 
    uint32_t        location;                                   //!<
    uint32_t        dataSize;                                   //!<
    uint64_t        startTime;                                  //!<
    uint64_t        endTime;                                    //!<

    _sComboDataSet() : dataSetType(0), location(0), dataSize(0), startTime(0), endTime(0) {};
}sComboDataSet;
#pragma pack(pop)
