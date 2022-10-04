//
// CFARM_WLM_Log.cpp
//
// Do NOT modify or remove this copyright and confidentiality notice.
//
// Copyright 2012 - 2017 Seagate Technology LLC.
//
// The code contained herein is CONFIDENTIAL to Seagate Technology LLC
// and may be covered under one or more Non-Disclosure Agreements.
// All or portions are also trade secret.
// Any use, modification, duplication, derivation, distribution or disclosure
// of this code, for any reason, not expressly authorized is prohibited.
// All other rights are expressly reserved by Seagate Technology LLC.
//
// *****************************************************************************

#include "CFARM_WLM_Log.h"   

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn   CFARMWLM()
//
//! \brief
//!   Description: Default Class constructor for the CFARMWLM
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::CFARMWLM()
    :pData(NULL)
    , m_name("WLM Log")
    , m_logSize(0)
    , m_FARMWLM(false)
    , m_version(0)
    , m_WLMstatus(IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
    , m_InterfaceType(PARSER_INTERFACE_TYPE_SATA)
    , m_logPageType(LOG_PAGE_WITH_HEADER)
{

}
//-----------------------------------------------------------------------------
//
//! \fn   CFARMWLM()
//
//! \brief
//!   Description:  Class constructor for the CWLMLog use for cdf and other logs \n
//!     where the log is part of another log
//
//  Entry:
//! \param bufferData - pointer to the buffer data for parsing
//! \param masterData - pointer to the json data
//! \param interfaceType - is this drive sas or sata
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::CFARMWLM( uint8_t *bufferData, JSONNODE *masterData, uint8_t interfaceType)
    :pData(bufferData)
    , m_name("WLM Log")
    , m_logSize(0)
    , m_FARMWLM(false)
    , m_version(0)
    , m_WLMstatus(IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
    , m_InterfaceType(interfaceType)
    , m_logPageType(LOG_PAGE_WITH_HEADER)
{
    if (bufferData != NULL)                           // if the buffer is null then exit something did not go right
    {

        m_DataHeaderPTR = &m_DataHeader;
        memcpy(&m_DataHeader, reinterpret_cast<sWLMDataHeader*>(pData), sizeof(sWLMDataHeader));

        byte_Swap_32(&m_DataHeader.signature);

        m_FARMWLM = (m_DataHeader.signature);
        m_logSize = m_DataHeader.frameSize;
        if (get_Version()) // init the data for getting the log
        {
            try
            {
                m_WLMstatus = parse_WLM_Summary(masterData);
                m_WLMstatus = print_WLM_Log(masterData);
            }
            catch (...)
            {
                m_WLMstatus = PARSING_EXCEPTION_FAILURE;
            }
        }
    }
    else
    {
        print_Empty_WLM_Log(masterData);
        m_WLMstatus = FAILURE;
    }
    pData = NULL; //so that its not deleted in destructor. Caller should noy=t use pData after this function returns
   // _CrtDumpMemoryLeaks();
}

//-----------------------------------------------------------------------------
//
//! \fn   ~CFARMWLM()
//
//! \brief
//!   Description:  Class deconstructor for for the CWLMLog
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::~CFARMWLM()
{
    if (!vFrame.empty())
    {
        vFrame.clear();
        vFrame.swap(vBlankFrame);
    }
    if (pData)
    {
        delete[] pData;
        pData = NULL;
    }
    //  _CrtDumpMemoryLeaks();
}

//-----------------------------------------------------------------------------
//
//   get_Version()
//
//! \brief
//!   Description:  get the ascii value of the string to find out the version of the logs
//
//  Entry:
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::get_Version()
{
    m_version = m_DataHeader.rev;
    return true;
}

//-----------------------------------------------------------------------------
//
//   is_FARM()
//
//! \brief
//!   Description:  Check the signature to see if it is an SSD drive vs HD
//
//  Entry:
//! \param signature - 32 bit holding the signature of the log
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
bool CFARMWLM::is_FARM(uint32_t signature)
{
    if (signature == FARMWLM)
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//
//   print_Standby_Percetage
//
//! \brief
//!   Description:  his field value indicates the percentage of the IDLE TIME that \n
//!   the device is in STANDBY mode (i.e., HDDs = media is not spinning; SSDs = full  \n
//!   power reduction mode) for this parameter interval.This is an encoded value \n
//!  (round to the nearest encoded range
//
//  Entry:
//! \param page - the page number in the vector
//! \param wlmJson - the json node that we are working with
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
bool CFARMWLM::print_Standby_Percetage(uint8_t standby, JSONNODE *wlmJson)
{
    std::string myStr;
    switch (M_GETBITRANGE(standby, 7, 4))
    {
    case 0:
        myStr.assign("no standby");
        break;
    case 0x1:
        myStr.assign("0 to 9 % standby");
        break;
    case 0x2:
        myStr.assign("10 to 19 % standby");
        break;
    case 0x3:
        myStr.assign("20 to 29 % standby");
        break;
    case 0x4:
        myStr.assign("30 to 39 % standby");
        break;
    case 0x5:
        myStr.assign("40 to 49 % standby");
        break;
    case 0x6:
        myStr.assign("50 to 59 % standby");
        break;
    case 0x7:
        myStr.assign("60 to 69 % standby");
        break;
    case 0x8:
        myStr.assign("70 to 79 % standby");
        break;
    case 0x9:
        myStr.assign("80 to 89 % standby");
        break;
    case 0xa:
        myStr.assign("90 to 100 % standby");
        break;
    case 0xb:
        myStr.assign("full standby");
        break;
    default:
        myStr.assign("reserved");
    }
    json_push_back(wlmJson, json_new_a("standby percentage", myStr.c_str()));
    return true;
}
//-----------------------------------------------------------------------------
//
//   print_Standby_Percetage
//
//! \brief
//!   Description:  This field value reports the lowest link rate for port 0 during this parameter interval as follows:
//!   �	0000 = unknown
//!   �	0001 = 1.5 Gb / s
//!   �	0010 = 3.0 Gb / s
//!   �	0011 = 6.0 Gb / s
//!   �	0100 to 1111 = reserved
//
//  Entry:
//! \param page - the page number in the vector
//! \param wlmJson - the json node that we are working with
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
bool CFARMWLM::print_Minimn_Link_Rate_Port(uint8_t link, JSONNODE *wlmJson)
{
    std::string myStr;
    //uint8_t link = vFrame.at(page).sataParamVersion3.methodPort;
    uint8_t port0 = M_GETBITRANGE(link, 3, 0);
    uint8_t port1 = M_GETBITRANGE(link, 7, 4);
    if (port0 & BIT0)
    {
        myStr.assign("1.5 GBs");
    }
    else if (port0 & BIT1)
    {
        myStr.assign("3.0 GBs");
    }
    else if (port0 & BIT2)
    {
        myStr.assign("6.0 GBs");
    }
    else
    {
        myStr.assign("unknown");
    }
    json_push_back(wlmJson, json_new_a("minimum link rate port 0", myStr.c_str()));
    if (port1 & BIT0)
    {
        myStr.assign("1.5 GBs");
    }
    else if (port1 & BIT1)
    {
        myStr.assign("3.0 GBs");
    }
    else if (port1 & BIT2)
    {
        myStr.assign("6.0 GBs");
    }
    else
    {
        myStr.assign("unknown");
    }
    json_push_back(wlmJson, json_new_a("minimum link rate port 1", myStr.c_str()));
    return true;
}
//-----------------------------------------------------------------------------
//
//   print_Empty_WLM_Log
//
//! \brief
//!   Description:  Check the signature to see if it is an SSD drive vs HD
//
//  Entry:
//! \param signature - 32 bit holding the signature of the log
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_Empty_WLM_Log(JSONNODE *wlm)
{
    std::string emptyStr;
#if defined( _AMAZON_NONE_PRINT)
    emptyStr = "None";
#else
    emptyStr = " -1 ";
#endif
#ifdef _DEBUG
    printf("\tPower on Hours:                     %s \n", "-1");
    printf("\tTotal Number of Other Commands:           %s \n", emptyStr.c_str());
    printf("\tTotal Number of LBAs Read:                %s \n", emptyStr.c_str());
    printf("\tTotal Number of LBAs Written:             %s \n", emptyStr.c_str());
    printf("\tTotal Number of Write Commands:           %s \n", emptyStr.c_str());
    printf("\tTotal Number of Read Commands:            %s \n", emptyStr.c_str());
    printf("\tUnrecoverable Read Errors:                %s \n", emptyStr.c_str());
    printf("\tUnrecoverable Write Errors:               %s \n", emptyStr.c_str());
    printf("\tTotal Number of Random Read Cmds:         %s \n", emptyStr.c_str());
    printf("\tTotal Number of Random Write Cmds:        %s \n", emptyStr.c_str());
    printf("\t# of Task Management Operations:          %s \n", emptyStr.c_str());
#endif

    JSONNODE* wlmJson = json_new(JSON_NODE);
    json_set_name(wlmJson, "WLS Summary");

    json_push_back(wlmJson, json_new_a("Power on Hours", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Other Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of LBAs Reads", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of LBAs Written", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Write Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Read Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Unrecoverable Read Errors", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Unrecoverable Write Errors", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Random Read Cmds", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Random Write Cmds", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("# of Task Management Operations", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Idle Time", emptyStr.c_str()));
    json_push_back(wlm, wlmJson);

    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//   CWLMLog::print_Summary_Log()
//
//! \brief
//!   Description:  Print out the summary data from the log
//
//  Entry:
//! \param wlm - pointer to the json data
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_Summary_Log(JSONNODE *wlm)
{

#ifdef _DEBUG
    printf("Workload Trace Header\n");
    printf("\tFrame Number:                             %" PRIu16" \n", m_DataHeader.farmeNumber);
    printf("\tStart Timestamp:                          %" PRIu64" \n", m_DataHeader.startTimestamp);
    printf("\tEnd Timestamp:                            %" PRIu64" \n", m_DataHeader.endTimestamp);
    printf("\tRead Operations:                          %" PRIu32" \n", m_DataHeader.readOps);
    printf("\tWrite Operations:                         %" PRIu32" \n", m_DataHeader.writeOps);
    printf("\tFrame Size:                               %" PRIu32" \n", m_DataHeader.frameSize);
    printf("\tCycle Count:                              %" PRIu16" \n", m_DataHeader.cycleCount);
    printf("\tOne Second Markers:                       %" PRIu32" \n", m_DataHeader.markers);

#endif
   
    JSONNODE* wlmJson = json_new(JSON_NODE);
    json_set_name(wlmJson, "Workload Frame Header");
    std::ostringstream temp;
    temp << std::dec << m_DataHeader.farmeNumber;
    json_push_back(wlmJson, json_new_a("Frame Number", temp.str().c_str()));

    temp.str(""); temp.clear();
    temp << std::dec << m_DataHeader.rev;
    json_push_back(wlmJson, json_new_a("version", temp.str().c_str()));
    opensea_parser::set_json_64bit(wlmJson, "Start Timestamp", m_DataHeader.startTimestamp, false);
    opensea_parser::set_json_64bit(wlmJson, "End Timestamp", m_DataHeader.endTimestamp, false);
    opensea_parser::set_json_64bit(wlmJson, "Read Operations", m_DataHeader.readOps, false);
    opensea_parser::set_json_64bit(wlmJson, "Write Operations", m_DataHeader.writeOps, false);
    opensea_parser::set_json_64bit(wlmJson, "Frame Size", m_DataHeader.frameSize, false);
    opensea_parser::set_json_64bit(wlmJson, "Cycle Count", m_DataHeader.cycleCount, false);
    opensea_parser::set_json_64bit(wlmJson, "One Second Markers", m_DataHeader.markers, false);

    json_push_back(wlm, wlmJson);
    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//   CWLMLog::print_WLM_SATA_Trace()
//
//! \brief
//!   Description:  Take the data and print the parameters
//
//  Entry:
//! \param POH - pointer to the buffer of data
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_WLM_SATA_Trace(JSONNODE *wlmSata, uint32_t page)
{
    JSONNODE* wlmJson = json_new(JSON_NODE);
    json_set_name(wlmJson, "WLM Sata Trace");
#ifdef _DEBUG
    printf("\tLBA:                                               0x%" PRIx32" \n", vFrame.at(page).sataTrace.LBA);
    printf("\tTransfer lenghts:                                  %" PRIu32" \n", vFrame.at(page).sataTrace.transLength);
    printf("\tSteaming Event:                                    0x%" PRIx32" \n", vFrame.at(page).sataTrace.streamEvent);
    printf("\tOne Second Marker:                                 %" PRIu32" \n", vFrame.at(page).sataTrace.markers);

#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << static_cast<uint64_t>(vFrame.at(page).sataTrace.LBA);
    json_push_back(wlmJson, json_new_a("LBA", temp.str().c_str()));
    json_push_back(wlmJson, json_new_i("LBA", vFrame.at(page).sataTrace.LBA));

    json_push_back(wlmJson, json_new_i("Transfer Length", vFrame.at(page).sataTrace.transLength ));
    temp.str("");
    temp.clear();
    temp << "0x" << std::hex << vFrame.at(page).sataTrace.streamEvent;
    json_push_back(wlmJson, json_new_a("Steaming Events", temp.str().c_str()));
   
    json_push_back(wlmJson, json_new_i( "One Second Marker", vFrame.at(page).sataTrace.markers));
   
    json_push_back(wlmSata, wlmJson);
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//  parse_WLM_Summary()
//
//! \brief
//!   Description:  
//
//  Entry:
//! \param ptr - 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::parse_WLM_Summary(JSONNODE *masterData)
{
    sWLMFrame workFrame;
    sWLMFrame *pWF = &workFrame;
    uint32_t offset = 0;
    // _CrtDumpMemoryLeaks();
    
    offset = sizeof(sWLMDataHeader);
        
    while (offset < m_logSize)
    {
         if (m_DataHeader.signature == FARMWLM && m_version == 0x03)
        {

            sWLM_Trace* data;
            data = reinterpret_cast<sWLM_Trace*>(&pData[offset]);
            memcpy(&workFrame.sataTrace, data, sizeof(sWLM_Trace));
            workFrame.version = m_version;
            vFrame.push_back(*pWF);
            offset += sizeof(sWLM_Trace);
        }
        else
        {
            print_Empty_WLM_Log(masterData);
            return FAILURE;
        }

    }
 
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//   CFARMWLM::print_Log()
//
//! \brief
//!   Description:  parses the work load management log from the CDF file
//
//  Entry:
//! \param pData - pointer to the buffer of data
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_Log(uint32_t page, JSONNODE *wlm)
{

    eReturnValues retStatus = IN_PROGRESS;

    if (vFrame.at(page).version == 3 )
    { 
        retStatus = print_WLM_SATA_Trace(wlm, page);
    }
    else
    {
        retStatus = print_Empty_WLM_Log(wlm);
    }

    return retStatus;

}
//-----------------------------------------------------------------------------
//
//   CFARMWLM::print_WLM_Log()
//
//! \brief
//!   Description: print out the WLM Log
//
//  Entry:
//! \param pData - pointer to the buffer of data
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_WLM_Log(JSONNODE *masterData)
{
    eReturnValues retStatus = IN_PROGRESS;
    if (vFrame.at(0).version == 3)
    {
        retStatus = print_Summary_Log(masterData);
    }
    if (retStatus == SUCCESS)
    {
        JSONNODE* wlmJson = json_new(JSON_ARRAY);
        json_set_name(wlmJson, "Workload Frame");
        for (uint32_t index = 0; index < vFrame.size(); ++index)
        {
            retStatus = print_Log(index, wlmJson);
            
        }
        json_push_back(masterData, wlmJson);
    }
    return retStatus;
}

