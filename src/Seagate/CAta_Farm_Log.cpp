//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
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
    : CFarmCommon()
    , m_totalPages(0)
    , m_logSize(0)
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_showStatusBits(false)
    , m_pHeader()
    , pBuf()
    , m_MajorRev(0)
    , m_MinorRev(0)
    , m_FrameReason(0)
    , m_ShowAct1(false)
    , m_showStatic(false)
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
CATA_Farm_Log::CATA_Farm_Log(uint8_t *bufferData, size_t bufferSize, bool showStatus, bool showStatic)
    : CFarmCommon()
    , m_totalPages(0)
    , m_logSize(0)
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_showStatusBits(showStatus)
    , m_pHeader()
    , pBuf()
    , m_MajorRev(0)
    , m_MinorRev(0)
    , m_FrameReason(0)
    , m_ShowAct1(false)
    , m_showStatic(showStatic)
{
    pBuf = new uint8_t[bufferSize];                             // new a buffer to the point                
#ifndef __STDC_SECURE_LIB__
    memcpy(pBuf, bufferData, bufferSize);
#else
    memcpy_s(pBuf, bufferSize, bufferData, bufferSize);         // copy the buffer data to the class member pBuf
#endif
    if (pBuf != NULL)
    {
        if (bufferSize < sizeof(sFarmHeader) || bufferSize < sizeof(sFarmFrame))
        {

            m_status = eReturnValues::BAD_PARAMETER;
        }
        else
        {
            m_pHeader = reinterpret_cast<sFarmHeader *>(&pBuf[0]);
            m_totalPages = m_pHeader->pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_logSize = m_pHeader->logSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_pageSize = m_pHeader->pageSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_heads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_MaxHeads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_copies = m_pHeader->reserved & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_status = eReturnValues::IN_PROGRESS;
            m_MajorRev = M_DoubleWord0(m_pHeader->majorRev);
            m_MinorRev = M_DoubleWord0(m_pHeader->minorRev);
            m_FrameReason = M_Byte0(m_pHeader->reasonForFrameCapture);
        }
    }
    else
    {
        m_status = eReturnValues::FAILURE;
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
//!   \return eReturnValues::SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
bool CATA_Farm_Log::Check_Page_number(uint64_t page, uint16_t pageNumber)
{
    if (pageNumber == M_Word0(page))
        return true;
    else
    {
        if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
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
//!   \return eReturnValues::SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::parse_Farm_Log()
{
    eReturnValues retStatus = eReturnValues::FAILURE;
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
        retStatus = eReturnValues::IN_PROGRESS;
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
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }

            offset += m_pageSize;

            sWorkLoadStat *pworkLoad = reinterpret_cast<sWorkLoadStat*>(&pBuf[offset]);           // get the work load information
            memcpy(&pFarmFrame->workLoadPage, pworkLoad, sizeof(sWorkLoadStat));
            if (!Check_Page_number(pworkLoad->pageNumber, 2))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sErrorStat *pError = reinterpret_cast<sErrorStat*>(&pBuf[offset]);                    // get the error status
            memcpy(&pFarmFrame->errorPage, pError, sizeof(sErrorStat));
            if (!Check_Page_number(pError->pageNumber, 3))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sEnvironementStat *pEnvironment = reinterpret_cast<sEnvironementStat*>(&pBuf[offset]); // get the envirmonent information 
            memcpy(&pFarmFrame->environmentPage, pEnvironment, sizeof(sEnvironementStat));
            if (!Check_Page_number(pEnvironment->pageNumber, 4))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sAtaReliabilityStat *pReli = reinterpret_cast<sAtaReliabilityStat*>(&pBuf[offset]);         // get the Reliabliity stat
            memcpy(&pFarmFrame->reliPage, pReli, sizeof(sAtaReliabilityStat));
            if (!Check_Page_number(pReli->pageNumber, 5))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;                                                  // add another page size. I think there is only on header
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
        }
        
    }
    if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)                                     // checking for an empty log aka all FFFF's
    {
        retStatus = eReturnValues::IN_PROGRESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Header(JSONNODE* masterData)
{
    JSONNODE* pageInfo = json_new(JSON_NODE);
    sFarmHeader* header = reinterpret_cast<sFarmHeader*>(&pBuf[0]);                                                                                    // pointer to the header to get the signature

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        printf("\n\nFARM Log Header \n");
        printf("\tLog Signature:                                                0x%" PRIX64" \n", header->signature & UINT64_C(0x00FFFFFFFFFFFFFF));                      //!< Log Signature = 0x00004641524D4552
        printf("\tMajor Revision:                                               %" PRIu64"  \n", header->majorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Log Major rev
        printf("\tMinor Revision:                                               %" PRIu64"  \n", header->minorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< minor rev 
        printf("\tPages Supported:                                              %" PRIu64"  \n", header->pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< number of pages supported
        printf("\tLog Size:                                                     %" PRIu64"  \n", header->logSize & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< log size in bytes
        printf("\tPage Size:                                                    %" PRIu64"  \n", header->pageSize & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< page size in bytes
        printf("\tHeads Supported:                                              %" PRIu64"  \n", header->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Maximum Drive Heads Supported
        printf("\tReason for Frame Capture:                                     %" PRIu64"  \n", header->reasonForFrameCapture & UINT64_C(0x00FFFFFFFFFFFFF));           //!< Reason for Frame Capture
    }

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
    set_json_64_bit_With_Status(pageInfo, "Reason for Frame Capture", header->reasonForFrameCapture, false, m_showStatusBits);
    std::string reason;
    Get_FARM_Reason_For_Capture(&reason, M_Byte0(header->reasonForFrameCapture));
    json_push_back(pageInfo, json_new_a("Reason Meaning", reason.c_str()));


    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Drive_Information(JSONNODE *masterData, uint32_t page)
{
#define SHORTHEADS 20
    std::ostringstream myStr;
    JSONNODE *pageInfo = json_new(JSON_NODE);

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
        {
            printf("\nDrive Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nDrive Information From Farm Log copy %" PRIu32" \n", page);
        }
        printf("\tSerial Number:                                                %s         \n", vFarmFrame.at(page).identStringInfo.serialNumber.c_str());
        printf("\tWorld Wide Name:                                              %s         \n", vFarmFrame.at(page).identStringInfo.worldWideName.c_str());
        printf("\tfirmware Rev:                                                 %s         \n", vFarmFrame.at(page).identStringInfo.firmwareRev.c_str());                                                 //!< Firmware Revision [0:3]
        printf("\tDevice Interface:                                             %s         \n", vFarmFrame.at(page).identStringInfo.deviceInterface.c_str());
        printf("\tDevice Capacity in sectors:                                   %" PRId64" \n", (vFarmFrame.at(page).driveInfo.deviceCapacity & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tPhysical Sector size:                                         %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.psecSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< Physical Sector Size in Bytes
        printf("\tLogical Sector Size:                                          %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.lsecSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< Logical Sector Size in Bytes
        printf("\tDevice Buffer Size:                                           %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.deviceBufferSize & UINT64_C(0x00FFFFFFFFFFFFFF)));                          //!< Device Buffer Size in Bytes
        printf("\tNumber of heads:                                              %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.heads & UINT64_C(0x00FFFFFFFFFFFFFF)));                                     //!< Number of Heads
        printf("\tDevice form factor:                                           %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.factor & UINT64_C(0x00FFFFFFFFFFFFFF)));                                    //!< Device Form Factor (ID Word 168)                                        
        printf("\tRotation Rate:                                                %" PRIu64"  \n", (vFarmFrame.at(page).driveInfo.rotationRate & UINT64_C(0x00FFFFFFFFFFFFFF)));                             //!< Rotational Rate of Device (ID Word 217)
        printf("\tATA Features Supported (ID Word 78):                          0x%04" PRIX64" \n", (vFarmFrame.at(page).driveInfo.featuresSupported & UINT64_C(0x00FFFFFFFFFFFFFF)));                     //!< ATA Features Supported (ID Word 78)
        printf("\tATA Features Enabled (ID Word 79):                            0x%04" PRIX64" \n", (vFarmFrame.at(page).driveInfo.featuresEnabled & UINT64_C(0x00FFFFFFFFFFFFFF)));                       //!< ATA Features Enabled (ID Word 79)
        printf("\tATA Security State (ID Word 128):                             0x%04" PRIX64" \n", (vFarmFrame.at(page).driveInfo.security & UINT64_C(0x00FFFFFFFFFFFFFF)));                              //!< ATA Security State (ID Word 128)
        printf("\tPower on Hours:                                               %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.poh & UINT64_C(0x00FFFFFFFFFFFFFF)));                                       //!< Power-on Hour
        printf("\tSpindle Power on hours:                                       %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.spoh & UINT64_C(0x00FFFFFFFFFFFFFF)));                                      //!< Spindle Power-on Hours
        printf("\tHead Flight Hours - Actuator 0:                               %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.headFlightHoursAct0 & UINT64_C(0x00FFFFFFFFFFFFFF)));                      //!< Head Flight Hours
        printf("\tHead Load Events - Actuator 0:                                %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.headLoadEventsAct0 & UINT64_C(0x00FFFFFFFFFFFFFF)));                       //!< Head Load Events
        printf("\tPower Cycle count:                                            %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.powerCycleCount & UINT64_C(0x00FFFFFFFFFFFFFF)));                           //!< Power Cycle Count
        printf("\tHardware Reset count:                                         %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.resetCount & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Hardware Reset Count
        printf("\tSpin-up Time:                                                 %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.spinUpTime & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< SMART Spin-Up time in millisecond
        printf("\tLowest timestamp of POH for frame (ms):                       %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.timeStamp1 & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
        printf("\tHighest timestamp of POH for frame (ms):                      %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.timeStamp2 & UINT64_C(0x00FFFFFFFFFFFFFF)));                                //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1
        printf("\tTime to ready of the last power cycle (ms):                   %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.timeToReady & UINT64_C(0x00FFFFFFFFFFFFFF)));                               //!< time to ready of the last power cycle
        printf("\tTime drive is held in staggered spin (ms):                    %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.timeHeld & UINT64_C(0x00FFFFFFFFFFFFFF)));                                  //!< time drive is held in staffered spin during the last power on sequence
        printf("\tDepopulation Head Mask:                                       %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.depopulationHeadMask & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tHead Flight Hours - Actuator 1:                               %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.headFlightHoursAct1 & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tHead Load Events - Actuator 1:                                %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.headLoadEventsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tDrive Recording Type:                                         %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.driveRecordingType & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tDrive been Depopped:                                          %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.depopped & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tMax number of sectors for reassingment:                       %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.maxNumberForReasign & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tDate of Assembly:                                             %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.dateOfAssembly & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tHAMR Data Protect Status:                                     %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.HAMRProtectStatus & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tRegen Head Mask:                                              %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.regenHeadMask & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tPOH most recent FARM time series save:                        %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.POHMostRecentSave & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tPOH second most recent FARM time series save:                 %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.POHSecondMostRecentSave & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tSequential Read before required for active zone configuration:%" PRIu64" \n", (vFarmFrame.at(page).driveInfo.SeqActiveZone & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tSequential Write Required for active zone configuration:      %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.SeqWriteActiveZone & UINT64_C(0x00FFFFFFFFFFFFFF)));
        printf("\tNumber of LBAs (HSMR SWR capacity):                           %" PRIu64" \n", (vFarmFrame.at(page).driveInfo.numLBA & UINT64_C(0x00FFFFFFFFFFFFFF)));
        for (uint32_t i = 0; i < MAX_HEAD_COUNT; i++)
        {
            printf("\tGet Physical Element Status for head %2" PRIu32":                     % " PRIi64" \n", i, (vFarmFrame.at(page).driveInfo.gpes[i] & INT64_C(0x00FFFFFFFFFFFFFF)));
        }
    }

    std::ostringstream temp;

    // check the level of print form the eToolVerbosityLevels 
    if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
    {
        temp << "Drive Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Drive Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    json_push_back(pageInfo, json_new_a("Serial Number", vFarmFrame.at(page).identStringInfo.serialNumber.c_str()));                                               //!< serial number of the device
    json_push_back(pageInfo, json_new_a("Model Number", vFarmFrame.at(page).identStringInfo.modelNumber.c_str()));                                                 //!< model Number  only on 3.0 and higher 
    json_push_back(pageInfo, json_new_a("World Wide Name", vFarmFrame.at(page).identStringInfo.worldWideName.c_str()));                                            //!< world wide Name
    json_push_back(pageInfo, json_new_a("Device Interface", vFarmFrame.at(page).identStringInfo.deviceInterface.c_str()));                                         //!< Device Interface
    set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame.at(page).driveInfo.poh, false, m_showStatusBits);                                            //!< Power-on Hour
    set_json_64_bit_With_Status(pageInfo, "Device Capacity in Sectors", vFarmFrame.at(page).driveInfo.deviceCapacity, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame.at(page).driveInfo.psecSize, false, m_showStatusBits);                                //!< Physical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame.at(page).driveInfo.lsecSize, false, m_showStatusBits);                                 //!< Logical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame.at(page).driveInfo.deviceBufferSize, false, m_showStatusBits);                          //!< Device Buffer Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame.at(page).driveInfo.heads, false, m_showStatusBits);                                        //!< Number of Heads
    if (check_Status_Strip_Status(vFarmFrame.at(page).driveInfo.heads) != 0)
    {
        if (!g_parseNULL)
        {
            m_heads = check_Status_Strip_Status(vFarmFrame.at(page).driveInfo.heads);
        }
        else
        {
            m_heads = SHORTHEADS;
        }
    }
    set_json_64_bit_With_Status(pageInfo, "Device form factor", vFarmFrame.at(page).driveInfo.factor, false, m_showStatusBits);                                    //!< Device Form Factor (ID Word 168)
    set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame.at(page).driveInfo.rotationRate, false, m_showStatusBits);
    json_push_back(pageInfo, json_new_a("Firmware Rev", vFarmFrame.at(page).identStringInfo.firmwareRev.c_str()));                                                 //!< Firmware Revision [0:3]
    set_json_64_bit_With_Status(pageInfo, "ATA Security State (ID Word 128)", vFarmFrame.at(page).driveInfo.security, true, m_showStatusBits);                     //!< ATA Security State (ID Word 128)
    set_json_64_bit_With_Status(pageInfo, "ATA Features Supported (ID Word 78)", vFarmFrame.at(page).driveInfo.featuresSupported, true, m_showStatusBits);         //!< ATA Features Supported (ID Word 78)
    set_json_64_bit_With_Status(pageInfo, "ATA Features Enabled (ID Word 79)", vFarmFrame.at(page).driveInfo.featuresEnabled, true, m_showStatusBits);             //!< ATA Features Enabled (ID Word 79)
    set_json_64_bit_With_Status(pageInfo, "Spindle Power on hours", vFarmFrame.at(page).driveInfo.spoh, false, m_showStatusBits);                                  //!< Spindle Power-on Hours
    set_json_64_bit_With_Status(pageInfo, "Head Flight Hours - Actuator 0", vFarmFrame.at(page).driveInfo.headFlightHoursAct0, false, m_showStatusBits);           //!< Head Flight Hours
    set_json_64_bit_With_Status(pageInfo, "Head Load Events - Actuator 0", vFarmFrame.at(page).driveInfo.headLoadEventsAct0, false, m_showStatusBits);             //!< Head Load Events
    set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame.at(page).driveInfo.powerCycleCount, false, m_showStatusBits);                            //!< Power Cycle Count
    set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame.at(page).driveInfo.resetCount, false, m_showStatusBits);                              //!< Hardware Reset Count
    set_json_64_bit_With_Status(pageInfo, "Spin-up Time", vFarmFrame.at(page).driveInfo.spinUpTime, false, m_showStatusBits);                                      //!< SMART Spin-Up time in milliseconds

    set_json_64_bit_With_Status(pageInfo, "Lowest timestamp of POH for frame (ms)", vFarmFrame.at(page).driveInfo.timeStamp1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Highest timestamp of POH for frame (ms)", vFarmFrame.at(page).driveInfo.timeStamp2, false, m_showStatusBits);      //!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds

    double tReady = static_cast<double>(M_DoubleWord0(vFarmFrame.at(page).driveInfo.timeToReady)) / 1000;
    set_json_float_With_Status(pageInfo, "Time to ready of the last power cycle (sec)", tReady, vFarmFrame.at(page).driveInfo.timeToReady, m_showStatusBits);         //!< time to ready of the last power cycle in milliseconds

    double spin = static_cast<double>(M_DoubleWord0(vFarmFrame.at(page).driveInfo.timeHeld)) / 1000;
    set_json_float_With_Status(pageInfo, "Time drive is held in staggered spin (sec)", spin, vFarmFrame.at(page).driveInfo.timeHeld, m_showStatusBits);

    myStr << "Drive Recording Type";
    std::string type = "not supported";
    if (check_For_Active_Status(&vFarmFrame.at(page).driveInfo.driveRecordingType))
    {
        type = "CMR";
        if (vFarmFrame.at(page).driveInfo.driveRecordingType & BIT0)
        {
            type = "SMR";
        }
    }

    set_json_string_With_Status(pageInfo, myStr.str().c_str(), type.c_str(), vFarmFrame.at(page).driveInfo.driveRecordingType, m_showStatusBits);
    myStr.str(""); myStr.clear();
    myStr<< "Has Drive been Depopped";
    if (check_Status_Strip_Status(vFarmFrame.at(page).driveInfo.depopped) != 0)
    {
        set_Json_Bool(pageInfo, myStr.str().c_str(), true);
    }
    else
    {
        set_Json_Bool(pageInfo, myStr.str().c_str(), false);
    }

    set_json_64_bit_With_Status(pageInfo, "Max Number of Available Sectors for Reassignment", vFarmFrame.at(page).driveInfo.maxNumberForReasign, false, m_showStatusBits);          //!< Max Number of Available Sectors for Reassignment � Value in disc sectors(started in 3.3 )

    //!< Date of Assembly in ASCII 
    if (check_For_Active_Status(&vFarmFrame.at(page).driveInfo.dateOfAssembly))
    {
        uint16_t week = M_Word1(vFarmFrame.at(page).driveInfo.dateOfAssembly);
        uint16_t year = M_Word0(vFarmFrame.at(page).driveInfo.dateOfAssembly);
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
    set_json_64_bit_With_Status(pageInfo, "Depopulation Head Mask", vFarmFrame.at(page).driveInfo.depopulationHeadMask, false, m_showStatusBits);              //!< Depopulation Head Mask

    //version 4.21

    set_json_64_bit_With_Status(pageInfo, "Head Flight Hours - Actuator 1", vFarmFrame.at(page).driveInfo.headFlightHoursAct1, false, m_showStatusBits);       //!< Head Flight Hours- Actuator 1
    set_json_64_bit_With_Status(pageInfo, "Head Load Events - Actuator 1", vFarmFrame.at(page).driveInfo.headLoadEventsAct1, false, m_showStatusBits);         //!< Head Load Events- Actuator 1
    set_json_bool_With_Status(pageInfo, "HAMR Data Protect Status", vFarmFrame.at(page).driveInfo.HAMRProtectStatus, m_showStatusBits);
    set_json_bool_With_Status(pageInfo, "Regen Head Mask", vFarmFrame.at(page).driveInfo.regenHeadMask, m_showStatusBits);
    // version 4.41
    set_json_64_bit_With_Status(pageInfo, "POH most recent FARM time series save", vFarmFrame.at(page).driveInfo.POHMostRecentSave, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "POH second most recent FARM time series save", vFarmFrame.at(page).driveInfo.POHSecondMostRecentSave, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Sequential Read Before Required for active zone configuration", vFarmFrame.at(page).driveInfo.SeqActiveZone, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Sequential Write Required for active zone configuration", vFarmFrame.at(page).driveInfo.SeqWriteActiveZone, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs (HSMR SWR capacity)", vFarmFrame.at(page).driveInfo.numLBA, false, m_showStatusBits);

    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Work_Load(JSONNODE *masterData, uint32_t page)
{
    JSONNODE *pageInfo = json_new(JSON_NODE);

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).workLoadPage.copyNumber == FACTORYCOPY)
        {
            printf("\nWorkload From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nWorkload From Farm Log copy %d \n", page);
        }
        printf("\tRated Workload Percentage                                     %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.workloadPercentage & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< rated Workload Percentage
        printf("\tTotal Number of Read Commands                                 %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.totalReadCommands & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Total Number of Read Commands
        printf("\tTotal Number of Write Commands                                %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.totalWriteCommands & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Total Number of Write Commands
        printf("\tTotal Number of Random Read Cmds                              %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.totalRandomReads & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Total Number of Random Read Commands
        printf("\tTotal Number of Random Write Cmds                             %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.totalRandomWrites & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Total Number of Random Write Commands
        printf("\tTotal Number of Other Commands                                %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.totalNumberofOtherCMDS & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Total Number Of Other Commands       
        printf("\tLogical Sectors Written                                       %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Logical Sectors Written
        printf("\tLogical Sectors Read                                          %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Logical Sectors Read
        printf("\tDither events during current power cycle - Actuator 0         %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.dither & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Number of dither events during current power cycle (added 3.4)
        printf("\tDither was held off during random- Actuator 0                 %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.ditherRandom & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Number of times dither was held off during random workloads during current power cycle(added 3.4)
        printf("\tDither was held off during sequential- Actuator 0             %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.ditherSequential & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Number of times dither was held off during sequential workloads during current power cycle(added 3.4)
        printf("\tRead cmds from 0-3.125%% of LBA space                          %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfReadCmds1 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tRead cmds from 3.125-25%% of LBA space                         %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfReadCmds2 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tRead cmds from 25-50%% of LBA space                            %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfReadCmds3 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tRead cmds from 50-100%% of LBA space                           %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfReadCmds4 & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tWrite cmds from 0-3.125%% of LBA space                         %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds1 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tWrite cmds from 3.125-25%% of LBA space                        %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds2 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tWrite cmds from 25-50%% of LBA space                           %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds3 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
        printf("\tWrite cmds from 50-100%% of LBA space                          %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds4 & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
        //4.21

        printf("\tNumber of Read Commands of transfer length <=16KB             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numReadTransferSmallATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Read Commands of transfer length (16KB - 512KB]     %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numReadTransferMid1ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Read Commands of transfer length (512KB - 2MB]      %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numReadTransferMid2ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Read Commands of transfer length > 2MB              %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numReadTransferLargeATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Write Commands of transfer length <=16KB            %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numWriteTransferSmallATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Write Commands of transfer length (16KB - 512KB]    %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numWriteTransferMid1ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Write Commands of transfer length (512KB - 2MB]     %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numWriteTransferMid2ATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tNumber of Write Commands of transfer length > 2MB             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numWriteTransferLargeATA & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =1 at 30s intervals                      %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =2 at 30s intervals                      %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth2 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =3-4 at 30s intervals                    %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth3to4 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =5-8 at 30s intervals                    %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth5to8 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =9-16 at 30s intervals                   %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth9to16 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =17-32 at 30s intervals                  %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth17to32 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =33-64 at 30s intervals                  %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepth33to64 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth >64 at 30s intervals                     %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.cntQueueDepthmorethan64 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDither events during current power cycle - Actuator 1         %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numDithEvtAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDither was held off during random - Actuator 1                %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.numRandWLDitherHoldOffAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDither was held off during sequential - Actuator 1            %" PRIu64" \n", vFarmFrame.at(page).workLoadPage.numSequentialWLDitherHoldOffAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));

    }
    std::ostringstream temp;

    if (vFarmFrame.at(page).workLoadPage.copyNumber == FACTORYCOPY)
    {
        temp << "Workload From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Workload From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentage", vFarmFrame.at(page).workLoadPage.workloadPercentage, false, m_showStatusBits);             //!< rated Workload Percentage
    set_json_64_bit_With_Status(pageInfo, "Total Read Commands", vFarmFrame.at(page).workLoadPage.totalReadCommands, false, m_showStatusBits);           //!< Total Number of Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Write Commands", vFarmFrame.at(page).workLoadPage.totalWriteCommands, false, m_showStatusBits);         //!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Read Cmds", vFarmFrame.at(page).workLoadPage.totalRandomReads, false, m_showStatusBits);         //!< Total Number of Random Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Write Cmds", vFarmFrame.at(page).workLoadPage.totalRandomWrites, false, m_showStatusBits);       //!< Total Number of Random Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Other Commands", vFarmFrame.at(page).workLoadPage.totalNumberofOtherCMDS, false, m_showStatusBits);     //!< Total Number Of Other Commands
    set_json_64_bit_With_Status(pageInfo, "Logical Sectors Written",  vFarmFrame.at(page).workLoadPage.logicalSecWritten, false, m_showStatusBits);      //!< Logical Sectors Written                    
    set_json_64_bit_With_Status(pageInfo, "Logical Sectors Read", vFarmFrame.at(page).workLoadPage.logicalSecRead, false, m_showStatusBits);      //!< Logical Sectors Read
    set_json_64_bit_With_Status(pageInfo, "dither events - Actuator 0", vFarmFrame.at(page).workLoadPage.dither, false, m_showStatusBits);             //!< Number of dither events during current power cycle (added 3.4)
    set_json_64_bit_With_Status(pageInfo, "dither held off during random workloads - Actuator 0", vFarmFrame.at(page).workLoadPage.ditherRandom, false, m_showStatusBits);          //!< Number of times dither was held off during random workloads during current power cycle(added 3.4)
    set_json_64_bit_With_Status(pageInfo, "dither was held off during sequential workloads - Actuator 0", vFarmFrame.at(page).workLoadPage.ditherSequential, false, m_showStatusBits);          //!< Number of times dither was held off during sequential workloads during current power cycle(added 3.4)

    set_json_64_bit_With_Status(pageInfo, "Read commands from 0-3.125% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfReadCmds1, false, m_showStatusBits);          //!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 3.125-25% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfReadCmds2, false, m_showStatusBits);         //!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 25-50% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfReadCmds3, false, m_showStatusBits);            //!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Read commands from 50-100% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfReadCmds4, false, m_showStatusBits);           //!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 0-3.125% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds1, false, m_showStatusBits);        //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 3.125-25% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds2, false, m_showStatusBits);       //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 25-50% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds3, false, m_showStatusBits);          //!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames(added 4.4)
    set_json_64_bit_With_Status(pageInfo, "Write commands from 50-100% of LBA space", vFarmFrame.at(page).workLoadPage.numberOfWriteCmds4, false, m_showStatusBits);         //!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames(added 4.4)
    //4.21

    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length <=16KB", vFarmFrame.at(page).workLoadPage.numReadTransferSmallATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (16KB - 512KB]", vFarmFrame.at(page).workLoadPage.numReadTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (512KB - 2MB]", vFarmFrame.at(page).workLoadPage.numReadTransferMid2ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length > 2MB", vFarmFrame.at(page).workLoadPage.numReadTransferLargeATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length <=16KB", vFarmFrame.at(page).workLoadPage.numWriteTransferSmallATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (16KB - 512KB]", vFarmFrame.at(page).workLoadPage.numWriteTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (512KB - 2MB]", vFarmFrame.at(page).workLoadPage.numWriteTransferMid2ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length > 2MB", vFarmFrame.at(page).workLoadPage.numWriteTransferLargeATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 1", vFarmFrame.at(page).workLoadPage.cntQueueDepth1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 2", vFarmFrame.at(page).workLoadPage.cntQueueDepth2, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 3-4", vFarmFrame.at(page).workLoadPage.cntQueueDepth3to4, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 5-8", vFarmFrame.at(page).workLoadPage.cntQueueDepth5to8, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 9-16", vFarmFrame.at(page).workLoadPage.cntQueueDepth9to16, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 17-32", vFarmFrame.at(page).workLoadPage.cntQueueDepth17to32, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 33-64", vFarmFrame.at(page).workLoadPage.cntQueueDepth33to64, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Queue Depth bin > 64", vFarmFrame.at(page).workLoadPage.cntQueueDepthmorethan64, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither events - Actuator 1", vFarmFrame.at(page).workLoadPage.numDithEvtAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither was held off during random workloads - Actuator 1", vFarmFrame.at(page).workLoadPage.numRandWLDitherHoldOffAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "dither was held off during sequential workloads - Actuator 1", vFarmFrame.at(page).workLoadPage.numSequentialWLDitherHoldOffAct1, false, m_showStatusBits);

    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Error_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr;
    std::string timeStr;
    uint32_t loopCount = 0;
    JSONNODE *pageInfo = json_new(JSON_NODE);

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).errorPage.copyNumber == FACTORYCOPY)
        {
            printf("\nError Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nError Information Log From Farm Log copy %d: \n", page);
        }
        printf("\tUnrecoverable Read Errors:                                    %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalReadECC & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< Number of Unrecoverable Read Errors
        printf("\tUnrecoverable Write Errors:                                   %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalWriteECC & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Number of Unrecoverable Write Errors
        printf("\tNumber of Reallocated Sectors - Actuator 0:                   %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalReallocations & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Number of Reallocated Sectors
        printf("\tNumber of Read Recovery Attempts:                             %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalReadRecoveryAttepts & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Number of Read Recovery Attempts
        printf("\tNumber of Mechanical Start Failures:                          %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalMechanicalFails & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Number of Mechanical Start Failures
        printf("\tNumber of Reallocation Candidate Sectors - Actuator 0:        %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalReallocatedCanidates & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Number of Reallocated Candidate Sectors
        printf("\tNumber of ASR Events:                                         %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalASREvents & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of ASR Events
        printf("\tNumber of Interface CRC Errors:                               %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalCRCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Interface CRC Errors
        printf("\tSpin Retry Count:                                             %" PRIu64" \n", vFarmFrame.at(page).errorPage.attrSpinRetryCount & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
        printf("\tSpin Retry Count Normalized:                                  %" PRIu64" \n", vFarmFrame.at(page).errorPage.normalSpinRetryCount & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Spin Retry Count (SMART Attribute 10 Normalized)
        printf("\tSpin Retry Count Worst:                                       %" PRIu64" \n", vFarmFrame.at(page).errorPage.worstSpinRretryCount & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
        printf("\tNumber of IOEDC Errors (Raw):                                 0x%" PRIx64" \n", vFarmFrame.at(page).errorPage.attrIOEDCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
        printf("\tCTO Count Total:                                              %" PRIu64" \n", vFarmFrame.at(page).errorPage.attrCTOCount & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
        printf("\tCTO Count Over 5s:                                            %" PRIu64" \n", vFarmFrame.at(page).errorPage.overfiveSecCTO & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
        printf("\tCTO Count Over 7.5s:                                          %" PRIu64" \n", vFarmFrame.at(page).errorPage.oversevenSecCTO & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< CTO Count Over 7.5s (SMART Attribute
        printf("\tTotal Flash LED (Assert) Events - Actuator 0:                 %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalFlashLED & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Total Flash LED (Assert) Events
        printf("\tIndex of the last Flash LED - Actuator 0:                     %" PRIu64" \n", vFarmFrame.at(page).errorPage.indexFlashLED & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< index of the last Flash LED of the array
        printf("\tUncorrectable errors:                                         %" PRIu64" \n", vFarmFrame.at(page).errorPage.uncorrectables & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< uncorrecatables errors (sata only)
        for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
        {
            printf("\tFlash LED event # %2d - Actuator 0:                            0x%" PRIx64" \n", loopCount, vFarmFrame.at(page).errorPage.flashLEDArray[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));          //!<Info on the last 8 Flash LED events Wrapping array.
            printf("\tTimestamp of event # %2d:                                      %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.timestampForLED[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tPower Cycle of event # %2d:                                    %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.powerCycleOfLED[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
        {
            printf("\tRead/Write retry events # %2d - Actuator 0:                    0x%" PRIx64" \n", loopCount, vFarmFrame.at(page).errorPage.readWriteRetry[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        
        printf("\tCum Lifetime Unrecoverable Read errors due to ERC:            %" PRIu64" \n", vFarmFrame.at(page).errorPage.cumLifeTimeECCReadDueErrorRecovery & UINT64_C(0x00FFFFFFFFFFFFFF));
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCum Lifetime Unrecoverable Read Repeating by Head %2" PRIu32":         %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.cumLifeUnRecoveralbeReadByhead[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
            printf("\tCum Lifetime Unrecoverable Read Unique by Head %2" PRIu32":            %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.cumLiveUnRecoveralbeReadUnique[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< Cumulative Lifetime Unrecoverable Read Unique by head
        }
        //verion 4.21
        if ((m_MajorRev >= 4) && (m_MinorRev > 20))
        {
            printf("\tReallocated Sectors - Actuator 1:                             %" PRIu64" \n", vFarmFrame.at(page).errorPage.reallocSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocation Candidate Sectors - Actuator 1:                  %" PRIu64" \n", vFarmFrame.at(page).errorPage.reallocCandidatesAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tTotal Flash LED (Assert) Events- Actuator 1:                  %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalFlashLEDEvents & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tIndex of the last Flash LED - Actuator 1:                     %" PRIu64" \n", vFarmFrame.at(page).errorPage.lastIDXFLEDInfoAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
            {
                printf("\tFlash LED event # %2d - Actuator 1:                            0x%" PRIx64" \n", loopCount, vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
                printf("\tTimestamp of event # %2d:                                      %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.last8FLEDEvtsAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
                printf("\tPower Cycle of event # %2d:                                    %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.last8FLEDEvtsPowerCycleAct1[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
            }
            for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
            {
                printf("\tRead/Write retry events # %2d - Actuator 1:                    0x%" PRIx64" \n", loopCount, vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
            }
            // 4.41
            printf("\tSATA PFA Attributes 1:                                        %" PRIu64" \n", vFarmFrame.at(page).errorPage.pfaAttribute1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tSATA PFA Attributes 2:                                        %" PRIu64" \n", vFarmFrame.at(page).errorPage.pfaAttribute2 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocated sectors last FARM Time series Frame save:         %" PRIu64" \n", vFarmFrame.at(page).errorPage.lastReallocatedSectorsAtc0 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocated sectors between FARM time series Frame:           %" PRIu64" \n", vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct0 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocation candidate sectors last FARM Time series Frame:   %" PRIu64" \n", vFarmFrame.at(page).errorPage.lastCandidateSectorsAct0 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocation candidate sectors between FARM time series Frame:%" PRIu64" \n", vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct0 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocated sectors last FARM Time series save Actuator 1:    %" PRIu64" \n", vFarmFrame.at(page).errorPage.lastReallocatedSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocated sectors between FARM time series Frame Actuator 1:%" PRIu64" \n", vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocation candidate sectors last FARM Time Actuator 1:     %" PRIu64" \n", vFarmFrame.at(page).errorPage.lastCandidateSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tReallocation candidate sectors between FARM time Actuator 1:  %" PRIu64" \n", vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            for (loopCount = 0; loopCount < MAX_HEAD_COUNT; ++loopCount)
            {
                printf("\tUnique Unrecoverable sectors last Time series by Head %2" PRIu32":     %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.lastUniqueURE[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
            }
            for (loopCount = 0; loopCount < MAX_HEAD_COUNT; ++loopCount)
            {
                printf("\tUnique Unrecoverable sectors between time series by Head %2" PRIu32":  %" PRIu64" \n", loopCount, vFarmFrame.at(page).errorPage.betweenUniqueURE[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
            }
            
        }

    }
    std::ostringstream temp;
    if (vFarmFrame.at(page).errorPage.copyNumber == FACTORYCOPY)
    {
        temp << "Error Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Error Information Log From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame.at(page).errorPage.totalReadECC, false, m_showStatusBits);                       //!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame.at(page).errorPage.totalWriteECC, false, m_showStatusBits);                     //!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Reallocated Sectors - Actuator 0", vFarmFrame.at(page).errorPage.totalReallocations, false, m_showStatusBits);                       //!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Read Recovery Attempts", vFarmFrame.at(page).errorPage.totalReadRecoveryAttepts, false, m_showStatusBits);              //!< Number of Read Recovery Attempts
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame.at(page).errorPage.totalMechanicalFails, false, m_showStatusBits);               //!< Number of Mechanical Start Failures

    set_json_64_bit_With_Status(pageInfo, "Reallocation Candidate Sectors - Actuator 0", vFarmFrame.at(page).errorPage.totalReallocatedCanidates,false, m_showStatusBits);       //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "ASR Events", vFarmFrame.at(page).errorPage.totalASREvents, false, m_showStatusBits);                                    //!< Number of ASR Events
    set_json_64_bit_With_Status(pageInfo, "Interface CRC Errors", vFarmFrame.at(page).errorPage.totalCRCErrors, false, m_showStatusBits);                          //!< Number of Interface CRC Errors
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count", vFarmFrame.at(page).errorPage.attrSpinRetryCount, false, m_showStatusBits);                          //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Normalized", vFarmFrame.at(page).errorPage.normalSpinRetryCount, false, m_showStatusBits);             //!< Spin Retry Count (SMART Attribute 10 Normalized)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Worst", vFarmFrame.at(page).errorPage.worstSpinRretryCount, false, m_showStatusBits);                  //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
    set_json_64_bit_With_Status(pageInfo, "IOEDC Errors (Raw)", vFarmFrame.at(page).errorPage.attrIOEDCErrors, false, m_showStatusBits);                           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "CTO Count Total", vFarmFrame.at(page).errorPage.attrCTOCount, false, m_showStatusBits);                                 //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 5s", vFarmFrame.at(page).errorPage.overfiveSecCTO, false, m_showStatusBits);                             //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 7.5s", vFarmFrame.at(page).errorPage.oversevenSecCTO, false, m_showStatusBits);                          //!< CTO Count Over 7.5s (SMART Attribute
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events - Actuator 0", vFarmFrame.at(page).errorPage.totalFlashLED, false, m_showStatusBits);                //!< Total Flash LED (Assert) Events

    set_json_int_With_Status(pageInfo, "Index of the last Flash LED - Actuator 0", vFarmFrame.at(page).errorPage.indexFlashLED, m_showStatusBits);                              //!< index of the last Flash LED of the array          
    set_json_64_bit_With_Status(pageInfo, "Uncorrectable errors", vFarmFrame.at(page).errorPage.uncorrectables, false, m_showStatusBits);                          //!< uncorrecatables errors (sata only)

    for (loopCount = 0; loopCount < FLASH_EVENTS; loopCount++)
    {
        temp.str("");temp.clear();
        JSONNODE *eventInfo = json_new(JSON_NODE);
        temp << "Flash LED Event " << std::dec << loopCount << "Actuator 0";
        json_set_name(eventInfo, temp.str().c_str());
        set_json_64_bit_With_Status(eventInfo, "Event Information", vFarmFrame.at(page).errorPage.flashLEDArray[loopCount], true, m_showStatusBits);              //!< Info on the last 8 Flash LED (assert) Events, wrapping array
        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(opensea_parser::check_Status_Strip_Status(vFarmFrame.at(page).errorPage.flashLEDArray[loopCount]));
        set_json_string_With_Status(eventInfo, "Flash LED Code", temp.str().c_str(), vFarmFrame.at(page).errorPage.flashLEDArray[loopCount],m_showStatusBits);
        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(opensea_parser::check_Status_Strip_Status(vFarmFrame.at(page).errorPage.flashLEDArray[loopCount]));
        set_json_string_With_Status(eventInfo, "Flash LED Address", temp.str().c_str(), vFarmFrame.at(page).errorPage.flashLEDArray[loopCount], m_showStatusBits);

        temp.str("");temp.clear();
        temp << "TimeStamp of Event(hours) " << std::dec << loopCount;
        std::ostringstream temp1;
        temp1 << std::setprecision(3) << std::setfill('0') << static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.timestampForLED[loopCount])) / 3600000) *.001;
        set_json_string_With_Status(eventInfo, temp.str().c_str(), temp1.str().c_str(), vFarmFrame.at(page).errorPage.timestampForLED[loopCount], m_showStatusBits);//!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array

        temp.str("");temp.clear();
        temp << "Power Cycle Event " << std::dec << loopCount;
        set_json_64_bit_With_Status(eventInfo, temp.str().c_str(), vFarmFrame.at(page).errorPage.powerCycleOfLED[loopCount], false, m_showStatusBits);            //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array
        temp.str("");temp.clear();

        json_push_back(pageInfo, eventInfo);
    }

    if ((m_MajorRev >= MAJORVERSION3) && (m_MinorRev >= 2) && (m_MinorRev <= 4))
    {
        // get read write retry events
        for (loopCount = 0; loopCount <= 7; ++loopCount)
        {
            JSONNODE *rwrInfo = json_new(JSON_NODE);
            temp.str("");temp.clear();
            temp << "Read Write Retry " << std::dec << loopCount << "Actuator 0";
            json_set_name(rwrInfo, temp.str().c_str());
            set_json_64_bit_With_Status(rwrInfo, "Log Entry", M_Byte5(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Error Type", M_Byte6(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Zone Group", M_Word1(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Head", M_Nibble3(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);
            set_json_64_bit_With_Status(rwrInfo, "Count", M_Byte0(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount])), false, m_showStatusBits);

            json_push_back(pageInfo, rwrInfo);
        }
    }
    else
    {
        for (loopCount = 0; loopCount <= 7; ++loopCount)
        {
            JSONNODE *rwrInfo = json_new(JSON_NODE);
            temp.str("");temp.clear();
            temp << "Read Write Retry " << std::dec << loopCount << "Actuator 0";
            json_set_name(rwrInfo, temp.str().c_str());
            set_json_int_Check_Status(rwrInfo, "Log Entry", M_Word2(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount]), vFarmFrame.at(page).errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Error Type", M_Byte6(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount]), vFarmFrame.at(page).errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Zone Group", M_Word1(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount]), vFarmFrame.at(page).errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Head", M_Byte1(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount]), vFarmFrame.at(page).errorPage.readWriteRetry[loopCount], m_showStatusBits);
            set_json_int_Check_Status(rwrInfo, "Count", M_Byte0(vFarmFrame.at(page).errorPage.readWriteRetry[loopCount]), vFarmFrame.at(page).errorPage.readWriteRetry[loopCount], m_showStatusBits);

            json_push_back(pageInfo, rwrInfo);
        }

    }
    set_json_64_bit_With_Status(pageInfo, "Cum Lifetime Unrecoverable Read errors due to ERC", vFarmFrame.at(page).errorPage.cumLifeTimeECCReadDueErrorRecovery, false, m_showStatusBits);
    //verion 4.20
    if ((m_MajorRev >= 4 && m_MinorRev > 20) || m_showStatic)
    {
        if (m_showStatic)
        {
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                temp.str(""); temp.clear();
                temp << "Cum Lifetime Unrecoverable Read Repeating by head " << std::dec << loopCount;
                set_json_64_bit_With_Status(pageInfo, temp.str().c_str(), vFarmFrame.at(page).errorPage.cumLifeUnRecoveralbeReadByhead[loopCount], false, m_showStatusBits);      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
            }
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                temp.str(""); temp.clear();
                temp << "Cum Lifetime Unrecoverable Read Unique by head " << std::dec << loopCount;
                set_json_64_bit_With_Status(pageInfo, temp.str().c_str(), vFarmFrame.at(page).errorPage.cumLiveUnRecoveralbeReadUnique[loopCount], false, m_showStatusBits);   //!< Cumulative Lifetime Unrecoverable Read Unique by head
            }
        }
        else
        {
            JSONNODE* cum = json_new(JSON_ARRAY);
            json_set_name(cum, "Cum Lifetime Unrecoverable Read Repeating");
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                set_json_64_bit_With_Status(cum, "Cum Lifetime Unrecoverable Read Repeating", vFarmFrame.at(page).errorPage.cumLifeUnRecoveralbeReadByhead[loopCount], false, m_showStatusBits);      //!< Cumulative Lifetime Unrecoverable Read Repeating by head
            }
            json_push_back(pageInfo, cum);

            JSONNODE* unique = json_new(JSON_ARRAY);
            json_set_name(unique, "Cum Lifetime Unrecoverable Read Unique");
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                set_json_64_bit_With_Status(unique, "Cum Lifetime Unrecoverable Read Unique", vFarmFrame.at(page).errorPage.cumLiveUnRecoveralbeReadUnique[loopCount], false, m_showStatusBits);   //!< Cumulative Lifetime Unrecoverable Read Unique by head
            }
            json_push_back(pageInfo, unique);
        }
    }
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors - Actuator 1", vFarmFrame.at(page).errorPage.reallocSectorsAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors - Actuator 1", vFarmFrame.at(page).errorPage.reallocCandidatesAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events - Actuator 1", vFarmFrame.at(page).errorPage.totalFlashLEDEvents, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Index of last entry in FLED Info array below - Actuator 1", vFarmFrame.at(page).errorPage.lastIDXFLEDInfoAct1, false, m_showStatusBits);
    for (loopCount = 0; loopCount < FLASH_EVENTS; ++loopCount)
    {
        JSONNODE *eventInfoact1 = json_new(JSON_NODE);
        temp.str("");temp.clear();
        temp << "Flash LED Event " << std::dec << loopCount << " Actuator 1";
        json_set_name(eventInfoact1, temp.str().c_str());

        set_json_64_bit_With_Status(eventInfoact1, "Event Information", vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount], true, m_showStatusBits);              //!> Info on the last 8 Flash LED(assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount]));
        json_push_back(eventInfoact1, json_new_a("Flash LED Code", temp.str().c_str()));
        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount]));
        json_push_back(eventInfoact1, json_new_a("Flash LED Address", temp.str().c_str()));

        temp.str(""); temp.clear();
        temp << "TimeStamp Event (hours)" << std::dec << loopCount;
        std::ostringstream temp1;
        temp1 << std::setprecision(3) << std::setfill('0') << static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEvtsAct1[loopCount])) / 3600000) * .001;
        set_json_string_With_Status(eventInfoact1, temp.str().c_str(), temp1.str().c_str(), vFarmFrame.at(page).errorPage.last8FLEDEvtsAct1[loopCount], m_showStatusBits);        //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "Power Cycle Event" << std::dec << loopCount;
        set_json_64_bit_With_Status(eventInfoact1, temp.str().c_str(), vFarmFrame.at(page).errorPage.last8FLEDEvtsPowerCycleAct1[loopCount], false, m_showStatusBits);            //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array
        temp.str(""); temp.clear();

        json_push_back(pageInfo, eventInfoact1);
    }
    for (loopCount = 0; loopCount <= 7; ++loopCount)
    {
        JSONNODE* rwrInfo = json_new(JSON_NODE);
        temp.str(""); temp.clear();
        temp << "Read Write Retry " << std::dec << loopCount << " Actuator 1";
        json_set_name(rwrInfo, temp.str().c_str());

        set_json_int_Check_Status(rwrInfo, "Error Type", M_Byte6(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Log Entry", M_Word2(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Zone Group", M_Word1(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Head", M_Byte1(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Count", M_Byte0(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);

        json_push_back(pageInfo, rwrInfo);
    }
    if (((m_MajorRev >= 4) && (m_MinorRev > 34)) || m_showStatic)
    {// 4.34
        opensea_parser::set_json_64_bit_With_Status(pageInfo, "SATA PFA Attributes 1", vFarmFrame.at(page).errorPage.pfaAttribute1, false, m_showStatusBits);
        if ((opensea_parser::check_For_Active_Status(&vFarmFrame.at(page).errorPage.pfaAttribute1)) && (((vFarmFrame.at(page).errorPage.pfaAttribute1 & UINT64_C(0x00FFFFFFFFFFFFFF)) != 0)) && (m_showStatic == false))
        {
            if (M_Byte0(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x01)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x01", true);
            }
            if (M_Byte1(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x03)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x03", true);
            }
            if (M_Byte2(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x05)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x05", true);
            }
            if (M_Byte3(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x07)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x07", true);
            }
            if (M_Byte4(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x10)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x10", true);
            }
            if (M_Byte5(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x12)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x12", true);
            }
        }
        opensea_parser::set_json_64_bit_With_Status(pageInfo, "SATA PFA Attributes 2", vFarmFrame.at(page).errorPage.pfaAttribute2, false, m_showStatusBits);
        if ((opensea_parser::check_For_Active_Status(&vFarmFrame.at(page).errorPage.pfaAttribute2)) && (((vFarmFrame.at(page).errorPage.pfaAttribute2 & UINT64_C(0x00FFFFFFFFFFFFFF)) != 0)) && (m_showStatic == false))
        {
            if (M_Byte0(vFarmFrame.at(page).errorPage.pfaAttribute2) == 0xc8)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0xC8", true);
            }
        }
        long long reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastReallocatedSectorsAtc0)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors since the last FARM Frame", reallo, vFarmFrame.at(page).errorPage.lastReallocatedSectorsAtc0, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct0)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors Previous FARM Frame", reallo, vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct0, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastCandidateSectorsAct0)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors since the last FARM Frame", reallo, vFarmFrame.at(page).errorPage.lastCandidateSectorsAct0, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct0)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors previous FARM Frame", reallo, vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct0, m_showStatusBits);
        if (m_ShowAct1)
        {
            reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastReallocatedSectorsAct1)), 8);
            opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors since the last FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.lastReallocatedSectorsAct1, m_showStatusBits);
            reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct1)), 8);
            opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors previous FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct1, m_showStatusBits);
            reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastCandidateSectorsAct1)), 8);
            opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors since the last FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.lastCandidateSectorsAct1, m_showStatusBits);
            reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct1)), 8);
            opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors previous FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct1, m_showStatusBits);
        }

        if (m_showStatic)
        {
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                temp.str(""); temp.clear();
                temp << "Unique unrecoverable sectors since the last FARM Frame by Head " << std::dec << loopCount;
                reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastUniqueURE[loopCount])), 8);
                opensea_parser::set_json_int_Check_Status(pageInfo, temp.str().c_str(), reallo, vFarmFrame.at(page).errorPage.lastUniqueURE[loopCount], m_showStatusBits);
            }
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                temp.str(""); temp.clear();
                temp << "Unique unrecoverable sectors previous FARM Frame by Head " << std::dec << loopCount;
                reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenUniqueURE[loopCount])), 8);
                opensea_parser::set_json_int_Check_Status(pageInfo, temp.str().c_str(), reallo, vFarmFrame.at(page).errorPage.betweenUniqueURE[loopCount], m_showStatusBits);
            }
        }
        else
        {
            JSONNODE* last = json_new(JSON_ARRAY);
            json_set_name(last, "Unique unrecoverable sectors since the last FARM Frame");
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastUniqueURE[loopCount])), 8);
                opensea_parser::set_json_int_Check_Status(last, "Unique unrecoverable sectors since the last FARM Frame", reallo, vFarmFrame.at(page).errorPage.lastUniqueURE[loopCount], m_showStatusBits);
            }
            json_push_back(pageInfo, last);

            JSONNODE* previous = json_new(JSON_ARRAY);
            json_set_name(previous, "Unique unrecoverable sectors previous FARM Frame");
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenUniqueURE[loopCount])), 8);
                opensea_parser::set_json_int_Check_Status(previous, "Unique unrecoverable sectors previous FARM Frame", reallo, vFarmFrame.at(page).errorPage.betweenUniqueURE[loopCount], m_showStatusBits);
            }
            json_push_back(pageInfo, previous);
        }
    }
    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Enviroment_Information(JSONNODE *masterData, uint32_t page)
{
    JSONNODE *pageInfo = json_new(JSON_NODE);

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).environmentPage.copyNumber == FACTORYCOPY)
        {
            printf("\nEnvironment Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nEnvironment Information From Farm Log copy %d:\n", page);
        }

        printf("\tCurrent Temperature:                                          %" PRIu64" \n", vFarmFrame.at(page).environmentPage.curentTemp & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Current Temperature in Celsius
        printf("\tHighest Temperature:                                          %" PRIu64" \n", vFarmFrame.at(page).environmentPage.highestTemp & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Highest Temperature in Celsius
        printf("\tLowest Temperature:                                           %" PRIu64" \n", vFarmFrame.at(page).environmentPage.lowestTemp & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Lowest Temperature
        printf("\tAverage Short Term Temperature:                               %" PRIu64" \n", vFarmFrame.at(page).environmentPage.averageTemp & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Average Short Term Temperature5
        printf("\tAverage Long Term Temperatures:                               %" PRIu64" \n", vFarmFrame.at(page).environmentPage.averageLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Average Long Term Temperature5
        printf("\tHighest Average Short Term Temperature:                       %" PRIu64" \n", vFarmFrame.at(page).environmentPage.highestShortTemp & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Highest Average Short Term Temperature5
        printf("\tLowest Average Short Term Temperature:                        %" PRIu64" \n", vFarmFrame.at(page).environmentPage.lowestShortTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Lowest Average Short Term Temperature5
        printf("\tHighest Average Long Term Temperature:                        %" PRIu64" \n", vFarmFrame.at(page).environmentPage.highestLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Highest Average Long Term Temperature5
        printf("\tLowest Average Long Term Temperature:                         %" PRIu64" \n", vFarmFrame.at(page).environmentPage.lowestLongTemp & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< Lowest Average Long Term Temperature5
        printf("\tTime In Over Temperature:                                     %" PRIu64" \n", vFarmFrame.at(page).environmentPage.overTempTime & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Time In Over Temperature5
        printf("\tTime In Under Temperature:                                    %" PRIu64" \n", vFarmFrame.at(page).environmentPage.underTempTime & UINT64_C(0x00FFFFFFFFFFFFFF));    //!< Time In Under Temperature5
        printf("\tSpecified Max Operating Temperature:                          %" PRIu64" \n", vFarmFrame.at(page).environmentPage.maxTemp & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Specified Max Operating Temperature
        printf("\tSpecified Min Operating Temperature:                          %" PRIu64" \n", vFarmFrame.at(page).environmentPage.minTemp & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Specified Min Operating Temperature
        printf("\tCurrent Relative Humidity:                                    %" PRIu64" \n", vFarmFrame.at(page).environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Current Relative Humidity (in units of .1%)
        printf("\tCurrent Motor Power:                                          %" PRIu64" \n", vFarmFrame.at(page).environmentPage.currentMotorPower & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Current Motor Power, value from most recent SMART Summary Frame6 
        printf("\tCurrent 12 volts:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.current12v)) * static_cast<double>(.001F));
        printf("\tMinimum 12 volts:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.min12v)) * static_cast<double>(.001F));
        printf("\tMaximum 12 volts:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.max12v)) * static_cast<double>(.001F));
        printf("\tCurrent 5 volts:                                              %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.current5v)) * static_cast<double>(.001F));
        printf("\tMinimum 5 volts:                                              %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.min5v)) * static_cast<double>(.001F));
        printf("\tMaximum 5 volts:                                              %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.max5v)) * static_cast<double>(.001F));
        printf("\t12V Power Average:                                            %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerAvg12v)) * static_cast<double>(.001L));
        printf("\t12V Power Minimum:                                            %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMin12v)) * static_cast<double>(.001L));
        printf("\t12V Power Maximum:                                            %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMax12v)) * static_cast<double>(.001L));
        printf("\t5V Power Average:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerAvg5v)) * static_cast<double>(.001L));
        printf("\t5V Power Minimum:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMin5v)) * static_cast<double>(.001L));
        printf("\t5V Power Maximum:                                             %2.3lf \n", static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMax5v)) * static_cast<double>(.001L));

    }
    std::ostringstream temp;
    if (vFarmFrame.at(page).environmentPage.copyNumber == FACTORYCOPY)
    {
        temp << "Environment Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Environment Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    temp.str("");temp.clear();

    double TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.curentTemp)) * static_cast<double>(1.00F);                                                                                      //!< Current Temperature in Celsius
    set_json_float_With_Status(pageInfo, "Current Temperature (Celsius)", TempValue, vFarmFrame.at(page).environmentPage.curentTemp, m_showStatusBits);
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.highestTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Highest Temperature", TempValue, vFarmFrame.at(page).environmentPage.highestTemp, m_showStatusBits);                                       //!< Highest Temperature in Celsius                             
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.lowestTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Lowest Temperature", TempValue, vFarmFrame.at(page).environmentPage.lowestTemp, m_showStatusBits);                                        //!< Lowest Temperature
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.averageTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Average Short Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.averageTemp, m_showStatusBits);                           //!< Average Short Term Temperature
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.averageLongTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Average Long Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.averageLongTemp, m_showStatusBits);                        //!< Average Long Term Temperature 
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.highestShortTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Highest Average Short Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.highestShortTemp, m_showStatusBits);              //!< Highest Average Short Term Temperature      
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.lowestShortTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Lowest Average Short Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.lowestShortTemp, m_showStatusBits);                //!< Lowest Average Short Term Temperature     
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.highestLongTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Highest Average Long Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.highestLongTemp, m_showStatusBits);                 //!< Highest Average Long Term Temperature       
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.lowestLongTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Lowest Average Long Term Temperature", TempValue, vFarmFrame.at(page).environmentPage.lowestLongTemp, m_showStatusBits);                   //!< Lowest Average Long Term Temperature

    set_json_int_With_Status(pageInfo, "Time In Over Temperature", vFarmFrame.at(page).environmentPage.overTempTime, m_showStatusBits);                                                //!< Time In Over Temperature
    set_json_int_With_Status(pageInfo, "Time In Under Temperature", vFarmFrame.at(page).environmentPage.underTempTime, m_showStatusBits);                                              //!< Time In Under Temperature
              
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.maxTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Specified Max Operating Temperature", TempValue, vFarmFrame.at(page).environmentPage.maxTemp, m_showStatusBits);                          //!< Specified Max Operating Temperature        
    TempValue = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.minTemp)) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Specified Min Operating Temperature", TempValue, vFarmFrame.at(page).environmentPage.minTemp, m_showStatusBits);                          //!< Specified Min Operating Temperature

    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << (check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.humidity)) * 0.1F;                                                    //!< Current Relative Humidity (in units of .1%)
    set_json_string_With_Status(pageInfo, "Current Relative Humidity", temp.str().c_str(), vFarmFrame.at(page).environmentPage.humidity, m_showStatusBits);
    set_json_int_With_Status(pageInfo, "Current Motor Power", vFarmFrame.at(page).environmentPage.currentMotorPower, m_showStatusBits);                                                //!< Current Motor Power, value from most recent SMART Summary Frame6
    double volts = 0.0;
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.current12v)) *static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Current 12 volts", volts, vFarmFrame.at(page).environmentPage.current12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.min12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 12 volts", volts, vFarmFrame.at(page).environmentPage.min12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.max12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 12 volts", volts, vFarmFrame.at(page).environmentPage.max12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.current5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Current 5 volts", volts, vFarmFrame.at(page).environmentPage.current5v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.min5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 5 volts", volts, vFarmFrame.at(page).environmentPage.min5v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.max5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 5 volts", volts, vFarmFrame.at(page).environmentPage.max5v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerAvg12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "12V Power Average", volts, vFarmFrame.at(page).environmentPage.powerAvg12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMin12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "12V Power Minimum", volts, vFarmFrame.at(page).environmentPage.powerMin12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMax12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "12V Power Maximum", volts, vFarmFrame.at(page).environmentPage.powerMax12v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerAvg5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "5V Power Average", volts, vFarmFrame.at(page).environmentPage.powerAvg5v, m_showStatusBits);  
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMin5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "5V Power Minimum", volts, vFarmFrame.at(page).environmentPage.powerMin5v, m_showStatusBits);
    volts = static_cast<double>(M_WordInt0(vFarmFrame.at(page).environmentPage.powerMax5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "5V Power Maximum", volts, vFarmFrame.at(page).environmentPage.powerMax5v, m_showStatusBits);

    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Reli_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myStr;
    JSONNODE *pageInfo = json_new(JSON_NODE);

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {

        if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
        {
            printf("\nReliability Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nReliability Information From Farm Log copy %" PRIu32"\n", page);
        }
        printf("\tNumber of DOS Scans Performed:                                %" PRIu64" \n", vFarmFrame.at(page).reliPage.numberDOSScans & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Number of DOS Scans Performed
        printf("\tNumber of LBAs Corrected by ISP:                              %" PRIu64" \n", vFarmFrame.at(page).reliPage.numberLBACorrect & UINT64_C(0x00FFFFFFFFFFFFFF));                      //!< Number of LBAs Corrected by ISP
        printf("\tError Rate (SMART Attribute 1 Raw):                           0x%016" PRIx64"\n", vFarmFrame.at(page).reliPage.attrErrorRateRaw & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Error Rate (SMART Attribute 1 Raw)
        printf("\tError Rate (SMART Attribute 1 Normalized):                    %" PRIu64" \n", vFarmFrame.at(page).reliPage.attrErrorRateNormal & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Error Rate (SMART Attribute 1 Normalized)
        printf("\tError Rate (SMART Attribute 1 Worst):                         %" PRIu64" \n", vFarmFrame.at(page).reliPage.attrErrorRateWorst & UINT64_C(0x00FFFFFFFFFFFFFF));                    //!< Error Rate (SMART Attribute 1 Worst)
        printf("\tSeek Error Rate (SMART Attr 7 Raw):                           0x%016" PRIx64" \n", vFarmFrame.at(page).reliPage.attrSeekErrorRateRaw & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Seek Error Rate (SMART Attribute 7 Raw)
        printf("\tSeek Error Rate (SMART Attr 7 Normalized):                    %" PRIu64" \n", vFarmFrame.at(page).reliPage.attrSeekErrorRateNormal & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Seek Error Rate (SMART Attribute 7 Normalized)
        printf("\tSeek Error Rate (SMART Attr 7 Worst):                         %" PRIu64" \n", vFarmFrame.at(page).reliPage.attrSeekErrorRateWorst & UINT64_C(0x00FFFFFFFFFFFFFF));                //!< Seek Error Rate (SMART Attribute 7 Worst)
        printf("\tHigh Priority Unload Events (Raw) :                           %" PRIu64" \n", vFarmFrame.at(page).reliPage.attrUnloadEventsRaw & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< High Priority Unload Events (SMART Attribute 192 Raw)
        printf("\tNumber of disc slip recalibrations performed:                 %" PRIu64" \n", vFarmFrame.at(page).reliPage.diskSlipRecalPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));                //!< Number of disc slip recalibrations performed
        printf("\tHelium Pressure Threshold Trip:                               %" PRIu64" \n", vFarmFrame.at(page).reliPage.heliumPresureTrip & UINT64_C(0x00FFFFFFFFFFFFFF));                     //!< Helium Pressure Threshold Trip (1 – trip 0 – no trip)
        printf("\tNumber of LBAs Corrected by Parity Sector - Actuator 0:       %" PRIu64" \n", vFarmFrame.at(page).reliPage.numberLBACorrectedByParitySector & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Number of LBAs Corrected by Parity Sector
        printf("\tPrimary Super Parity Coverage Percentage - Actuator 0:        %" PRIu64" \n", vFarmFrame.at(page).reliPage.SuperParityCovPercent & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Primary Super Parity Coverage Percentage
        printf("\tPrimary Super Parity Coverage Percentage SMR - Actuator 0:    %" PRIu64" \n", vFarmFrame.at(page).reliPage.superParityCoveragePercentageAct0 & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Primary Super Parity Coverage Percentage SMR/HSMR-SWR, Actuator 0
        printf("\tNumber of DOS Scans Performed - Actuator 1:                   %" PRIu64" \n", vFarmFrame.at(page).reliPage.DOSScansAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));                          //!< Number of LBAs Corrected by Parity Sector
        printf("\tNumber of LBAs Corrected by ISP - Actuator 1:                 %" PRIu64" \n", vFarmFrame.at(page).reliPage.correctedLBAsAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));                     //!< Number of LBAs Corrected by ISP, Actuator 1
        printf("\tNumber of LBAs Corrected by Parity Sector - Actuator 1:       %" PRIu64" \n", vFarmFrame.at(page).reliPage.numberLBACorrectedByParitySectorAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Number of LBAs Corrected by Parity Sector, Actuator 1 
        printf("\tPrimary Super Parity Coverage Percentage SMR - Actuator 1:    %" PRIu64" \n", vFarmFrame.at(page).reliPage.superParityCoveragePercentageAct1 & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Primary Super Parity Coverage Percentage SMR/HSMR-SWR, Actuator 1
    }
    std::ostringstream temp;
    if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
    {
        temp << "Reliability Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Reliability Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());
  
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", vFarmFrame.at(page).reliPage.numberDOSScans, false, m_showStatusBits);                              //!< Number of DOS Scans Performed
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", vFarmFrame.at(page).reliPage.numberLBACorrect, false, m_showStatusBits);                          //!< Number of LBAs Corrected by ISP
    set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Raw)", vFarmFrame.at(page).reliPage.attrErrorRateRaw, true, m_showStatusBits);                        //!< Error Rate (SMART Attribute 1 Raw)
    set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Normalized)", vFarmFrame.at(page).reliPage.attrErrorRateNormal, false, m_showStatusBits);             //!< Error Rate (SMART Attribute 1 Normalized)
    set_json_64_bit_With_Status(pageInfo, "Error Rate (SMART Attribute 1 Worst)", vFarmFrame.at(page).reliPage.attrErrorRateWorst, false, m_showStatusBits);                   //!< Error Rate (SMART Attribute 1 Worst)
    set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Raw)", vFarmFrame.at(page).reliPage.attrSeekErrorRateRaw, true, m_showStatusBits);                    //!< Seek Error Rate (SMART Attribute 7 Raw)
    set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Normalized)", vFarmFrame.at(page).reliPage.attrSeekErrorRateNormal, false, m_showStatusBits);         //!< Seek Error Rate (SMART Attribute 7 Normalized)
    set_json_64_bit_With_Status(pageInfo, "Seek Error Rate (SMART Attr 7 Worst)", vFarmFrame.at(page).reliPage.attrSeekErrorRateWorst, false, m_showStatusBits);               //!< Seek Error Rate (SMART Attribute 7 Worst)
    set_json_64_bit_With_Status(pageInfo, "High Priority Unload Events", vFarmFrame.at(page).reliPage.attrUnloadEventsRaw, false, m_showStatusBits);                           //!< High Priority Unload Events (SMART Attribute 192 Raw)
    set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame.at(page).reliPage.diskSlipRecalPerformed, false, m_showStatusBits);       //!< Number of disc slip recalibrations performed
    set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame.at(page).reliPage.heliumPresureTrip, false, m_showStatusBits);                       //!< Helium Pressure Threshold Tripped ( 1- trip, 0 -no trip)
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector - Actuator 0", vFarmFrame.at(page).reliPage.numberLBACorrectedByParitySector, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage - Actuator 0", vFarmFrame.at(page).reliPage.SuperParityCovPercent, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage SMR SWR - Actuator 0", vFarmFrame.at(page).reliPage.superParityCoveragePercentageAct0, false, m_showStatusBits);           //!< Primary Super Parity Coverage Percentage SMR/SWR- Actuator 0
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed - Actuator 1", vFarmFrame.at(page).reliPage.DOSScansAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP - Actuator 1", vFarmFrame.at(page).reliPage.correctedLBAsAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector - Actuator 1", vFarmFrame.at(page).reliPage.numberLBACorrectedByParitySectorAct1, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Primary Super Parity Coverage Percentage SMR SWR - Actuator 1", vFarmFrame.at(page).reliPage.superParityCoveragePercentageAct1, false, m_showStatusBits);
  
    json_push_back(masterData, pageInfo);
    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CATA_Farm_Log::print_Head_Information(JSONNODE *masterData, uint32_t page)
{
    std::string myHeader;
    JSONNODE *headInfo = json_new(JSON_NODE);
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        //double remander = 0;
        uint32_t loopCount = 0;
        int16_t whole = 0;
        if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
        {
            printf("\n Head Information From Farm Log copy FACTORY");
        }
        else
        {
            printf("\n Head Information From Farm Log copy %" PRIu32"\n", page);
        }



        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tDVGA Skip Write Detect by Head %2" PRIu32":                            %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.DVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] DVGA Skip Write Detect by Head7
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tRVGA Skip Write Detect by Head %2" PRIu32":                            %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.RVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] RVGA Skip Write Detect by Head7
        }

        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tFVGA Skip Write Detect by Head %2" PRIu32":                            %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.FVGASkipWriteDetect[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));   //!< [24] FVGA Skip Write Detect by Head7
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tSkip Write Detect Threshold Exceeded by Head %2" PRIu32":              %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.skipWriteDetectThresExceeded[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< [24] Skip Write Detect Threshold Exceeded Count by Head7
        }
        if (vFarmFrame.at(page).reliPage.MRHeadResistance[0] & BIT49)
        {
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                printf("\tMR Head Resistance from Head %2" PRIu32":                              %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.MRHeadResistance[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
            }
        }
        else
        {
            // version 4.34 MR Head Resistance became a percentage. Check bit 49 if set then it is a percentage
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
                int64_t delta = M_IGETBITRANGE((check_Status_Strip_Status(vFarmFrame.at(page).reliPage.MRHeadResistance[loopCount])), 48, 0);
                whole = M_WordInt2(delta);							                             // get 5:4 whole part of the float
                double decimal = static_cast<double>(M_DoubleWordInt0(delta));                   // get 3:0 for the Deciaml Part of the float
                double number = 0.0;
                if (whole >= 0 && ((vFarmFrame.at(page).reliPage.MRHeadResistance[loopCount] & BIT49) != BIT49))
                {
                    number = static_cast<double>(whole) + (decimal * static_cast<double>(.0001F));
                }
                else
                {
                    number = static_cast<double>(whole) - (decimal * static_cast<double>(.0001F));
                }
                printf("\tMR Head Resistance percentage for Head %2" PRIu32":                    %.4lf \n", loopCount, number);       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
            }
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tVelocity Observer by Head %2" PRIu32":                                %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.velocityObserver[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head9,10
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tNumber of Velocity Observer by Head %2" PRIu32":                       %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.numberOfVelocityObserver[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head9,10
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tFly height outer clearance delta by Head %2" PRIu32":                  %0.04lf \n", loopCount,  \
                (M_WordInt0((check_Status_Strip_Status(vFarmFrame.at(page).reliPage.flyHeightClearance[loopCount].outer))) * .001));                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tFly height inner clearance delta by Head %2" PRIu32":                  %0.04lf \n", loopCount,  \
                (M_WordInt0((check_Status_Strip_Status(vFarmFrame.at(page).reliPage.flyHeightClearance[loopCount].inner))) * .001));                //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tFly height middle clearance delta by Head %2" PRIu32":                 %0.04lf \n",loopCount,  \
                (M_WordInt0(check_Status_Strip_Status((vFarmFrame.at(page).reliPage.flyHeightClearance[loopCount].middle))) * .001));               //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT trimmed mean bits in error Zone 0 by head %2" PRIu32":   %0.04lf \n",loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SAT[loopCount].zone0)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT trimmed mean bits in error Zone 1 by head %2" PRIu32":   %0.04lf \n", loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SAT[loopCount].zone1)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT trimmed mean bits in error Zone 2 by head %2" PRIu32":   %0.04lf \n", loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SAT[loopCount].zone2)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT iterations to converge Test Zone 0 by head %2" PRIu32":  %0.04lf \n", loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SATIterations[loopCount].zone0)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT iterations to converge Test Zone 1 by head %2" PRIu32":  %0.04lf \n", loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SATIterations[loopCount].zone1)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT iterations to converge Test Zone 2 by head %2" PRIu32":  %0.04lf \n", loopCount, static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(vFarmFrame.at(page).reliPage.currentH2SATIterations[loopCount].zone2)), 32) * 0.10));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT percentage of codewords at iteration level by head %2" PRIu32":  %" PRIu64" \n", loopCount, vFarmFrame.at(page).reliPage.currentH2SATPercentage[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT amplitude by head %2" PRIu32":                           %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.currentH2SATamplitude[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF)); 
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tCurrent H2SAT asymmetry by head %2" PRIu32":                           %0.04lf \n", loopCount, static_cast<double>(M_WordInt0(vFarmFrame.at(page).reliPage.currentH2SATasymmetry[loopCount]) * 0.10)); 
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tNumber of Reallocated Sectors by Head %2" PRIu32":                     %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.gList[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));                //!< [24] Number of Resident G-List per Head
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tNumber of pending Entries by Head %2" PRIu32":                         %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.pendingEntries[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));        //!< [24] Number of pending Entries per Head
        }

        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tDOS Ought to scan count by Head %2" PRIu32":                           %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.oughtDOS[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));               //!< [24] DOS Ought to scan count per head
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tDOS needs to scans count by Head %2" PRIu32":                          %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.needDOS[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));               //!< [24] DOS needs to scans count per head
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tDOS write Fault scans by Head %2" PRIu32":                             %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.writeDOSFault[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));            //!< [24] DOS  write Fault scans per head
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tWrite POS On (hrs) by Head %2" PRIu32":                                %0.04lf \n", loopCount, static_cast<double>(M_DoubleWord0(vFarmFrame.at(page).reliPage.writePOH[loopCount])) /3600.0);             //!< [24] write POS in sec value from most recent SMART Frame by head
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tSecond MR Head Resistance by Head %2" PRIu32":                         %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.secondMRHeadResistance[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));
        }
        for (loopCount = 0; loopCount < m_heads; ++loopCount)
        {
            printf("\tLifetime Terabytes Written per Head %2" PRIu32":                       %" PRIi64" \n", loopCount, vFarmFrame.at(page).reliPage.lifetimeWrites[loopCount] & INT64_C(0x00FFFFFFFFFFFFFF));
        }
        printf("\n\n");
    }
    std::ostringstream temp;
    if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
    {
        temp << "Head Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Head Information From Farm Log copy " << std::dec << page;
    }
    json_set_name(headInfo, temp.str().c_str());

    int_Data(headInfo, "DVGA Skip Write Detect", vFarmFrame.at(page).reliPage.DVGASkipWriteDetect, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "RVGA Skip Write Detect", vFarmFrame.at(page).reliPage.RVGASkipWriteDetect, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "FVGA Skip Write Detect", vFarmFrame.at(page).reliPage.FVGASkipWriteDetect, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Skip Write Detect Threshold Exceeded", vFarmFrame.at(page).reliPage.skipWriteDetectThresExceeded, m_heads, m_showStatusBits, m_showStatic);
    // version 4.34 MR Head Resistance became a percentage. Check bit 49 if set then it is a percentage
    if (vFarmFrame.at(page).reliPage.MRHeadResistance[0] & BIT49)
    {
        int_Data(headInfo, "MR Head Resistance", vFarmFrame.at(page).reliPage.MRHeadResistance, m_heads, m_showStatusBits);
    }
    else
    {
        int_Percent_Dword_Data(headInfo, "MR Head Resistance Percentage", vFarmFrame.at(page).reliPage.MRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    }
    int_Data(headInfo, "Velocity Observer", vFarmFrame.at(page).reliPage.velocityObserver, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo,"Fly height clearance delta outer", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, OUTER, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo, "Fly height clearance delta inner", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, INNER, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo, "Fly height clearance delta middle", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, MIDDLE, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 0", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 1", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE1, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 2", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE2, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 0", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 1", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE1, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 2", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE2, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Current H2SAT percentage of codewords at iteration level", vFarmFrame.at(page).reliPage.currentH2SATPercentage, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Current H2SAT amplitude", vFarmFrame.at(page).reliPage.currentH2SATamplitude, m_heads, m_showStatusBits, m_showStatic);
    float_Cal_Word_Data(headInfo, "Current H2SAT asymmetry", 0.10, vFarmFrame.at(page).reliPage.currentH2SATasymmetry, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Number of Reallocated Sectors", vFarmFrame.at(page).reliPage.gList, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Number of Reallocation Candidate Sectors", vFarmFrame.at(page).reliPage.pendingEntries, m_heads, m_showStatusBits, m_showStatic);

    int_Data(headInfo, "DOS Ought to scan count", vFarmFrame.at(page).reliPage.oughtDOS, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "DOS needs to scans count", vFarmFrame.at(page).reliPage.needDOS, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "DOS write Fault scans", vFarmFrame.at(page).reliPage.writeDOSFault, m_heads, m_showStatusBits, m_showStatic);
    float_Cal_DoubleWord_Data(headInfo, "Write Power On (hrs)", 3600, vFarmFrame.at(page).reliPage.writePOH, m_heads, m_showStatusBits, m_showStatic);
	int_Data(headInfo, "Second MR Head Resistance", vFarmFrame.at(page).reliPage.secondMRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    uint_Data(headInfo, "Lifetime Terabytes Written", vFarmFrame.at(page).reliPage.lifetimeWrites, m_heads, m_showStatusBits, m_showStatic);
    int_Dword_Data(headInfo, "Get Physical Element Status", vFarmFrame.at(page).driveInfo.gpes, m_heads, m_showStatusBits, m_showStatic);

    json_push_back(masterData, headInfo);
    return eReturnValues::SUCCESS;
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
