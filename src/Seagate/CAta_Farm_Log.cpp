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

#include "CAta_Farm_Log.h"

using namespace opensea_parser;

#define DATE_SIZE           4

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
    , m_MajorRev(0)
    , m_MinorRev(0)
    , m_FrameReason(0)
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
CATA_Farm_Log::CATA_Farm_Log(uint8_t *bufferData, size_t bufferSize, bool showStatus)
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
    , m_MajorRev(0)
    , m_MinorRev(0)
    , m_FrameReason(0)
{
    pBuf = new uint8_t[bufferSize];                             // new a buffer to the point                
#ifndef __STDC_SECURE_LIB__
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
            m_pHeader = reinterpret_cast<sFarmHeader *>(&pBuf[0]);
            m_totalPages = m_pHeader->pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_logSize = m_pHeader->logSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_pageSize = m_pHeader->pageSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_heads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_MaxHeads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_copies = m_pHeader->copies & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_status = IN_PROGRESS;
            m_MajorRev = M_DoubleWord0(m_pHeader->majorRev);
            m_MinorRev = M_DoubleWord0(m_pHeader->minorRev);
            m_FrameReason = M_Byte0(m_pHeader->reasonForFrameCapture);
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
        delete[] pBuf;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn Check_Page_number()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param page - the number that the page should be
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
bool CATA_Farm_Log::Check_Page_number(uint64_t page, uint16_t pageNumber)
{
    if (pageNumber == M_Word0(page))
        return true;
    else
    {
        if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("\n page number did not match pointer information.\n");
        }
        return false;
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
    uint64_t signature = m_pHeader->signature & UINT64_C(0x00FFFFFFFFFFFFFF);
    // TODO:   Add in a check for time series that has all FFFF's even for the signature - show as empty 
    if (signature == FARMSIGNATURE || signature == FACTORYCOPY)                                     // check the head to see if it has the farm signature else fail
    {
        retStatus = IN_PROGRESS;
        for (uint32_t index = 0; index <= m_copies; ++index)                       // loop for the number of copies. I don't think it's zero base as of now
        {
            sDriveInfo *idInfo = reinterpret_cast<sDriveInfo*>(&pBuf[offset]);                   // get the id drive information at the time.
            pFarmFrame->driveInfo = *idInfo;
            sStringIdentifyData strIdInfo;

            create_Serial_Number(pFarmFrame->identStringInfo.serialNumber, M_DoubleWord0(idInfo->serialNumber), M_DoubleWord0(idInfo->serialNumber2), m_MajorRev, false);
            create_World_Wide_Name(pFarmFrame->identStringInfo.worldWideName, idInfo->worldWideName2, idInfo->worldWideName,false);
            create_Firmware_String(pFarmFrame->identStringInfo.firmwareRev, M_DoubleWord0(idInfo->firmware), M_DoubleWord0(idInfo->firmwareRev),false);
            create_Device_Interface_String(pFarmFrame->identStringInfo.deviceInterface, M_DoubleWord0(idInfo->deviceInterface),false);
            if (m_MajorRev >= MAJORVERSION3)                    // must be higher then version 3.0 for the model number
            {
                create_Model_Number_String(pFarmFrame->identStringInfo.modelNumber, idInfo->modelNumber,false);
            }
            else
            {
                pFarmFrame->identStringInfo.modelNumber = "ST12345678";
            }
            if (!Check_Page_number(idInfo->pageNumber, 1))
            {
                retStatus = VALIDATION_FAILURE;
            }

            offset += m_pageSize;

            sWorkLoadStat *pworkLoad = reinterpret_cast<sWorkLoadStat*>(&pBuf[offset]);           // get the work load information
            memcpy(&pFarmFrame->workLoadPage, pworkLoad, sizeof(sWorkLoadStat));
            if (!Check_Page_number(pworkLoad->pageNumber, 2))
            {
                retStatus = VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sErrorStat *pError = reinterpret_cast<sErrorStat*>(&pBuf[offset]);                    // get the error status
            memcpy(&pFarmFrame->errorPage, pError, sizeof(sErrorStat));
            if (!Check_Page_number(pError->pageNumber, 3))
            {
                retStatus = VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sEnvironementStat *pEnvironment = reinterpret_cast<sEnvironementStat*>(&pBuf[offset]); // get the envirmonent information 
            memcpy(&pFarmFrame->environmentPage, pEnvironment, sizeof(sEnvironementStat));
            if (!Check_Page_number(pEnvironment->pageNumber, 4))
            {
                retStatus = VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sAtaReliabilityStat *pReli = reinterpret_cast<sAtaReliabilityStat*>(&pBuf[offset]);         // get the Reliabliity stat
            memcpy(&pFarmFrame->reliPage, pReli, sizeof(sAtaReliabilityStat));
            if (!Check_Page_number(pReli->pageNumber, 5))
            {
                retStatus = VALIDATION_FAILURE;
            }
            offset += m_pageSize;                                                  // add another page size. I think there is only on header
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
        }
        
    }
    if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)                                     // checking for an empty log aka all FFFF's
    {
        retStatus = IN_PROGRESS;
    }
    delete (pFarmFrame);
    return retStatus;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Header()
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
    JSONNODE *pageInfo = json_new(JSON_NODE);
    sFarmHeader *header = reinterpret_cast<sFarmHeader *>(&pBuf[0]);                                                                                    // pointer to the header to get the signature

#if defined _DEBUG
    printf("\n\n \tFARM Log Header \n");
    printf("\tLog Signature           (debug):               0x%" PRIX64" \n", header->signature & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Log Signature = 0x00004641524D4552
    printf("\tMajor Revision          (debug):                 %" PRIu64"  \n", header->majorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Log Major rev
    printf("\tMinor Revision          (debug):                 %" PRIu64"  \n", header->minorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< minor rev 
    printf("\tPages Supported         (debug):                 %" PRIu64"  \n", header->pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< number of pages supported
    printf("\tLog Size                (debug):                 %" PRIu64"  \n", header->logSize & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< log size in bytes
    printf("\tPage Size               (debug):                 %" PRIu64"  \n", header->pageSize & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< page size in bytes
    printf("\tHeads Supported         (debug):                 %" PRIu64"  \n", header->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Maximum Drive Heads Supported
    printf("\tNumber of Copies        (debug):                 %" PRIu64"  \n", header->copies & UINT64_C(0x00FFFFFFFFFFFFF));                          //!< Number of Historical Copies
    printf("\tReason for Frame Capture(debug):                 %" PRIu64"  \n", header->reasonForFrameCapture & UINT64_C(0x00FFFFFFFFFFFFF));           //!< Reason for Frame Capture

#endif
    json_set_name(pageInfo, "FARM Log Header");
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::uppercase << check_Status_Strip_Status(header->signature);
    json_push_back(pageInfo, json_new_a("Log Signature", temp.str().c_str()));
    set_json_64_bit_With_Status(pageInfo, "Major Revision", header->majorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Minor Revision", header->minorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Pages Supported", header->pagesSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Log Size", header->logSize, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Page Size", header->pageSize, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Heads Supported", header->headsSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of Copies", header->copies, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Reason for Frame Capture", header->reasonForFrameCapture, false, m_showStatusBits);
    std::string reason;
    Get_FARM_Reason_For_Capture(&reason, M_Byte0(header->reasonForFrameCapture));
    json_push_back(pageInfo, json_new_a("Reason Meaning", reason.c_str()));


    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Drive_Information()
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
    std::ostringstream myStr;
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined _DEBUG
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        printf("\nDrive Information From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nDrive Information From Farm Log copy %" PRIu32" \n", page);
    }
    printf("\tserial number(debug):                            %s         \n", vFarmFrame[page].identStringInfo.serialNumber.c_str());
    printf("\tworkd wide name(debug):                          %s         \n", vFarmFrame[page].identStringInfo.worldWideName.c_str());
    printf("\tfirmware Rev(debug):                             %s         \n", vFarmFrame[page].identStringInfo.firmwareRev.c_str());                                                 //!< Firmware Revision [0:3]
    printf("\nDrive Information From Farm Log copy %d(debug): \n", page);
    printf("\tDevice Interface(debug):                         %s \n", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    printf("\tDevice Capacity in sectors(debug):               %" PRId64" \n", (vFarmFrame[page].driveInfo.deviceCapacity & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tPhysical Sector size(debug):                     %" PRIX64" \n", (vFarmFrame[page].driveInfo.psecSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< Physical Sector Size in Bytes
    printf("\tLogical Sector Size(debug):                      %" PRIX64" \n", (vFarmFrame[page].driveInfo.lsecSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< Logical Sector Size in Bytes
    printf("\tDevice Buffer Size(debug):                       %" PRIX64" \n", (vFarmFrame[page].driveInfo.deviceBufferSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                          //!< Device Buffer Size in Bytes
    printf("\tNumber of heads(debug):                          %" PRId64" \n", (vFarmFrame[page].driveInfo.heads & UINT64_C(0x00FFFFFFFFFFFFFF)));                                     //!< Number of Heads
    printf("\tDevice form factor(debug):                       %" PRIX64" \n", (vFarmFrame[page].driveInfo.factor & UINT64_C(0x00FFFFFFFFFFFFFF)));                                    //!< Device Form Factor (ID Word 168)                                        
    printf("\tRotation Rate(debug):                            %" PRIu64"  \n", (vFarmFrame[page].driveInfo.rotationRate & UINT64_C(0x00FFFFFFFFFFFFFF)));                             //!< Rotational Rate of Device (ID Word 217)
    printf("\tATA Features Supported (ID Word 78)(debug):      0x%04" PRIX64" \n", (vFarmFrame[page].driveInfo.featuresSupported & UINT64_C(0x00FFFFFFFFFFFFFF)));                     //!< ATA Features Supported (ID Word 78)
    printf("\tATA Features Enabled (ID Word 79)(debug):        0x%04" PRIX64" \n", (vFarmFrame[page].driveInfo.featuresEnabled & UINT64_C(0x00FFFFFFFFFFFFFF)));                       //!< ATA Features Enabled (ID Word 79)
    printf("\tATA Security State (ID Word 128)(debug):         0x%04" PRIX64" \n", (vFarmFrame[page].driveInfo.security & UINT64_C(0x00FFFFFFFFFFFFFF)));                              //!< ATA Security State (ID Word 128)
    printf("\tPower on Hours(debug):                           %" PRIu64" \n", (vFarmFrame[page].driveInfo.poh & UINT64_C(0x00FFFFFFFFFFFFFF)));                                       //!< Power-on Hour
    printf("\tSpindle Power on hours(debug):                   %" PRIu64" \n", (vFarmFrame[page].driveInfo.spoh & UINT64_C(0x00FFFFFFFFFFFFFF)));                                      //!< Spindle Power-on Hours
    printf("\tHead Flight Hours - Actuator 0(debug):            %" PRIu64" \n", (vFarmFrame[page].driveInfo.headFlightHoursAct0 & UINT64_C(0x00FFFFFFFFFFFFFF)));                      //!< Head Flight Hours
    printf("\tHead Load Events - Actuator 0(debug):             %" PRIu64" \n", (vFarmFrame[page].driveInfo.headLoadEventsAct0 & UINT64_C(0x00FFFFFFFFFFFFFF)));                       //!< Head Load Events
    printf("\tPower Cycle count(debug):                        %" PRIu64" \n", (vFarmFrame[page].driveInfo.powerCycleCount & UINT64_C(0x00FFFFFFFFFFFFFF)));                           //!< Power Cycle Count
    printf("\tHardware Reset count(debug):                     %" PRIu64" \n", (vFarmFrame[page].driveInfo.resetCount & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Hardware Reset Count
    printf("\tSpin-up Time(debug):                             %" PRIu64" \n", (vFarmFrame[page].driveInfo.spinUpTime & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< SMART Spin-Up time in milliseconds
    printf("\tTime Available to save(debug):                   %" PRIu64" \n", (vFarmFrame[page].driveInfo.timeAvailable & UINT64_C(0x00FFFFFFFFFFFFFF)));                             //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    printf("\tTime of latest frame(debug):                     %" PRIu64" \n", (vFarmFrame[page].driveInfo.timeStamp1 & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
    printf("\tTime of latest frame (milliseconds)(debug):      %" PRIu64" \n", (vFarmFrame[page].driveInfo.timeStamp2 & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1
    printf("\tTime to ready of the last power cycle(debug):    %" PRIu64" \n", (vFarmFrame[page].driveInfo.timeToReady & UINT64_C(0x00FFFFFFFFFFFFFF)));                               //!< time to ready of the last power cycle
    printf("\tTime drive is held in staggered spin(debug):     %" PRIu64" \n", (vFarmFrame[page].driveInfo.timeHeld & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< time drive is held in staffered spin during the last power on sequence
    printf("\tDepopulation Head Mask(debug):                   %" PRIu64" \n", (vFarmFrame[page].driveInfo.depopulationHeadMask & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tHead Flight Hours - Actuator 1(debug):            %" PRIu64" \n", (vFarmFrame[page].driveInfo.headFlightHoursAct1 & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tHead Load Events - Actuator 1(debug):             %" PRIu64" \n", (vFarmFrame[page].driveInfo.headLoadEventsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tDrive Recording Type(debug):                     %" PRIu64" \n", (vFarmFrame[page].driveInfo.driveRecordingType & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tdepopped(debug):                                 %" PRIu64" \n", (vFarmFrame[page].driveInfo.depopped & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tMax number of sectors for reasssingment(debug):  %" PRIu64" \n", (vFarmFrame[page].driveInfo.maxNumberForReasign & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tDate of Assembly(debug):                         %" PRIu64" \n", (vFarmFrame[page].driveInfo.dateOfAssembly & UINT64_C(0x00FFFFFFFFFFFFFF)));
    printf("\tHAMR Data Protect Status(debug):                 %" PRIu64" \n", (vFarmFrame[page].driveInfo.HAMRProtectStatus & UINT64_C(0x00FFFFFFFFFFFFFF)));
#endif

    std::ostringstream temp;

    // check the level of print form the eToolVerbosityLevels 
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        temp << "Drive Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Drive Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    json_push_back(pageInfo, json_new_a("Serial Number", vFarmFrame[page].identStringInfo.serialNumber.c_str()));                                               //!< serial number of the device
    json_push_back(pageInfo, json_new_a("Model Number", vFarmFrame[page].identStringInfo.modelNumber.c_str()));                                                 //!< model Number  only on 3.0 and higher 
    json_push_back(pageInfo, json_new_a("World Wide Name", vFarmFrame[page].identStringInfo.worldWideName.c_str()));                                            //!< world wide Name
    json_push_back(pageInfo, json_new_a("Device Interface", vFarmFrame[page].identStringInfo.deviceInterface.c_str()));                                         //!< Device Interface
    set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame[page].driveInfo.poh, false, m_showStatusBits);                                            //!< Power-on Hour
    set_json_64_bit_With_Status(pageInfo, "Device Capacity in Sectors", vFarmFrame[page].driveInfo.deviceCapacity, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame[page].driveInfo.psecSize, false, m_showStatusBits);                                //!< Physical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame[page].driveInfo.lsecSize, false, m_showStatusBits);                                 //!< Logical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame[page].driveInfo.deviceBufferSize, false, m_showStatusBits);                          //!< Device Buffer Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame[page].driveInfo.heads, false, m_showStatusBits);                                        //!< Number of Heads
    if (check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads) != 0)
    {
        m_heads = static_cast<uint64_t>(check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads));
    }
    set_json_64_bit_With_Status(pageInfo, "Device form factor", vFarmFrame[page].driveInfo.factor, false, m_showStatusBits);                                    //!< Device Form Factor (ID Word 168)
    set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame[page].driveInfo.rotationRate, false, m_showStatusBits);
    json_push_back(pageInfo, json_new_a("Firmware Rev", vFarmFrame[page].identStringInfo.firmwareRev.c_str()));                                                 //!< Firmware Revision [0:3]
    set_json_64_bit_With_Status(pageInfo, "ATA Security State (ID Word 128)", vFarmFrame[page].driveInfo.security, true, m_showStatusBits);                     //!< ATA Security State (ID Word 128)
    set_json_64_bit_With_Status(pageInfo, "ATA Features Supported (ID Word 78)", vFarmFrame[page].driveInfo.featuresSupported, true, m_showStatusBits);         //!< ATA Features Supported (ID Word 78)
    set_json_64_bit_With_Status(pageInfo, "ATA Features Enabled (ID Word 79)", vFarmFrame[page].driveInfo.featuresEnabled, true, m_showStatusBits);             //!< ATA Features Enabled (ID Word 79)
    set_json_64_bit_With_Status(pageInfo, "Spindle Power on hours", vFarmFrame[page].driveInfo.spoh, false, m_showStatusBits);                                  //!< Spindle Power-on Hours
    set_json_64_bit_With_Status(pageInfo, "Head Flight Hours - Actuator 0", vFarmFrame[page].driveInfo.headFlightHoursAct0, false, m_showStatusBits);           //!< Head Flight Hours
    set_json_64_bit_With_Status(pageInfo, "Head Load Events - Actuator 0", vFarmFrame[page].driveInfo.headLoadEventsAct0, false, m_showStatusBits);             //!< Head Load Events
    set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame[page].driveInfo.powerCycleCount, false, m_showStatusBits);                            //!< Power Cycle Count
    set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame[page].driveInfo.resetCount, false, m_showStatusBits);                              //!< Hardware Reset Count
    set_json_64_bit_With_Status(pageInfo, "Spin-up Time", vFarmFrame[page].driveInfo.spinUpTime, false, m_showStatusBits);                                      //!< SMART Spin-Up time in milliseconds
    
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].driveInfo.timeStamp1 & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Timestamp of First SMART Summary Frame (ms)", temp.str().c_str(), vFarmFrame[page].driveInfo.timeStamp1, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].driveInfo.timeStamp2 & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "TimeStamp of Last SMART Summary Frame (ms)", temp.str().c_str(), vFarmFrame[page].driveInfo.timeStamp2, m_showStatusBits);      //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1

    temp.str("");temp.clear();
    temp << std::setfill('0') << std::setprecision(6) << static_cast<float>(M_Word0(vFarmFrame[page].driveInfo.timeToReady)) / 1000;
    set_json_string_With_Status(pageInfo, "Time to ready of the last power cycle (sec)", temp.str().c_str(), vFarmFrame[page].driveInfo.timeToReady, m_showStatusBits);         //!< time to ready of the last power cycle in milliseconds
    temp.str("");temp.clear();
    temp << std::setfill('0') << std::setprecision(3) << static_cast<float>(M_Word0(vFarmFrame[page].driveInfo.timeHeld)) / 1000;
    set_json_64_bit_With_Status(pageInfo, "Time drive is held in staggered spin (sec)", vFarmFrame[page].driveInfo.timeHeld, false, m_showStatusBits);                //!< time drive is held in staggered spin during the last power on sequence in milliseconds

    myStr << "Drive Recording Type";
    std::string type = "not supported";
    if (check_For_Active_Status(&vFarmFrame[page].driveInfo.driveRecordingType))
    {
        type = "CMR";
        if (vFarmFrame[page].driveInfo.driveRecordingType & BIT0)
        {
            type = "SMR";
        }
    }

    set_json_string_With_Status(pageInfo, myStr.str().c_str(), type.c_str(), vFarmFrame[page].driveInfo.driveRecordingType, m_showStatusBits);
    myStr.str(""); myStr.clear();
    myStr<< "Has Drive been Depopped";
    if (check_Status_Strip_Status(vFarmFrame[page].driveInfo.depopped) != 0)
    {
        set_Json_Bool(pageInfo, myStr.str().c_str(), true);
    }
    else
    {
        set_Json_Bool(pageInfo, myStr.str().c_str(), false);
    }

    set_json_64_bit_With_Status(pageInfo, "Max Number of Available Sectors for Reassignment", vFarmFrame[page].driveInfo.maxNumberForReasign, false, m_showStatusBits);          //!< Max Number of Available Sectors for Reassignment � Value in disc sectors(started in 3.3 )

    //!< Date of Assembly in ASCII 
    if (check_For_Active_Status(&vFarmFrame[page].driveInfo.dateOfAssembly))
    {
        uint16_t week = M_Word1(vFarmFrame[page].driveInfo.dateOfAssembly);
        uint16_t year = M_Word0(vFarmFrame[page].driveInfo.dateOfAssembly);
        std::string dataAndTime;
        create_Year_Assembled_String(dataAndTime, year, false);
        json_push_back(pageInfo, json_new_a("Year of Assembled", dataAndTime.c_str()));
        dataAndTime.clear();
        create_Year_Assembled_String(dataAndTime, week, false);
        json_push_back(pageInfo, json_new_a("Week of Assembled", dataAndTime.c_str()));
    }
    else
    {
        json_push_back(pageInfo, json_new_a("Year of Assembled", "00"));
        json_push_back(pageInfo, json_new_a("Week of Assembled", "00"));
    }
    set_json_64_bit_With_Status(pageInfo, "Depopulation Head Mask", vFarmFrame[page].driveInfo.depopulationHeadMask, false, m_showStatusBits);              //!< Depopulation Head Mask

    //version 4.21

    set_json_64_bit_With_Status(pageInfo, "Head Flight Hours - Actuator 1", vFarmFrame[page].driveInfo.headFlightHoursAct1, false, m_showStatusBits);       //!< Head Flight Hours- Actuator 1
    set_json_64_bit_With_Status(pageInfo, "Head Load Events - Actuator 1", vFarmFrame[page].driveInfo.headLoadEventsAct1, false, m_showStatusBits);         //!< Head Load Events- Actuator 1
    set_json_bool_With_Status(pageInfo, "HAMR Data Protect Status", vFarmFrame[page].driveInfo.HAMRProtectStatus, m_showStatusBits);

    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn printWorkLoad()
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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined _DEBUG
    if (vFarmFrame[page].workLoadPage.copyNumber == FACTORYCOPY)
    {
        printf("\nWorkload From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nWorkload From Farm Log copy %d \n", page);
    }
    printf("\tRated Workload Percentage                                     %" PRIu64" \n", vFarmFrame[page].workLoadPage.workloadPercentage & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< rated Workload Percentage
    printf("\tTotal Number of Other Commands                                %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Total Number Of Other Commands
    printf("\tTotal Number of Write Commands                                %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalWriteCommands & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Total Number of Write Commands
    printf("\tTotal Number of Read Commands                                 %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalReadCommands & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Total Number of Read Commands
    printf("\tLogical Sectors Written                                       %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Logical Sectors Written
    printf("\tLogical Sectors Read                                          %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFF));                //!< Logical Sectors Read
    printf("\tTotal Number of Random Read Cmds                              %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalRandomReads & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Total Number of Random Read Commands
    printf("\tTotal Number of Random Write Cmds                             %" PRIu64" \n", vFarmFrame[page].workLoadPage.totalRandomWrites & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Total Number of Random Write Commands
    printf("\tLogical Sectors Written                                       %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Logical Sectors Written
    printf("\tLogical Sectors Read                                          %" PRIu64" \n", vFarmFrame[page].workLoadPage.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Logical Sectors Read
    printf("\tDither events during current power cycle - Actuator 0         %" PRIu64" \n", vFarmFrame[page].workLoadPage.dither & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Number of dither events during current power cycle (added 3.4)
    printf("\tDither was held off during random- Actuator 0                 %" PRIu64" \n", vFarmFrame[page].workLoadPage.ditherRandom & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Number of times dither was held off during random workloads during current power cycle(added 3.4)
    printf("\tDither was held off during sequential- Actuator 0             %" PRIu64" \n", vFarmFrame[page].workLoadPage.ditherSequential & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Number of times dither was held off during sequential workloads during current power cycle(added 3.4)
    printf("\tRead cmds from 0-3.125%% of LBA space                         %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfReadCmds1 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tRead cmds from 3.125-25%% of LBA space                        %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfReadCmds2 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tRead cmds from 25-50%% of LBA space                           %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfReadCmds3 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tRead cmds from 50-100%% of LBA space                          %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfReadCmds4 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tWrite cmds from 0-3.125%% of LBA space                        %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfWriteCmds1 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tWrite cmds from 3.125-25%% of LBA space                       %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfWriteCmds2 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tWrite cmds from 25-50%% of LBA space                          %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfWriteCmds3 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    printf("\tWrite cmds from 50-100%% of LBA space                         %" PRIu64" \n", vFarmFrame[page].workLoadPage.numberOfWriteCmds4 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    //4.21

    printf("\tNumber of Read Commands of transfer length <=16KB             %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numReadTransferSmallATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Read Commands of transfer length (16KB - 512KB]     %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numReadTransferMid1ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Read Commands of transfer length (512KB - 2MB]      %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numReadTransferMid2ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Read Commands of transfer length > 2MB              %" PRIu64" \n", vFarmFrame[page].workLoadPage.numReadTransferLargeATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Write Commands of transfer length <=16KB            %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numWriteTransferSmallATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Write Commands of transfer length (16KB - 512KB]    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numWriteTransferMid1ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Write Commands of transfer length (512KB - 2MB]     %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numWriteTransferMid2ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tNumber of Write Commands of transfer length > 2MB             %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numWriteTransferLargeATA & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =1 at 30s intervals                      %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth1 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =2 at 30s intervals                      %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth2 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =3-4 at 30s intervals                    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth3to4 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =5-8 at 30s intervals                    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth5to8 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =9-16 at 30s intervals                   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth9to16 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =17-32 at 30s intervals                  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth17to32 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth =33-64 at 30s intervals                  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepth33to64 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCount of Queue Depth >64 at 30s intervals                     %" PRIu64"  \n", vFarmFrame[page].workLoadPage.cntQueueDepthmorethan64 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tDither events during current power cycle - Actuator 1         %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numDithEvtAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tDither was held off during random - Actuator 1                %" PRIu64"  \n", vFarmFrame[page].workLoadPage.numRandWLDitherHoldOffAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tDither was held off during sequential - Actuator 1            %" PRIu64" \n", vFarmFrame[page].workLoadPage.numSequentialWLDitherHoldOffAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));

#endif
    std::ostringstream temp;

    if (vFarmFrame[page].workLoadPage.copyNumber == FACTORYCOPY)
    {
        temp << "Workload From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Workload From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentage", vFarmFrame[page].workLoadPage.workloadPercentage, false, m_showStatusBits);             //!< rated Workload Percentage
    set_json_64_bit_With_Status(pageInfo, "Total Read Commands", vFarmFrame[page].workLoadPage.totalReadCommands, false, m_showStatusBits);           //!< Total Number of Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Write Commands", vFarmFrame[page].workLoadPage.totalWriteCommands, false, m_showStatusBits);         //!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Read Cmds", vFarmFrame[page].workLoadPage.totalRandomReads, false, m_showStatusBits);         //!< Total Number of Random Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Write Cmds", vFarmFrame[page].workLoadPage.totalRandomWrites, false, m_showStatusBits);       //!< Total Number of Random Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Other Commands", vFarmFrame[page].workLoadPage.totalNumberofOtherCMDS, false, m_showStatusBits);     //!< Total Number Of Other Commands
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].workLoadPage.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Logical Sectors Written", temp.str().c_str(), vFarmFrame[page].workLoadPage.logicalSecWritten, m_showStatusBits);                  //!< Logical Sectors Written
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].workLoadPage.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Logical Sectors Read", temp.str().c_str(), vFarmFrame[page].workLoadPage.logicalSecRead, m_showStatusBits);                        //!< Logical Sectors Read
    temp.str("");temp.clear();
    set_json_64_bit_With_Status(pageInfo, "dither events - Actuator 0", vFarmFrame[page].workLoadPage.dither, false, m_showStatusBits);             //!< Number of dither events during current power cycle (added 3.4)
    set_json_64_bit_With_Status(pageInfo, "dither held off during random workloads - Actuator 0", vFarmFrame[page].workLoadPage.ditherRandom, false, m_showStatusBits);          //!< Number of times dither was held off during random workloads during current power cycle(added 3.4)
    set_json_64_bit_With_Status(pageInfo, "dither was held off during sequential workloads - Actuator 0", vFarmFrame[page].workLoadPage.ditherSequential, false, m_showStatusBits);          //!< Number of times dither was held off during sequential workloads during current power cycle(added 3.4)

    set_json_64_bit_With_Status(pageInfo, "Read commands from 0-3.125% of LBA space", vFarmFrame[page].workLoadPage.numberOfReadCmds1, false, m_showStatusBits);          //!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 3.125-25% of LBA space", vFarmFrame[page].workLoadPage.numberOfReadCmds2, false, m_showStatusBits);         //!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 25-50% of LBA space", vFarmFrame[page].workLoadPage.numberOfReadCmds3, false, m_showStatusBits);            //!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 50-100% of LBA space", vFarmFrame[page].workLoadPage.numberOfReadCmds4, false, m_showStatusBits);           //!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 0-3.125% of LBA space", vFarmFrame[page].workLoadPage.numberOfWriteCmds1, false, m_showStatusBits);        //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 3.125-25% of LBA space", vFarmFrame[page].workLoadPage.numberOfWriteCmds2, false, m_showStatusBits);       //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 25-50% of LBA space", vFarmFrame[page].workLoadPage.numberOfWriteCmds3, false, m_showStatusBits);          //!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 50-100% of LBA space", vFarmFrame[page].workLoadPage.numberOfWriteCmds4, false, m_showStatusBits);         //!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    //4.21

    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length <=16KB", vFarmFrame[page].workLoadPage.numReadTransferSmallATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (16KB - 512KB]", vFarmFrame[page].workLoadPage.numReadTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (512KB - 2MB]", vFarmFrame[page].workLoadPage.numReadTransferMid2ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length > 2MB", vFarmFrame[page].workLoadPage.numReadTransferLargeATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length <=16KB", vFarmFrame[page].workLoadPage.numWriteTransferSmallATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (16KB - 512KB]", vFarmFrame[page].workLoadPage.numWriteTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (512KB - 2MB]", vFarmFrame[page].workLoadPage.numWriteTransferMid2ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length > 2MB", vFarmFrame[page].workLoadPage.numWriteTransferLargeATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 1", vFarmFrame[page].workLoadPage.cntQueueDepth1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 2", vFarmFrame[page].workLoadPage.cntQueueDepth2, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 3-4", vFarmFrame[page].workLoadPage.cntQueueDepth3to4, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 5-8", vFarmFrame[page].workLoadPage.cntQueueDepth5to8, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 9-16", vFarmFrame[page].workLoadPage.cntQueueDepth9to16, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 17-32", vFarmFrame[page].workLoadPage.cntQueueDepth17to32, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 33-64", vFarmFrame[page].workLoadPage.cntQueueDepth33to64, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin > 64", vFarmFrame[page].workLoadPage.cntQueueDepthmorethan64, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither events - Actuator 1", vFarmFrame[page].workLoadPage.numDithEvtAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither was held off during random workloads - Actuator 1", vFarmFrame[page].workLoadPage.numRandWLDitherHoldOffAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither was held off during sequential workloads - Actuator 1", vFarmFrame[page].workLoadPage.numSequentialWLDitherHoldOffAct1, false, m_showStatusBits);

    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Error_Information()
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
    std::string myStr;
    std::string timeStr;
    uint32_t loopCount = 0;
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined _DEBUG
    if (vFarmFrame[page].errorPage.copyNumber == FACTORYCOPY)
    {
        printf("\nError Information From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nError Information Log From Farm Log copy %d: \n", page);
    }
    printf("\tUnrecoverable Read Errors(debug):                %" PRId64" \n", vFarmFrame[page].errorPage.totalReadECC & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors(debug):               %" PRId64" \n", vFarmFrame[page].errorPage.totalWriteECC & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Number of Unrecoverable Write Errors
    printf("\tNumber of Reallocated Sectors(debug):            %" PRId64" \n", vFarmFrame[page].errorPage.totalReallocations & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Number of Reallocated Sectors
    printf("\tNumber of Read Recovery Attempts(debug):         %" PRId64" \n", vFarmFrame[page].errorPage.totalReadRecoveryAttepts & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Number of Read Recovery Attempts
    printf("\tNumber of Mechanical Start Failures(debug):      %" PRId64" \n", vFarmFrame[page].errorPage.totalMechanicalFails & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Number of Mechanical Start Failures
    printf("\tNumber of Reallocated Candidate Sectors(debug):  %" PRId64" \n", vFarmFrame[page].errorPage.totalReallocatedCanidates & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Number of Reallocated Candidate Sectors
    printf("\tNumber of ASR Events(debug):                     %" PRIu64" \n", vFarmFrame[page].errorPage.totalASREvents & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of ASR Events
    printf("\tNumber of Interface CRC Errors(debug):           %" PRIu64" \n", vFarmFrame[page].errorPage.totalCRCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Interface CRC Errors
    printf("\tSpin Retry Count(debug):                         %" PRIu64" \n", vFarmFrame[page].errorPage.attrSpinRetryCount & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
    printf("\tSpin Retry Count Normalized(debug):              %" PRIu64" \n", vFarmFrame[page].errorPage.normalSpinRetryCount & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Spin Retry Count (SMART Attribute 10 Normalized)
    printf("\tSpin Retry Count Worst(debug):                   %" PRIu64" \n", vFarmFrame[page].errorPage.worstSpinRretryCount & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
    printf("\tNumber of IOEDC Errors (Raw)(debug):           0x%" PRIx64" \n", vFarmFrame[page].errorPage.attrIOEDCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    printf("\tCTO Count Total(debug):                          %" PRId64" \n", vFarmFrame[page].errorPage.attrCTOCount & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
    printf("\tCTO Count Over 5s(debug):                        %" PRIu64" \n", vFarmFrame[page].errorPage.overfiveSecCTO & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
    printf("\tCTO Count Over 7.5s(debug):                      %" PRIu64" \n", vFarmFrame[page].errorPage.oversevenSecCTO & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< CTO Count Over 7.5s (SMART Attribute
    printf("\tTotal Flash LED (Assert) Events(debug):          %" PRId64" \n", vFarmFrame[page].errorPage.totalFlashLED & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Total Flash LED (Assert) Events
    printf("\tIndex of the last Flash LED(debug):              %" PRId64" \n", vFarmFrame[page].errorPage.indexFlashLED & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< index of the last Flash LED of the array
    printf("\tUncorrectable errors(debug):                     %" PRId64" \n", vFarmFrame[page].errorPage.uncorrectables & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< uncorrecatables errors (sata only)
    for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
    {
        printf("\tFlash LED event # %d:                             %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.flashLEDArray[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));          //!<Info on the last 8 Flash LED events Wrapping array.
        printf("\tRead/Write retry events:                         %" PRIu64" \n", vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tRetry Count:                                     %" PRIu8"  \n", M_Byte0(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tHead:                                            %" PRIu8"  \n", M_Byte1(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tZone Group LSB:                                  %" PRIu8"  \n", M_Byte2(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tZone Group MSB:                                  %" PRIu8"  \n", M_Byte3(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tRW Retry Log Entry LSB:                          %" PRIu8"  \n", M_Byte4(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tRW Retry Log Entry MSB:                          %" PRIu8"  \n", M_Byte5(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tError Type:                                      %" PRIu8"  \n", M_Byte6(vFarmFrame[page].errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tTimestamp of event:                              %" PRIu64" \n", vFarmFrame[page].errorPage.timestampForLED[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPower Cycle of event:                            %" PRIu64" \n", vFarmFrame[page].errorPage.powerCycleOfLED[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));

    }
    printf("\tCum Lifetime Unrecoverable Read errors due to ERC:          %" PRIu64" \n", vFarmFrame[page].errorPage.cumLifeTimeECCReadDueErrorRecovery & UINT64_C(0x00FFFFFFFFFFFFFF));
    for (loopCount = 0; loopCount < MAX_HEAD_COUNT; ++loopCount)
    {
        printf("\tCum Lifetime Unrecoverable Read Repeating by Head %" PRIu32":        %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.cumLifeUnRecoveralbeReadByhead[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
        printf("\tCum Lifetime Unrecoverable Read Unique by Head %" PRIu32":           %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.cumLiveUnRecoveralbeReadUnique[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< Cumulative Lifetime Unrecoverable Read Unique by head
    }
    //verion 4.21
    if (m_MajorRev >= 4 && m_MinorRev > 20)
    {
        printf("\tReallocated Sectors - Actuator 1 %" PRIu32":        %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.reallocSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReallocation Candidate Sectors - Actuator 1 %" PRIu32":           %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.reallocCandidatesAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tTotal Flash LED (Assert) Event s- Actuator 1 %" PRIu32":        %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.totalFlashLEDEvents & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tIndex of last entry in FLED Info array below, in case the array wraps - Actuator 1 %" PRIu32":           %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.lastIDXFLEDInfoAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
        {
            printf("\tInfo on the last 8 Flash LED (assert) Events, wrapping array - Actuator 1 %" PRIu32":        %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.last8FLEDEventsAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tInfo on the last 8 Read/Write Retry events, wrapping array - Actuator 1 %" PRIu32":           %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
        {
            printf("\tUniversal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array - Actuator 1 %" PRIu32":        %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.last8FLEDEvtsAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tPower Cycle of the last 8 Flash LED (assert) Events, wrapping array - Actuator 1 %" PRIu32":           %" PRIu64" \n", loopCount, vFarmFrame[page].errorPage.last8FLEDEvtsPowerCycleAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
    }

#endif
    std::ostringstream temp;
    if (vFarmFrame[page].errorPage.copyNumber == FACTORYCOPY)
    {
        temp << "Error Information From Farm Log copy FACTORYe";
    }
    else
    {
        temp << "Error Information Log From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.totalReadECC, false, m_showStatusBits);                       //!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.totalWriteECC, false, m_showStatusBits);                     //!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Reallocated Sectors", vFarmFrame[page].errorPage.totalReallocations, false, m_showStatusBits);                       //!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Read Recovery Attempts", vFarmFrame[page].errorPage.totalReadRecoveryAttepts, false, m_showStatusBits);              //!< Number of Read Recovery Attempts
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame[page].errorPage.totalMechanicalFails, false, m_showStatusBits);               //!< Number of Mechanical Start Failures

    set_json_64_bit_With_Status(pageInfo, "Reallocated Candidate Sectors", vFarmFrame[page].errorPage.totalReallocatedCanidates,false, m_showStatusBits);       //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "ASR Events", vFarmFrame[page].errorPage.totalASREvents, false, m_showStatusBits);                                    //!< Number of ASR Events
    set_json_64_bit_With_Status(pageInfo, "Interface CRC Errors", vFarmFrame[page].errorPage.totalCRCErrors, false, m_showStatusBits);                          //!< Number of Interface CRC Errors
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count", vFarmFrame[page].errorPage.attrSpinRetryCount, false, m_showStatusBits);                          //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Normalized", vFarmFrame[page].errorPage.normalSpinRetryCount, false, m_showStatusBits);             //!< Spin Retry Count (SMART Attribute 10 Normalized)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Worst", vFarmFrame[page].errorPage.worstSpinRretryCount, false, m_showStatusBits);                  //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
    set_json_64_bit_With_Status(pageInfo, "IOEDC Errors (Raw)", vFarmFrame[page].errorPage.attrIOEDCErrors, false, m_showStatusBits);                           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "CTO Count Total", vFarmFrame[page].errorPage.attrCTOCount, false, m_showStatusBits);                                 //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 5s", vFarmFrame[page].errorPage.overfiveSecCTO, false, m_showStatusBits);                             //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 7.5s", vFarmFrame[page].errorPage.oversevenSecCTO, false, m_showStatusBits);                          //!< CTO Count Over 7.5s (SMART Attribute
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events", vFarmFrame[page].errorPage.totalFlashLED, false, m_showStatusBits);                //!< Total Flash LED (Assert) Events

    set_json_64_bit_With_Status(pageInfo, "Index of the last Flash LED", vFarmFrame[page].errorPage.indexFlashLED,false, m_showStatusBits);                     //!< index of the last Flash LED of the array          
    set_json_64_bit_With_Status(pageInfo, "Uncorrectable errors", vFarmFrame[page].errorPage.uncorrectables, false, m_showStatusBits);                          //!< uncorrecatables errors (sata only)

    for (loopCount = 0; loopCount < FLASH_EVENTS; loopCount++)
    {
        temp.str("");temp.clear();
        JSONNODE *eventInfo = json_new(JSON_NODE);
        temp << "Flash LED Event " << std::dec << loopCount;
        json_set_name(eventInfo, temp.str().c_str());

        set_json_64_bit_With_Status(eventInfo, "Event Information", vFarmFrame[page].errorPage.flashLEDArray[loopCount], true, m_showStatusBits);              //!< Info on the last 8 Flash LED (assert) Events, wrapping array

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(check_Status_Strip_Status(vFarmFrame[page].errorPage.flashLEDArray[loopCount]));
        json_push_back(eventInfo, json_new_a("Flash LED Code", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(check_Status_Strip_Status(vFarmFrame[page].errorPage.flashLEDArray[loopCount]));
        json_push_back(eventInfo, json_new_a("Flash LED Address", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "TimeStamp of Event(hours) " << std::dec << loopCount;
        std::ostringstream temp1;
        temp1 << std::setprecision(3) << std::setfill('0') << static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(vFarmFrame[page].errorPage.timestampForLED[loopCount])) / 3600000) *.001;
        set_json_string_With_Status(eventInfo, temp.str().c_str(), temp1.str().c_str(), vFarmFrame[page].errorPage.timestampForLED[loopCount], m_showStatusBits);//!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array

        temp.str("");temp.clear();
        temp << "Power Cycle Event " << std::dec << loopCount;
        set_json_64_bit_With_Status(eventInfo, temp.str().c_str(), vFarmFrame[page].errorPage.powerCycleOfLED[loopCount], false, m_showStatusBits);            //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array
        temp.str("");temp.clear();

        json_push_back(pageInfo, eventInfo);
    }

    if (m_MajorRev >= MAJORVERSION3 && m_MinorRev >= 2 && m_MinorRev <= 4)
    {
        // get read write retry events
        for (loopCount = 0; loopCount <= 7; ++loopCount)
        {
            JSONNODE *rwrInfo = json_new(JSON_NODE);
            temp.str("");temp.clear();
            temp << "Read Write Retry " << std::dec << loopCount;
            json_set_name(rwrInfo, temp.str().c_str());

            set_json_64_bit_With_Status(rwrInfo, "Error Type", M_Byte6(check_Status_Strip_Status(vFarmFrame[page].errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Log Entry", M_Byte5(check_Status_Strip_Status(vFarmFrame[page].errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Zone Group", M_Word1(check_Status_Strip_Status(vFarmFrame[page].errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Head", M_Nibble3(check_Status_Strip_Status(vFarmFrame[page].errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Count", M_Byte0(check_Status_Strip_Status(vFarmFrame[page].errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);

            json_push_back(pageInfo, rwrInfo);
        }
    }
    else
    {
        for (loopCount = 0; loopCount <= 7; ++loopCount)
        {
            JSONNODE *rwrInfo = json_new(JSON_NODE);
            temp.str("");temp.clear();
            temp << "Read Write Retry " << std::dec << loopCount;
            json_set_name(rwrInfo, temp.str().c_str());

            set_json_int_Check_Status(rwrInfo, "Error Type", M_Byte6(vFarmFrame[page].errorPage.readWriteRetry[loopCount]), vFarmFrame[page].errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Log Entry", M_Word2(vFarmFrame[page].errorPage.readWriteRetry[loopCount]), vFarmFrame[page].errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Zone Group", M_Word1(vFarmFrame[page].errorPage.readWriteRetry[loopCount]), vFarmFrame[page].errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Head", M_Byte1(vFarmFrame[page].errorPage.readWriteRetry[loopCount]), vFarmFrame[page].errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Count", M_Byte0(vFarmFrame[page].errorPage.readWriteRetry[loopCount]), vFarmFrame[page].errorPage.readWriteRetry[loopCount], m_showStatusBits);

            json_push_back(pageInfo, rwrInfo);
        }

    }
    set_json_64_bit_With_Status(pageInfo, "Cum Lifetime Unrecoverable Read errors due to ERC", vFarmFrame[page].errorPage.cumLifeTimeECCReadDueErrorRecovery, false, m_showStatusBits);
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        JSONNODE *eventInfo = json_new(JSON_NODE);
        temp.str("");temp.clear();
        temp << "Cum Lifetime Unrecoverable by Head " << std::dec << loopCount;
        json_set_name(eventInfo, temp.str().c_str());
        set_json_64_bit_With_Status(eventInfo, "Cum Lifetime Unrecoverable Read Repeating", vFarmFrame[page].errorPage.cumLifeUnRecoveralbeReadByhead[loopCount], false, m_showStatusBits);      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
        set_json_64_bit_With_Status(eventInfo, "Cum Lifetime Unrecoverable Read Unique", vFarmFrame[page].errorPage.cumLiveUnRecoveralbeReadUnique[loopCount], false, m_showStatusBits);   //!< Cumulative Lifetime Unrecoverable Read Unique by head
        json_push_back(pageInfo, eventInfo);
    }
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors - Actuator 1", vFarmFrame[page].errorPage.reallocSectorsAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors - Actuator 1", vFarmFrame[page].errorPage.reallocCandidatesAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events - Actuator 1", vFarmFrame[page].errorPage.totalFlashLEDEvents, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Index of last entry in FLED Info array below - Actuator 1", vFarmFrame[page].errorPage.lastIDXFLEDInfoAct1, false, m_showStatusBits);
    for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
    {
        JSONNODE *eventInfoact1 = json_new(JSON_NODE);
        temp.str("");temp.clear();
        temp << "Flash LED Event " << std::dec << loopCount << " Actuator 1";
        json_set_name(eventInfoact1, temp.str().c_str());

        set_json_64_bit_With_Status(eventInfoact1, "Event Information", vFarmFrame[page].errorPage.last8FLEDEventsAct1[loopCount], true, m_showStatusBits);              //!> Info on the last 8 Flash LED(assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(check_Status_Strip_Status(vFarmFrame[page].errorPage.last8FLEDEventsAct1[loopCount]));
        json_push_back(eventInfoact1, json_new_a("Flash LED Code Actuator 1", temp.str().c_str()));
        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(check_Status_Strip_Status(vFarmFrame[page].errorPage.last8FLEDEventsAct1[loopCount]));
        json_push_back(eventInfoact1, json_new_a("Flash LED Address Actuator 1", temp.str().c_str()));

        temp.str(""); temp.clear();
        temp << "TimeStamp Actuator 1 Event(hours) " << std::dec << loopCount;
        std::ostringstream temp1;
        temp1 << std::setprecision(3) << std::setfill('0') << static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(vFarmFrame[page].errorPage.last8FLEDEvtsAct1[loopCount])) / 3600000) * .001;
        set_json_string_With_Status(eventInfoact1, temp.str().c_str(), temp1.str().c_str(), vFarmFrame[page].errorPage.last8FLEDEvtsAct1[loopCount], m_showStatusBits);        //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "Power Cycle Actuator 1 Event " << std::dec << loopCount;
        set_json_64_bit_With_Status(eventInfoact1, temp.str().c_str(), vFarmFrame[page].errorPage.last8FLEDEvtsPowerCycleAct1[loopCount], false, m_showStatusBits);            //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array- Actuator 1
        temp.str(""); temp.clear();

        json_push_back(pageInfo, eventInfoact1);
    }
    for (loopCount = 0; loopCount <= 7; ++loopCount)
    {
        JSONNODE* rwrInfo = json_new(JSON_NODE);
        temp.str(""); temp.clear();
        temp << "Read Write Retry " << std::dec << loopCount << " Actuator 1";
        json_set_name(rwrInfo, temp.str().c_str());

        set_json_int_Check_Status(rwrInfo, "Error Type Actuator 1", M_Byte6(vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Log Entry Actuator 1", M_Word2(vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Zone Group Actuator 1", M_Word1(vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Head Actuator 1", M_Byte1(vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Count Actuator 1", M_Byte0(vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame[page].errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);

        json_push_back(pageInfo, rwrInfo);
    }

    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn Print_Enviroment_Information()
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
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined _DEBUG
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        printf("\nEnvironment Information From Farm Log copy FACTORY");
    }
    else
    {
        printf("\nEnvironment Information From Farm Log copy %d:", page);
    }

    printf("\tCurrent Temperature (debug):                     %" PRIu64" \n", vFarmFrame[page].environmentPage.curentTemp & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Current Temperature in Celsius
    printf("\tHighest Temperature (debug):                     %" PRIu64" \n", vFarmFrame[page].environmentPage.highestTemp & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Highest Temperature in Celsius
    printf("\tLowest Temperature (debug):                      %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestTemp & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Lowest Temperature
    printf("\tAverage Short Term Temperature (debug):          %" PRIu64" \n", vFarmFrame[page].environmentPage.averageTemp & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Average Short Term Temperature5
    printf("\tAverage Long Term Temperatures (debug):          %" PRIu64" \n", vFarmFrame[page].environmentPage.averageLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Average Long Term Temperature5
    printf("\tHighest Average Short Term Temperature (debug):  %" PRIu64" \n", vFarmFrame[page].environmentPage.highestShortTemp & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Highest Average Short Term Temperature5
    printf("\tLowest Average Short Term Temperature (debug):   %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestShortTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Lowest Average Short Term Temperature5
    printf("\tHighest Average Long Term Temperature (debug):   %" PRIu64" \n", vFarmFrame[page].environmentPage.highestLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Highest Average Long Term Temperature5
    printf("\tLowest Average Long Term Temperature (debug):    %" PRIu64" \n", vFarmFrame[page].environmentPage.lowestLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< Lowest Average Long Term Temperature5
    printf("\tTime In Over Temperature (debug):                %" PRIu64" \n", vFarmFrame[page].environmentPage.overTempTime & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Time In Over Temperature5
    printf("\tTime In Under Temperature (debug):               %" PRIu64" \n", vFarmFrame[page].environmentPage.underTempTime & UINT64_C(0x00FFFFFFFFFFFFFF));    //!< Time In Under Temperature5
    printf("\tSpecified Max Operating Temperature (debug):     %" PRIu64" \n", vFarmFrame[page].environmentPage.maxTemp & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Specified Max Operating Temperature
    printf("\tSpecified Min Operating Temperature (debug):     %" PRIu64" \n", vFarmFrame[page].environmentPage.minTemp & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Specified Min Operating Temperature
    printf("\tCurrent Relative Humidity (debug):               %" PRIu64" \n", vFarmFrame[page].environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Current Relative Humidity (in units of .1%)
    printf("\tCurrent Motor Power (debug):                     %" PRIu64" \n", vFarmFrame[page].environmentPage.currentMotorPower & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Current Motor Power, value from most recent SMART Summary Frame6 
    printf("\tCurrent 12 volts (debug):                      0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.current12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current12v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current12v)) % 1000));

    printf("\tMinimum 12 volts (debug):                      0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.min12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) % 1000));

    printf("\tMaximum 12 volts (debug):                      0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.max12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) % 1000));


    printf("\tCurrent 5 volts (debug):                       0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.current5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current5v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current5v)) % 1000));

    printf("\tMinimum 5 volts (debug):                       0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.min5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) % 1000));

    printf("\tMaximum 5 volts (debug):                       0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.max5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) / 1000), (M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) % 1000));


    printf("\t12V Power Average (debug):                     0x%" PRIx64" Translation %" PRIu16". % 03" PRId16" \n", vFarmFrame[page].environmentPage.powerAvg12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg12v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerAvg12v % 1000));

    printf("\t12V Power Minimum (debug):                     0x%" PRIx64" Translation %" PRIu16". % 03" PRId16" \n", vFarmFrame[page].environmentPage.powerMin12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin12v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerMin12v % 1000));

    printf("\t12V Power Maximum (debug):                     0x%" PRIx64" Translation %" PRIu16". % 03" PRId16" \n", vFarmFrame[page].environmentPage.powerMax12v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax12v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerMax12v % 1000));

    printf("\t5V Power Average(debug):                       0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.powerAvg5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg5v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerAvg5v % 1000));

    printf("\t5V Power Minimum (debug):                      0x%" PRIx64" Translation %" PRIu16". % 03" PRId16"  \n", vFarmFrame[page].environmentPage.powerMin5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin5v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerMin5v % 1000));

    printf("\t5V Power Maximum (debug):                      0x%" PRIx64" Translation %" PRIu16". % 03" PRId16" \n", vFarmFrame[page].environmentPage.powerMax5v & UINT64_C(0x00FFFFFFFFFFFFFF), \
        static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax5v)) / 1000), static_cast<uint16_t>(vFarmFrame[page].environmentPage.powerMax5v % 1000));
    
#endif
    std::ostringstream temp;
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        temp << "Environment Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Environment Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.curentTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Current Temperature (Celsius)", temp.str().c_str(), vFarmFrame[page].environmentPage.curentTemp, m_showStatusBits);                                //!< Current Temperature in Celsius
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_Byte0(vFarmFrame[page].environmentPage.highestTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Highest Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.highestTemp, m_showStatusBits);                             //!< Highest Temperature in Celsius
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.lowestTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Lowest Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.lowestTemp, m_showStatusBits);                               //!< Lowest Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.averageTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Average Short Term Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.averageTemp, m_showStatusBits);                  //!< Average Short Term Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.averageLongTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Average Long Term Temperatures", temp.str().c_str(), vFarmFrame[page].environmentPage.averageLongTemp, m_showStatusBits);              //!< Average Long Term Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.highestShortTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Highest Average Short Term Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.highestShortTemp, m_showStatusBits);     //!< Highest Average Short Term Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.lowestShortTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Lowest Average Short Term Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.lowestShortTemp, m_showStatusBits);       //!< Lowest Average Short Term Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.highestLongTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Highest Average Long Term Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.highestLongTemp, m_showStatusBits);       //!< Highest Average Long Term Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.lowestLongTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Lowest Average Long Term Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.lowestLongTemp, m_showStatusBits);         //!< Lowest Average Long Term Temperature

    set_json_64_bit_With_Status(pageInfo, "Time In Over Temperature", vFarmFrame[page].environmentPage.overTempTime,false, m_showStatusBits);                                                //!< Time In Over Temperature
    set_json_64_bit_With_Status(pageInfo, "Time In Under Temperature", vFarmFrame[page].environmentPage.underTempTime, false, m_showStatusBits);                                              //!< Time In Under Temperature
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.maxTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Specified Max Operating Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.maxTemp, m_showStatusBits);                 //!< Specified Max Operating Temperature

    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.minTemp)*1.00);
    set_json_string_With_Status(pageInfo, "Specified Min Operating Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.minTemp, m_showStatusBits);                 //!< Specified Min Operating Temperature

    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << static_cast<double>((check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidity)))*0.1;                                                    //!< Current Relative Humidity (in units of .1%)
    set_json_string_With_Status(pageInfo, "Current Relative Humidity", temp.str().c_str(), vFarmFrame[page].environmentPage.humidity, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Current Motor Power", vFarmFrame[page].environmentPage.currentMotorPower,false, m_showStatusBits);                                                //!< Current Motor Power, value from most recent SMART Summary Frame6

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current12v)) % 1000;
    set_json_string_With_Status(pageInfo, "Current 12 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.current12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) % 1000);
    set_json_string_With_Status(pageInfo, "Minimum 12 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.min12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) % 1000);
    set_json_string_With_Status(pageInfo, "Maximum 12 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.max12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.current5v)) % 1000);
    set_json_string_With_Status(pageInfo, "Current 5 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.current5v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) % 1000;
    set_json_string_With_Status(pageInfo, "Minimum 5 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.min5v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) % 1000);
    set_json_string_With_Status(pageInfo, "Maximum 5 volts", temp.str().c_str(), vFarmFrame[page].environmentPage.max5v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg12v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg12v)) % 1000;
    set_json_string_With_Status(pageInfo, "12V Power Average", temp.str().c_str(), vFarmFrame[page].environmentPage.powerAvg12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin12v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin12v)) % 1000;
    set_json_string_With_Status(pageInfo, "12V Power Minimum", temp.str().c_str(), vFarmFrame[page].environmentPage.powerMin12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax12v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax12v)) % 1000;
    set_json_string_With_Status(pageInfo, "12V Power Maximum", temp.str().c_str(), vFarmFrame[page].environmentPage.powerMax12v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg5v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerAvg5v)) % 1000;
    set_json_string_With_Status(pageInfo, "5V Power Average",temp.str().c_str(), vFarmFrame[page].environmentPage.powerAvg5v, m_showStatusBits);
    
    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin5v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMin5v)) % 1000;
    set_json_string_With_Status(pageInfo, "5V Power Minimum", temp.str().c_str(), vFarmFrame[page].environmentPage.powerMin5v, m_showStatusBits);

    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax5v)) / 1000 << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.powerMax5v)) % 1000;
    set_json_string_With_Status(pageInfo, "5V Power Maximum", temp.str().c_str(), vFarmFrame[page].environmentPage.powerMax5v, m_showStatusBits);
 
    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Reli_Information()
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
    std::string myStr;
    JSONNODE *pageInfo = json_new(JSON_NODE);

#if defined _DEBUG

    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        printf("\nReliability Information From Farm Log copy FACTORY");
    }
    else
    {
        printf("\nReliability Information From Farm Log copy %" PRIu32"\n", page);
    }
    printf("\tNumber of DOS Scans Performed(debug):            %" PRIu64" \n", vFarmFrame[page].reliPage.numberDOSScans & UINT64_C(0x00FFFFFFFFFFFFFF));                            //!< Number of DOS Scans Performed
    printf("\tNumber of LBAs Corrected by ISP(debug):          %" PRIu64" \n", vFarmFrame[page].reliPage.numberLBACorrect & UINT64_C(0x00FFFFFFFFFFFFFF));                          //!< Number of LBAs Corrected by ISP
    printf("\tError Rate (SMART Attribute 1 Raw)(debug):       0x%016" PRIx64"\n", vFarmFrame[page].reliPage.attrErrorRateRaw & UINT64_C(0x00FFFFFFFFFFFFFF));                      //!< Error Rate (SMART Attribute 1 Raw)
    printf("\tError Rate (SMART Attribute 1 Normalized)(debug):%" PRIu64" \n", vFarmFrame[page].reliPage.attrErrorRateNormal & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Error Rate (SMART Attribute 1 Normalized)
    printf("\tError Rate (SMART Attribute 1 Worst)(debug):     %" PRIu64" \n", vFarmFrame[page].reliPage.attrErrorRateWorst & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Error Rate (SMART Attribute 1 Worst)
    printf("\tSeek Error Rate (SMART Attr 7 Raw)(debug):       0x%016" PRIx64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateRaw & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Seek Error Rate (SMART Attribute 7 Raw)
    printf("\tSeek Error Rate (SMART Attr 7 Normalized)(debug):%" PRIu64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateNormal & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Seek Error Rate (SMART Attribute 7 Normalized)
    printf("\tSeek Error Rate (SMART Attr 7 Worst)(debug):     %" PRIu64" \n", vFarmFrame[page].reliPage.attrSeekErrorRateWorst & UINT64_C(0x00FFFFFFFFFFFFFF));                    //!< Seek Error Rate (SMART Attribute 7 Worst)
    printf("\tHigh Priority Unload Events (Raw) (debug):       0x%016" PRIx64" \n", vFarmFrame[page].reliPage.attrUnloadEventsRaw & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< High Priority Unload Events (SMART Attribute 192 Raw)
    printf("\tNumber of LBAs Corrected by Parity Sector (debug)%" PRIu64" \n", vFarmFrame[page].reliPage.numberLBACorrectedByParitySector & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Number of LBAs Corrected by Parity Sector
    printf("\tPrimary Super Parity Coverage Percentage - Actuator 0%" PRIu64" \n", vFarmFrame[page].reliPage.SuperParityCovPercent & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Primary Super Parity Coverage Percentage
    printf("\tNumber of LBAs Corrected by ISP - Actuator 1%" PRIu64" \n", vFarmFrame[page].reliPage.correctedLBAsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));                              //!< Number of LBAs Corrected by Parity Sector
           
 #endif
    std::ostringstream temp;
    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        temp << "Reliability Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Reliability Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());
  
    set_json_int_With_Status(pageInfo, "Number of DOS Scans Performed", vFarmFrame[page].reliPage.numberDOSScans,  m_showStatusBits);                                       //!< Number of DOS Scans Performed
    set_json_int_With_Status(pageInfo, "Number of LBAs Corrected by ISP", vFarmFrame[page].reliPage.numberLBACorrect, m_showStatusBits);                                    //!< Number of LBAs Corrected by ISP
    set_json_int_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Raw)", vFarmFrame[page].reliPage.attrErrorRateRaw, m_showStatusBits);                                 //!< Error Rate (SMART Attribute 1 Raw)
    set_json_int_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Normalized)", vFarmFrame[page].reliPage.attrErrorRateNormal, m_showStatusBits);                       //!< Error Rate (SMART Attribute 1 Normalized)
    set_json_int_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Worst)", vFarmFrame[page].reliPage.attrErrorRateWorst, m_showStatusBits);                             //!< Error Rate (SMART Attribute 1 Worst)
    set_json_int_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Raw)", vFarmFrame[page].reliPage.attrSeekErrorRateRaw, m_showStatusBits);                             //!< Seek Error Rate (SMART Attribute 7 Raw)
    set_json_int_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Normalized)", vFarmFrame[page].reliPage.attrSeekErrorRateNormal, m_showStatusBits);                   //!< Seek Error Rate (SMART Attribute 7 Normalized)
    set_json_int_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Worst)", vFarmFrame[page].reliPage.attrSeekErrorRateWorst, m_showStatusBits);                         //!< Seek Error Rate (SMART Attribute 7 Worst)
    set_json_int_With_Status(pageInfo, "High Priority Unload Events", vFarmFrame[page].reliPage.attrUnloadEventsRaw, m_showStatusBits);                                     //!< High Priority Unload Events (SMART Attribute 192 Raw)
    set_json_int_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame[page].reliPage.diskSlipRecalPerformed, m_showStatusBits);                 //!< Number of disc slip recalibrations performed
    set_json_int_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame[page].reliPage.heliumPresureTrip, m_showStatusBits);                                 //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)//!< idle Time, Value from most recent SMART Summary Frame

    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector - Actuator 0", vFarmFrame[page].reliPage.numberLBACorrectedByParitySector, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage - Actuator 0", vFarmFrame[page].reliPage.SuperParityCovPercent, false, m_showStatusBits);
    
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage SMR SWR - Actuator 0", vFarmFrame[page].reliPage.superParityCoveragePercentageAct0, false, m_showStatusBits);           //!< Primary Super Parity Coverage Percentage SMR/SWR- Actuator 0 
    
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP - Actuator 1", vFarmFrame[page].reliPage.correctedLBAsAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector - Actuator 1", vFarmFrame[page].reliPage.numberLBACorrectedByParitySectorAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage SMR SWR - Actuator 1", vFarmFrame[page].reliPage.superParityCoveragePercentageAct1, false, m_showStatusBits);
  
    json_push_back(masterData, pageInfo);
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Head_Information()
//
//! \brief
//!   Description:  print out the Reli head log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Head_Information(JSONNODE *masterData, uint32_t page)
{
    uint32_t loopCount = 0;
    std::string myHeader;
    JSONNODE *headInfo = json_new(JSON_NODE);
#if defined _DEBUG
    
    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        printf("\n Head Information From Farm Log copy FACTORY");
    }
    else
    {
        printf("\n Head Information From Farm Log copy %" PRIu32"\n", page);
    }



    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDVGA Skip Write Detect by Head %d:        %" PRIu64"(debug) \n", loopCount, vFarmFrame[page].reliPage.DVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] DVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tRVGA Skip Write Detect by Head %d:         %" PRIu64" (debug)\n", loopCount, vFarmFrame[page].reliPage.RVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] RVGA Skip Write Detect by Head7
    }

    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tFVGA Skip Write Detect by Head %d:        %" PRIu64" (debug)\n", loopCount, vFarmFrame[page].reliPage.FVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< [24] FVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tSkip Write Detect Threshold Exceeded by Head %d:  %" PRIu64" (debug)\n", loopCount, vFarmFrame[page].reliPage.skipWriteDetectThresExceeded[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] Skip Write Detect Threshold Exceeded Count by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tMR Head Resistance from Head %d:          %" PRIu64" (debug) \n", loopCount, vFarmFrame[page].reliPage.MRHeadResistance[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tVelocity Observer by Head %d:             %" PRIu64"(debug) \n", loopCount, vFarmFrame[page].reliPage.velocityObserver[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tNumber of Velocity Observer by Head %d:   %" PRIu64" (debug)\n", loopCount, vFarmFrame[page].reliPage.numberOfVelocityObserver[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tFly height outer clearance delta by Head  %d:   raw 0x%" PRIx64" outer, calculated %0.02lf (debug)\n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer, \
            (M_WordInt0((static_cast<uint64_t>(check_Status_Strip_Status(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer)))) * .001));                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        printf("\tFly height inner clearance delta by Head  %d:   raw 0x%" PRIx64" inner, calculated %0.02lf (debug)\n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner, \
            (M_WordInt0((static_cast<uint64_t>(check_Status_Strip_Status(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner)))) * .001));                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        printf("\tFly height middle clearance delta by Head  %d:   raw 0x%" PRIx64" middle, calculated %0.02lf (debug)\n", loopCount, vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle, \
            (M_WordInt0(static_cast<uint64_t>(check_Status_Strip_Status((vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle)))) * .001));               //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    printf("\tNumber of disc slip recalibrations performed:  %" PRId64" (debug)\n", vFarmFrame[page].reliPage.diskSlipRecalPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Number of disc slip recalibrations performed
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tNumber of Reallocated Sectors by Head %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.gList[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));                //!< [24] Number of Resident G-List per Head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tNumber of pending Entries by Head %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.pendingEntries[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< [24] Number of pending Entries per Head
    }
    printf("\tHelium Pressure Threshold Tripped:             %" PRId64" (debug)\n", vFarmFrame[page].reliPage.heliumPresureTrip & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS Ought to scan count by Head  %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.oughtDOS[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< [24] DOS Ought to scan count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS needs to scans count by Head  %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.needDOS[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< [24] DOS needs to scans count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\tDOS write Fault scans by Head  %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.writeDOSFault[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< [24] DOS  write Fault scans per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        printf("\twrite POS in sec value by Head  %d:  %" PRId64" (debug)\n", loopCount, vFarmFrame[page].reliPage.writePOH[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< [24] write POS in sec value from most recent SMART Frame by head
    }

#endif
    std::ostringstream temp;
    if (vFarmFrame[page].reliPage.copyNumber == FACTORYCOPY)
    {
        temp << "Head Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Head Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(headInfo, temp.str().c_str());

    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "DVGA Skip Write Detect by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.DVGASkipWriteDetect[loopCount], m_showStatusBits);              //!< [24] DVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "RVGA Skip Write Detect by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.RVGASkipWriteDetect[loopCount],  m_showStatusBits);              //!< [24] RVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "FVGA Skip Write Detect by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.FVGASkipWriteDetect[loopCount],  m_showStatusBits);              //!< [24] FVGA Skip Write Detect by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Skip Write Detect Threshold Exceeded by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.skipWriteDetectThresExceeded[loopCount], m_showStatusBits);    //!< [24] Skip Write Detect Threshold Exceeded Count by Head7
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "MR Head Resistance for Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.MRHeadResistance[loopCount], m_showStatusBits);                     //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Velocity Observer by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.velocityObserver[loopCount], m_showStatusBits);                     //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Servo Velocity No Timing Mark Detect by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.numberOfVelocityObserver[loopCount], m_showStatusBits);             //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head9,10
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Fly height clearance delta outer by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_WordInt0(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer) * 0.001);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].outer), m_showStatusBits);    //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Fly height clearance delta inner by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_WordInt0(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner) * 0.001);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].inner), m_showStatusBits);    //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Fly height clearance delta middle by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_WordInt0(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle) * 0.001);;
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.flyHeightClearance[loopCount].middle), m_showStatusBits);    //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT trimmed mean bits in error Zone 0 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone0) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone0), m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT trimmed mean bits in error Zone 1 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone1) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone1), m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT trimmed mean bits in error Zone 2 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone2) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SAT[loopCount].zone2), m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT iterations to converge Test Zone 0 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone0) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone0), m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT iterations to converge Test Zone 1 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone1) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone1), m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT iterations to converge Test Zone 2 by Head " << std::dec << loopCount;// Head count
        double number = static_cast<double>(M_Word0(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone2) * 0.10);
        set_json_float_With_Status(headInfo, temp.str().c_str(), number, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SATIterations[loopCount].zone2), m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT percentage of codewords at iteration level by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.currentH2SATPercentage[loopCount], m_showStatusBits);               //!< [24] Qword[24] Current H2SAT percentage of codewords at iteration level by Head, averaged
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT amplitude by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.currentH2SATamplitude[loopCount], m_showStatusBits);                //!< [24] Qword[24] Current H2SAT amplitude by Head, averaged across Test Zones 9
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Current H2SAT asymmetry by Head " << std::dec << loopCount;// Head count
        set_json_float_With_Status(headInfo, temp.str(), static_cast<double>(M_WordInt0(vFarmFrame[page].reliPage.currentH2SATasymmetry[loopCount])) * 0.1, static_cast<uint64_t>(vFarmFrame[page].reliPage.currentH2SATasymmetry[loopCount]), m_showStatusBits);
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Number of Reallocated Sectors by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.gList[loopCount], m_showStatusBits);                            //!< [24] Number of Reallocated Sectors per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Number of Reallocation Candidate Sectors by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.pendingEntries[loopCount], m_showStatusBits);                   //!< [24] Number of Reallocation Candidate Sectors per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "DOS Ought to scan count by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.oughtDOS[loopCount], m_showStatusBits);                         //!< [24] DOS Ought to scan count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "DOS needs to scans count by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.needDOS[loopCount], m_showStatusBits);                          //!< [24] DOS needs to scans count per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "DOS write Fault scans by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.writeDOSFault[loopCount], m_showStatusBits);                    //!< [24] DOS  write Fault scans per head
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Write Power On (hrs) by Head " << std::dec << loopCount;// Head count
        set_json_float_With_Status(headInfo, temp.str().c_str(), static_cast<double>(M_DoubleWord0(vFarmFrame[page].reliPage.writePOH[loopCount])) / 3600, static_cast<uint64_t>(vFarmFrame[page].reliPage.writePOH[loopCount]), m_showStatusBits);
    }
    for (loopCount = 0; loopCount < m_heads; ++loopCount)
    {
        temp.str("");temp.clear();
        temp << "Second MR Head Resistance by Head " << std::dec << loopCount;// Head count
        set_json_int_With_Status(headInfo, temp.str(), vFarmFrame[page].reliPage.secondMRHeadResistance[loopCount], m_showStatusBits);                   //!< [24] DOS Write Count Threshold per head
    }


    json_push_back(masterData, headInfo);
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn printAllPages()
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
    if ((m_pHeader->signature & UINT64_C(0x00FFFFFFFFFFFFFF)) == FARMEMPTYSIGNATURE || \
        (m_pHeader->signature & UINT64_C(0x00FFFFFFFFFFFFFF)) == FARMPADDINGSIGNATURE)
    {
        json_push_back(masterData, json_new_a("Empty FARM Log", "data has not yet been gathered"));
    }
    else
    {
        print_Header(masterData);
        for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
        {
                // #if defined _DEBUG
                print_Drive_Information(masterData, index);
                print_Work_Load(masterData, index);
                // #endif
                print_Error_Information(masterData, index);
                print_Enviroment_Information(masterData, index);
                print_Reli_Information(masterData, index);
                print_Head_Information(masterData, index);
        }
    }
}
//----------------------------------------------------------------------------
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
        print_Drive_Information(masterData, page);
        print_Work_Load(masterData, page);
        print_Error_Information(masterData, page);
        print_Enviroment_Information(masterData, page);
        print_Reli_Information(masterData, page);
        print_Head_Information(masterData, page);
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
        print_Head_Information(masterData, page);
    }

}
//-----------------------------------------------------------------------------
//
//! \fn print_Page_One_Node()
//
//! \brief
//!   Description:  print out all the pages and the copies of each page but within same node
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CATA_Farm_Log::print_Page_One_Node(JSONNODE * masterData)
{
    if (vFarmFrame.size() > 0)
    {
        JSONNODE* pageInfo = json_new(JSON_NODE);
        if ((m_pHeader->signature & UINT64_C(0x00FFFFFFFFFFFFFF)) == FARMEMPTYSIGNATURE || \
            (m_pHeader->signature & UINT64_C(0x00FFFFFFFFFFFFFF)) == FARMPADDINGSIGNATURE)
        {
            json_push_back(masterData, json_new_a("Empty FARM Log", "data has not yet been gathered"));
        }
        else
        {
            json_set_name(pageInfo, "FARM Log");
            print_All_Pages(pageInfo);
            json_push_back(masterData, pageInfo);
        }
    }
}
