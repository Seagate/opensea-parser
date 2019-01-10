//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2016 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_Farm_Log.h"

using namespace opensea_parser;


//-----------------------------------------------------------------------------
//
//! \fn CATA_Farm_Log::CATA_Farm_Log()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CATA_Farm_Log::CATA_Farm_Log()
    : m_totalPages(0)
    , m_logSize(0)
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_status(IN_PROGRESS)
    , m_showStatusBits(false)
    , m_pHeader()
    , pBuf()
{

}
//-----------------------------------------------------------------------------
//
//! \fn CATA_Farm_Log::CATA_Farm_Log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//! \parma securityPrintLevel = the level of the print
//! \param bufferData = pointer to the buffer data.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CATA_Farm_Log::CATA_Farm_Log( uint8_t *bufferData, size_t bufferSize, bool showStatus)
    : m_totalPages(0)
    , m_logSize(0)
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_status(IN_PROGRESS)
    , m_showStatusBits(showStatus)
    , m_pHeader()
    , pBuf()
{
	pBuf = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef _WIN64
	memcpy(pBuf, bufferData, bufferSize);
#else
	memcpy_s(pBuf, bufferSize, bufferData, bufferSize);// copy the buffer data to the class member pBuf
#endif
    if (pBuf != NULL)
    {
        if (bufferSize < sizeof(sFarmHeader) || bufferSize < sizeof(sFarmFrame)) 
        {
        
            m_status = BAD_PARAMETER;
        }
        else
        {
            m_pHeader = (sFarmHeader *)&pBuf[0];
            m_totalPages = m_pHeader->pagesSupported & 0x00FFFFFFFFFFFFFFLL;
            m_logSize = m_pHeader->logSize & 0x00FFFFFFFFFFFFFFLL;
            m_pageSize = m_pHeader->pageSize & 0x00FFFFFFFFFFFFFFLL;
            m_heads = m_pHeader->headsSupported & 0x00FFFFFFFFFFFFFFLL;
			m_MaxHeads = m_pHeader->headsSupported & 0x00FFFFFFFFFFFFFFLL;
            m_copies = m_pHeader->copies & 0x00FFFFFFFFFFFFFFLL;
            m_status = IN_PROGRESS;
        }
    }
    else
    {
        m_status = FAILURE;
    }
}

//-----------------------------------------------------------------------------
//
//! \fn CATA_Farm_Log::~CATA_Farm_Log()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param pData  pointer to the buffer data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CATA_Farm_Log::~CATA_Farm_Log()
{
    if (!vFarmFrame.empty())
    {
        vFarmFrame.clear();                                    // clear the vector
        vFarmFrame.swap(vBlankFarmFrame);                     // Free memory with a empty vector
    }
    if (pBuf != NULL)
    {
        delete [] pBuf;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn ParseFarmLog()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::parse_Farm_Log()
{
	eReturnValues retStatus = FAILURE;
    uint64_t offset = m_pageSize;                                                 // the first page starts at 1* the page size
    if (pBuf == NULL)
    {
        return retStatus;
    }
    sFarmFrame *pFarmFrame = new sFarmFrame();                                      // create the pointer to the union
    if ((m_pHeader->signature & 0x00FFFFFFFFFFFFFFLL) == FARMSIGNATURE)                                     // check the head to see if it has the farm signature else fail
    {
        for (uint32_t index = 0; index <= m_copies; ++index)                       // loop for the number of copies. I don't think it's zero base as of now
        {
            sDriveInfo *idInfo = (sDriveInfo *)&pBuf[offset];                   // get the id drive information at the time.
            pFarmFrame->driveInfo = *idInfo;
            sStringIdentifyData strIdInfo;

            create_Serial_Number(&pFarmFrame->identStringInfo.serialNumber, idInfo);
            create_World_Wide_Name(&pFarmFrame->identStringInfo.worldWideName, idInfo);
            create_Firmware_String(&pFarmFrame->identStringInfo.firmwareRev, idInfo);
            create_Device_Interface_String(&pFarmFrame->identStringInfo.deviceInterface, idInfo);
            offset += m_pageSize;

            sWorkLoadStat *pworkLoad = (sWorkLoadStat *)&pBuf[offset];           // get the work load information
            memcpy(&pFarmFrame->workLoadPage, pworkLoad, sizeof(sWorkLoadStat));
            offset += m_pageSize;

            sErrorStat *pError = (sErrorStat *)&pBuf[offset];                    // get the error status
            memcpy(&pFarmFrame->errorPage, pError, sizeof(sErrorStat));
            offset += m_pageSize;

            sEnvironementStat *pEnvironment = (sEnvironementStat *)&pBuf[offset]; // get the envirmonent information 
            memcpy(&pFarmFrame->environmentPage, pEnvironment, sizeof(sEnvironementStat));
            offset += m_pageSize;

            sAtaReliabilityStat *pReli = (sAtaReliabilityStat *)&pBuf[offset];         // get the Reliabliity stat
            memcpy(&pFarmFrame->reliPage, pReli, sizeof(sAtaReliabilityStat));
            offset += m_pageSize;                                                  // add another page size. I think there is only on header
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
        }
		retStatus = IN_PROGRESS;
    }
	delete (pFarmFrame);
	return retStatus;
}


//-----------------------------------------------------------------------------
//
//! \fn PrintHeader()
//
//! \brief
//!   Description:  print out the header information all data is done in Json format
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Header(JSONNODE *masterData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    JSONNODE *FARMheader = json_new(JSON_NODE);
    sFarmHeader *header = (sFarmHeader *)&pBuf[0];																				// pointer to the header to get the signature

#if defined( _DEBUG)
    printf("\tLog Signature   =  0x%" PRIX64" \n", header->signature & 0x00FFFFFFFFFFFFFFLL);									//!< Log Signature = 0x00004641524D4552
    printf("\tMajor Revision =   %" PRId64"  \n", header->majorRev & 0x00FFFFFFFFFFFFFFLL);										//!< Log Major rev
    printf("\tMinor Revision =   %" PRId64"  \n", header->minorRev & 0x00FFFFFFFFFFFFFFLL);										//!< minor rev 
    printf("\tPages Supported =   %" PRId64"  \n", header->pagesSupported & 0x00FFFFFFFFFFFFFFLL);								//!< number of pages supported
    printf("\tLog Size        =   %" PRId64"  \n", header->logSize & 0x00FFFFFFFFFFFFFFLL);										//!< log size in bytes
    printf("\tPage Size       =   %" PRId64"  \n", header->pageSize & 0x00FFFFFFFFFFFFFFLL);									//!< page size in bytes
    printf("\tHeads Supported =   %" PRId64"  \n", header->headsSupported & 0x00FFFFFFFFFFFFFFLL);								//!< Maximum Drive Heads Supported
    printf("\tNumber of Copies=   %" PRId64"  \n", header->copies & 0x00FFFFFFFFFFFFF);											//!< Number of Historical Copies
#endif
    json_set_name(FARMheader, "FARM Log");
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Header");

    snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIX64"", check_Status_Strip_Status(header->signature));
    json_push_back(pageInfo, json_new_a("Log Signature", (char*)myStr.c_str()));
	json_push_back(pageInfo, json_new_i("Major Revision", static_cast<uint32_t>(check_Status_Strip_Status(header->majorRev))));
	json_push_back(pageInfo, json_new_i("Minor Revision", static_cast<uint32_t>(check_Status_Strip_Status(header->minorRev))));
	json_push_back(pageInfo, json_new_i("Pages Supported", static_cast<uint32_t>(check_Status_Strip_Status(header->pagesSupported))));
	json_push_back(pageInfo, json_new_i("Log Size", static_cast<uint32_t>(check_Status_Strip_Status(header->logSize))));
	json_push_back(pageInfo, json_new_i("Page Size", static_cast<uint32_t>(check_Status_Strip_Status(header->pageSize))));
	json_push_back(pageInfo, json_new_i("Heads Supported", static_cast<uint32_t>(check_Status_Strip_Status(header->headsSupported))));
	json_push_back(pageInfo, json_new_i("Number of Copies", static_cast<uint32_t>(check_Status_Strip_Status(header->copies))));

    json_push_back(FARMheader, pageInfo);
    json_push_back(masterData, FARMheader);
        
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintDriveInformation()
//
//! \brief
//!   Description:  print out the drive information
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Drive_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *driveInfo = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\tserial number:                            %s         \n", vFarmFrame[page].identStringInfo.serialNumber.c_str());
    printf("\tworkd wide name:                          %s         \n", vFarmFrame[page].identStringInfo.worldWideName.c_str());
    printf("\tfirmware Rev:                             %s         \n", vFarmFrame[page].identStringInfo.firmwareRev.c_str());                                                 //!< Firmware Revision [0:3]
    printf("\nDrive Information From Farm Log copy %d: \n", page);
    printf("\tDevice Interface:                         %s \n", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    printf("\tDevice Capacity in sectors:               %" PRId64" \n", vFarmFrame[page].driveInfo.deviceCapacity & 0x00FFFFFFFFFFFFFFLL);
    printf("\tPhysical Sector size:                     %" PRIX64" \n", vFarmFrame[page].driveInfo.psecSize & 0x00FFFFFFFFFFFFFFLL);                                  //!< Physical Sector Size in Bytes
    printf("\tLogical Sector Size:                      %" PRIX64" \n", vFarmFrame[page].driveInfo.lsecSize & 0x00FFFFFFFFFFFFFFLL);                                  //!< Logical Sector Size in Bytes
    printf("\tDevice Buffer Size:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.deviceBufferSize & 0x00FFFFFFFFFFFFFFLL);                          //!< Device Buffer Size in Bytes
    printf("\tNumber of heads:                          %" PRId64" \n", vFarmFrame[page].driveInfo.heads & 0x00FFFFFFFFFFFFFFLL);                                     //!< Number of Heads
    printf("\tDevice form factor:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.factor & 0x00FFFFFFFFFFFFFFLL);                                    //!< Device Form Factor (ID Word 168)                                        
    printf("\tRotation Rate:                            %" PRIu64"  \n", vFarmFrame[page].driveInfo.rotationRate & 0x00FFFFFFFFFFFFFFLL);                             //!< Rotational Rate of Device (ID Word 217)
    printf("\tATA Features Supported (ID Word 78):      0x%04" PRIX64" \n", vFarmFrame[page].driveInfo.featuresSupported & 0x00FFFFFFFFFFFFFFLL);                     //!< ATA Features Supported (ID Word 78)
    printf("\tATA Features Enabled (ID Word 79):        0x%04" PRIX64" \n", vFarmFrame[page].driveInfo.featuresEnabled & 0x00FFFFFFFFFFFFFFLL);                       //!< ATA Features Enabled (ID Word 79)
    printf("\tATA Security State (ID Word 128)          0x%04" PRIX64" \n", vFarmFrame[page].driveInfo.security & 0x00FFFFFFFFFFFFFFLL);                              //!< ATA Security State (ID Word 128)
    printf("\tPower on Hours:                           %" PRIu64" \n", vFarmFrame[page].driveInfo.poh & 0x00FFFFFFFFFFFFFFLL);                                       //!< Power-on Hour
    printf("\tSpindle Power on hours:                   %" PRIu64" \n", vFarmFrame[page].driveInfo.spoh & 0x00FFFFFFFFFFFFFFLL);                                      //!< Spindle Power-on Hours
    printf("\tHead Flight Hours:                        %" PRIu64" \n", vFarmFrame[page].driveInfo.headFlightHours & 0x00FFFFFFFFFFFFFFLL);                           //!< Head Flight Hours
    printf("\tHead Load Events:                         %" PRIu64" \n", vFarmFrame[page].driveInfo.headLoadEvents & 0x00FFFFFFFFFFFFFFLL);                            //!< Head Load Events
    printf("\tPower Cycle count:                        %" PRIu64" \n", vFarmFrame[page].driveInfo.powerCycleCount & 0x00FFFFFFFFFFFFFFLL);                           //!< Power Cycle Count
    printf("\tHardware Reset count:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.resetCount & 0x00FFFFFFFFFFFFFFLL);                                //!< Hardware Reset Count
    printf("\tSpin-up Time:                             %" PRIu64" \n", vFarmFrame[page].driveInfo.spinUpTime & 0x00FFFFFFFFFFFFFFLL);                                //!< SMART Spin-Up time in milliseconds
    printf("\tNVC Status @ power on:                    %" PRIu64" \n", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn & 0x00FFFFFFFFFFFFFFLL);                       //!< NVC Status on Power-on
    printf("\tTime Available to save:                   %" PRIu64" \n", vFarmFrame[page].driveInfo.timeAvailable & 0x00FFFFFFFFFFFFFFLL);                             //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    printf("\tTime of latest frame:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.timeStamp1 & 0x00FFFFFFFFFFFFFFLL);                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
    printf("\tTime of latest frame (milliseconds):      %" PRIu64" \n", vFarmFrame[page].driveInfo.timeStamp2 & 0x00FFFFFFFFFFFFFFLL);                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1
    printf("\tTime to ready of the last power cycle:    %" PRIu64" \n", vFarmFrame[page].driveInfo.timeToReady & 0x00FFFFFFFFFFFFFFLL);                               //!< time to ready of the last power cycle
    printf("\tTime drive is held in staggered spin:     %" PRIu64" \n", vFarmFrame[page].driveInfo.timeHeld & 0x00FFFFFFFFFFFFFFLL);                                  //!< time drive is held in staffered spin during the last power on sequence
#endif
    // check the level of print form the eToolVerbosityLevels 
    snprintf((char*)myStr.c_str(), BASIC,"Drive Information From Farm Log copy %" PRId32"", page);
    json_set_name(driveInfo, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Drive Information");
                                              
    snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.serialNumber.c_str());
    json_push_back(pageInfo, json_new_a("Serial Number", (char*)myStr.c_str()));																				//!< serial number of the device
    snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.worldWideName.c_str());
    json_push_back(pageInfo, json_new_a("World Wide Name", (char*)myStr.c_str()));																				//!< world wide Name
	snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.firmwareRev.c_str());															//!< Firmware Revision [0:3]
	json_push_back(pageInfo, json_new_a("Firmware Rev", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "%s", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    json_push_back(pageInfo, json_new_a("Device Interface", (char*)myStr.c_str()));																				//!< Device Interface
	set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame[page].driveInfo.poh, false, m_showStatusBits);											//!< Power-on Hour
	set_json_64_bit_With_Status(pageInfo, "Device Capacity in Sectors", vFarmFrame[page].driveInfo.deviceCapacity, false, m_showStatusBits);
	set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame[page].driveInfo.psecSize, false, m_showStatusBits);								//!< Physical Sector Size in Bytes
	set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame[page].driveInfo.lsecSize, false, m_showStatusBits);									//!< Logical Sector Size in Bytes
	set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame[page].driveInfo.deviceBufferSize, false, m_showStatusBits);							//!< Device Buffer Size in Bytes
	set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame[page].driveInfo.heads, false, m_showStatusBits);										//!< Number of Heads
	if (check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads) != 0)
	{
		m_heads = check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads);
	}
	set_json_64_bit_With_Status(pageInfo, "Device form factor", vFarmFrame[page].driveInfo.factor, false, m_showStatusBits);									//!< Device Form Factor (ID Word 168)
	set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame[page].driveInfo.rotationRate, false, m_showStatusBits);									//!< Rotational Rate of Device (ID Word 217)
       
	set_json_64_bit_With_Status(pageInfo, "ATA Security State (ID Word 128)", vFarmFrame[page].driveInfo.security, false, m_showStatusBits);					//!< ATA Security State (ID Word 128)
	set_json_64_bit_With_Status(pageInfo, "ATA Features Supported (ID Word 78)", vFarmFrame[page].driveInfo.featuresSupported, false, m_showStatusBits);		//!< ATA Features Supported (ID Word 78)
	set_json_64_bit_With_Status(pageInfo, "ATA Features Enabled (ID Word 79)", vFarmFrame[page].driveInfo.featuresEnabled, false, m_showStatusBits);			//!< ATA Features Enabled (ID Word 79)
        
	set_json_64_bit_With_Status(pageInfo, "Spindle Power on hours", vFarmFrame[page].driveInfo.spoh, false, m_showStatusBits);									//!< Spindle Power-on Hours
	set_json_64_bit_With_Status(pageInfo, "Head Flight Hours", vFarmFrame[page].driveInfo.headFlightHours, false, m_showStatusBits);							//!< Head Flight Hours
	set_json_64_bit_With_Status(pageInfo, "Head Load Events", vFarmFrame[page].driveInfo.headLoadEvents, false, m_showStatusBits);								//!< Head Load Events
	set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame[page].driveInfo.powerCycleCount, false, m_showStatusBits);							//!< Power Cycle Count
	set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame[page].driveInfo.resetCount, false, m_showStatusBits);								//!< Hardware Reset Count
	set_json_64_bit_With_Status(pageInfo, "Spin-up Time", vFarmFrame[page].driveInfo.spinUpTime, false, m_showStatusBits);										//!< SMART Spin-Up time in milliseconds
        
	set_json_64_bit_With_Status(pageInfo, "NVC Status @ power on", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn, false, m_showStatusBits);					//!< NVC Status on Power-on
	set_json_64_bit_With_Status(pageInfo, "Time Available to save", vFarmFrame[page].driveInfo.timeAvailable, false, m_showStatusBits);							//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
	set_json_64_bit_With_Status(pageInfo, "Time Stamp (Milliseconds) start", vFarmFrame[page].driveInfo.timeStamp1, false, m_showStatusBits);
	set_json_64_bit_With_Status(pageInfo, "Time Stamp (Milliseconds) end", vFarmFrame[page].driveInfo.timeStamp2, false, m_showStatusBits);						//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1
	set_json_64_bit_With_Status(pageInfo, "Time to ready of the last power cycle", vFarmFrame[page].driveInfo.timeToReady, false, m_showStatusBits);			//!< time to ready of the last power cycle
	set_json_64_bit_With_Status(pageInfo, "Time drive is held in staggered spin", vFarmFrame[page].driveInfo.timeHeld, false, m_showStatusBits);				//!< time drive is held in staggered spin during the last power on sequence
    json_push_back(driveInfo, pageInfo);
    json_push_back(masterData, driveInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintWorkLoad()
//
//! \brief
//!   Description:  print out the work load log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Work_Load(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *workLoad = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nWork Load From Farm Log copy %d:", page);
    printf("\tRated Workload Percentaged:               %" PRId64" \n", vFarmFrame[page].workLoadPage.workloadPercentage & 0x00FFFFFFFFFFFFFFLL);         //!< rated Workload Percentage
    printf("\tTotal Number of Other Commands:           %" PRId64" \n", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS & 0x00FFFFFFFFFFFFFFLL);     //!< Total Number Of Other Commands
    printf("\tTotal Number of Write Commands:           %" PRId64" \n", vFarmFrame[page].workLoadPage.totalWriteCommands & 0x00FFFFFFFFFFFFFFLL);         //!< Total Number of Write Commands
    printf("\tTotal Number of Read Commands:            %" PRId64" \n", vFarmFrame[page].workLoadPage.totalReadCommands & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Read Commands
    printf("\tLogical Sectors Written:                  %" PRId64" \n", vFarmFrame[page].workLoadPage.logicalSecWritten & 0x00FFFFFFFFFFFFFFLL);          //!< Logical Sectors Written
    printf("\tLogical Sectors Read:                     %" PRId64" \n", vFarmFrame[page].workLoadPage.logicalSecRead & 0x00FFFFFFFFFFFFFFLL);             //!< Logical Sectors Read
    printf("\tTotal Number of Random Read Cmds:         %" PRId64" \n", vFarmFrame[page].workLoadPage.totalRandomReads & 0x00FFFFFFFFFFFFFFLL);           //!< Total Number of Random Read Commands
    printf("\tTotal Number of Random Write Cmds:        %" PRId64" \n", vFarmFrame[page].workLoadPage.totalRandomWrites & 0x00FFFFFFFFFFFFFFLL);          //!< Total Number of Random Write Commands
#endif

    snprintf((char*)myStr.c_str(), BASIC, "Work Load From Farm Log copy %" PRId32"", page);
    json_set_name(workLoad, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Work Load");

	set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentaged", vFarmFrame[page].workLoadPage.workloadPercentage, false, m_showStatusBits);				//!< rated Workload Percentage
	set_json_64_bit_With_Status(pageInfo, "Total Number of Read Commands", vFarmFrame[page].workLoadPage.totalReadCommands, false, m_showStatusBits);			//!< Total Number of Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Write Commands", vFarmFrame[page].workLoadPage.totalWriteCommands, false, m_showStatusBits);			//!< Total Number of Write Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Random Read Cmds", vFarmFrame[page].workLoadPage.totalRandomReads, false, m_showStatusBits);			//!< Total Number of Random Read Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Random Write Cmds", vFarmFrame[page].workLoadPage.totalRandomWrites, false, m_showStatusBits);		//!< Total Number of Random Write Commands
	set_json_64_bit_With_Status(pageInfo, "Total Number of Other Commands", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS, false, m_showStatusBits);		//!< Total Number Of Other Commands
	set_json_64_bit_With_Status(pageInfo, "Logical Sectors Written", vFarmFrame[page].workLoadPage.logicalSecWritten, false, m_showStatusBits);					//!< Logical Sectors Written
	set_json_64_bit_With_Status(pageInfo, "Logical Sectors Read", vFarmFrame[page].workLoadPage.logicalSecRead, false, m_showStatusBits);						//!< Logical Sectors Read
        
    json_push_back(workLoad, pageInfo);
    json_push_back(masterData, workLoad);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintErrorInformation()
//
//! \brief
//!   Description:  print out the work load log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Error_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    uint32_t loopCount = 0;
    myStr.resize(BASIC);
    JSONNODE *errorPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nError Information Log From Farm Log copy %d:", page);                    
    printf("\tUnrecoverable Read Errors:                %" PRId64" \n", vFarmFrame[page].errorPage.totalReadECC & 0x00FFFFFFFFFFFFFFLL);              //!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors:               %" PRId64" \n", vFarmFrame[page].errorPage.totalWriteECC & 0x00FFFFFFFFFFFFFFLL);             //!< Number of Unrecoverable Write Errors
    printf("\tNumber of Reallocated Sectors:            %" PRId64" \n", vFarmFrame[page].errorPage.totalReallocations & 0x00FFFFFFFFFFFFFFLL);        //!< Number of Reallocated Sectors
    printf("\tNumber of Read Recovery Attempts:         %" PRId64" \n", vFarmFrame[page].errorPage.totalReadRecoveryAttepts & 0x00FFFFFFFFFFFFFFLL);  //!< Number of Read Recovery Attempts
    printf("\tNumber of Mechanical Start Failures:      %" PRId64" \n", vFarmFrame[page].errorPage.totalMechanicalFails & 0x00FFFFFFFFFFFFFFLL);      //!< Number of Mechanical Start Failures
    printf("\tNumber of Reallocated Candidate Sectors:  %" PRId64" \n", vFarmFrame[page].errorPage.totalReallocatedCanidates & 0x00FFFFFFFFFFFFFFLL); //!< Number of Reallocated Candidate Sectors
    printf("\tNumber of ASR Events:                     %" PRIu64" \n", vFarmFrame[page].errorPage.totalASREvents & 0x00FFFFFFFFFFFFFFLL);            //!< Number of ASR Events
    printf("\tNumber of Interface CRC Errors:           %" PRIu64" \n", vFarmFrame[page].errorPage.totalCRCErrors & 0x00FFFFFFFFFFFFFFLL);            //!< Number of Interface CRC Errors
    printf("\tSpin Retry Count:                         %" PRIu64" \n", vFarmFrame[page].errorPage.attrSpinRetryCount & 0x00FFFFFFFFFFFFFFLL);        //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
    printf("\tSpin Retry Count Normalized:              %" PRIu64" \n", vFarmFrame[page].errorPage.normalSpinRetryCount & 0x00FFFFFFFFFFFFFFLL);      //!< Spin Retry Count (SMART Attribute 10 Normalized)
    printf("\tSpin Retry Count Worst:                   %" PRIu64" \n", vFarmFrame[page].errorPage.worstSpinRretryCount & 0x00FFFFFFFFFFFFFFLL);      //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
    printf("\tNumber of IOEDC Errors (Raw):             %" PRId64" \n", vFarmFrame[page].errorPage.attrIOEDCErrors & 0x00FFFFFFFFFFFFFFLL);           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    printf("\tCTO Count Total:                          %" PRId64" \n", vFarmFrame[page].errorPage.attrCTOCount & 0x00FFFFFFFFFFFFFFLL);              //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
    printf("\tCTO Count Over 5s:                        %" PRIu64" \n", vFarmFrame[page].errorPage.overfiveSecCTO & 0x00FFFFFFFFFFFFFFLL);            //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
    printf("\tCTO Count Over 7.5s:                      %" PRIu64" \n", vFarmFrame[page].errorPage.oversevenSecCTO & 0x00FFFFFFFFFFFFFFLL);           //!< CTO Count Over 7.5s (SMART Attribute
    printf("\tTotal Flash LED (Assert) Events:          %" PRId64" \n", vFarmFrame[page].errorPage.totalFlashLED & 0x00FFFFFFFFFFFFFFLL);             //!< Total Flash LED (Assert) Events
    printf("\tIndex of the last Flash LED:              %" PRId64" \n", vFarmFrame[page].errorPage.indexFlashLED & 0x00FFFFFFFFFFFFFFLL);             //!< index of the last Flash LED of the array
    printf("\tUncorrectable errors:                   %" PRId64" \n", vFarmFrame[page].errorPage.uncorrectables & 0x00FFFFFFFFFFFFFFLL);              //!< uncorrecatables errors (sata only)
    for (loopCount = 0; loopCount < 8; ++loopCount)
    {
        printf("\tFlash LED event # %d:                     %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.flashLEDArray[loopCount] & 0x00FFFFFFFFFFFFFFLL);          //!<Info on the last 8 Flash LED events Wrapping array.
    }

#endif

    snprintf((char*)myStr.c_str(), BASIC, "Error Information Log From Farm Log copy %" PRId32"", page);
    json_set_name(errorPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);

    json_set_name(pageInfo, "Error Information");
	set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.totalReadECC , false, m_showStatusBits);						//!< Number of Unrecoverable Read Errors
	set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.totalWriteECC, false, m_showStatusBits);						//!< Number of Unrecoverable Write Errors
	set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", vFarmFrame[page].errorPage.totalReallocations, false, m_showStatusBits);				//!< Number of Reallocated Sectors
	set_json_64_bit_With_Status(pageInfo, "Number of Read Recovery Attempts", vFarmFrame[page].errorPage.totalReadRecoveryAttepts, false, m_showStatusBits);	//!< Number of Read Recovery Attempts
	set_json_64_bit_With_Status(pageInfo, "Number of Mechanical Start Failures",vFarmFrame[page].errorPage.totalMechanicalFails, false, m_showStatusBits);		//!< Number of Mechanical Start Failures
    
	set_json_int_With_Status(pageInfo, "Number of Reallocated Candidate Sectors", vFarmFrame[page].errorPage.totalReallocatedCanidates, m_showStatusBits);		//!< Number of Reallocated Candidate Sectors
	set_json_64_bit_With_Status(pageInfo, "Number of ASR Events", vFarmFrame[page].errorPage.totalASREvents, false, m_showStatusBits);							//!< Number of ASR Events
	set_json_64_bit_With_Status(pageInfo, "Number of Interface CRC Errors", vFarmFrame[page].errorPage.totalCRCErrors, false, m_showStatusBits);				//!< Number of Interface CRC Errors
	set_json_64_bit_With_Status(pageInfo, "Spin Retry Count", vFarmFrame[page].errorPage.attrSpinRetryCount, false, m_showStatusBits);							//!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
	set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Normalized", vFarmFrame[page].errorPage.normalSpinRetryCount, false, m_showStatusBits);				//!< Spin Retry Count (SMART Attribute 10 Normalized)
	set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Worst", vFarmFrame[page].errorPage.worstSpinRretryCount, false, m_showStatusBits);					//!< Spin Retry Count (SMART Attribute 10 Worst Ever)
	set_json_64_bit_With_Status(pageInfo, "Number of IOEDC Errors (Raw)", vFarmFrame[page].errorPage.attrIOEDCErrors, false, m_showStatusBits);					//!< Number of IOEDC Errors (SMART Attribute 184 Raw)
	set_json_64_bit_With_Status(pageInfo, "CTO Count Total", vFarmFrame[page].errorPage.attrCTOCount, false, m_showStatusBits);									//!< CTO Count Total (SMART Attribute 188 Raw[0..1])
	set_json_64_bit_With_Status(pageInfo, "CTO Count Over 5s", vFarmFrame[page].errorPage.overfiveSecCTO, false, m_showStatusBits);								//!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
	set_json_64_bit_With_Status(pageInfo, "CTO Count Over 7.5s", vFarmFrame[page].errorPage.oversevenSecCTO, false, m_showStatusBits);							//!< CTO Count Over 7.5s (SMART Attribute
	set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events", vFarmFrame[page].errorPage.totalFlashLED, false, m_showStatusBits);				//!< Total Flash LED (Assert) Events
    
	set_json_int_With_Status(pageInfo, "Index of the last Flash LED", vFarmFrame[page].errorPage.indexFlashLED, m_showStatusBits);								//!< index of the last Flash LED of the array          
	set_json_64_bit_With_Status(pageInfo, "Uncorrectable errors", vFarmFrame[page].errorPage.uncorrectables, false, m_showStatusBits);							//!< uncorrecatables errors (sata only)
    for (loopCount = 0; loopCount < 8; ++loopCount)
    {
        snprintf((char *)myStr.c_str(), BASIC, "Flash LED event # %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char *)myStr.c_str(), vFarmFrame[page].errorPage.flashLEDArray[loopCount], false, m_showStatusBits);		          //!<Info on the last 8 Flash LED events Wrapping array.
    }

    json_push_back(errorPage, pageInfo);
    json_push_back(masterData, errorPage);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintEnviromentInformation()
//
//! \brief
//!   Description:  print out the Envirnment log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Enviroment_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
    JSONNODE *envPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    printf("\nEnvironment Information From Farm Log copy %d:", page);

    printf("\tCurrent Temperature:                      %" PRIu64" \n", vFarmFrame[page].environmentPage.curentTemp & 0x00FFFFFFFFFFFFFFLL);       //!< Current Temperature in Celsius
    printf("\tHighest Temperature:                      %" PRIu64" \n", vFarmFrame[page].environmentPage.highestTemp & 0x00FFFFFFFFFFFFFFLL);      //!< Highest Temperature in Celsius
    printf("\tLowest Temperature:                       %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestTemp & 0x00FFFFFFFFFFFFFFLL);       //!< Lowest Temperature
    printf("\tAverage Short Term Temperature:           %" PRIu64" \n", vFarmFrame[page].environmentPage.averageTemp & 0x00FFFFFFFFFFFFFFLL);      //!< Average Short Term Temperature5
    printf("\tAverage Long Term Temperatures:           %" PRIu64" \n", vFarmFrame[page].environmentPage.averageLongTemp & 0x00FFFFFFFFFFFFFFLL);  //!< Average Long Term Temperature5
    printf("\tHighest Average Short Term Temperature:   %" PRIu64" \n", vFarmFrame[page].environmentPage.highestShortTemp & 0x00FFFFFFFFFFFFFFLL); //!< Highest Average Short Term Temperature5
    printf("\tLowest Average Short Term Temperature:    %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestShortTemp & 0x00FFFFFFFFFFFFFFLL);  //!< Lowest Average Short Term Temperature5
    printf("\tHighest Average Long Term Temperature:    %" PRIu64" \n", vFarmFrame[page].environmentPage.highestLongTemp & 0x00FFFFFFFFFFFFFFLL);  //!< Highest Average Long Term Temperature5
    printf("\tLowest Average Long Term Temperature:     %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestLongTemp & 0x00FFFFFFFFFFFFFFLL);   //!< Lowest Average Long Term Temperature5
    printf("\tTime In Over Temperature:                 %" PRIu64" \n", vFarmFrame[page].environmentPage.overTempTime & 0x00FFFFFFFFFFFFFFLL);     //!< Time In Over Temperature5
    printf("\tTime In Under Temperature:                %" PRIu64" \n", vFarmFrame[page].environmentPage.underTempTime & 0x00FFFFFFFFFFFFFFLL);    //!< Time In Under Temperature5
    printf("\tSpecified Max Operating Temperature:      %" PRIu64" \n", vFarmFrame[page].environmentPage.maxTemp & 0x00FFFFFFFFFFFFFFLL);          //!< Specified Max Operating Temperature
    printf("\tSpecified Min Operating Temperature:      %" PRIu64" \n", vFarmFrame[page].environmentPage.minTemp & 0x00FFFFFFFFFFFFFFLL);          //!< Specified Min Operating Temperature
    printf("\tOver-Limit Shock Events Count(Raw):       %" PRIu64" \n", vFarmFrame[page].environmentPage.shockEvents & 0x00FFFFFFFFFFFFFFLL);      //!< Over-Limit Shock Events Count(SMART Attribute 191 Raw)
    printf("\tHigh Fly Write Count (Raw):               %" PRIu64" \n", vFarmFrame[page].environmentPage.hfWriteCounts & 0x00FFFFFFFFFFFFFFLL);    //!< High Fly Write Count (SMART Attribute 189 Raw)
    printf("\tCurrent Relative Humidity:                %" PRIu64" \n", vFarmFrame[page].environmentPage.humidity & 0x00FFFFFFFFFFFFFFLL);         //!< Current Relative Humidity (in units of .1%)
    printf("\tHumidity Mixed Ratio:                     %" PRIu64" \n", ((vFarmFrame[page].environmentPage.humidityRatio & 0x00FFFFFFFFFFFFFFLL) / 8)); //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    printf("\tCurrent Motor Power:                      %" PRIu64" \n", vFarmFrame[page].environmentPage.currentMotorPower & 0x00FFFFFFFFFFFFFFLL); //!< Current Motor Power, value from most recent SMART Summary Frame6
#endif
    snprintf((char*)myStr.c_str(), BASIC, "Environment Information From Farm Log copy %" PRId32"", page);
    json_set_name(envPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Environment");
								
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f",(check_Status_Strip_Status(vFarmFrame[page].environmentPage.curentTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Current Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.curentTemp,m_showStatusBits);								//!< Current Temperature in Celsius
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Highest Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.highestTemp, m_showStatusBits);								//!< Highest Temperature in Celsius
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Lowest Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.lowestTemp, m_showStatusBits);								//!< Lowest Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.averageTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Average Short Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.averageTemp, m_showStatusBits);					//!< Average Short Term Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.averageLongTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Average Long Term Temperatures", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.averageLongTemp, m_showStatusBits);				//!< Average Long Term Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestShortTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Highest Average Short Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.highestShortTemp, m_showStatusBits);		//!< Highest Average Short Term Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestShortTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Lowest Average Short Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.lowestShortTemp, m_showStatusBits);		//!< Lowest Average Short Term Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestLongTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Highest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.highestLongTemp, m_showStatusBits);		//!< Highest Average Long Term Temperature
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestLongTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Lowest Average Long Term Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.lowestLongTemp,m_showStatusBits);			//!< Lowest Average Long Term Temperature
	
	set_json_int_With_Status(pageInfo, "Time In Over Temperature", vFarmFrame[page].environmentPage.overTempTime, m_showStatusBits);												//!< Time In Over Temperature
	set_json_int_With_Status(pageInfo, "Time In Under Temperature", vFarmFrame[page].environmentPage.underTempTime, m_showStatusBits);												//!< Time In Under Temperature
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.maxTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Specified Max Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.maxTemp, m_showStatusBits);					//!< Specified Max Operating Temperature
    
	snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.minTemp))*1.00);
	set_json_string_With_Status(pageInfo, "Specified Min Operating Temperature", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.minTemp, m_showStatusBits);					//!< Specified Min Operating Temperature
	
	set_json_int_With_Status(pageInfo, "Over-Limit Shock Events Count(Raw)", vFarmFrame[page].environmentPage.shockEvents, m_showStatusBits);										//!< Over-Limit Shock Events Count
	set_json_int_With_Status(pageInfo, "High Fly Write Count (Raw)", vFarmFrame[page].environmentPage.hfWriteCounts, m_showStatusBits);												//!< High Fly Write Count 
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidity))*0.1);													//!< Current Relative Humidity (in units of .1%)
	set_json_string_With_Status(pageInfo, "Current Relative Humidity", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidity, m_showStatusBits);
    snprintf((char*)myStr.c_str(), BASIC, "%0.02f", (check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidityRatio) / 8.0));												//!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
	set_json_string_With_Status(pageInfo, "Humidity Mixed Ratio", (char*)myStr.c_str(), vFarmFrame[page].environmentPage.humidityRatio, m_showStatusBits);
	set_json_int_With_Status(pageInfo, "Current Motor Power", vFarmFrame[page].environmentPage.currentMotorPower, m_showStatusBits);												//!< Current Motor Power, value from most recent SMART Summary Frame6

    json_push_back(envPage, pageInfo);
    json_push_back(masterData, envPage);

return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn PrintReliInformation()
//
//! \brief
//!   Description:  print out the Reli log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Reli_Information(JSONNODE *masterData, uint32_t page)
{
    uint32_t loopCount = 0;
    std::string myStr = " ";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);
    JSONNODE *reliPage = json_new(JSON_NODE);

#if defined( _DEBUG)
    int16_t whole = 0;
    uint32_t remander = 0;
    printf("\nReliability Information From Farm Log copy: %d\n", page);
    printf("\tTimestamp of last IDD test:               %" PRIu64" \n", vFarmFrame[page].reliPage.lastIDDTest & 0x00FFFFFFFFFFFFFFLL);                        //!< Timestamp of last IDD test
    printf("\tSub-command of last IDD test:             %" PRIu64" \n", vFarmFrame[page].reliPage.cmdLastIDDTest & 0x00FFFFFFFFFFFFFFLL);                     //!< Sub-command of last IDD test
    for (loopCount = 0;loopCount < m_heads; ++loopCount)
    {
        whole = vFarmFrame[page].reliPage.discSlip[loopCount].wholePartofFloat;
        remander = vFarmFrame[page].reliPage.discSlip[loopCount].decimalPartofFloat;
        remander = remander / 10000;
        printf("\tDisc Slip in micro-inches by Head %d:      %f \n", loopCount, (std::abs((double)whole + (double)remander)));  //!< Disc Slip in micro-inches by Head
		
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
		double myWhole = 0;
		double myRemander = 0;
		myWhole = (double) std::abs(vFarmFrame[page].reliPage.bitErrorRate[loopCount].wholePartofFloat);
		myRemander = (double) vFarmFrame[page].reliPage.bitErrorRate[loopCount].decimalPartofFloat;
		myRemander = myRemander / 10000;
        printf("\tBit Error Rate of Zone 0 by Head %d:      %f\n", loopCount, (std::abs((double)vFarmFrame[page].reliPage.bitErrorRate[loopCount].wholePartofFloat + ((double)vFarmFrame[page].reliPage.bitErrorRate[loopCount].decimalPartofFloat) / 10000)) * -1);  //!< Bit Error Rate of Zone 0 by Drive Head
    }
    printf( "\tNumber of G-List Reclamations:            %" PRIu64" \n", vFarmFrame[page].reliPage.gListReclamed & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of G-List Reclamations 
    printf( "\tServo Status:                             %" PRIu64" \n", vFarmFrame[page].reliPage.servoStatus & 0x00FFFFFFFFFFFFFFLL);                     //!< Servo Status (follows standard DST error code definitions)
    printf( "\tAlts List Entries Before IDD Scan:        %" PRIu64" \n", vFarmFrame[page].reliPage.altsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Alt List Entries Before IDD Scan
    printf( "\tAltz List Entries After IDD Scan:         %" PRIu64" \n", vFarmFrame[page].reliPage.altsAfterIDD & 0x00FFFFFFFFFFFFFFLL);                    //!< Number of Alt List Entries After IDD Scan
    printf( "\tResident G-List Entries Before IDD Scan:  %" PRIu64" \n", vFarmFrame[page].reliPage.gListBeforIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Resident G-List Entries Before IDD Scan
    printf( "\tResident G-List Entries After IDD Scan:   %" PRIu64" \n", vFarmFrame[page].reliPage.gListAfterIDD & 0x00FFFFFFFFFFFFFFLL);                   //!< Number of Resident G-List Entries After IDD Scan
    printf( "\tScrubs List Entries Before IDD Scan:      %" PRIu64" \n", vFarmFrame[page].reliPage.scrubsBeforeIDD & 0x00FFFFFFFFFFFFFFLL);                 //!< Number of Scrub List Entries Before IDD Scan
    printf( "\tScrubs List Entries After IDD Scan:       %" PRIu64" \n", vFarmFrame[page].reliPage.scrubsAfterIDD & 0x00FFFFFFFFFFFFFFLL);                  //!< Number of Scrub List Entries After IDD Scan
    printf( "\tNumber of DOS Scans Performed:            %" PRIu64" \n", vFarmFrame[page].reliPage.numberDOSScans & 0x00FFFFFFFFFFFFFFLL);                  //!< Number of DOS Scans Performed
    printf( "\tNumber of LBAs Corrected by ISP:          %" PRIu64" \n", vFarmFrame[page].reliPage.numberLBACorrect & 0x00FFFFFFFFFFFFFFLL);                //!< Number of LBAs Corrected by ISP
    printf( "\tNumber of Valid Parity Sectors:           %" PRIu64" \n", vFarmFrame[page].reliPage.numberValidParitySec & 0x00FFFFFFFFFFFFFFLL);            //!< Number of Valid Parity Sectors
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tDOS Write Refresh Count Head %d:          %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.dosWriteCount[loopCount] & 0x00FFFFFFFFFFFFFFLL);          //!< [24] DOS Write Refresh Count7
    }
    printf( "\tNumber of RAW Operations:                  %" PRIu64" \n", vFarmFrame[page].reliPage.numberRAWops & 0x00FFFFFFFFFFFFFFLL);           //!< Number of RAW Operations
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tDVGA Skip Write Detect by Head %d:        %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.DVGASkipWriteDetect[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< [24] DVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tRVGA Skip Write Detect by Head %d:         %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.RVGASkipWriteDetect[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< [24] RVGA Skip Write Detect by Head7
    }

    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tFVGA Skip Write Detect by Head %d:        %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.FVGASkipWriteDetect[loopCount] & 0x00FFFFFFFFFFFFFFLL);   //!< [24] FVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tSkip Write Detect Threshold Exceeded Head %d:  %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.skipWriteDetectThresExceeded[loopCount] & 0x00FFFFFFFFFFFFFFLL);  //!< [24] Skip Write Detect Threshold Exceeded Count by Head7
    }
    printf( "\tError Rate (SMART Attribute 1 Raw):       %" PRIu64" \n", vFarmFrame[page].reliPage.attrErrorRateRaw & 0x00FFFFFFFFFFFFFFLL);                //!< Error Rate (SMART Attribute 1 Raw)
    printf( "\tError Rate (SMART Attribute 1 Normalized):%" PRIu64" \n", vFarmFrame[page].reliPage.attrErrorRateNormal & 0x00FFFFFFFFFFFFFFLL);             //!< Error Rate (SMART Attribute 1 Normalized)
    printf( "\tError Rate (SMART Attribute 1 Worst):     %" PRIu64" \n", vFarmFrame[page].reliPage.attrErrorRateWorst & 0x00FFFFFFFFFFFFFFLL);              //!< Error Rate (SMART Attribute 1 Worst)
    printf( "\tSeek Error Rate (SMART Attr 7 Raw):       %" PRIu64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateRaw & 0x00FFFFFFFFFFFFFFLL);            //!< Seek Error Rate (SMART Attribute 7 Raw)
    printf( "\tSeek Error Rate (SMART Attr 7 Normalized):%" PRIu64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateNormal & 0x00FFFFFFFFFFFFFFLL);         //!< Seek Error Rate (SMART Attribute 7 Normalized)
    printf( "\tSeek Error Rate (SMART Attr 7 Worst):     %" PRIu64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateWorst & 0x00FFFFFFFFFFFFFFLL);          //!< Seek Error Rate (SMART Attribute 7 Worst)
    printf( "\tHigh Priority Unload Events (Raw):        %" PRIu64" \n", vFarmFrame[page].reliPage.attrUnloadEventsRaw & 0x00FFFFFFFFFFFFFFLL);             //!< High Priority Unload Events (SMART Attribute 192 Raw)
    printf("\tMicro Actuator Lock-out accumulated:      %" PRIu64" \n", vFarmFrame[page].reliPage.microActuatorLockOUt & 0x00FFFFFFFFFFFFFFLL);             //!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tACFF Sine 1X,  Head %d:                   %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.sineACFF[loopCount] & 0x00FFFFFFFFFFFFFFLL);          //!< [24] ACFF Sine 1X, value from most recent SMART Summary Frame by Head7,8
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tACFF Cosine 1X, Head %d:                  %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.cosineACFF[loopCount] & 0x00FFFFFFFFFFFFFFLL);     //!< [24] ACFF Cosine 1X, value from most recent SMART Summary Frame by Head7,8
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tPZT Calibration, Head %d:                 %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.PZTCalibration[loopCount] & 0x00FFFFFFFFFFFFFFLL);      //!< [24] PZT Calibration, value from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tMR Head Resistance from Head %d:          %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.MRHeadResistance[loopCount] & 0x00FFFFFFFFFFFFFFLL);       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tNumber of TMD for Head %d:                %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.numberOfTMD[loopCount] & 0x00FFFFFFFFFFFFFFLL);               //!< [24] Number of TMD over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tVelocity Observer by Head %d:             %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.velocityObserver[loopCount] & 0x00FFFFFFFFFFFFFFLL);         //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tNumber of Velocity Observer by Head %d:   %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.numberOfVelocityObserver[loopCount] & 0x00FFFFFFFFFFFFFFLL);          //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf( "\tFly height clearance delta by Head  %d:   %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner & 0x00FFFFFFFFFFFFFFLL);                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        printf( "\tFly height clearance delta by Head  %d:   %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle & 0x00FFFFFFFFFFFFFFLL);               //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        printf( "\tFly height clearance delta by Head  %d:   %" PRIu64" \n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer & 0x00FFFFFFFFFFFFFFLL);                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    printf("\tNumber of disc slip recalibrations performed:  %" PRId64" \n", vFarmFrame[page].reliPage.diskSlipRecalPerformed & 0x00FFFFFFFFFFFFFFLL);                  //!< Number of disc slip recalibrations performed
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tNumber of Resident G-List by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.gList[loopCount] & 0x00FFFFFFFFFFFFFFLL);                //!< [24] Number of Resident G-List per Head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tNumber of pending Entrie by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.pendingEntries[loopCount] & 0x00FFFFFFFFFFFFFFLL);        //!< [24] Number of pending Entries per Head
    }
    printf("\tHelium Pressure Threshold Tripped:             %" PRId64" \n", vFarmFrame[page].reliPage.heliumPresureTrip & 0x00FFFFFFFFFFFFFFLL);                       //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS Ought to scan count by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.oughtDOS[loopCount] & 0x00FFFFFFFFFFFFFFLL);               //!< [24] DOS Ought to scan count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS needs to scans count by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.needDOS[loopCount] & 0x00FFFFFFFFFFFFFFLL);               //!< [24] DOS needs to scans count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS write Fault scans by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.writeDOSFault[loopCount] & 0x00FFFFFFFFFFFFFFLL);            //!< [24] DOS  write Fault scans per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\twrite POS in sec value by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.writePOH[loopCount] & 0x00FFFFFFFFFFFFFFLL);				//!< [24] write POS in sec value from most recent SMART Frame by head
    }
    printf("\tRV Absolute Mean:                              %" PRIu64" \n", vFarmFrame[page].reliPage.RVAbsoluteMean & 0x00FFFFFFFFFFFFFFLL);							//!< RV Absolute Mean, value from the most recent SMART Frame
    printf("\tMax RV Absolute Meane:                         %" PRIu64" \n", vFarmFrame[page].reliPage.maxRVAbsluteMean & 0x00FFFFFFFFFFFFFFLL);						//!< Max RV Absolute Mean, value from the most recent SMART Summary Frame
    printf("\tIdle Time:                                     %" PRIu64" \n", vFarmFrame[page].reliPage.idleTime & 0x00FFFFFFFFFFFFFFLL);								//!< idle Time, Value from most recent SMART Summary Frame
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS Write Count Threshold by Head  %d:  %" PRId64" \n", loopCount, vFarmFrame[page].reliPage.countDOSWrite[loopCount] & 0x00FFFFFFFFFFFFFFLL);		//!< [24] DOS Write Count Threshold per head
    }
#endif

    snprintf((char*)myStr.c_str(), BASIC, "Reliability Information From Farm Log copy: %" PRId32"", page);
    json_set_name(reliPage, (char*)myStr.c_str());
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Reliability");
            
	set_json_64_bit_With_Status(pageInfo, "Timestamp of last IDD test", vFarmFrame[page].reliPage.lastIDDTest, false, m_showStatusBits);								//!< Timestamp of last IDD test
	set_json_64_bit_With_Status(pageInfo, "Sub-command of last IDD test", vFarmFrame[page].reliPage.cmdLastIDDTest, false, m_showStatusBits);							//!< Sub-command of last IDD test
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Disc Slip in micro-inches by Head %d", loopCount); // Head count
        snprintf((char*)myStr.c_str(), BASIC, "%0.04f", (std::abs((double)check_Status_Strip_Status(vFarmFrame[page].reliPage.discSlip[loopCount].wholePartofFloat)) + ((double)vFarmFrame[page].reliPage.discSlip[loopCount].decimalPartofFloat) / 10000)); //!< Disc Slip in micro-inches by Head
		set_json_string_With_Status(pageInfo, (char*)myHeader.c_str(), (char*)myStr.c_str(), (uint64_t)vFarmFrame[page].reliPage.discSlip[loopCount].status << 56 , m_showStatusBits);
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Bit Error Rate of Zone 0 by Head %d", loopCount); // Head count
		snprintf((char*)myStr.c_str(), BASIC, "%0.04f",((std::abs((double)vFarmFrame[page].reliPage.bitErrorRate[loopCount].wholePartofFloat) + ((double)vFarmFrame[page].reliPage.bitErrorRate[loopCount].decimalPartofFloat) / 10000))  * -1);
		set_json_string_With_Status(pageInfo, (char*)myHeader.c_str(), (char*)myStr.c_str(), (uint64_t)vFarmFrame[page].reliPage.bitErrorRate[loopCount].status << 56, m_showStatusBits);
    }
	set_json_64_bit_With_Status(pageInfo, "Number of G-List Reclamations", vFarmFrame[page].reliPage.gListReclamed, false, m_showStatusBits);							//!< Number of G-List Reclamations 
	set_json_64_bit_With_Status(pageInfo, "Servo Status", vFarmFrame[page].reliPage.servoStatus, false, m_showStatusBits);												//!< Servo Status (follows standard DST error code definitions)
	set_json_64_bit_With_Status(pageInfo, "Alts List Entries Before IDD Scan", vFarmFrame[page].reliPage.altsBeforeIDD, false, m_showStatusBits);						//!< Number of Alt List Entries Before IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Altz List Entries After IDD Scan", vFarmFrame[page].reliPage.altsAfterIDD, false, m_showStatusBits);							//!< Number of Alt List Entries After IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Resident G-List Entries Before IDD Scan", vFarmFrame[page].reliPage.gListBeforIDD, false, m_showStatusBits);					//!< Number of Resident G-List Entries Before IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Resident G-List Entries After IDD Scan", vFarmFrame[page].reliPage.gListAfterIDD, false, m_showStatusBits);					//!< Number of Resident G-List Entries After IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Scrubs List Entries Before IDD Scan", vFarmFrame[page].reliPage.scrubsBeforeIDD, false, m_showStatusBits);					//!< Number of Scrub List Entries Before IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Scrubs List Entries After IDD Scan", vFarmFrame[page].reliPage.scrubsAfterIDD, false, m_showStatusBits);						//!< Number of Scrub List Entries After IDD Scan
	set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", vFarmFrame[page].reliPage.numberDOSScans, false, m_showStatusBits);							//!< Number of DOS Scans Performed
	set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", vFarmFrame[page].reliPage.numberLBACorrect, false, m_showStatusBits);						//!< Number of LBAs Corrected by ISP
	set_json_64_bit_With_Status(pageInfo, "Number of Valid Parity Sectors", vFarmFrame[page].reliPage.numberValidParitySec, false, m_showStatusBits);					//!< Number of Valid Parity Sector
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DOS Write Refresh Count Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.dosWriteCount[loopCount], false, m_showStatusBits);					//!< [24] DOS Write Refresh Count7
    }
	set_json_64_bit_With_Status(pageInfo, "Number of RAW Operations", vFarmFrame[page].reliPage.numberRAWops, false, m_showStatusBits);									//!< Number of RAW Operations
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DVGA Skip Write Detect by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.DVGASkipWriteDetect[loopCount], false, m_showStatusBits);				//!< [24] DVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "RVGA Skip Write Detect by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.RVGASkipWriteDetect[loopCount], false, m_showStatusBits);				//!< [24] RVGA Skip Write Detect by Head7
    }

    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "FVGA Skip Write Detect by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.FVGASkipWriteDetect[loopCount], false, m_showStatusBits);				//!< [24] FVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Skip Write Detect Threshold Exceeded Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.skipWriteDetectThresExceeded[loopCount], false, m_showStatusBits);    //!< [24] Skip Write Detect Threshold Exceeded Count by Head7
    }
	set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Raw)", vFarmFrame[page].reliPage.attrErrorRateRaw, false, m_showStatusBits);					//!< Error Rate (SMART Attribute 1 Raw)
	set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Normalized)", vFarmFrame[page].reliPage.attrErrorRateNormal, false, m_showStatusBits);			//!< Error Rate (SMART Attribute 1 Normalized)
	set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Worst)", vFarmFrame[page].reliPage.attrErrorRateWorst, false, m_showStatusBits);				//!< Error Rate (SMART Attribute 1 Worst)
	set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Raw)", vFarmFrame[page].reliPage.attrSeekErrorRateRaw, false, m_showStatusBits);				//!< Seek Error Rate (SMART Attribute 7 Raw)
	set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Normalized)", vFarmFrame[page].reliPage.attrSeekErrorRateNormal, false, m_showStatusBits);     //!< Seek Error Rate (SMART Attribute 7 Normalized)
	set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Worst)", vFarmFrame[page].reliPage.attrSeekErrorRateWorst, false, m_showStatusBits);			//!< Seek Error Rate (SMART Attribute 7 Worst)
	set_json_64_bit_With_Status(pageInfo, "High Priority Unload Events (Raw)", vFarmFrame[page].reliPage.attrUnloadEventsRaw, false, m_showStatusBits);					//!< High Priority Unload Events (SMART Attribute 192 Raw)
	set_json_64_bit_With_Status(pageInfo, "Micro Actuator Lock-out accumulated", vFarmFrame[page].reliPage.microActuatorLockOUt, false, m_showStatusBits);				//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
        
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "ACFF Sine 1X for Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.sineACFF[loopCount], false, m_showStatusBits);								//!< [24] ACFF Sine 1X, value from most recent SMART Summary Frame by Head7,8
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "ACFF Cosine 1X for Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.cosineACFF[loopCount], false, m_showStatusBits);							//!< [24] ACFF Cosine 1X, value from most recent SMART Summary Frame by Head7,8
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "PZT Calibration for Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.PZTCalibration[loopCount], false, m_showStatusBits);						//!< [24] PZT Calibration, value from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "MR Head Resistance from Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.MRHeadResistance[loopCount], false, m_showStatusBits);						//!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Number of TMD for Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.numberOfTMD[loopCount], false, m_showStatusBits);							//!< [24] Number of TMD over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Velocity Observer by Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.velocityObserver[loopCount], false, m_showStatusBits);						//!< [24] Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Number of Velocity Observer by Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.numberOfVelocityObserver[loopCount], false, m_showStatusBits);				//!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }                     
       
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Fly height clearance delta by Head %" PRId32" inner", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner, false, m_showStatusBits);				//!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Fly height clearance delta by Head %" PRId32" middle", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle, false, m_showStatusBits);            //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Fly height clearance delta by Head %" PRId32" outer", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer, false, m_showStatusBits);				//!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
	set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame[page].reliPage.diskSlipRecalPerformed, false, m_showStatusBits);       //!< Number of disc slip recalibrations performed
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error by Head %" PRId32" inner", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SAT[loopCount].inner, false, m_showStatusBits);                   //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 9,
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error by Head %" PRId32" middle", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SAT[loopCount].middle, false, m_showStatusBits);                  //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 9,
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT trimmed mean bits in error by Head %" PRId32" outer", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SAT[loopCount].outer, false, m_showStatusBits);                  //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 9,
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge by Head %" PRId32" inner", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].inner, false, m_showStatusBits);        //!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 9, 11
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge by Head %" PRId32" middle", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].middle, false, m_showStatusBits);		//!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 9, 11
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Current H2SAT iterations to converge by Head %" PRId32" outer", loopCount); // Head count
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].outer, false, m_showStatusBits);			//!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 9, 11
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Current H2SAT percentage of codewords at iteration level by Head  %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATPercentage[loopCount], false, m_showStatusBits);				//!< [24] Qword[24] Current H2SAT percentage of codewords at iteration level by Head, averaged
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Current H2SAT amplitude by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATamplitude[loopCount], false, m_showStatusBits);				//!< [24] Qword[24] Current H2SAT amplitude by Head, averaged across Test Zones 9
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Current H2SAT asymmetry by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.currentH2SATasymmetry[loopCount], false, m_showStatusBits);             //!< [24] Qword[24] Current H2SAT asymmetry by Head, averaged across Test Zones
    }

	set_json_64_bit_With_Status(pageInfo, "Number of disc slip recalibrations performed", vFarmFrame[page].reliPage.diskSlipRecalPerformed, false, m_showStatusBits);    //!< Number of disc slip recalibrations performed
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Number of Resident G-List by Head %" PRId32"", loopCount); 
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.gList[loopCount], false, m_showStatusBits);							//!< [24] Number of Resident G-List per Head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Number of pending Entries by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.pendingEntries[loopCount], false, m_showStatusBits);                   //!< [24] Number of pending Entries per Head
    }
	set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame[page].reliPage.heliumPresureTrip, false, m_showStatusBits);                   //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DOS Ought to scan count by Head %" PRId32"", loopCount); 
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.oughtDOS[loopCount], false, m_showStatusBits);							//!< [24] DOS Ought to scan count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DOS needs to scans count by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.needDOS[loopCount], false, m_showStatusBits);							//!< [24] DOS needs to scans count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DOS write Fault scans by Head %" PRId32"", loopCount); 
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.writeDOSFault[loopCount], false, m_showStatusBits);					//!< [24] DOS  write Fault scans per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "Write POS in sec value by Head %" PRId32"", loopCount);
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.writePOH[loopCount], false, m_showStatusBits);                         //!< [24] write POS in sec value from most recent SMART Frame by head
    }
	set_json_64_bit_With_Status(pageInfo, "RV Absolute Mean", vFarmFrame[page].reliPage.RVAbsoluteMean, false, m_showStatusBits);										//!< RV Absolute Mean, value from the most recent SMART Frame
	set_json_64_bit_With_Status(pageInfo, "Max RV Absolute Mean", vFarmFrame[page].reliPage.maxRVAbsluteMean, false, m_showStatusBits);									//!< Max RV Absolute Mean, value from the most recent SMART Summary Frame
	set_json_64_bit_With_Status(pageInfo, "Idle Time", vFarmFrame[page].reliPage.idleTime, false, m_showStatusBits);													//!< idle Time, Value from most recent SMART Summary Frame
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        snprintf((char *)myHeader.c_str(), BASIC, "DOS Write Count Threshold by Head %" PRId32"", loopCount); 
		set_json_64_bit_With_Status(pageInfo, (char*)myHeader.c_str(), vFarmFrame[page].reliPage.countDOSWrite[loopCount], false, m_showStatusBits);					//!< [24] DOS Write Count Threshold per head
    }
        
    json_push_back(reliPage, pageInfo);
    json_push_back(masterData, reliPage);
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn PrintAllPages()
//
//! \brief
//!   Description:  print out all the pages and the copies of each page
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CATA_Farm_Log::print_All_Pages(JSONNODE *masterData)
{
    print_Header(masterData);
    for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
    {
// #if defined( _DEBUG)
        print_Drive_Information(masterData,index);
        print_Work_Load(masterData,index);
// #endif
        print_Error_Information(masterData, index);
        print_Enviroment_Information(masterData, index); 
        print_Reli_Information(masterData, index);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn PrintPage()
//
//! \brief
//!   Description:  print out a copy of a the pages
//
//  Entry:
//! \param page  = the page copy number of the data we want to print.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CATA_Farm_Log::print_Page(JSONNODE *masterData, uint32_t page)
{
    if (page <= vFarmFrame.size())
    {
        print_Drive_Information(masterData,page);
        print_Work_Load(masterData, page);
        print_Error_Information(masterData, page);
        print_Enviroment_Information(masterData, page);
        print_Reli_Information(masterData, page);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn PrintPageWithoutDriveInfo()
//
//! \brief
//!   Description:  print out a copy of a the pages without the drive info page.
//
//  Entry:
//! \param page  = the page copy number of the data we want to print.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CATA_Farm_Log::print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page)
{
    if (page <= vFarmFrame.size())
    {
        print_Work_Load(masterData, page);
        print_Error_Information(masterData, page);
        print_Enviroment_Information(masterData, page);
        print_Reli_Information(masterData, page);
    }
}