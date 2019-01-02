//
// CAta_Farm_Log.h
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

// \file CAta_Farm_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"

namespace opensea_parser {

#ifndef ATAFARM
#define ATAFARM

    class CATA_Farm_Log 
    {
        protected:
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
			}sFarmHeader;

			typedef struct _sDriveInfo
			{
				uint64_t        pageNumber;                                 //!< page Number 1
				uint64_t        copyNumber;                                 //!< copy number
				uint64_t        serialNumber;                               //!< Serial Number
				uint64_t        serialNumber2;                              //!< Serial Number 2
				uint64_t        worldWideName;                              //!< world wide name 
				uint64_t        worldWideName2;                             //!< world wide name 2
				uint64_t        deviceInterface;                            //!< device interface
				uint64_t        deviceCapcity;                              //!< 48-bit Device Capacity
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
				uint64_t        timeStamp1;                                 //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds
				uint64_t        timeStamp2;                                 //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds
				uint64_t        timeToReady;								//!< time to ready of the last power cycle
				uint64_t        timeHeld;									//!< time drive is held in staggered spin during the last power on sequence
			}sDriveInfo;

			typedef struct _sStringIdentifyData
			{
				std::string serialNumber;										//!< seiral number of the drive
				std::string worldWideName;										//!< World Wide Name of the device
				std::string deviceInterface;									//!< Device Interface 
				std::string firmwareRev;										//!< Firmware Rev
			}sStringIdentifyData;
			
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
			}sWorkLoadStat;

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
                uint64_t         flashLEDArray[8];                          //!< Info on the last 8 Flash LED events Wrapping array.
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
			}sEnvironementStat;

            typedef struct _sHeadInfo
            {
                uint32_t        decimalPartofFloat;                         //!< data after the decimal
                int16_t         wholePartofFloat;                           //!< the whole number
                uint8_t         reseved;
                uint8_t         status;                                     //!< status bit is set here
            }sHeadInfo;

            typedef struct _sflyHeight
            {
                uint64_t        outer;
                uint64_t        inner;
                uint64_t        middle;
            }sflyHeight;

            typedef struct _sAtaReliabilityStat
            {
                uint64_t        pageNumber;                                  //!< Page Number = 5
                uint64_t        copyNumber;                                  //!< Copy Number
                uint64_t        lastIDDTest;                                 //!< Timestamp of last IDD test
                uint64_t        cmdLastIDDTest;                              //!< Sub-command of last IDD test
                sHeadInfo       discSlip[RELISTAT];                          //!< [24] Disc Slip in micro-inches by Head
                sHeadInfo       bitErrorRate[RELISTAT];                      //!< [24] Bit Error Rate of Zone 0 by Drive Head
                uint64_t        gListReclamed;                               //!< Number of G-List Reclamations 
                uint64_t        servoStatus;                                 //!< Servo Status (follows standard DST error code definitions)
                uint64_t        altsBeforeIDD;                               //!< Number of Alt List Entries Before IDD Scan
                uint64_t        altsAfterIDD;                                //!< Number of Alt List Entries After IDD Scan
                uint64_t        gListBeforIDD;                               //!< Number of Resident G-List Entries Before IDD Scan
                uint64_t        gListAfterIDD;                               //!< Number of Resident G-List Entries After IDD Scan
                uint64_t        scrubsBeforeIDD;                             //!< Number of Scrub List Entries Before IDD Scan
                uint64_t        scrubsAfterIDD;                              //!< Number of Scrub List Entries After IDD Scan
                uint64_t        numberDOSScans;                              //!< Number of DOS Scans Performed
                uint64_t        numberLBACorrect;                            //!< Number of LBAs Corrected by ISP
                uint64_t        numberValidParitySec;                        //!< Number of Valid Parity Sectors
                uint64_t        dosWriteCount[RELISTAT];                     //!< [24] DOS Write Refresh Count7
                uint64_t        numberRAWops;                                //!< Number of RAW Operations
                uint64_t        DVGASkipWriteDetect[RELISTAT];               //!< [24] DVGA Skip Write Detect by Head
                uint64_t        RVGASkipWriteDetect[RELISTAT];               //!< [24] RVGA Skip Write Detect by Head
                uint64_t        FVGASkipWriteDetect[RELISTAT];               //!< [24] FVGA Skip Write Detect by Head
                uint64_t        skipWriteDetectThresExceeded[RELISTAT];      //!< [24] Skip Write Detect Threshold Exceeded Count by Head
                uint64_t        attrErrorRateRaw;                            //!< Error Rate 
                uint64_t        attrErrorRateNormal;                         //!< Error Rate 
                uint64_t        attrErrorRateWorst;                          //!< Error Rate 
                uint64_t        attrSeekErrorRateRaw;                        //!< Seek Error Rate 
                uint64_t        attrSeekErrorRateNormal;                     //!< Seek Error Rate 
                uint64_t        attrSeekErrorRateWorst;                      //!< Seek Error Rate 
                uint64_t        attrUnloadEventsRaw;                         //!< High Priority Unload Events 
                uint64_t        microActuatorLockOUt;                        //!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
                uint64_t        sineACFF[RELISTAT];                          //!< [24] ACFF Sine 1X, value from most recent SMART Summary Frame by Head
                uint64_t        cosineACFF[RELISTAT];                        //!< [24] ACFF Cosine 1X, value from most recent SMART Summary Frame by Head
                uint64_t        PZTCalibration[RELISTAT];                    //!< [24] PZT Calibration, value from most recent SMART Summary Frame by Head
                uint64_t        MRHeadResistance[RELISTAT];                  //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head
                uint64_t        numberOfTMD[RELISTAT];                       //!< [24] Number of TMD over last 3 SMART Summary Frames by Head
                uint64_t        velocityObserver[RELISTAT];                  //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head
                uint64_t        numberOfVelocityObserver[RELISTAT];          //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head
                sflyHeight      currentH2SAT[RELISTAT];                      //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 
                sflyHeight      currentH2SATIterations[RELISTAT];            //!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 
                uint64_t        currentH2SATPercentage[RELISTAT];            //!< [24] Qword[24] Current H2SAT percentage of codewords at iteration level by Head, averaged
                uint64_t        currentH2SATamplitude[RELISTAT];             //!< [24] Qword[24] Current H2SAT amplitude by Head, averaged across Test Zones 
                uint64_t        currentH2SATasymmetry[RELISTAT];             //!< [24] Qword[24] Current H2SAT asymmetry by Head, averaged across Test Zones
                sflyHeight      flyHeightClearance[RELISTAT];                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
                uint64_t        diskSlipRecalPerformed;                      //!< Number of disc slip recalibrations performed
                uint64_t        gList[RELISTAT];                             //!< [24] Number of Resident G-List per Head
                uint64_t        pendingEntries[RELISTAT];                    //!< [24] Number of pending Entries per Head
                uint64_t        heliumPresureTrip;                           //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
                uint64_t        oughtDOS[RELISTAT];                          //!< [24] DOS Ought to scan count per head
                uint64_t        needDOS[RELISTAT];                           //!< [24] DOS needs to scanns count per head
                uint64_t        writeDOSFault[RELISTAT];                     //!< [24] DOS  write Fault scans per head
                uint64_t        writePOH[RELISTAT];                          //!< [24] write POS in sec value from most recent SMART Frame by head
                uint64_t        RVAbsoluteMean;                              //!< RV Absolute Mean, value from the most recent SMART Frame
                uint64_t        maxRVAbsluteMean;                            //!< Max RV Absolute Mean, value from the most recent SMART Summary Frame
                uint64_t        idleTime;                                    //!< idle Time, Value from most recent SMART Summary Frame
                uint64_t        countDOSWrite[RELISTAT];                     //!< [24] DOS Write Count Threshold per head

            }sAtaReliabilityStat;

            typedef struct _sFarmFrame
            {
                sDriveInfo              driveInfo;                          //!< drive information page
                sStringIdentifyData     identStringInfo;                    //!< all the string information from the drive information page
                sWorkLoadStat           workLoadPage;                       //!< work load data page
                sErrorStat              errorPage;                          //<! error data page
                sEnvironementStat       environmentPage;                    //<! environment information page 
                sAtaReliabilityStat     reliPage;                           //!< reliability information page
            }sFarmFrame;
#pragma pack(pop)
            std::vector <sFarmFrame > vFarmFrame;
            std::vector <sFarmFrame >vBlankFarmFrame;

            uint64_t                    m_totalPages;                       //!< number of pages supported
            uint64_t                    m_logSize;                          //!< log size in bytes
            uint64_t                    m_pageSize;                         //!< page size in bytes
            uint64_t                    m_heads;                            //!< number of heads - first fill in with header information then changed to heads reported on the drive
			uint64_t					m_MaxHeads;							//!< Maximum Drive Heads Supported
            uint64_t                    m_copies;                           //!< Number of Historical Copies  
            eReturnValues               m_status;                           //!< status of the class
            sFarmHeader                 *m_pHeader;                         //!< Member pointer to the header of the farm log  
            uint8_t                     *pBuf;                              //!< pointer to the buffer data that is the binary of FARM LOG

            eReturnValues print_Header(JSONNODE *masterData);
            eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Work_Load(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Serial_Number()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
			//
			//  Entry:
			//! \param serialNumber - string for holding the serial number of the drive ( putting it all together)
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return serialNumber = the string serialNumber
			//
			//---------------------------------------------------------------------------
			inline void create_Serial_Number(std::string *serialNumber, const sDriveInfo * const idInfo)
			{
				uint64_t sn = 0;
				sn = (idInfo->serialNumber & 0x00FFFFFFFFFFFFFFLL) | ((idInfo->serialNumber2 & 0x00FFFFFFFFFFFFFFLL) << 32);
				serialNumber->resize(SERIAL_NUMBER_LEN);
				memset((char*)serialNumber->c_str(), 0, SERIAL_NUMBER_LEN);
				strncpy((char *)serialNumber->c_str(), (char*)&sn, SERIAL_NUMBER_LEN);
				byte_Swap_String((char *)serialNumber->c_str());
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_World_Wide_Name()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit world wide name values and create a string world wide name 
			//
			//  Entry:
			//! \param worldwideName - string to hold the world wide name ... putting it all together
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return wordWideName = the string wordWideName
			//
			//---------------------------------------------------------------------------
			inline void create_World_Wide_Name(std::string *worldWideName, const sDriveInfo * const idInfo)
			{
				uint64_t wwn = 0;
				uint64_t wwn1 = idInfo->worldWideName2 & 0x00FFFFFFFFFFFFFFLL;
				uint64_t wwn2 = idInfo->worldWideName & 0x00FFFFFFFFFFFFFFLL;
				word_Swap_64(&wwn1);
				word_Swap_64(&wwn2);
				wwn = (wwn2) | ((wwn1) >> 32);
				worldWideName->resize(WORLD_WIDE_NAME_LEN);
				memset((char *)worldWideName->c_str(), 0, WORLD_WIDE_NAME_LEN);
				snprintf((char *)worldWideName->c_str(), WORLD_WIDE_NAME_LEN, "0x%" PRIX64"", wwn);
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Firmware_String()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
			//
			//  Entry:
			//! \param firmwareRev - string for holding the firmware rev
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return firmwareRev = the string firmwareRev
			//
			//---------------------------------------------------------------------------
			inline void create_Firmware_String(std::string *firmwareRev, const sDriveInfo * const idInfo)
			{
				uint64_t firm = 0;
				firm = (idInfo->firmware & 0x00FFFFFFFFFFFFFFLL);
				firmwareRev->resize(FIRMWARE_REV_LEN);
				memset((char *)firmwareRev->c_str(), 0, FIRMWARE_REV_LEN);
				strncpy((char *)firmwareRev->c_str(), (char*)&firm, FIRMWARE_REV_LEN);
				byte_Swap_String((char *)firmwareRev->c_str());
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Device_Interface_String()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
			//
			//  Entry:
			//! \param dInterface - pointer to the Devie interface v, where once constructed, will hold the Devie interface of the drive
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return dInterface = the string dInterface
			//
			//---------------------------------------------------------------------------
			inline void create_Device_Interface_String(std::string *dInterface, const sDriveInfo * const idInfo)
			{
				uint64_t dFace = 0;
				dFace = (idInfo->deviceInterface & 0x00FFFFFFFFFFFFFFLL);
				byte_Swap_64(&dFace);
				dFace = (dFace >> 32);
				dInterface->resize(DEVICE_INTERFACE_LEN);
				memset((char *)dInterface->c_str(), 0, DEVICE_INTERFACE_LEN);
				strncpy((char *)dInterface->c_str(), (char*)&dFace, DEVICE_INTERFACE_LEN);
			}

        public:
            CATA_Farm_Log();
            CATA_Farm_Log( uint8_t *bufferData, size_t bufferSize);
            virtual ~CATA_Farm_Log();
            eReturnValues parse_Farm_Log();
            void print_All_Pages(JSONNODE *masterData);
            void print_Page(JSONNODE *masterData, uint32_t page);
            void print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page);
            virtual eReturnValues get_Log_Status(){ return m_status; };
            virtual void get_Serial_Number(std::string sn){ sn.assign( vFarmFrame[0].identStringInfo.serialNumber); };
            virtual void get_Firmware_String(std::string firmware){ firmware.assign(vFarmFrame[0].identStringInfo.firmwareRev); };
			virtual void get_World_Wide_Name(std::string wwn) {wwn.assign(vFarmFrame[0].identStringInfo.worldWideName);};
    };
#endif //!ATAFARM
}
  

