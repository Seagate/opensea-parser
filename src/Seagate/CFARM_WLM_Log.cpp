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
    , m_WLMstatus(IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
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
CFARMWLM::CFARMWLM( uint8_t *bufferData, uint32_t dataSize, JSONNODE *masterData)
    :pData(bufferData)
    , m_name("WLM Log")
    , m_logSize(dataSize)
    , m_WLMstatus(IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
{
    if (bufferData != NULL)                           // if the buffer is null then exit something did not go right
    {
        m_DataHeaderPTR = &m_DataHeader;
        if (validate_WLM())
        {
            try
            {
                m_WLMstatus = parse_WLM_Summary(masterData);
            }
            catch (...)
            {
                m_WLMstatus = PARSING_EXCEPTION_FAILURE;
            }

        }
        else
        {
            print_Empty_WLM_Log(masterData);
            m_WLMstatus = VALIDATION_FAILURE;
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
//   validate_WLM()
//
//! \brief
//!   Description:  Check the header and footer singatures for validation
//
//  Entry:
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::validate_WLM()
{
#define BYTES 8

    size_t offset = 0;
    uint32_t endSignature = 0;
    
    // get Header information Then get the ending signature
    if (get_WLM_Header_Data(offset))
    { 
        // go to the end of the frame and get the ending singature.   TRACESIZE and back 8 bytes to get the ending signature
        offset = TRACESIZE - BYTES;   
        // get the ending signature
        endSignature = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
    }
    else
    {
        m_WLMstatus = VALIDATION_FAILURE;
        return false;
    }

    // check the signatures if good return true
    if (m_DataHeader.signature == FARMWLM && endSignature == FARMFOOTWLM && (m_DataHeader.rev == WLM3 || m_DataHeader.rev == WLM6))
    {
        return true;
    }
    else
    {
        m_WLMstatus = VALIDATION_FAILURE;
    }
    // doesn't look right pass false back
    return false;
}
//-----------------------------------------------------------------------------
//
//   get_WLM_Header_Data()
//
//! \brief
//!   Description:  get WLM version 6 Header information 
//
//  Entry:
//! \param  offset: the begining offset for getting the data 
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::get_WLM_Header_Data(size_t offset)
{
    // get the signature of the WLM trace
    m_DataHeader.signature = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
    offset += sizeof(m_DataHeader.signature);
    m_DataHeader.farmeNumber = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.farmeNumber);
    m_DataHeader.rev = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.rev);

    m_DataHeader.startTimestamp = M_BytesTo8ByteValue(pData[offset + 7], pData[offset + 6], pData[offset + 5], pData[offset + 4], pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.startTimestamp);
    m_DataHeader.endTimestamp = M_BytesTo8ByteValue(pData[offset + 7], pData[offset + 6], pData[offset + 5], pData[offset + 4], pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.endTimestamp);

    m_DataHeader.preFrameOffset = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.preFrameOffset);
    m_DataHeader.cycleCount = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.cycleCount) + sizeof(m_DataHeader.pad1);

    m_DataHeader.frameSize = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    if (m_DataHeader.frameSize >= TRACESIZE || m_DataHeader.frameSize <= 1)
    {
        return false;
    }
    offset += sizeof(m_DataHeader.frameSize);
    m_DataHeader.duration = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.duration);
    m_DataHeader.readOps = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.readOps);
    m_DataHeader.writeOps = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.writeOps);
    m_DataHeader.markers = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.markers);
    return true;


    
}
//-----------------------------------------------------------------------------
//
//   get_transferSize()
//
//! \brief
//!   Description:  get the size of the transfer
//
//  Entry:
//! \param tlength - the tlen from the codeByte
//
//  Exit:
//!   \return uint8_t 
//
//---------------------------------------------------------------------------
uint16_t CFARMWLM::get_transferSize(uint8_t tLen, size_t* offset)
{
    uint16_t len = 0;
    if (tLen == 0)
    {
        len = 1;
    }
    else if (tLen == 1)
    {
        len = 2;
    }
    else if (tLen == 2)
    {
        len = 4;
    }
    else if (tLen == 3)
    {
        len = 8;
    }
    else if (tLen == 4)
    {
        len = 16;
    }
    else if (tLen == 5)
    {
        len = 32;
    }
    else if (tLen == 6)
    {
        len = 128;
    }
    else if (tLen == 7)
    {
        len = 256;
    }
    else if (tLen == 9)
    {
        len = pData[*offset];
        *offset += 1;
    }
    else if (tLen == 0xA)
    {
        len = M_BytesTo2ByteValue(pData[*offset +1],pData[*offset]);
        *offset += 2;
    }
    else
    {
        len = 0;
    }
    return len;
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
    emptyStr = "None";

    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
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
    }

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
eReturnValues CFARMWLM::print_Summary_Log(size_t* offset, JSONNODE *wlm)
{
#define LBAOFFSET 0x3c
#define HEADERSIZE 8
    eReturnValues retStatus = SUCCESS;
    if (get_WLM_Header_Data(*offset ))
    {

        if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("Workload Trace Header\n");
            printf("\tFrame Number:                             %" PRIu16" \n", m_DataHeader.farmeNumber);
            printf("\tStart Timestamp:                          %" PRIu64" \n", m_DataHeader.startTimestamp);
            printf("\tEnd Timestamp:                            %" PRIu64" \n", m_DataHeader.endTimestamp);
            printf("\tRead Operations:                          %" PRIu32" \n", m_DataHeader.readOps);
            printf("\tWrite Operations:                         %" PRIu32" \n", m_DataHeader.writeOps);
            printf("\tFrame Size:                               %" PRIu32" \n", m_DataHeader.frameSize);
            printf("\tCycle Count:                              %" PRIu16" \n", m_DataHeader.cycleCount);
            printf("\tOne Second Markers:                       %" PRIu32" \n", m_DataHeader.markers);

        }
   
        JSONNODE* wlmJson = json_new(JSON_NODE);
        std::ostringstream temp;
        temp << "Workload Frame Number " << std::dec << m_DataHeader.farmeNumber;
        json_set_name(wlmJson, temp.str().c_str());

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

        *offset += LBAOFFSET;
        if (!get_Trace_Data(offset, wlmJson))
        {
            retStatus = PARSE_FAILURE;
        }

        json_push_back(wlm, wlmJson);
    }
    else
    {
        retStatus = PARSE_FAILURE;
    }
    return retStatus;
}
//-----------------------------------------------------------------------------
//
//  get_Trace_Data()
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
bool CFARMWLM::get_Trace_Data(size_t* offset, JSONNODE* wlmJson)
{
    uint64_t lba = 0;
    uint16_t tranSize = 0;
    size_t frameOffset = 0x3c;

    
    JSONNODE* tData = json_new(JSON_ARRAY);
    json_set_name(tData, ("Trace Data"));

    while (frameOffset <= m_DataHeader.frameSize)
    {
        size_t startOffset = *offset;
        uint8_t encodeByte = pData[*offset];
        uint8_t tLength = M_GETBITRANGE(encodeByte, 7, 4);
        uint8_t LDes = M_GETBITRANGE(encodeByte, 2, 0);
        if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("LDes  0x %" PRIx8" \n", LDes);
            printf("offset %llu \n", *offset);
            printf("encodeByte  0x %" PRIx8" \n", encodeByte);
            printf("tLength  0x %" PRIx8" \n", tLength);
        }
        *offset += 1;

        // if tLength is 0x08 the transfer size is the same as the previous operation, so don't change it
        if (tLength != 0x08)
        {
            tranSize = get_transferSize(tLength, offset);
        }

        if (LDes == 0)
        {
            if (tLength == 0xe)
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
            else if (tLength == 0xd)
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
            else if (tLength == 8)
            {
                lba += tranSize;
            }
            else
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
        }
        else if (LDes == 1)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, 0, pData[*offset]);
            *offset += 1;
        }
        else if (LDes == 2)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, pData[*offset + 1], pData[*offset]);
            *offset += 2;
        }
        else if (LDes == 3)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, pData[*offset + 2], pData[*offset + 1], pData[*offset]);
            *offset += 3;
        }
        else if (LDes == 4)
        {
            *offset += 1; // ???
        }
        else if (LDes == 5)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, 0, pData[*offset]);
            *offset += 1;
        }
        else if (LDes == 6)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, pData[*offset + 1], pData[*offset]);
            *offset += 2;
        }
        else if (LDes == 7)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, pData[*offset + 2], pData[*offset + 1], pData[*offset]);
            *offset += 3;
        }
        else
        {
            // push what we have and then return false
            printf(">>>>  Lost \n");
            json_push_back(wlmJson, tData);
            return false;
        }
        if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("LBA  0x %" PRIx64" \n", lba);
            printf("Transfer length %" PRIu16" \n", tranSize);
        }
        std::ostringstream temp;
        if (encodeByte & BIT3)
        {
            temp << "write" << "," << tranSize << "," "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << lba;
            json_push_back(tData, json_new_a( "write", temp.str().c_str()));
        }
        else
        {
            if (encodeByte == 0xf0)
            {
                json_push_back(tData, json_new_a("Maker", "One Second Marker"));
            }
            else
            {
                
                temp << "read" << "," << tranSize << "," "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << lba;
                json_push_back(tData, json_new_a("read", temp.str().c_str()));
            }
        }
        frameOffset += *offset - startOffset;
    }
    json_push_back(wlmJson, tData);
    return true;
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
    // _CrtDumpMemoryLeaks();
    eReturnValues retStatus = SUCCESS;
    size_t offset = 0;
    uint32_t traceLine = 0;
    size_t traceSize = TRACESIZE;
   
    offset = 0;
    while (offset  < m_logSize)
    {
        retStatus = print_Summary_Log(&offset, masterData);
        if (retStatus != SUCCESS)
        {
            return retStatus;
        }
        traceLine += TRACESIZE;
        offset = traceLine;
    }
 
    return retStatus;
}


