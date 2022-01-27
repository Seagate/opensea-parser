//
// CScsi_Logical_Block_Provisioning_Logg.cpp  Definition of Logical Block Provisioning Log page
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Logical_Block_Provisioning_Log.cpp  Definition of Logical Block Provisioning Log page reports the logical block provisioning status 
//   of the logical
//
#include "CScsi_Logical_Block_Provisioning_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiLBAProvisionLog()
//
//! \brief
//!   Description: Default Class constructor 
//
//  Entry:
// \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiLBAProvisionLog::CScsiLBAProvisionLog()
    : pData()
    , m_LBAName("Logical Block Provisioning Log")
    , m_LBAStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength()
    , m_Provision()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_LBAName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiLBAProvisionLog()
//
//! \brief
//!   Description: Class constructor for the Cache Statistics
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiLBAProvisionLog::CScsiLBAProvisionLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_LBAName("Logical Block Provisioning Log")
    , m_LBAStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_Provision()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_LBAName.c_str());
    }
    if (buffer != NULL)
    {
        m_LBAStatus = IN_PROGRESS;
    }
    else
    {
        m_LBAStatus = FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiLBAProvisionLog
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiLBAProvisionLog::~CScsiLBAProvisionLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Resource_Percentage
//
//! \brief
//!   Description: parser out the data for the percentage
//
//  Entry:
//! \param percentage - string to give the percentage
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiLBAProvisionLog::get_Resource_Percentage(std::string *percentStr)
{
    if (m_Provision->resourceCount < 100)
    {
        std::ostringstream temp;
        temp << std::dec << m_Provision->resourceCount << " Percent";
        percentStr->assign(temp.str());
    }
    else
    {
        percentStr->assign("Invalid Percentage");
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_LBA_Provision_Parameter_Description
//
//! \brief
//!   Description: parser out the data for Logical Block Provisioning log
//
//  Entry:
//! \param description - string to give the Logical Block Provision depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiLBAProvisionLog::get_LBA_Provision_Parameter_Description(std::string *lbaStr)
{
    if (m_Provision->paramCode == 0x0000
        || (m_Provision->paramCode >= 0x0004 && m_Provision->paramCode <= 0x00FF)
        || (m_Provision->paramCode >= 0x0103 && m_Provision->paramCode <= 0xFFEF))
    {
        lbaStr->assign("Reserved");
    }
    else
    {
        switch (m_Provision->paramCode)
        {
        case 0x0001:
        {
            lbaStr->assign("available lba mapping resource count");
            break;
        }
        case 0x0002:
        {
            lbaStr->assign("used lba mapping resource count");
            break;
        }
        case 0x0003:
        {
            lbaStr->assign("available provisioning resource percentage");
            break;
        }
        case 0x0100:
        {
            lbaStr->assign("de-duplicated lba resource count");
            break;
        }
        case 0x0101:
        {
            lbaStr->assign("compressed LBA resource count");
            break;
        }
        case 0x0102:
        {
            lbaStr->assign("total efficiency LBA resource count");
            break;
        }
        default:
        {
            std::ostringstream temp;
            temp << "vendor specific " << std::dec << m_Provision->paramCode;
            lbaStr->assign(temp.str());
            break;
        }
        }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_LBA_Provision_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed LBA Provision data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiLBAProvisionLog::process_LBA_Provision_Data(JSONNODE *lbaData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
#if defined _DEBUG
    printf("Logical Block Provisioning Log \n");
#endif
    byte_Swap_16(&m_Provision->paramCode);
    get_LBA_Provision_Parameter_Description(&myStr);
    JSONNODE *lbaInfo = json_new(JSON_NODE);
    json_set_name(lbaInfo, myStr.c_str());

    std::ostringstream temp;
    temp << "0x" << std::hex << std::setfill('0') << std::setw(4) << m_Provision->paramCode;
    json_push_back(lbaInfo, json_new_a("Logical Block Provisioning Parameter Code", temp.str().c_str()));
    temp.str().clear(); temp.clear();
    temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Provision->paramControlByte);
    json_push_back(lbaInfo, json_new_a("Logical Block Provisioning Control Byte ", temp.str().c_str()));
    temp.str().clear(); temp.clear();
    temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Provision->paramLength);
    json_push_back(lbaInfo, json_new_a("Logical Block Provisioning Length ", temp.str().c_str()));
    if (m_Provision->paramCode == 0x0003)
    {
        get_Resource_Percentage(&myStr);
        json_push_back(lbaInfo, json_new_a("Percentage of Resources Available", myStr.c_str()));
    }
    else
    {
        json_push_back(lbaInfo, json_new_i("Logical Block Provisioning Resource Count", m_Provision->resourceCount));
    }

    json_push_back(lbaData, lbaInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data
//
//! \brief
//!   Description: parser out the data for the Logical Block Provisioning Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiLBAProvisionLog::get_LBA_Data(JSONNODE *masterData)
{
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Logical Block Provisioning Log - 0Ch");

        for (size_t offset = 0; offset < static_cast<size_t>(m_PageLength); )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_Provision = reinterpret_cast<sLBA *>(&pData[offset]);
                process_LBA_Provision_Data(pageInfo);
                offset += sizeof(sLBA);
            }
            else
            {
                json_push_back(masterData, pageInfo);
                return BAD_PARAMETER;
            }

        }

        json_push_back(masterData, pageInfo);
        retStatus = SUCCESS;
    }
    else
    {
        retStatus = MEMORY_FAILURE;
    }
    return retStatus;
}