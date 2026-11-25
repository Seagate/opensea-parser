//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
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
    , v_Buff()
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
CATA_Farm_Log::CATA_Farm_Log(std::vector<uint8_t>& buff, bool showStatus, bool showStatic)
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
    , v_Buff(buff)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , m_FrameReason(0)
    , m_ShowAct1(false)
    , m_showStatic(showStatic)
{
    if (v_Buff.size() != 0)
    {
        m_logSize = v_Buff.size();
        if (m_logSize < sizeof(sFarmHeader) || m_logSize < sizeof(sFarmFrame))
        {

            m_status = eReturnValues::BAD_PARAMETER;
        }
        else
        {
            m_pHeader = reinterpret_cast<sFarmHeader *>(&v_Buff.at(0));
            m_totalPages = m_pHeader->pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_logSize = m_pHeader->logSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_pageSize = m_pHeader->pageSize & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_heads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_MaxHeads = m_pHeader->headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF);
            m_copies = m_pHeader->historicalCopies & UINT64_C(0x00FFFFFFFFFFFFFF);
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
    if (v_Buff.size() == 0)
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
            // TODO : Add in a check to make sure we don't go past the buffer size
            if ((offset + m_pageSize) > v_Buff.size())
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
                break;
            }
            sDriveInfo *idInfo = reinterpret_cast<sDriveInfo*>(&v_Buff.at(offset));                   // get the id drive information at the time.
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

            sWorkLoadStat *pworkLoad = reinterpret_cast<sWorkLoadStat*>(&v_Buff.at(offset));           // get the work load information
            safe_memmove(&pFarmFrame->workLoadPage, sizeof(sWorkLoadStat), pworkLoad, sizeof(sWorkLoadStat));
            if (!Check_Page_number(pworkLoad->pageNumber, 2))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sErrorStat *pError = reinterpret_cast<sErrorStat*>(&v_Buff.at(offset));                    // get the error status
            safe_memmove(&pFarmFrame->errorPage, sizeof(sErrorStat), pError, sizeof(sErrorStat));
            if (!Check_Page_number(pError->pageNumber, 3))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sEnvironementStat *pEnvironment = reinterpret_cast<sEnvironementStat*>(&v_Buff.at(offset)); // get the envirmonent information 
            safe_memmove(&pFarmFrame->environmentPage, sizeof(sEnvironementStat), pEnvironment, sizeof(sEnvironementStat));
            if (!Check_Page_number(pEnvironment->pageNumber, 4))
            {
                retStatus = eReturnValues::VALIDATION_FAILURE;
            }
            offset += m_pageSize;

            sAtaReliabilityStat *pReli = reinterpret_cast<sAtaReliabilityStat*>(&v_Buff.at(offset));         // get the Reliabliity stat
            safe_memmove(&pFarmFrame->reliPage, sizeof(sAtaReliabilityStat), pReli, sizeof(sAtaReliabilityStat));
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
    sFarmHeader* header = reinterpret_cast<sFarmHeader*>(&v_Buff.at(0));                                                                                    // pointer to the header to get the signature

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
    std::string reason;
    Get_FARM_Reason_For_Capture(&reason, M_Byte0(header->reasonForFrameCapture));
    json_push_back(pageInfo, json_new_a("Reason for Frame Capture", reason.c_str()));


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

    std::ostringstream temp;
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

    // customer wanted to see data in hex
    temp.str(""); temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(opensea_parser::check_Status_Strip_Status(vFarmFrame.at(page).driveInfo.depopulationHeadMask));
    set_json_string_With_Status(pageInfo, "Depopulation Head Mask", temp.str().c_str(), vFarmFrame.at(page).driveInfo.depopulationHeadMask, m_showStatusBits);


    //version 4.21

    set_json_64_bit_With_Status(pageInfo, "Head Flight Hours - Actuator 1", vFarmFrame.at(page).driveInfo.headFlightHoursAct1, false, m_showStatusBits);       //!< Head Flight Hours- Actuator 1
    set_json_64_bit_With_Status(pageInfo, "Head Load Events - Actuator 1", vFarmFrame.at(page).driveInfo.headLoadEventsAct1, false, m_showStatusBits);         //!< Head Load Events- Actuator 1
    set_json_bool_With_Status(pageInfo, "Data Protect Status", vFarmFrame.at(page).driveInfo.ProtectStatus, m_showStatusBits);
    temp.str(""); temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(opensea_parser::check_Status_Strip_Status(vFarmFrame.at(page).driveInfo.regenHeadMask));
    set_json_string_With_Status(pageInfo, "Regen Head Mask", temp.str().c_str(), vFarmFrame.at(page).driveInfo.regenHeadMask, m_showStatusBits);
    // version 4.41
    set_json_64_bit_With_Status(pageInfo, "POH most recent FARM time series save", vFarmFrame.at(page).driveInfo.POHMostRecentSave, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "POH second most recent FARM time series save", vFarmFrame.at(page).driveInfo.POHSecondMostRecentSave, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Sequential Read Before Required for active zone configuration", vFarmFrame.at(page).driveInfo.SeqActiveZone, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Sequential Write Required for active zone configuration", vFarmFrame.at(page).driveInfo.SeqWriteActiveZone, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs (SWR capacity)", vFarmFrame.at(page).driveInfo.numLBA, false, m_showStatusBits);




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
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (16KB - 512KB)", vFarmFrame.at(page).workLoadPage.numReadTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (512KB - 2MB)", vFarmFrame.at(page).workLoadPage.numReadTransferMid2ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length > 2MB", vFarmFrame.at(page).workLoadPage.numReadTransferLargeATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length <=16KB", vFarmFrame.at(page).workLoadPage.numWriteTransferSmallATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (16KB - 512KB)", vFarmFrame.at(page).workLoadPage.numWriteTransferMid1ATA, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (512KB - 2MB)", vFarmFrame.at(page).workLoadPage.numWriteTransferMid2ATA, false, m_showStatusBits);
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
    set_json_64_bit_With_Status(pageInfo, "Read Cmd transfer length bin 4", vFarmFrame.at(page).workLoadPage.readCmdBin4, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Cmd transfer length bin 5", vFarmFrame.at(page).workLoadPage.readCmdBin5, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Cmd transfer length bin 6", vFarmFrame.at(page).workLoadPage.readCmdBin6, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Read Cmd transfer length bin 7", vFarmFrame.at(page).workLoadPage.readCmdBin7, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Cmd transfer length bin 4",vFarmFrame.at(page).workLoadPage.writeCmdBin4, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Cmd transfer length bin 5",vFarmFrame.at(page).workLoadPage.writeCmdBin5, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Cmd transfer length bin 6",vFarmFrame.at(page).workLoadPage.writeCmdBin6, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Write Cmd transfer length bin 7",vFarmFrame.at(page).workLoadPage.writeCmdBin7, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Total Number of Unaligned Writes Commands", vFarmFrame.at(page).workLoadPage.TotalUnalignedWrites, false, m_showStatusBits);

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
eReturnValues CATA_Farm_Log::print_Sense_Data(JSONNODE* masterData, uint32_t page)
{
    JSONNODE *pageInfo = json_new(JSON_ARRAY);
    std::ostringstream temp;
    temp << "Sense Code From Farm Log copy " << std::dec << page;
    json_set_name(pageInfo, temp.str().c_str());

    std::vector<SenseData> senseData;
    senseData.reserve(MAX_NUMBER_SENSE_DATA);
    for (uint32_t loopCount = 0; loopCount < MAX_NUMBER_SENSE_DATA; ++loopCount)
    {
        SenseData data;
        data.entry0 = static_cast<std::uint64_t>(vFarmFrame.at(page).errorPage.senseData[loopCount].entry0);
        data.entry1 = static_cast<std::uint64_t>(vFarmFrame.at(page).errorPage.senseData[loopCount].entry1);
        data.entry2 = static_cast<std::uint64_t>(vFarmFrame.at(page).errorPage.senseData[loopCount].entry2);
        senseData.push_back(data);
    }

    SenseCodeTable table;
    SenseCodeTable::SenseCodeClasses classes = table.ParseATASenseCodeData(senseData);

    const std::array<std::pair<const char*, const char*>, 13> printOrder = { {
        {"CDB", "CDB Operation Code"},
        {"StatusRegister", "Status Register"},
        {"ErrorRegister", "Error Register"},
        {"ErrorCode", "Error Code"},
        {"SenseKey", "Additional Sense Key"},
        {"SenseQualifier", "Additional Sense Key Qualifier"},
        {"Actuator", "Actuator"},
        {"LogicalHead", "Logical Head"},
        {"ErrorLBA", "Error LBA"},
        {"PowerCycle", "Power Cycle"},
        {"TimeStamp", "Time Stamp in seconds"},
        {nullptr, nullptr}
    } };

    for (const auto& classEntry : classes)
    {
        for (const auto& senseEntry : classEntry.second)
        {
            JSONNODE *senseNode = json_new(JSON_NODE);
            uint64_t myClass = senseEntry.at("Class").raw;
            uint64_t myCount = senseEntry.at("Class Count").raw;
            set_json_int_With_Status(senseNode, "Sense Code Class", myClass, false);
            set_json_int_With_Status(senseNode, "Class Count", myCount, false);

            for (const auto& fieldPair : printOrder)
            {
                if (fieldPair.first == nullptr)
                {
                    break;
                }
                const auto& attribute = senseEntry.at(fieldPair.first);
                if (attribute.isString)
                {
                    if (attribute.supported && attribute.valid)
                    {
                        json_push_back(senseNode, json_new_a(fieldPair.second, attribute.strValue.c_str()));
                    }
                    else
                    {
                        JSONNODE *nullNode = json_new(JSON_NODE);
                        json_set_name(nullNode, fieldPair.second);
                        json_nullify(nullNode);
                        json_push_back(senseNode, nullNode);
                    }
                }
                else
                {
                    if (attribute.supported && attribute.valid)
                    {
                        json_push_back(senseNode, json_new_i(fieldPair.second, static_cast<int64_t>(attribute.value)));
                    }
                    else
                    {
                        JSONNODE *nullNode = json_new(JSON_NODE);
                        json_set_name(nullNode, fieldPair.second);
                        json_nullify(nullNode);
                        json_push_back(senseNode, nullNode);
                    }
                }
            }
            json_push_back(pageInfo, senseNode);
        }
    }

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
    set_json_64_bit_With_Status(pageInfo, "Reallocated Sectors", vFarmFrame.at(page).errorPage.totalReallocations, false, m_showStatusBits);                       //!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Read Recovery Attempts", vFarmFrame.at(page).errorPage.totalReadRecoveryAttepts, false, m_showStatusBits);              //!< Number of Read Recovery Attempts
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame.at(page).errorPage.totalMechanicalFails, false, m_showStatusBits);               //!< Number of Mechanical Start Failures

    set_json_64_bit_With_Status(pageInfo, "Reallocated Candidate Sectors", vFarmFrame.at(page).errorPage.totalReallocatedCanidates,false, m_showStatusBits);       //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "ASR Events", vFarmFrame.at(page).errorPage.totalASREvents, false, m_showStatusBits);                                    //!< Number of ASR Events
    set_json_64_bit_With_Status(pageInfo, "Interface CRC Errors", vFarmFrame.at(page).errorPage.totalCRCErrors, false, m_showStatusBits);                          //!< Number of Interface CRC Errors
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count", vFarmFrame.at(page).errorPage.attrSpinRetryCount, false, m_showStatusBits);                          //!< Spin Retry Count (Most recent value from array at byte 401 of attribute sector)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Normalized", vFarmFrame.at(page).errorPage.normalSpinRetryCount, false, m_showStatusBits);             //!< Spin Retry Count (SMART Attribute 10 Normalized)
    set_json_64_bit_With_Status(pageInfo, "Spin Retry Count Worst", vFarmFrame.at(page).errorPage.worstSpinRretryCount, false, m_showStatusBits);                  //!< Spin Retry Count (SMART Attribute 10 Worst Ever)
    set_json_64_bit_With_Status(pageInfo, "IOEDC Errors (Raw)", vFarmFrame.at(page).errorPage.attrIOEDCErrors, false, m_showStatusBits);                           //!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "CTO Count Total", vFarmFrame.at(page).errorPage.attrCTOCount, false, m_showStatusBits);                                 //!< CTO Count Total (SMART Attribute 188 Raw[0..1])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 5s", vFarmFrame.at(page).errorPage.overfiveSecCTO, false, m_showStatusBits);                             //!< CTO Count Over 5s (SMART Attribute 188 Raw[2..3])
    set_json_64_bit_With_Status(pageInfo, "CTO Count Over 7.5s", vFarmFrame.at(page).errorPage.oversevenSecCTO, false, m_showStatusBits);                          //!< CTO Count Over 7.5s (SMART Attribute
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events", vFarmFrame.at(page).errorPage.totalFlashLED, false, m_showStatusBits);                //!< Total Flash LED (Assert) Events

    set_json_int_With_Status(pageInfo, "Index of the last Flash LED", vFarmFrame.at(page).errorPage.indexFlashLED, m_showStatusBits);                              //!< index of the last Flash LED of the array          
    set_json_64_bit_With_Status(pageInfo, "Uncorrectable errors", vFarmFrame.at(page).errorPage.uncorrectables, false, m_showStatusBits);                          //!< uncorrecatables errors (sata only)

    for (loopCount = 0; loopCount < FLASH_EVENTS; loopCount++)
    {
        temp.str("");temp.clear();
        JSONNODE *eventInfo = json_new(JSON_NODE);
        temp << "Flash LED Event " << std::dec << loopCount;
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
        double timeStamp = static_cast<double>(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.timestampForLED[loopCount]) / 3600000) * .001;
        set_json_float_With_Status(eventInfo, temp.str().c_str(), timeStamp, vFarmFrame.at(page).errorPage.timestampForLED[loopCount], m_showStatusBits);//!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array

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
            temp << "Read Write Retry " << std::dec << loopCount;
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
            temp << "Read Write Retry " << std::dec << loopCount;
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

        set_json_64_bit_With_Status(eventInfoact1, "Event Information Actuator 1", vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount], true, m_showStatusBits);              //!> Info on the last 8 Flash LED(assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount]));
        set_json_string_With_Status(eventInfoact1, "Flash LED Code Actuator 1", temp.str().c_str(), vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount], m_showStatusBits);

        temp.str(""); temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount]));
        set_json_string_With_Status(eventInfoact1, "Flash LED Address Actuator 1", temp.str().c_str(), vFarmFrame.at(page).errorPage.last8FLEDEventsAct1[loopCount], m_showStatusBits);

        temp.str(""); temp.clear();
        temp << "Timestamp Actuator 1 Event (hours)" << std::dec << loopCount;
        double timestamp = static_cast<double>(check_Status_Strip_Status(vFarmFrame.at(page).errorPage.last8FLEDEvtsAct1[loopCount]) / 3600000) * .001;
        set_json_float_With_Status(eventInfoact1, temp.str().c_str(), timestamp, vFarmFrame.at(page).errorPage.last8FLEDEvtsAct1[loopCount], m_showStatusBits);        //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array- Actuator 1

        temp.str(""); temp.clear();
        temp << "Power Cycle Actuator 1 Event" << std::dec << loopCount;
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
        set_json_int_Check_Status(rwrInfo, "Log Entry Actuator 1", M_Word2(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Error Type Actuator 1", M_Byte6(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Zone Group Actuator 1", M_Word1(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Head Actuator 1", M_Byte1(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);
        set_json_int_Check_Status(rwrInfo, "Count Actuator 1", M_Byte0(vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount]), vFarmFrame.at(page).errorPage.last8ReadWriteRetryEvts[loopCount], m_showStatusBits);

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
            if (M_Byte4(vFarmFrame.at(page).errorPage.pfaAttribute1) == 0x0A)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x0A", true);
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
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastReallocatedSectorsAct1)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors since the last FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.lastReallocatedSectorsAct1, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct1)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocated sectors Previous FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.betweenReallocatedSectorsAct1, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.lastCandidateSectorsAct1)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors since the last FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.lastCandidateSectorsAct1, m_showStatusBits);
        reallo = opensea_parser::check_for_signed_int(opensea_parser::check_Status_Strip_Status(static_cast<uint64_t>(vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct1)), 8);
        opensea_parser::set_json_int_Check_Status(pageInfo, "Reallocation candidate sectors previous FARM Frame, Actuator 1", reallo, vFarmFrame.at(page).errorPage.betweenCandidateSectorsAct1, m_showStatusBits);
        
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

    double humidity = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.humidity)) * static_cast<double>(.1F);                                                      //!< Current Relative Humidity (in units of .1%)
    set_json_float_With_Status(pageInfo, "Current Relative Humidity", humidity, vFarmFrame.at(page).environmentPage.humidity, m_showStatusBits);
    set_json_int_With_Status(pageInfo, "Current Motor Power", vFarmFrame.at(page).environmentPage.currentMotorPower, m_showStatusBits);                                                //!< Current Motor Power, value from most recent SMART Summary Frame6
    double volts = 0.0;
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.current12v)) *static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Current 12 volts", volts, vFarmFrame.at(page).environmentPage.current12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.min12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 12 volts", volts, vFarmFrame.at(page).environmentPage.min12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.max12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 12 volts", volts, vFarmFrame.at(page).environmentPage.max12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.current5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Current 5 volts", volts, vFarmFrame.at(page).environmentPage.current5v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.min5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 5 volts", volts, vFarmFrame.at(page).environmentPage.min5v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.max5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 5 volts", volts, vFarmFrame.at(page).environmentPage.max5v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerAvg12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Average 12V Power mW", volts, vFarmFrame.at(page).environmentPage.powerAvg12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerMin12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 12V Power mW", volts, vFarmFrame.at(page).environmentPage.powerMin12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerMax12v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 12V Power mW", volts, vFarmFrame.at(page).environmentPage.powerMax12v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerAvg5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Average 5V Power mW", volts, vFarmFrame.at(page).environmentPage.powerAvg5v, m_showStatusBits);  
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerMin5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 5V Power mW", volts, vFarmFrame.at(page).environmentPage.powerMin5v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.powerMax5v)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 5V Power mW", volts, vFarmFrame.at(page).environmentPage.powerMax5v, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.minPower12V)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo,"Minimum 12V Power mV since last reset", volts, vFarmFrame.at(page).environmentPage.minPower12V, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.maxPower12V)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 12V Power mV since last reset", volts, vFarmFrame.at(page).environmentPage.maxPower12V, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.minPower5V)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Minimum 5V Power mV since last reset", volts, vFarmFrame.at(page).environmentPage.minPower5V, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.maxPower5V)) * static_cast<double>(.001F);
    set_json_float_With_Status(pageInfo, "Maximum 5V Power mV since last reset", volts, vFarmFrame.at(page).environmentPage.maxPower5V, m_showStatusBits);
    volts = static_cast<double>(M_Word0(vFarmFrame.at(page).environmentPage.timeSinceLast)) / static_cast<double>(60.0F);
    set_json_float_With_Status(pageInfo, "Time Since last power cycle or pulling of FARM (hours)", volts, vFarmFrame.at(page).environmentPage.timeSinceLast, m_showStatusBits);

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
    if ((vFarmFrame.at(page).reliPage.heliumPresureTrip & BIT48) == BIT48)
    {
        vFarmFrame.at(page).reliPage.heliumPresureTrip = vFarmFrame.at(page).reliPage.heliumPresureTrip & UINT64_C(0xFFF0FFFFFFFFFFFF);
    }
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

    gpes_Status_Data(headInfo, "Get Physical Element Status", vFarmFrame.at(page).driveInfo.gpes, m_heads, false, m_showStatusBits, m_showStatic);
    uint_bit_range_Data(headInfo, "GPES Timestamp", vFarmFrame.at(page).driveInfo.gpes, 39, 8, m_heads, m_showStatusBits, m_showStatic);
    //verion 4.57
    if (m_MajorRev >= 4 && m_MinorRev < 57)
    {
        uint_Data(headInfo, "Lifetime Sector Read in TB", vFarmFrame.at(page).workLoadPage.lifeTimeReads, m_heads, m_showStatusBits, m_showStatic);
    }
    else
    {
        uint_Cal_Byte_Data(headInfo, "Lifetime Sector Read in GB", 10, vFarmFrame.at(page).workLoadPage.lifeTimeReads, m_heads, m_showStatusBits, m_showStatic);
    }

    //verion 4.20
    if ((m_MajorRev >= 4 && m_MinorRev > 20) || m_showStatic)
    {
        uint_Data(headInfo, "Cum Lifetime Unrecoverable Read Repeating", vFarmFrame.at(page).errorPage.cumLifeUnRecoveralbeReadByhead, m_heads, m_showStatusBits, m_showStatusBits);
        uint_Data(headInfo, "Cum Lifetime Unrecoverable Read Unique", vFarmFrame.at(page).errorPage.cumLiveUnRecoveralbeReadUnique, m_heads, m_showStatusBits, m_showStatusBits);

        uint_Data(headInfo, "Unique unrecoverable sectors since the last FARM Frame", vFarmFrame.at(page).errorPage.lastUniqueURE, m_heads, m_showStatusBits, m_showStatusBits);
        int_Data(headInfo, "Unique unrecoverable sectors previous FARM Frame", vFarmFrame.at(page).errorPage.betweenUniqueURE, m_heads, m_showStatusBits, m_showStatusBits);
    }
    
    if (m_MajorRev >= 4 && m_MinorRev < 57)
    {
        int_Data(headInfo, "DVGA Skip Write Detect", vFarmFrame.at(page).reliPage.reserved15, m_heads, m_showStatusBits, m_showStatic);
        int_Data(headInfo, "RVGA Skip Write Detect", vFarmFrame.at(page).reliPage.reserved16, m_heads, m_showStatusBits, m_showStatic);
        int_Data(headInfo, "FVGA Skip Write Detect", vFarmFrame.at(page).reliPage.reserved17, m_heads, m_showStatusBits, m_showStatic);
        int_Data(headInfo, "Skip Write Detect Threshold Exceeded", vFarmFrame.at(page).reliPage.reserved18, m_heads, m_showStatusBits, m_showStatic);
    }
    // version 4.34 MR Head Resistance became a percentage. Check bit 48 if set then it is a percentage
    if ((vFarmFrame.at(page).reliPage.MRHeadResistance[0] & BIT48) == BIT48)
    {
        int_Percent_Dword_Data(headInfo, "MR Head Resistance Percentage", vFarmFrame.at(page).reliPage.MRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    }
    else
    {
        int_Data(headInfo, "MR Head Resistance", vFarmFrame.at(page).reliPage.MRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    }
    int_Data(headInfo, "Velocity Observer", vFarmFrame.at(page).reliPage.velocityObserver, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo,"Fly height clearance delta OD", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, OUTER, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo, "Fly height clearance delta ID", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, INNER, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    sflyHeightData(headInfo, "Fly height clearance delta MD", 0.001, vFarmFrame.at(page).reliPage.flyHeightClearance, MIDDLE, WORDINT0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 0", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 1", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE1, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT trimmed mean bits in error Zone 2", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SAT, ZONE2, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 0", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE0, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 1", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE1, m_heads, m_showStatusBits, m_showStatic);
    h2sat_Float_Data(headInfo, "Current H2SAT iterations to converge Test Zone 2", WORD0, 0.10, vFarmFrame.at(page).reliPage.currentH2SATIterations, ZONE2, m_heads, m_showStatusBits, m_showStatic);
    float_Cal_DoubleWord_Data(headInfo, "Current H2SAT percentage of codewords at iteration level", 10.0, vFarmFrame.at(page).reliPage.currentH2SATPercentage, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Current H2SAT amplitude", vFarmFrame.at(page).reliPage.currentH2SATamplitude, m_heads, m_showStatusBits, m_showStatic);
    float_Cal_Word_Data(headInfo, "Current H2SAT asymmetry", 0.10, vFarmFrame.at(page).reliPage.currentH2SATasymmetry, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Number of Reallocated Sectors", vFarmFrame.at(page).reliPage.gList, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "Number of Reallocation Candidate Sectors", vFarmFrame.at(page).reliPage.pendingEntries, m_heads, m_showStatusBits, m_showStatic);

    int_Data(headInfo, "DOS Ought to scan count", vFarmFrame.at(page).reliPage.oughtDOS, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "DOS needs to scans count", vFarmFrame.at(page).reliPage.needDOS, m_heads, m_showStatusBits, m_showStatic);
    int_Data(headInfo, "DOS write Fault scans", vFarmFrame.at(page).reliPage.writeDOSFault, m_heads, m_showStatusBits, m_showStatic);
    float_Cal_DoubleWord_Data(headInfo, "Write Power On (hrs)", 3600, vFarmFrame.at(page).reliPage.writePOH, m_heads, m_showStatusBits, m_showStatic);
    if ((vFarmFrame.at(page).reliPage.secondMRHeadResistance[0] & BIT48) == BIT48)
    {
        int_Percent_Dword_Data(headInfo, "Second MR Head Resistance Percentage", vFarmFrame.at(page).reliPage.secondMRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    }
    else
    {
        int_Data(headInfo, "Second MR Head Resistance", vFarmFrame.at(page).reliPage.secondMRHeadResistance, m_heads, m_showStatusBits, m_showStatic);
    }
    //4.57
    if (m_MajorRev >= 4 && m_MinorRev < 57)
    {
        uint_Data(headInfo, "Lifetime Terabytes Written", vFarmFrame.at(page).reliPage.lifetimeWrites, m_heads, m_showStatusBits, m_showStatic);
    }
    else
    {
        uint_Cal_Byte_Data(headInfo, "Lifetime Gigabytes Written",10, vFarmFrame.at(page).reliPage.lifetimeWrites, m_heads, m_showStatusBits, m_showStatic);
    }

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
                print_Drive_Information(masterData, index);
                print_Work_Load(masterData, index);
                print_Sense_Data(masterData, index);
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
        print_Sense_Data(masterData, page);
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
        print_Sense_Data(masterData, page);
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
