//
// Ata_Farm_Types.h   Structures of ATA FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Ata_Farm_Types.h  
#pragma once

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"

#pragma pack(push, 1)
typedef struct _sDriveInfo
{
    uint64_t        pageNumber;                                 //!< page Number 1
    uint64_t        copyNumber;                                 //!< copy number
    uint64_t        serialNumber;                               //!< Serial Number
    uint64_t        serialNumber2;                              //!< Serial Number 2
    uint64_t        worldWideName;                              //!< world wide name 
    uint64_t        worldWideName2;                             //!< world wide name 2
    uint64_t        deviceInterface;                            //!< device interface
    uint64_t        deviceCapacity;                             //!< 48-bit Device Capacity
    uint64_t        psecSize;                                   //!< Physical Sector Size in Bytes
    uint64_t        lsecSize;                                   //!< Logical Sector Size in Bytes
    uint64_t        deviceBufferSize;                           //!< Device Buffer Size in Bytes
    uint64_t        heads;                                      //!< Number of Heads
    uint64_t        factor;                                     //!< Device Form Factor (ID Word 168)
    uint64_t        rotationRate;                               //!< Rotational Rate of Device (ID Word 217)
    uint64_t        firmware;                                   //!< firmware 
    uint64_t        firmwareRev;                                //!< firmware Rev
    uint64_t        security;									//!< ATA Security State (ID Word 128)
    uint64_t        featuresSupported;							//!< ATA Features Supported (ID Word 78)
    uint64_t        featuresEnabled;							//!< ATA Features Enabled (ID Word 79)
    uint64_t        poh;                                        //!< Power-on Hours
    uint64_t        spoh;										//!< Spindle Power-on Hours
    uint64_t        headFlightHours;							//!< Head Flight Hours
    uint64_t        headLoadEvents;                             //!< Head Load Events
    uint64_t        powerCycleCount;                            //!< Power Cycle Count
    uint64_t        resetCount;                                 //!< Hardware Reset Count
    uint64_t        spinUpTime;                                 //!< SMART Spin-Up time in milliseconds
    uint64_t        NVC_StatusATPowerOn;                        //!< NVC Status on Power-on
    uint64_t        timeAvailable;                              //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    uint64_t        timeStamp1;                                 //!< Timestamp of most recent SMART Summary Frame in Power-On Hours Milliseconds
    uint64_t        timeStamp2;                                 //!< Timestamp of last SMART Summary Frame in Power-On Hours Milliseconds
    uint64_t        timeToReady;								//!< time to ready of the last power cycle
    uint64_t        timeHeld;									//!< time drive is held in staggered spin during the last power on sequence
    uint64_t        modelNumber[10];                            //!< lower 32 Model Number (started support in 2.14 )
    uint64_t        driveRecordingType;                         //!< 0 for SMR and 1 for CMR (started support in 2.15 )
    uint64_t        depopped;                                   //!< has the drive been depopped  1= depopped and 0 = not depopped(started support in 2.15 )
    uint64_t        maxNumberForReasign;                        //!< Max Number of Available Sectors for Reassignment – Value in disc sectors(started in 3.3 )
    uint64_t        dateOfAssembly;                             //!< Date of Assembly in ASCII “YYWW” where YY is the year and WW is the calendar week(started in 4.2)
    uint64_t        depopulationHeadMask;                       //!< Depopulation Head Mask(started in 4.7)
}sDriveInfo;

typedef struct _sErrorStat
{
    uint64_t         pageNumber;                                //!< Page Number = 3
    uint64_t         copyNumber;                                //!< Copy Number
    uint64_t         totalReadECC;                              //!< Number of Unrecoverable Read Errors
    uint64_t         totalWriteECC;                             //!< Number of Unrecoverable Write Errors
    uint64_t         totalReallocations;                        //!< Number of Reallocated Sectors
    uint64_t         totalReadRecoveryAttepts;                  //!< Number of Read Recovery Attempts
    uint64_t         totalMechanicalFails;                      //!< Number of Mechanical Start Failures
    uint64_t         totalReallocatedCanidates;                 //!< Number of Reallocated Candidate Sectors2
    uint64_t         totalASREvents;                            //!< Number of ASR Events3
    uint64_t         totalCRCErrors;                            //!< Number of Interface CRC Errors3
    uint64_t         attrSpinRetryCount;                        //!< Spin Retry Count 
    uint64_t         normalSpinRetryCount;                      //!< Spin Retry Count 
    uint64_t         worstSpinRretryCount;                      //!< Spin Retry Count 
    uint64_t         attrIOEDCErrors;                           //!< Number of IOEDC Errors 
    uint64_t         attrCTOCount;                              //!< CTO Count Total 
    uint64_t         overfiveSecCTO;                            //!< CTO Count Over 5s 
    uint64_t         oversevenSecCTO;                           //!< CTO Count Over 7.5s 
    uint64_t         totalFlashLED;                             //!< Total Flash LED (Assert) Events
    uint64_t         indexFlashLED;                             //!< index of the last Flash LED of the array
    uint64_t         uncorrectables;                            //!< uncorrecatables errors (sata only)
    uint64_t         reserved1;                                 //!< reserved
    uint64_t         flashLEDArray[8];                          //!< Info on the last 8 Flash LED events Wrapping array. (added 2.7)
    uint64_t         readWriteRetry[8];                         //!< Info on the last 8 read/write Retry events (added 2.7)
    uint64_t         reserved2[2];                              //!< reserved  (added 3.0)
    uint64_t         reallocatedSectors[15];                    //!< Reallocated Secotors by cause(added 3.0)
    uint64_t         timestampForLED[8];                        //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array (added 4.7)
    uint64_t         powerCycleOfLED[8];                        //<! Power Cycle of the last 8 Flash LED (assert) Events, wrapping array (added 4.7)
    uint64_t         cumLifeTimeECCReadDueErrorRecovery;        //<! Cumulative Lifetime Unrecoverable Read errors due to Error Recovery Control 
    uint64_t         cumLifeUnRecoveralbeReadByhead[24];        //<! Cumulative Lifetime Unrecoverable Read Repeating by head
    uint64_t         cumLiveUnRecoveralbeReadUnique[24];        //<! Cumulative Lifetime Unrecoverable Read Unique by head
}sErrorStat;

typedef struct _sEnvironementStat
{
    uint64_t         pageNumber;                                //!< Page Number = 4
    uint64_t         copyNumber;                                //!< Copy Number
    uint64_t         curentTemp;                                //!< Current Temperature in Celsius
    uint64_t         highestTemp;                               //!< Highest Temperature in Celsius
    uint64_t         lowestTemp;                                //!< Lowest Temperature
    uint64_t         averageTemp;                               //!< Average Short Term Temperature
    uint64_t         averageLongTemp;                           //!< Average Long Term Temperature
    uint64_t         highestShortTemp;                          //!< Highest Average Short Term Temperature
    uint64_t         lowestShortTemp;                           //!< Lowest Average Short Term Temperature
    uint64_t         highestLongTemp;                           //!< Highest Average Long Term Temperature
    uint64_t         lowestLongTemp;                            //!< Lowest Average Long Term Temperature
    uint64_t         overTempTime;                              //!< Time In Over Temperature
    uint64_t         underTempTime;                             //!< Time In Under Temperature
    uint64_t         maxTemp;                                   //!< Specified Max Operating Temperature
    uint64_t         minTemp;                                   //!< Specified Min Operating Temperature
    uint64_t         shockEvents;                               //!< Over-Limit Shock Events Count
    uint64_t         hfWriteCounts;                             //!< High Fly Write Count 
    uint64_t         humidity;                                  //!< Current Relative Humidity (in units of .1%)
    uint64_t         humidityRatio;                             //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    uint64_t         currentMotorPower;                         //!< Current Motor Power, value from most recent SMART Summary Frame
    uint64_t         current12v;                                //!< Current 12V input (added 3.7)
    uint64_t         min12v;                                    //!< Minimum 12V input from last 3 SMART Summary Frames (3.7)
    uint64_t         max12v;                                    //!< Maximum 12V input from last 3 SMART Summary Frames (3.7)
    uint64_t         current5v;                                 //!< Current 5V input (3.7)
    uint64_t         min5v;                                     //!< Minimum 5V input from last 3 SMART Summary Frames (3.7)
    uint64_t         max5v;                                     //!< Maximum 5V input from last 3 SMART Summary Frames (3.7)
    uint64_t         powerAvg12v;                               //!< 12V Power Average(mw) - Average of the three summary frames (4.3)
    uint64_t         powerMin12v;                               //!< 12V Power Min(mw) - Lowest of last 3 SMART summary frames (4.3)
    uint64_t         powerMax12v;                               //!< 12V Power Max(mw) - Highest of last 3 SMART summary frames (4.3)
    uint64_t         powerAvg5v;                                //!< 5V Power Average(mw) - Average of the three summary frames (4.3)
    uint64_t         powerMin5v;                                //!< 5V Power Min(mw) - Lowest of last 3 SMART summary frames (4.3)
    uint64_t         powerMax5v;                                //!< 5V Power Max(mw) - Highest of last 3 SMART summary frames (4.3)
}sEnvironementStat;

typedef struct _sHeadInfo
{
    uint32_t        decimalPartofFloat;                         //!< data after the decimal
    int16_t         wholePartofFloat;                           //!< the whole number
    uint8_t         reseved;
    uint8_t         status;                                     //!< status bit is set here
    _sHeadInfo() : decimalPartofFloat(0), wholePartofFloat(0), reseved(0), status(0) {};
}sHeadInfo;

typedef struct _sflyHeight
{
    int64_t        inner;
    int64_t        middle;
    int64_t        outer;
    _sflyHeight() : inner(0), middle(0), outer(0) {};
}sflyHeight;

typedef struct _sAtaReliabilityStat
{
    uint64_t        pageNumber;                                  //!< Page Number = 5
    uint64_t        copyNumber;                                  //!< Copy Number
    int64_t         lastIDDTest;                                 //!< Timestamp of last IDD test
    int64_t         cmdLastIDDTest;                              //!< Sub-command of last IDD test
    uint64_t        discSlip[MAX_HEAD_COUNT];                    //!< [24] Disc Slip in micro-inches by Head
    uint64_t        bitErrorRate[MAX_HEAD_COUNT];                //!< [24] Bit Error Rate of Zone 0 by Drive Head
    int64_t         gListReclamed;                               //!< Number of G-List Reclamations 
    int64_t         servoStatus;                                 //!< Servo Status (follows standard DST error code definitions)
    int64_t         altsBeforeIDD;                               //!< Number of Alt List Entries Before IDD Scan
    int64_t         altsAfterIDD;                                //!< Number of Alt List Entries After IDD Scan
    int64_t         gListBeforIDD;                               //!< Number of Resident G-List Entries Before IDD Scan
    int64_t         gListAfterIDD;                               //!< Number of Resident G-List Entries After IDD Scan
    int64_t         scrubsBeforeIDD;                             //!< Number of Scrub List Entries Before IDD Scan
    int64_t         scrubsAfterIDD;                              //!< Number of Scrub List Entries After IDD Scan
    int64_t         numberDOSScans;                              //!< Number of DOS Scans Performed
    int64_t         numberLBACorrect;                            //!< Number of LBAs Corrected by ISP
    int64_t         numberValidParitySec;                        //!< Number of Valid Parity Sectors
    int64_t         dosWriteCount[MAX_HEAD_COUNT];               //!< [24] DOS Write Refresh Count7
    int64_t         numberRAWops;                                //!< Number of RAW Operations
    int64_t         DVGASkipWriteDetect[MAX_HEAD_COUNT];         //!< [24] DVGA Skip Write Detect by Head
    int64_t         RVGASkipWriteDetect[MAX_HEAD_COUNT];         //!< [24] RVGA Skip Write Detect by Head
    int64_t         FVGASkipWriteDetect[MAX_HEAD_COUNT];         //!< [24] FVGA Skip Write Detect by Head
    int64_t         skipWriteDetectThresExceeded[MAX_HEAD_COUNT];//!< [24] Skip Write Detect Threshold Exceeded Count by Head
    int64_t         attrErrorRateRaw;                            //!< Error Rate 
    int64_t         attrErrorRateNormal;                         //!< Error Rate 
    int64_t         attrErrorRateWorst;                          //!< Error Rate 
    int64_t         attrSeekErrorRateRaw;                        //!< Seek Error Rate 
    int64_t         attrSeekErrorRateNormal;                     //!< Seek Error Rate 
    int64_t         attrSeekErrorRateWorst;                      //!< Seek Error Rate 
    int64_t         attrUnloadEventsRaw;                         //!< High Priority Unload Events 
    uint64_t        microActuatorLockOUt;                        //!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
    int64_t         sineACFF[MAX_HEAD_COUNT];                    //!< [24] ACFF Sine 1X, value from most recent SMART Summary Frame by Head
    int64_t         cosineACFF[MAX_HEAD_COUNT];                  //!< [24] ACFF Cosine 1X, value from most recent SMART Summary Frame by Head
    int64_t         PZTCalibration[MAX_HEAD_COUNT];              //!< [24] PZT Calibration, value from most recent SMART Summary Frame by Head
    int64_t         MRHeadResistance[MAX_HEAD_COUNT];            //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head
    int64_t         numberOfTMD[MAX_HEAD_COUNT];                 //!< [24] Number of TMD over last 3 SMART Summary Frames by Head
    int64_t         velocityObserver[MAX_HEAD_COUNT];            //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head
    int64_t         numberOfVelocityObserver[MAX_HEAD_COUNT];    //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head
    sflyHeight      currentH2SAT[MAX_HEAD_COUNT];                //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 
    sflyHeight      currentH2SATIterations[MAX_HEAD_COUNT];      //!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 
    int64_t         currentH2SATPercentage[MAX_HEAD_COUNT];      //!< [24] Qword[24] Current H2SAT percentage of codewords at iteration level by Head, averaged
    int64_t         currentH2SATamplitude[MAX_HEAD_COUNT];       //!< [24] Qword[24] Current H2SAT amplitude by Head, averaged across Test Zones 
    int64_t         currentH2SATasymmetry[MAX_HEAD_COUNT];       //!< [24] Qword[24] Current H2SAT asymmetry by Head, averaged across Test Zones
    sflyHeight      flyHeightClearance[MAX_HEAD_COUNT];          //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    int64_t         diskSlipRecalPerformed;                      //!< Number of disc slip recalibrations performed
    int64_t         gList[MAX_HEAD_COUNT];                       //!< [24] Number of Reallocated Sectors per head
    int64_t         pendingEntries[MAX_HEAD_COUNT];              //!< [24] Number of Reallocation Candidate Sectors per head
    int64_t         heliumPresureTrip;                           //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
    int64_t         oughtDOS[MAX_HEAD_COUNT];                    //!< [24] DOS Ought to scan count per head
    int64_t         needDOS[MAX_HEAD_COUNT];                     //!< [24] DOS needs to scanns count per head
    int64_t         writeDOSFault[MAX_HEAD_COUNT];               //!< [24] DOS  write Fault scans per head
    int64_t         writePOH[MAX_HEAD_COUNT];                    //!< [24] write POS in sec value from most recent SMART Frame by head
    int64_t         RVAbsoluteMean;                              //!< RV Absolute Mean, value from the most recent SMART Frame
    int64_t         maxRVAbsluteMean;                            //!< Max RV Absolute Mean, value from the most recent SMART Summary Frame
    int64_t         idleTime;                                    //!< idle Time, Value from most recent SMART Summary Frame
    int64_t         countDOSWrite[MAX_HEAD_COUNT];               //!< [24] DOS Write Count Threshold per head
    int64_t         secondMRHeadResistance[MAX_HEAD_COUNT];      //!< [24] Second Head, MR Head Resistance from most recent SMART Summary Frame by Head (added in 2.11)
    int64_t         FAFHMeasurementStatus[MAX_HEAD_COUNT];       //!< [24] FAFH Measurement Status, bitwise OR across all diameters per head (added in 4.5)
    int64_t         FAFHRelativeAmptitude[MAX_HEAD_COUNT];       //!< [24] FAFH HF/LF Relative Amplitude in tenths, maximum value across all 3 zones per head (added in 4.5)
    sflyHeight      FAFHBitErrorRate[MAX_HEAD_COUNT];            //!< [24][3] FAFH Bit Error Rate, write then read BER on reserved tracks (added in 4.5)
    sflyHeight      FAFHLowFrequency[MAX_HEAD_COUNT];            //<! [24][3] FAFH Low Frequency Passive Clearance in ADC counts (added in 4.5)
    sflyHeight      FAFHHighFrequency[MAX_HEAD_COUNT];           //<! [24][3] FAFH High Frequency Passive Clearance in ADC counts (added in 4.5)
    uint64_t        numberLBACorrectedByParitySector;            //<! Number of LBAs Corrected by Parity Sector
}sAtaReliabilityStat;

typedef struct _sFarmFrame
{
    sDriveInfo              driveInfo;                          //<! drive information page
    sStringIdentifyData     identStringInfo;                    //<! all the string information from the drive information page
    sWorkLoadStat           workLoadPage;                       //<! work load data page
    sErrorStat              errorPage;                          //<! error data page
    sEnvironementStat       environmentPage;                    //<! environment information page 
    sAtaReliabilityStat     reliPage;                           //<! reliability information page
}sFarmFrame;
#pragma pack(pop)
