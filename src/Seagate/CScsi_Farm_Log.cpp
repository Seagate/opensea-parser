//
// CScsi_Farm_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Now supporting 3.7 version of the FARM LOG
// ******************************************************************************************

#include "CScsi_Farm_Log.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::CSCSI_Farm_Log()
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
CSCSI_Farm_Log::CSCSI_Farm_Log()
	: CFarmCommon()
    , m_totalPages()
	, m_logSize(0)                                             
	, m_pageSize(0)                                      
	, m_heads(0)                                   
	, m_MaxHeads(0)                                
	, m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf()
    , m_status(eReturnValues::IN_PROGRESS)                                
	, m_logParam()   
	, m_pageParam()
    , m_pHeader()
    , m_pDriveInfo() 
	, m_alreadySet(false)          
	, m_showStatusBits(false)
    , m_fromScsiLogPages(false)
    , m_farmSubPage(0)
    , m_showStatic(false)
{                                                  
	m_status = eReturnValues::IN_PROGRESS;

}
//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::CSCSI_Farm_Log()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//! \parma securityPrintLevel = the level of the print
//! \param bufferData = pointer to the buffer data.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------

CSCSI_Farm_Log::CSCSI_Farm_Log(uint8_t* bufferData, size_t bufferSize, uint8_t subPage, bool fromLogPage, bool showStatic)
    : CFarmCommon()
    , m_totalPages()
    , m_logSize(static_cast<uint16_t>(bufferSize))
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf(M_NULLPTR)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_logParam()
    , m_pageParam()
    , m_pHeader()
    , m_pDriveInfo()
    , m_alreadySet(false)
    , m_showStatusBits(false)
    , m_fromScsiLogPages(fromLogPage)
    , m_farmSubPage(subPage)
    , m_showStatic(showStatic)
{
    m_status = eReturnValues::IN_PROGRESS;
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("SCSI FARM Log \n");
    }

    pBuf = new uint8_t[bufferSize];								// new a buffer to the point	
    m_pHeader = new sScsiFarmHeader();
#ifndef __STDC_SECURE_LIB__
    memcpy(pBuf, bufferData, bufferSize);
#else
    memcpy_s(pBuf, bufferSize, bufferData, bufferSize);// copy the buffer data to the class member pBuf
#endif
    if (pBuf != M_NULLPTR)
    {
        if (init_Header_Data(bufferSize) == eReturnValues::SUCCESS)							// init the data for getting the log
        {
            m_status = parse_Farm_Log();
        }
        else
        {
            m_status = eReturnValues::FAILURE;
        }
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
    delete[] pBuf;
   
}
//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::CSCSI_Farm_Log()
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//! \parma securityPrintLevel = the level of the print
//! \param bufferData = pointer to the buffer data.
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------

CSCSI_Farm_Log::CSCSI_Farm_Log( uint8_t *bufferData, size_t bufferSize, uint8_t subPage, bool fromLogPage, bool showStatus, bool showStatic)
	:CFarmCommon()
    , m_totalPages()                                       
	, m_logSize(static_cast<uint16_t>(bufferSize))
	, m_pageSize(0)                                      
	, m_heads(0)                                   
	, m_MaxHeads(0)                                
	, m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf(M_NULLPTR)
    , m_status(eReturnValues::IN_PROGRESS)                                
	, m_logParam()
	, m_pageParam()
    , m_pHeader()
    , m_pDriveInfo() 
	, m_alreadySet(false)          
	, m_showStatusBits(showStatus)
    , m_fromScsiLogPages(fromLogPage)
    , m_farmSubPage(subPage)
    , m_showStatic(showStatic)
{
    m_status = eReturnValues::IN_PROGRESS;
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("SCSI FARM Log \n");
    }

	pBuf = new uint8_t[bufferSize];								// new a buffer to the point	
    m_pHeader = new sScsiFarmHeader();                          // new the pHeader pointer
#ifndef __STDC_SECURE_LIB__
    memcpy(pBuf, bufferData, bufferSize);
#else
    memcpy_s(pBuf, bufferSize, bufferData, bufferSize);// copy the buffer data to the class member pBuf
#endif
    if (pBuf != M_NULLPTR)
    {
        if (init_Header_Data(bufferSize) == eReturnValues::SUCCESS)							// init the data for getting the log
        {
            m_status = parse_Farm_Log();
        }
        else
        {
            m_status = eReturnValues::FAILURE;
        }
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
    delete[] pBuf;
    
}
//-----------------------------------------------------------------------------
//
//! \fn CSCSI_Farm_Log::~CSCSI_Farm_Log()
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
CSCSI_Farm_Log::~CSCSI_Farm_Log()
{
    if (!vFarmFrame.empty())
    {
        vFarmFrame.clear();                                    // clear the vector
    }
    if (m_pHeader != M_NULLPTR)
    {
        delete m_pHeader;
    }

}
//-----------------------------------------------------------------------------
//
//! \fn init_Header_Data()
//
//! \brief
//!   Description:  initialize teh header data and the objects member data.
//
//  Entry:
//! None
//
//  Exit:
//!   \return eReturnValues - eReturnValues::MEMORY_FAILURE - if the buffer is NULL
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::init_Header_Data(size_t bufferSize)
{
    if (pBuf == M_NULLPTR)
    {
        return eReturnValues::MEMORY_FAILURE;
    }
    else
    {

        if (m_fromScsiLogPages == true)
        {
            m_pHeader->pPageHeader = *reinterpret_cast<opensea_parser::sLogParams*>(&pBuf[0]);
            get_sFarmHeader(reinterpret_cast<opensea_parser::sScsiFarmHeader*>(m_pHeader), &pBuf[4],4);											// swap all the bytes for the header
            m_logSize = static_cast<uint16_t>(bufferSize);                          // force the size to the buffersize
        }
        else
        {
            m_logParam = reinterpret_cast<sLogPageStruct*>(&pBuf[0]);
            m_logSize = m_logParam->pageLength;									    // set the class log size 
            byte_Swap_16(&m_logSize);

            m_pHeader->pPageHeader = *reinterpret_cast<sLogParams*>(&pBuf[4]);
            get_sFarmHeader(m_pHeader, &pBuf[8], 8);											// swap all the bytes for the header
        }

        m_totalPages = M_DoubleWord0(m_pHeader->farmHeader.pagesSupported);			// get the total pages
        m_pageSize = M_DoubleWord0(m_pHeader->farmHeader.pageSize);					// get the page size
        uint64_t heads = m_pHeader->farmHeader.headsSupported;                      // get the number of heads, but don't se m_heads yet
        if (check_For_Active_Status(&heads))			                            // the the number of heads if supported
        {
            if ((m_pHeader->farmHeader.headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF)) > 0)
            {
                m_heads = M_DoubleWord0(m_pHeader->farmHeader.headsSupported);
                m_MaxHeads = M_DoubleWord0(m_pHeader->farmHeader.headsSupported);
            }
        }
        m_copies = M_DoubleWord0(m_pHeader->farmHeader.reserved);						// finish up with the number of copies (not supported "YET" in SAS)
    }
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn strip_Active_Status()
//
//! \brief
//!   Description:  field supported and Field valid bits stripped off
//
//  Entry:
//! \param value  =  64 bit value have the field supported and Field valid bits stripped off
//
//  Exit:
//!   \return bool - false or true
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::strip_Active_Status(uint64_t *value)
{
    if( check_For_Active_Status(value))
    {
       *value = *value & UINT64_C(0x00FFFFFFFFFFFFFF);
       return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
//
//! \fn set_Head_Header()
//
//! \brief
//!   Description:  set the header name in a string for printing
//
//  Entry:
//! \param headerName  =  pointer the string variable
//! \param index  =  index of the log page types
//
//  Exit:
//!  Void 
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::set_Head_Header(std::string &headerName, eSASLogPageTypes index)
{
    bool ret = true;
    switch (index)
    {
    case eSASLogPageTypes::FARM_HEADER_PARAMETER:
    case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_PARAMETER:
    case eSASLogPageTypes::WORKLOAD_STATISTICS_PARAMETER:
    case eSASLogPageTypes::ERROR_STATISTICS_PARAMETER:
    case eSASLogPageTypes::ENVIRONMENTAL_STATISTICS_PARAMETER:
    case eSASLogPageTypes::RELIABILITY_STATISTICS_PARAMETER:
    case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_06:
    case eSASLogPageTypes::ENVIRONMENT_STATISTICS_PAMATER_07:
    case eSASLogPageTypes::WORKLOAD_STATISTICS_PAMATER_08:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_4:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_5:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_6:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_7:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_8:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_9:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_10:
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_1:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_2:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_3:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_4:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_5:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_6:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_7:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_8:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_9:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_10:
        break;
    case eSASLogPageTypes::MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "MR Head Resistance from most recent SMART Summary Frame";
        break;
    case eSASLogPageTypes::NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Number of TMD over last 3 SMART Summary Frame";
        break;
    case eSASLogPageTypes::VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Velocity Observer over last 3 SMART Summary Frame";
        break;
    case eSASLogPageTypes::NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
        headerName = "Number of Velocity Observer over last 3 SMART SummaryFrame";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_14:
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT amplitude, averaged across Test Zone";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT asymmetry, averaged across Test Zone";
        break;
    case eSASLogPageTypes::NUMBER_OF_RESIDENT_GLIST_ENTRIES:
        headerName = "Resident Glist Entries";
        break;
    case eSASLogPageTypes::NUMBER_OF_PENDING_ENTRIES:
        headerName = "Resident Pending list Entries";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_15:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_16:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_17:
        break;
    case eSASLogPageTypes::WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
        headerName = "Write Power On Hours from most recent SMART";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_18:
		break;
    case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
        headerName = "Cumlative Lifetime Unrecoverable Read Repeat by head";
        break;
    case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
        headerName = "Cumlative Lifetime Unrecoverable Read Unique by head";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_19:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_20:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_21:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_22:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_23:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_24:
        headerName = "Reserved Head information";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 0";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 1";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 2";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT iterations to converge Test Zone 0";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT iterations to converge Test Zone 1";
        break;
    case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT iterations to converge Test Zone 2";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_25:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_26:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_27:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_28:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_29:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_30:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_31:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_32:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_33:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_34:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_35:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_36:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_37:
        headerName = "Reserved Head information";
        break;
    case eSASLogPageTypes::SECOND_MR_HEAD_RESISTANCE:
        headerName = "Second Head MR Head Resistance";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_38:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_39:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_40:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_41:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_42:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_43:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_44:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_45:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_46:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_47:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_48:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_49:
        headerName = "Reserved Head information";
        break;
    case eSASLogPageTypes::LUN_0_ACTUATOR:
    case eSASLogPageTypes::LUN_0_FLASH_LED:
    case eSASLogPageTypes::LUN_REALLOCATION_0:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_42:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_43:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_44:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_45:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_46:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_47:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_48:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_49:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_50:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_51:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_52:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_53:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_54:
    case eSASLogPageTypes::LUN_1_ACTUATOR:
    case eSASLogPageTypes::LUN_1_FLASH_LED:
    case eSASLogPageTypes::LUN_REALLOCATION_1:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_61:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_62:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_63:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_64:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_65:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_66:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_67:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_68:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_69:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_70:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_71:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_72:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_73:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_74:
    case eSASLogPageTypes::LUN_2_ACTUATOR:
    case eSASLogPageTypes::LUN_2_FLASH_LED:
    case eSASLogPageTypes::LUN_REALLOCATION_2:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_81:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_82:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_83:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_84:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_85:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_86:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_87:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_88:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_89:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_90:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_91:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_92:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_93:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_94:
    case eSASLogPageTypes::LUN_3_ACTUATOR:
    case eSASLogPageTypes::LUN_3_FLASH_LED:
    case eSASLogPageTypes::LUN_REALLOCATION_3:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_101:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_102:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_103:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_104:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_105:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_106:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_107:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_108:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_109:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_110:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_111:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_112:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_113:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_114:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_115:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_116:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_117:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_118:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_119:
    case eSASLogPageTypes::LIFETIME_TERABYTES_WRITTEN:
        headerName = "Lifetime Terabytes Written";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_120:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_121:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_122:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_123:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_124:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_125:
        headerName = "Future Expansion";
        break;
    case eSASLogPageTypes::UNIQUE_UNRECOVERABLES_SINCE:
        headerName = "Unique unrecoverable sectors since the last FARM Frame";
        break;
    case eSASLogPageTypes::UNIQUE_UNRECOVERALBES_BETWEEN:
        headerName = "Unique unrecoverable sectors between FARM Frame";
        break;
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_128:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_129:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_130:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_131:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_132:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_133:
    case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_134:
    case eSASLogPageTypes::MAX_RESERVED_FOR_FUTURE_EXPANSION:
        headerName = "Future Expansion";
        break;
    default:
        headerName = "Something is wrong. Please report error SAS FARM 615";
        ret = false;
        break;

    }
    return ret;
}
//-----------------------------------------------------------------------------
//
//! \fn Get_sDriveInfo()
//
//! \brief
//!   Description:  takes the uint64 bit drive information values byte swaps the data   
//
//  Entry:
//! \param di  =  pointer to the drive info structure that holds the information needed, we will need to swap the data
//! \param offset - pointer to the the offset of the buffer
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sDriveInfo(sScsiDriveInfo *di, uint64_t offset)
{
    di->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    di->pPageHeader.paramControlByte =pBuf[offset];
    offset++;
    di->pPageHeader.paramLength = pBuf[offset];
    offset++;
    di->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->serialNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->serialNumber2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->worldWideName = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->worldWideName2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->deviceInterface = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->deviceCapacity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->psecSize = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->lsecSize = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->deviceBufferSize = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->heads = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->factor = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->rotationRate = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->firmware = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->firmwareRev = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->reserved = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved
    di->reserved1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved1
    di->reserved2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved2
    di->poh = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->reserved3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved3
    di->reserved4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved4
    di->headLoadEvents = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->powerCycleCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->resetCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->reserved6 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;  //reserved6
    di->NVC_StatusATPowerOn = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;   
    di->timeAvailable = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->firstTimeStamp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->lastTimeStamp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    if (di->pPageHeader.paramLength > 0xf0)
    {
        di->dateOfAssembly = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn Get_sDrive_Info_Page_06()
//
//! \brief
//!   Description:  takes the uint64 bit drive information values byte swaps the data  
//
//  Entry:
//! \param gd  =  pointer to the drive info structure that holds the information needed, we will need to swap the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sDrive_Info_Page_06(sGeneralDriveInfoPage06* gd, uint64_t offset)
{
    gd->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    gd->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    gd->pPageHeader.paramLength = pBuf[offset];
    offset++;
    gd->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->Depop = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->productID[0] = M_BytesTo8ByteValue(pBuf[offset + 7], pBuf[offset + 6], pBuf[offset + 5], pBuf[offset + 4], pBuf[offset + 3], pBuf[offset + 2], pBuf[offset + 1], pBuf[offset]);
    offset += SIZEPARAM;
    gd->productID[1] = M_BytesTo8ByteValue(pBuf[offset + 7], pBuf[offset + 6], pBuf[offset + 5], pBuf[offset + 4], pBuf[offset + 3], pBuf[offset + 2], pBuf[offset + 1], pBuf[offset]);
    offset += SIZEPARAM;
    gd->productID[2] = M_BytesTo8ByteValue(pBuf[offset + 7], pBuf[offset + 6], pBuf[offset + 5], pBuf[offset + 4], pBuf[offset + 3], pBuf[offset + 2], pBuf[offset + 1], pBuf[offset]);
    offset += SIZEPARAM;
    gd->productID[3] = M_BytesTo8ByteValue(pBuf[offset + 7], pBuf[offset + 6], pBuf[offset + 5], pBuf[offset + 4], pBuf[offset + 3], pBuf[offset + 2], pBuf[offset + 1], pBuf[offset]);
    offset += SIZEPARAM;
    gd->driveType = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->isDepopped = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->maxNumAvaliableSectors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->timeToReady = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->holdTime = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->servoSpinUpTime = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    if (offset <= gd->pPageHeader.paramLength)
    {
        gd->writeProtect = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= gd->pPageHeader.paramLength)
    {
        gd->regenHeadMask = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= gd->pPageHeader.paramLength)
    {
        gd->pohFirst = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= gd->pPageHeader.paramLength)
    {
        gd->pohSecond = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn Get_sWorkLoadStat()
//
//! \brief
//!   Description:  takes the two uint64 bit work load stat and swaps all the bytes 
//
//  Entry:
//! \param sWorkLoadStat  =  pointer to the work load stats and swaps all the bytes
//
//  Exit:
//!   \return true when done
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sWorkLoadStat(sScsiWorkLoadStat *wl, uint64_t offset)  
{
    wl->PageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    wl->PageHeader.paramControlByte = pBuf[offset];
    offset++;
    wl->PageHeader.paramLength = pBuf[offset];
    offset++;
    wl->workLoad.pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.workloadPercentage = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.totalReadCommands = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.totalWriteCommands = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.totalRandomReads = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.totalRandomWrites = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.totalNumberofOtherCMDS = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.logicalSecWritten = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    wl->workLoad.logicalSecRead = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;



    // found a log where the length of the workload log does not match the spec. Need to check for the 0x50 length
    if (wl->PageHeader.paramLength > 0x50)
    {
        wl->workLoad.totalReadCmdsFromFrames1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalReadCmdsFromFrames2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalReadCmdsFromFrames3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalReadCmdsFromFrames4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalWriteCmdsFromFrames1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalWriteCmdsFromFrames2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalWriteCmdsFromFrames3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.totalWriteCmdsFromFrames4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;

        wl->workLoad.numReadTransferSmall = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numReadTransferMid1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numReadTransferMid2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numReadTransferLarge = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;

        wl->workLoad.numWriteTransferSmall = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numWriteTransferMid1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numWriteTransferMid2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        wl->workLoad.numWriteTransferLarge = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;

    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sErrorStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the error stat
//
//  Entry:
//! \param sErrorStat  =  pointer to the error stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sErrorStat(sScsiErrorFrame * es, uint64_t offset)
{

    es->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    es->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    es->pPageHeader.paramLength = pBuf[offset];
    offset++;
    es->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->totalReadECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->totalWriteECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->readRecoveryAtt = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved1
    es->totalMechanicalFails = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; 
    es->reserved2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved2
    es->reserved3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved3
    es->reserved4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved4
    es->reserved5 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved5
    es->reserved6 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved6
    es->reserved7 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved7
    es->reserved8 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved8
    es->reserved9 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved9
    es->reserved10 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved10
    es->smartTripParam1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // smartTripParam1
    es->smartTripParam2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //smartTripParam2
    es->reserved13 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; // reserved13
    es->reserved14 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved14
    es->FRUCode = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portAInvalidDwordCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portBInvalidDwordCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portADisparityErrorCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portBDisparityErrorCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portALossDwordSync = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portBLossDwordSync = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portAPhyResetProblem = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->portBPhyResetProblem = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);


    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sEnvironementStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the environment stat
//
//  Entry:
//! \param sEnvironementStat  =  pointer to the environment stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sEnvironmentStat(sScsiEnvironmentStat *es, uint64_t offset)
{
#define SIZEPARAM   8
    es->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    es->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    es->pPageHeader.paramLength = pBuf[offset];
    offset++;
    es->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->curentTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->highestTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->lowestTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->reserved = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved5 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved6 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved7 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->maxTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->minTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->reserved8 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->reserved9 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->humidity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->reserved10 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    es->currentMotorPower = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;

    if (es->pPageHeader.paramLength > 0xa0)
    {
        es->average12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->min12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->max12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->average5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->min5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->max5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_EnvironmentPage07()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the environment stat
//
//  Entry:
//! \param ep  =  pointer to the environment stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_EnvironmentPage07(sScsiEnvStatPage07 *ep, uint64_t offset)
{
    ep->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    ep->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    ep->pPageHeader.paramLength = pBuf[offset];
    offset++;
    ep->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->average12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->min12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->max12v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->average5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->min5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->max5v = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);

    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_WorkloadPage08()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the environment stat
//
//  Entry:
//! \param ep  =  pointer to the environment stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_WorkloadPage08(sScsiWorkloadStatPage08 *ep, uint64_t offset)
{
    ep->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    ep->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    ep->pPageHeader.paramLength = pBuf[offset];
    offset++;
    ep->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth3_4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth5_8 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth9_16 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth17_32 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth33_64 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ep->countQueDepth_gt_64 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);

    return true;
}



//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sScsiReliabilityStat()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the reliability stat
//
//  Entry:
//! \param sScsiReliabilityStat  =  pointer to the reliability stat
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sScsiReliabilityStat(sScsiReliablility *ss, uint64_t offset)
{
    ss->pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    ss->pPageHeader.paramControlByte = pBuf[offset];
    offset++;
    ss->pPageHeader.paramLength = pBuf[offset];
    offset++;
    ss->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ss->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ss->reserved = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved5 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved6 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved7 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved8 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved9 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved10 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved11 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved12 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->numberRAWops = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ss->cumECCDueToERC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ss->reserved13 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved14 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved15 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved16 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved17 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->unloadEvents = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved19 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->diskSlipRecalPerformed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->heliumPressuretThreshold = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    ss->reserved21 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved22 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    ss->reserved23 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sFarmHeader()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the farm header
//
//  Entry:
//! \param sScsiFarmHeader  =  pointer to the Farm header information
//! \param pData = pointer to the buffer data
//! \param offset = how many byte we have removed from the size ( 4 or is it 8)
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::get_sFarmHeader(sScsiFarmHeader* fh, uint8_t* pData, uint64_t position)
{
    uint64_t offset = 0;
    fh->farmHeader.signature = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
    fh->farmHeader.majorRev = M_BytesTo8ByteValue(pData[offset + 8], pData[offset + 9], pData[offset + 10], pData[offset + 11], pData[offset + 12], pData[offset + 13], pData[offset + 14], pData[offset + 15]);
    fh->farmHeader.minorRev = M_BytesTo8ByteValue(pData[offset + 16], pData[offset + 17], pData[offset + 18], pData[offset + 19], pData[offset + 20], pData[offset + 21], pData[offset + 22], pData[offset + 23]);
    fh->farmHeader.pagesSupported = M_BytesTo8ByteValue(pData[offset + 24], pData[offset + 25], pData[26], pData[offset + 27], pData[offset + 28], pData[offset + 29], pData[offset + 30], pData[offset + 31]);
    fh->farmHeader.logSize = M_BytesTo8ByteValue(pData[offset + 32], pData[offset + 33], pData[offset + 34], pData[offset + 35], pData[offset + 36], pData[offset + 37], pData[offset + 38], pData[offset + 39]);
    fh->farmHeader.pageSize = M_BytesTo8ByteValue(pData[offset + 40], pData[offset + 41], pData[offset + 42], pData[offset + 43], pData[offset + 44], pData[offset + 45], pData[offset + 46], pData[offset + 47]);
    fh->farmHeader.headsSupported = M_BytesTo8ByteValue(pData[offset + 48], pData[offset + 49], pData[offset + 50], pData[offset + 51], pData[offset + 52], pData[offset + 53], pData[offset + 54], pData[offset + 55]);
    fh->farmHeader.reserved = M_BytesTo8ByteValue(pData[offset + 56], pData[offset + 57], pData[offset + 58], pData[offset + 59], pData[offset + 60], pData[offset + 61], pData[offset + 62], pData[offset + 63]);
    if (position + 64 <= m_logSize && m_logSize != 0 && m_logSize != UINT16_C(0x00FF))
    {
        fh->farmHeader.reasonForFrameCapture = M_BytesTo8ByteValue(pData[offset + 64], pData[offset + 65], pData[offset + 66], pData[offset + 67], pData[offset + 68], pData[offset + 69], pData[offset + 70], pData[offset + 71]);
    }

    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn swap_Bytes_sLUNStruct()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the LUN Structure
//
//  Entry:
//! \param LUN  =  pointer to the LUN Structure
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_sLUNStruct(sLUNStruct *LUN,uint64_t offset)
{
    LUN->pageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    LUN->pageHeader.paramControlByte = pBuf[offset];
    offset++;
    LUN->pageHeader.paramLength = pBuf[offset];
    offset++;
    LUN->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->LUNID = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->headLoadEvents = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->reserved = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved1 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved2 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved3 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved4 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved5 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved6 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved7 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved8 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved9 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved10 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->reserved11 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM; //reserved
    LUN->dosScansPerformed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->correctedLBAbyISP = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved12 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved13 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved14 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved15 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->lbasCorrectedByParity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM * 7;
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved16 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved17 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved18 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved19 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved20 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->reserved21 = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM; //reserved
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->primarySPCovPercentageCMR = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= LUN->pageHeader.paramLength)
    {
        LUN->primarySPCovPercentageSMR = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn Get_Flash_LED()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the Flash LED Structure
//
//  Entry:
//! \param fled  =  pointer to the LUN Structure
//! \param offset = the number of address locations to the buffer
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_Flash_LED(sActuatorFLEDInfo *fled, uint64_t offset)
{
    fled->pageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    fled->pageHeader.paramControlByte = pBuf[offset];
    offset++;
    fled->pageHeader.paramLength = pBuf[offset];
    offset++;
    fled->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    fled->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    fled->actID = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    fled->totalFLEDEvents = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    fled->index = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    
    for (uint16_t k = 0; k < FLASH_EVENTS; k++)
    {
        fled->flashLEDArray[k] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    
    for (uint16_t j = 0; j < FLASH_EVENTS; j++)
    {
        fled->timestampForLED[j] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    for (uint16_t i = 0; i < FLASH_EVENTS; i++)
    {
        fled->powerCycleOfLED[i] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
   
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn Get_Reallocation_Data()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for the reallocation data
//
//  Entry:
//! \param fled  =  pointer to the reallocation Data
//! \param offset = the number of address locations to the buffer
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::Get_Reallocation_Data(sActReallocationData *real, uint64_t offset)
{
    real->pageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
    offset += 2;
    real->pageHeader.paramControlByte = pBuf[offset];
    offset++;
    real->pageHeader.paramLength = pBuf[offset];
    offset++;
    real->pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    real->copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    real->actID = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    real->numberReallocatedSectors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    real->numberReallocatedCandidates = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    
    for (uint16_t i = 0; i < REALLOCATIONEVENTS; i++)
    {
        real->reallocatedCauses[i] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= real->pageHeader.paramLength)
    {
        real->numReallocatedSince = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= real->pageHeader.paramLength)
    {
        real->numReallocatedBetween = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= real->pageHeader.paramLength)
    {
        real->numCandidateSince = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }
    if (offset <= real->pageHeader.paramLength)
    {
        real->numCandidateBetween = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
    }

    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Head_Info()
//
//! \brief
//!   Description:  takes the pointer to the structure an does a byte swap on all the data for heads
//
//  Entry:
//! \param phead  =  pointer to the head information
//! \param buffer = the pointer to the buffer data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::get_Head_Info(sHeadInformation *phead, uint8_t *buffer)
{
    memcpy(phead->headValue, reinterpret_cast<sHeadInformation *>(&buffer[4]), (sizeof(uint64_t) * static_cast<size_t>(m_heads)));
    memcpy(&phead->pageHeader, reinterpret_cast<sLogParams *>(&buffer[0]), sizeof(sLogParams));
    for (uint64_t index = 0; index < m_heads; index++)
    {
        byte_Swap_64(&phead->headValue[index] );
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn parse_Farm_Log()
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
eReturnValues CSCSI_Farm_Log::parse_Farm_Log()
{
    uint64_t offset = 0;                                                                    // the first page starts at offset 4                                   
    bool headerAlreadyFound = false;                                                        // set to false, for files that are missing data
    sScsiFarmFrame *pFarmFrame = new sScsiFarmFrame();								       	// create the pointer to the union

    if (m_fromScsiLogPages == false)
    {
        offset = 4;
    }

    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("SCSI parse FARM Log\n");
    }

    if (pBuf == M_NULLPTR)
    {
        return eReturnValues::FAILURE;
    }
    uint64_t signature = m_pHeader->farmHeader.signature & UINT64_C(0x00FFFFFFFFFFFFFF);
    m_MajorRev = M_DoubleWord0(m_pHeader->farmHeader.majorRev);
    m_MinorRev = M_DoubleWord0(m_pHeader->farmHeader.minorRev);
    if ((signature != FARMSIGNATURE && signature != FACTORYCOPY) || signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
    {
        if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
        {
            delete pFarmFrame;
            return eReturnValues::SUCCESS;
        }
        else
        {
            delete pFarmFrame;
            return eReturnValues::VALIDATION_FAILURE;
        }
    }
    if (signature == FARMSIGNATURE || signature == FACTORYCOPY)				// check the head to see if it has the farm signature else fail
    {
        
        for (uint32_t index = 0; index <= m_copies; ++index)						// loop for the number of copies. I don't think it's zero base as of now
        {
            if (pFarmFrame->vFramesFound.size() > 1)
            {
                pFarmFrame->vFramesFound.clear();                                                 // clear the vector for the next copy
            }

            while (offset < m_logSize)
            {
                m_pageParam = reinterpret_cast<sLogParams*>(&pBuf[offset]);										 // get the page params, so we know what the param code is. 
                byte_Swap_16(&m_pageParam->paramCode);
                if (!headerAlreadyFound || (m_pageParam->paramCode != 0x0000 && m_pageParam->paramCode < 0x008F))
                {
                    pFarmFrame->vFramesFound.push_back(static_cast<eSASLogPageTypes>(m_pageParam->paramCode));                // collect all the log page types in a vector to pump them out at the end
                }
                switch (static_cast<eSASLogPageTypes>(m_pageParam->paramCode))
                {
                    
                case eSASLogPageTypes::FARM_HEADER_PARAMETER:
                    {
                        if (headerAlreadyFound == false)                                    // check to see if we have already found the header
                        {
                            memcpy(reinterpret_cast<sScsiFarmHeader *>(&pFarmFrame->farmHeader), m_pHeader, static_cast<size_t>(m_pageParam->paramLength) + PARAMSIZE);
                            offset += (m_pageParam->paramLength + sizeof(sLogParams));
                            headerAlreadyFound = true;                                      // set the header to true so we will not look at the data a second time
                        }
                        else
                        {
                            offset += (sizeof(sLogParams));
                        }
                    }
                    break; 
 
                case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_PARAMETER:
                    {
                        Get_sDriveInfo(&pFarmFrame->driveInfo, offset);       // get the id drive information at the time.
                        create_Serial_Number(pFarmFrame->identStringInfo.serialNumber, M_DoubleWord0(pFarmFrame->driveInfo.serialNumber), M_DoubleWord0(pFarmFrame->driveInfo.serialNumber2), m_MajorRev, true);
                        create_World_Wide_Name(pFarmFrame->identStringInfo.worldWideName, pFarmFrame->driveInfo.worldWideName, pFarmFrame->driveInfo.worldWideName2, true);
                        create_Firmware_String(pFarmFrame->identStringInfo.firmwareRev, M_DoubleWord0(pFarmFrame->driveInfo.firmware), M_DoubleWord0(pFarmFrame->driveInfo.firmwareRev), true);
                        create_Device_Interface_String(pFarmFrame->identStringInfo.deviceInterface, M_DoubleWord0(pFarmFrame->driveInfo.deviceInterface), true);

                        offset += (m_pageParam->paramLength + sizeof(sLogParams));
                    }
                    break;
                   
                case eSASLogPageTypes::WORKLOAD_STATISTICS_PARAMETER:
                    {                    
                        Get_sWorkLoadStat(&pFarmFrame->workLoadPage,offset);
                        offset += (pFarmFrame->workLoadPage.PageHeader.paramLength + sizeof(sLogParams));
                    }
                    break;
                   
                case eSASLogPageTypes::ERROR_STATISTICS_PARAMETER:
                {
                    Get_sErrorStat(&pFarmFrame->errorPage,offset);
                    offset += (pFarmFrame->errorPage.pPageHeader.paramLength + sizeof(sLogParams));

                }
                break;
                    
                case eSASLogPageTypes::ENVIRONMENTAL_STATISTICS_PARAMETER:
                {
                    Get_sEnvironmentStat(&pFarmFrame->environmentPage,offset);
                    offset += (pFarmFrame->environmentPage.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                   
                case eSASLogPageTypes::RELIABILITY_STATISTICS_PARAMETER:
                {
                    Get_sScsiReliabilityStat(&pFarmFrame->reliPage, offset);
                    offset += (pFarmFrame->reliPage.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break; 
                case  eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_06:
                {
                    Get_sDrive_Info_Page_06(&pFarmFrame->gDPage06,offset);
                    create_Model_Number_String(pFarmFrame->identStringInfo.modelNumber, pFarmFrame->gDPage06.productID,true);
                    offset += (pFarmFrame->gDPage06.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  eSASLogPageTypes::ENVIRONMENT_STATISTICS_PAMATER_07:
                {
                    Get_EnvironmentPage07(&pFarmFrame->envStatPage07,offset);
                    offset += (pFarmFrame->envStatPage07.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  eSASLogPageTypes::WORKLOAD_STATISTICS_PAMATER_08:
                {
                    Get_WorkloadPage08(&pFarmFrame->workloadStatPage08,offset);
                    offset += (pFarmFrame->workloadStatPage08.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_4:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_5:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_6:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_7:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_8:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_9:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_10:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_1:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_2:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_3:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_4:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_5:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_6:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_7:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_8:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_9:
                case  eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_10:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->discSlipPerHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->mrHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->numberOfTMDByHead, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->velocityObserverByHead, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->numberOfVelocityObservedByHead, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                }
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_14:
                    break;
                case eSASLogPageTypes::CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAmplituedByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAsymmetryByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::NUMBER_OF_RESIDENT_GLIST_ENTRIES:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentGlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case  eSASLogPageTypes::NUMBER_OF_PENDING_ENTRIES:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentPlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_15:
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_16:
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_17:
                    break;
                case eSASLogPageTypes::WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->writePowerOnHours, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_18:
                case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->cumECCReadRepeat, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->cumECCReadUnique, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_19:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_20:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_21:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_22:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_23:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_24:
                    break;
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_25:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_26:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_27:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_28:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_29:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_30:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_31:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_32:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_33:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_34:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_35:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_36:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_37:
                    break;
                case eSASLogPageTypes::SECOND_MR_HEAD_RESISTANCE:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->secondMRHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_38:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_39:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_40:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_41:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_42:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_43:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_44:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_45:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_46:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_47:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_48:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_49:
                    break;
                case eSASLogPageTypes::LUN_0_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN50,offset);
                    offset += (pFarmFrame->vLUN50.pageHeader.paramLength + sizeof(sLogParams));

                }
                break;
                case eSASLogPageTypes::LUN_0_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled51, offset);
                    offset += (pFarmFrame->fled51.pageHeader.paramLength + sizeof(sLogParams));

                }
                break;
                case eSASLogPageTypes::LUN_REALLOCATION_0:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall52, offset);
                    offset += (pFarmFrame->reall52.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_42:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_43:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_44:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_45:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_46:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_47:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_48:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_49:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_50:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_51:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_52:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_53:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_54:
                    break;
                case eSASLogPageTypes::LUN_1_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN60,offset);
                    offset += (pFarmFrame->vLUN60.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_1_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled61,offset);
                    offset += (pFarmFrame->fled61.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_REALLOCATION_1:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall62, offset);
                    offset += (pFarmFrame->reall62.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_61:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_62:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_63:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_64:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_65:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_66:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_67:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_68:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_69:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_70:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_71:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_72:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_73:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_74:
                    break;
                case eSASLogPageTypes::LUN_2_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN70,offset);
                    offset += (pFarmFrame->vLUN70.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_2_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled71,offset);
                    offset += (pFarmFrame->fled71.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_REALLOCATION_2:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall72, offset);
                    offset += (pFarmFrame->reall72.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_81:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_82:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_83:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_84:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_85:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_86:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_87:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_88:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_89:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_90:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_91:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_92:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_93:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_94:
                    break;
                case eSASLogPageTypes::LUN_3_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN80,offset);
                    offset += (pFarmFrame->vLUN80.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_3_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled81,offset);
                    offset += (pFarmFrame->fled81.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LUN_REALLOCATION_3:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall82, offset);
                    offset += (pFarmFrame->reall82.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case eSASLogPageTypes::LIFETIME_TERABYTES_WRITTEN:           // LIFETIME_TERABYTES_WRITTEN
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->lifeTimeWritten, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                    //new
                    offset += m_logSize;
                    break;
                }
                case eSASLogPageTypes::UNIQUE_UNRECOVERABLES_SINCE:
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->uniqueURESince, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                    offset += m_logSize;
                }
                break;
                case eSASLogPageTypes::UNIQUE_UNRECOVERALBES_BETWEEN:
                {
                    opensea_parser::sHeadInformation* pHeadInfo = new opensea_parser::sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->uniqueUREBetween, pHeadInfo, sizeof(opensea_parser::sHeadInformation));
                    offset += static_cast<size_t>((pHeadInfo->pageHeader.paramLength) + sizeof(opensea_parser::sLogParams));
                    delete pHeadInfo;
                    offset += m_logSize;
                }
                break;
                default:
                    offset += m_logSize;
                    break;
               }  
            }
            vFarmFrame.push_back(*pFarmFrame);                                   // push the data to the vector
        }
	    delete pFarmFrame;	
        return eReturnValues::SUCCESS;
    }
    delete pFarmFrame;
    return eReturnValues::FAILURE;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Header(JSONNODE *masterData)
{
	uint32_t page = 0;
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        printf("\tLog Signature:                                                0x%" PRIX64" \n", vFarmFrame.at(page).farmHeader.farmHeader.signature);                                  //!< Log Signature = 0x00004641524D4552
        printf("\tMajor Revision:                                               %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.majorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< Log Major rev
        printf("\tMinor Revision:                                               %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.minorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< minor rev 
        printf("\tPages Supported:                                              %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                             //!< number of pages supported
        printf("\tLog Size:                                                     %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.logSize & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< log size in bytes
        if (m_MajorRev < MAJORVERSION4)
        {
            printf("\tPage Size:                                                    %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.pageSize & UINT64_C(0x00FFFFFFFFFFFFFF));                                   //!< page size in bytes
        }
        printf("\tHeads Supported:                                              %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                             //!< Maximum Drive Heads Supported
        printf("\tReason for Frame Capture:                                     %" PRIu64"  \n", vFarmFrame.at(page).farmHeader.farmHeader.reasonForFrameCapture & 0x00FFFFFFFFFFFFF);	      //!< Reason for Frame Capture
    }

    JSONNODE* pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "FARM Log Header");
    std::ostringstream temp;
    temp << "0x" <<std::hex << std::uppercase << check_Status_Strip_Status(vFarmFrame.at(page).farmHeader.farmHeader.signature);
    json_push_back(pageInfo, json_new_a("Log Signature", temp.str().c_str()));
    set_json_64_bit_With_Status(pageInfo, "Major Revision", vFarmFrame.at(page).farmHeader.farmHeader.majorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Minor Revision", vFarmFrame.at(page).farmHeader.farmHeader.minorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Pages Supported", vFarmFrame.at(page).farmHeader.farmHeader.pagesSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Log Size", vFarmFrame.at(page).farmHeader.farmHeader.logSize, false, m_showStatusBits);
        if (m_MajorRev < MAJORVERSION4 && m_showStatic == false)
        {
        set_json_64_bit_With_Status(pageInfo, "Page Size", vFarmFrame.at(page).farmHeader.farmHeader.pageSize, false, m_showStatusBits);
    }
    set_json_64_bit_With_Status(pageInfo, "Heads Supported", vFarmFrame.at(page).farmHeader.farmHeader.headsSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Reason for Frame Capture", vFarmFrame.at(page).farmHeader.farmHeader.reasonForFrameCapture, false, m_showStatusBits);
    std::string meaning;
    Get_FARM_Reason_For_Capture(&meaning, M_Byte0(vFarmFrame.at(page).farmHeader.farmHeader.reasonForFrameCapture));

    json_push_back(pageInfo, json_new_a("Reason meaning", meaning.c_str()));
    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------

eReturnValues CSCSI_Farm_Log::print_Drive_Information(JSONNODE *masterData, uint32_t page)
{
#define  SHORTHEADS 20
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
        {
            printf("\nDrive Information From Farm Log copy FACTORY\n");
        }
        else
        {
            printf("\nDrive Information From Farm Log\n");
        }
        printf("\tDevice Interface:                                             %s         \n", vFarmFrame.at(page).identStringInfo.deviceInterface.c_str());
        printf("\tDevice Capcaity in sectors:                                   %" PRIu64" \n", vFarmFrame.at(page).driveInfo.deviceCapacity & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPhysical Sector size:                                         %" PRIu64" \n", vFarmFrame.at(page).driveInfo.psecSize & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Physical Sector Size in Bytes
        printf("\tLogical Sector Size:                                          %" PRIu64" \n", vFarmFrame.at(page).driveInfo.lsecSize & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Logical Sector Size in Bytes
        printf("\tDevice Buffer Size:                                           %" PRIu64" \n", vFarmFrame.at(page).driveInfo.deviceBufferSize & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< Device Buffer Size in Bytes
        printf("\tNumber of heads:                                              %" PRIu64" \n", vFarmFrame.at(page).driveInfo.heads & UINT64_C(0x00FFFFFFFFFFFFFF));										//!< Number of Heads
        printf("\tDevice form factor:                                           %" PRIu64" \n", vFarmFrame.at(page).driveInfo.factor & UINT64_C(0x00FFFFFFFFFFFFFF));										//!< Device Form Factor (ID Word 168)
        printf("\tSerial Number:                                                %s         \n", vFarmFrame.at(page).identStringInfo.serialNumber.c_str());
        printf("\tModel Number:                                                 %s         \n", vFarmFrame.at(page).identStringInfo.modelNumber.c_str());
        printf("\tWorld Wide Name:                                              %s         \n", vFarmFrame.at(page).identStringInfo.worldWideName.c_str());
        printf("\tFirmware Rev:                                                 %s         \n", vFarmFrame.at(page).identStringInfo.firmwareRev.c_str());											//!< Firmware Revision [0:3]
        printf("\tRotation Rate:                                                %" PRIu64" \n", vFarmFrame.at(page).driveInfo.rotationRate & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Rotational Rate of Device 
        printf("\tPower on Hours:                                               %" PRIu64" \n", vFarmFrame.at(page).driveInfo.poh & UINT64_C(0x00FFFFFFFFFFFFFF));											//!< Power-on Hour
        if (m_MajorRev < MAJORVERSION4)
        {
            printf("\tHead Load Events:                                             %" PRIu64" \n", vFarmFrame.at(page).driveInfo.headLoadEvents & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Head Load Events
        }
        printf("\tPower Cycle count:                                            %" PRIu64" \n", vFarmFrame.at(page).driveInfo.powerCycleCount & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Power Cycle Count
        printf("\tHardware Reset count:                                         %" PRIu64" \n", vFarmFrame.at(page).driveInfo.resetCount & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Hardware Reset Count
        printf("\tNVC Status @ power on:                                        %" PRIu64" \n", vFarmFrame.at(page).driveInfo.NVC_StatusATPowerOn & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< NVC Status on Power-on
        printf("\tTime Available to save:                                       %" PRIu64" \n", vFarmFrame.at(page).driveInfo.timeAvailable & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
        printf("\tTime of latest frame:                                         %" PRIu64" \n", vFarmFrame.at(page).driveInfo.firstTimeStamp & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Timestamp of first SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
        printf("\tTime of latest frame (milliseconds):                          %" PRIu64" \n", vFarmFrame.at(page).driveInfo.lastTimeStamp & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1

    }
    
    std::ostringstream temp;
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::string header;
    if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
    {
        header.assign("Drive Information From Farm Log copy FACTORY");
    }
    else
    {
        temp.str("");temp.clear();
        temp << "Drive Information From Farm Log ";
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());
        
    json_push_back(pageInfo, json_new_a("Serial Number", vFarmFrame.at(page).identStringInfo.serialNumber.c_str()));
    json_push_back(pageInfo, json_new_a("World Wide Name", vFarmFrame.at(page).identStringInfo.worldWideName.c_str()));
    json_push_back(pageInfo, json_new_a("Firmware Rev", vFarmFrame.at(page).identStringInfo.firmwareRev.c_str()));

    if (vFarmFrame.at(page).identStringInfo.modelNumber == "")
    {
        vFarmFrame.at(page).identStringInfo.modelNumber = "ST12345678";
    }
    json_push_back(pageInfo, json_new_a("Model Number", vFarmFrame.at(page).identStringInfo.modelNumber.c_str()));

    json_push_back(pageInfo, json_new_a("Device Interface", vFarmFrame.at(page).identStringInfo.deviceInterface.c_str()));
    set_json_64_bit_With_Status(pageInfo, "Device Capacity in Sectors", vFarmFrame.at(page).driveInfo.deviceCapacity,false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame.at(page).driveInfo.psecSize, false, m_showStatusBits);									//!< Physical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame.at(page).driveInfo.lsecSize, false, m_showStatusBits);										//!< Logical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame.at(page).driveInfo.deviceBufferSize, false, m_showStatusBits);								//!< Device Buffer Size in Bytes
    
    set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame.at(page).driveInfo.heads, false, m_showStatusBits);											//!< Number of Heads
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

    set_json_64_bit_With_Status(pageInfo, "Device form factor", vFarmFrame.at(page).driveInfo.factor, false, m_showStatusBits);										//!< Device Form Factor (ID Word 168)

    set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame.at(page).driveInfo.rotationRate, false, m_showStatusBits);										//!< Rotational Rate of Device (ID Word 217)
    set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame.at(page).driveInfo.poh, false, m_showStatusBits);                                                //!< Power-on Hour
    if (m_MajorRev < MAJORVERSION4)
    {
        set_json_64_bit_With_Status(pageInfo, "Head Load Events", vFarmFrame.at(page).driveInfo.headLoadEvents, false, m_showStatusBits);									//!< Head Load Events
    }
    set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame.at(page).driveInfo.powerCycleCount, false, m_showStatusBits);								//!< Power Cycle Count
    set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame.at(page).driveInfo.resetCount, false, m_showStatusBits);									//!< Hardware Reset Count
    set_json_64_bit_With_Status(pageInfo, "NVC Status @ power on", vFarmFrame.at(page).driveInfo.NVC_StatusATPowerOn, false, m_showStatusBits);						//!< NVC Status on Power-on

    set_json_64_bit_With_Status(pageInfo, "NVC Time Available to save (in 100us)", vFarmFrame.at(page).driveInfo.timeAvailable, false, m_showStatusBits);					//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    set_json_64_bit_With_Status(pageInfo, "Timestamp of First SMART Summary Frame (ms)", vFarmFrame.at(page).driveInfo.firstTimeStamp, false, m_showStatusBits);		//!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
    set_json_64_bit_With_Status(pageInfo, "TimeStamp of Last SMART Summary Frame (ms)", vFarmFrame.at(page).driveInfo.lastTimeStamp, false, m_showStatusBits);			//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds

    if (check_For_Active_Status(&vFarmFrame.at(page).driveInfo.dateOfAssembly) || \
        (vFarmFrame.at(page).driveInfo.dateOfAssembly < 0x40000000 && vFarmFrame.at(page).driveInfo.dateOfAssembly > 0x3030))
    {
            std::string dataAndTime;
            uint16_t year = M_Word1(vFarmFrame.at(page).driveInfo.dateOfAssembly);
            uint16_t week = M_Word0(vFarmFrame.at(page).driveInfo.dateOfAssembly);

        create_Year_Assembled_String(dataAndTime, year, true);
        json_push_back(pageInfo, json_new_a("Year of Assembled", dataAndTime.c_str()));
        dataAndTime.clear();
        create_Year_Assembled_String(dataAndTime, week, true);
        json_push_back(pageInfo, json_new_a("Week of Assembled", dataAndTime.c_str()));
    }
    else
      {
            // if printing static we need to set the data to NULL. Else skip it
            if (g_parseNULL)
            {
                json_push_back(pageInfo, json_new_a("Year of Assembled", "NULL"));
                json_push_back(pageInfo, json_new_a("Week of Assembled", "NULL"));
            }
            else
            {
                json_push_back(pageInfo, json_new_a("Year of Assembled", "00"));
                json_push_back(pageInfo, json_new_a("Week of Assembled", "00"));
            }
        }
        json_push_back(masterData, pageInfo);
        return eReturnValues::SUCCESS;
    }
    

//-----------------------------------------------------------------------------
//
//! \fn get_Regen_Head_Mask()
//
//! \brief
//!   Description:  creates the bitmap data for which head is good or bad.  1 = bad 0 = good
//
//  Entry:
//! \param headMask - pointer to the json data that will be printed or passed on
//! \param mask  = the 64 bit data for creating the bitmap 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::get_Regen_Head_Mask(JSONNODE* headMask, uint64_t mask, bool showStatus)
{
    eReturnValues status = eReturnValues::IN_PROGRESS;
    if (opensea_parser::check_For_Active_Status(&mask))
    {
        uint32_t tempHeadMask = M_DoubleWord0(mask);
        std::ostringstream temp;
        for (uint32_t i = 0; i <= m_MaxHeads; i++)
        {
            temp.str(""); temp.clear();
            temp << "Regen Head Mask " << std::dec << i;
            if (tempHeadMask & BIT0)
            {
                set_json_string_With_Status(headMask, temp.str().c_str(), "bad", mask, showStatus);
            }
            else
            {
                set_json_string_With_Status(headMask, temp.str().c_str(), "good", mask, showStatus);
            }
            tempHeadMask >>= 1;
        }
        status = eReturnValues::SUCCESS;
    }
    else
    {
        set_json_string_With_Status(headMask, "Regen Head Mask", "", mask, showStatus);
        status = eReturnValues::NOT_SUPPORTED;
    }
    return status;
}
//-----------------------------------------------------------------------------
//
//! \fn print_General_Drive_Information_Continued()
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
eReturnValues CSCSI_Farm_Log::print_General_Drive_Information_Continued(JSONNODE* masterData, uint32_t page)
{
    eReturnValues status = eReturnValues::IN_PROGRESS;
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
        {
            printf("\nGeneral Drive Information From Farm Log copy FACTORY\n");
        }
        else
        {
            printf("\nGeneral Drive Information From Farm Log \n");
        }
        printf("\tDepopulation Head Mask:                                       %" PRIu64"  \n", vFarmFrame.at(page).gDPage06.Depop & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDrive Recording Type:                                         %" PRIu64"  \n", vFarmFrame.at(page).gDPage06.driveType & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tHas Drive been Depopped:                                      %" PRIu64"  \n", vFarmFrame.at(page).gDPage06.isDepopped & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tMax Number of Available Sectors for Reassignment:             %" PRIu64"  \n", vFarmFrame.at(page).gDPage06.maxNumAvaliableSectors & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tTime to ready of the last power cycle (sec):                  %0.3f       \n", (vFarmFrame.at(page).gDPage06.timeToReady & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tTime drive is held in staggered spin (sec)                    %0.3f       \n", (vFarmFrame.at(page).gDPage06.holdTime & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tLast Servo Spin up Time (sec):                                %0.3f       \n", (vFarmFrame.at(page).gDPage06.servoSpinUpTime & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tHAMR Write Protect:                                           0x%" PRIx64"  \n", vFarmFrame.at(page).gDPage06.writeProtect & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tRegen Head Mask bitmap:                                       0x%" PRIx64"  \n", vFarmFrame.at(page).gDPage06.regenHeadMask & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPower on Hours most recent FARM Time series frame             0x%" PRIx64"  \n", vFarmFrame.at(page).gDPage06.pohFirst & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPower on Hours of second FARM Time series frame               0x%" PRIx64"  \n", vFarmFrame.at(page).gDPage06.pohSecond & UINT64_C(0x00FFFFFFFFFFFFFF));

    }
    if (vFarmFrame.at(page).driveInfo.copyNumber == FACTORYCOPY)
    {
        header.assign("General Drive Informatio From Farm Log copy FACTORY");
    }
    else
    {
        temp << "General Drive Information From Farm Log ";
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());

    // customer wanted to see data in hex
    temp.str(""); temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(opensea_parser::check_Status_Strip_Status(vFarmFrame.at(page).gDPage06.Depop));
    set_json_string_With_Status(pageInfo, "Depopulation Head Mask", temp.str().c_str(), vFarmFrame.at(page).gDPage06.Depop, m_showStatusBits);

    std::string type = "not supported";
    uint64_t pageDriveType = vFarmFrame.at(page).gDPage06.driveType;
    if (check_For_Active_Status( &pageDriveType))
    {
        type = "CMR";
        if (vFarmFrame.at(page).gDPage06.driveType & BIT0)
        {
            type = "SMR";
        }
    }

    set_json_string_With_Status(pageInfo, "Drive Recording Type", type.c_str(), vFarmFrame.at(page).gDPage06.driveType, m_showStatusBits);

    if (check_Status_Strip_Status(vFarmFrame.at(page).gDPage06.isDepopped) != 0)
    {
        set_Json_Bool(pageInfo, "Has Drive been Depopped", true);
    }
    else
    {
        set_Json_Bool(pageInfo, "Has Drive been Depopped", false);
    }

    set_json_64_bit_With_Status(pageInfo, "Max Number of Available Sectors for Reassignment", vFarmFrame.at(page).gDPage06.maxNumAvaliableSectors, false, m_showStatusBits);          //!< Max Number of Available Sectors for Reassignment � Value in disc sectors(started in 3.3 )

    double tempValue = (static_cast<double>(M_Word0(vFarmFrame.at(page).gDPage06.timeToReady)) * static_cast<double>(.001F));
    set_json_float_With_Status(pageInfo, "Time to ready of the last power cycle (sec)", tempValue, vFarmFrame.at(page).gDPage06.timeToReady, m_showStatusBits);			//!< time to ready of the last power cycle

    tempValue =(static_cast<double>(M_Word0(vFarmFrame.at(page).gDPage06.holdTime)) * static_cast<double>(.001F));
    set_json_float_With_Status(pageInfo, "Time drive is held in staggered spin (sec)", tempValue, vFarmFrame.at(page).gDPage06.holdTime, m_showStatusBits);                //!< time drive is held in staggered spin during the last power on sequence

    tempValue = (static_cast<double>(M_Word0(vFarmFrame.at(page).gDPage06.servoSpinUpTime)) * static_cast<double>(.001F));
    set_json_float_With_Status(pageInfo, "Last Servo Spin up Time (sec)", tempValue, vFarmFrame.at(page).gDPage06.servoSpinUpTime, m_showStatusBits);			//!< time to ready of the last power cycle

    set_json_bool_With_Status(pageInfo, "HAMR Write Protect", vFarmFrame.at(page).gDPage06.writeProtect, m_showStatusBits);
    status = get_Regen_Head_Mask(pageInfo, vFarmFrame.at(page).gDPage06.regenHeadMask, m_showStatusBits);
    if ((status == eReturnValues::NOT_SUPPORTED) || (status == eReturnValues::SUCCESS))
    {
        status = eReturnValues::SUCCESS;
    }
    opensea_parser::set_json_bool_With_Status(pageInfo, "Power on Hours most recent FARM Time series frame", vFarmFrame.at(page).gDPage06.pohFirst, m_showStatusBits);
    opensea_parser::set_json_bool_With_Status(pageInfo, "Power on Hours of second FARM Time series frame", vFarmFrame.at(page).gDPage06.pohSecond, m_showStatusBits);

    json_push_back(masterData, pageInfo);
    return eReturnValues::SUCCESS;
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
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_WorkLoad(JSONNODE *masterData, uint32_t page)
{
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).workLoadPage.workLoad.copyNumber == FACTORYCOPY)
        {
            printf("\nWork Load From Farm Log copy FACTORY");
        }
        else
        {
            printf("\nWork Load From Farm Log \n");
        }
        printf("\tRated Workload Percentage:                                    %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.workloadPercentage & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< rated Workload Percentage
        printf("\tTotal Number of Read Commands:                                %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCommands & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Total Number of Read Commands
        printf("\tTotal Number of Write Commands:                               %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCommands & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Total Number of Write Commands
        printf("\tTotal Number of Random Read Cmds:                             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalRandomReads & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Total Number of Random Read Commands
        printf("\tTotal Number of Random Write Cmds:                            %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalRandomWrites & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Total Number of Random Write Commands
        printf("\tTotal Number of Other Commands:                               %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalNumberofOtherCMDS & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Total Number Of Other Commands
        printf("\tLogical Sectors Written:                                      %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Logical Sectors Written
        printf("\tLogical Sectors Read:                                         %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Logical Sectors Read
        printf("\tNumber of Read commands from 0-3.125%% of LBA space            %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames1 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read commands from 3.125-25%% of LBA space           %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read commands from 25-50%% of LBA space              %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames3 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read commands from 50-100%% of LBA space             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames4 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
        printf("\tNumber of Write commands from 0-3.125%% of LBA space           %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames1 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write commands from 3.125-25%% of LBA space          %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames2 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write commands from 25-50%% of LBA space             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames3 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write commands from 50-100%% of LBA space            %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames4 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames
        //4.21
        if (m_MajorRev >= 4 && m_MinorRev > 20) {
            printf("\tNumber of Read Commands of transfer length <=16KB             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferSmall & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Read Commands of transfer length (16KB - 512KB]     %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferMid1 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Read Commands of transfer length (512KB - 2MB]      %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferMid2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Read Commands of transfer length > 2MB              %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferLarge & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
            printf("\tNumber of Write Commands of transfer length <=16KB            %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferSmall & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Write Commands of transfer length (16KB - 512KB]    %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferMid1 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Write Commands of transfer length (512KB - 2MB]     %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferMid2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
            printf("\tNumber of Write Commands of transfer length > 2MB             %" PRIu64"  \n", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferLarge & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames
        }
    }

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame.at(page).workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        header.assign("Workload From Farm Log copy FACTORY");
    }
    else
    {
        temp << "Workload From Farm Log ";  
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());
    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentage", vFarmFrame.at(page).workLoadPage.workLoad.workloadPercentage, false, m_showStatusBits);				//!< rated Workload Percentage
    set_json_64_bit_With_Status(pageInfo, "Total Read Commands", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCommands, false, m_showStatusBits);			//!< Total Number of Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Write Commands", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCommands, false, m_showStatusBits);			//!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Read Cmds", vFarmFrame.at(page).workLoadPage.workLoad.totalRandomReads, false, m_showStatusBits);			//!< Total Number of Random Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Write Cmds", vFarmFrame.at(page).workLoadPage.workLoad.totalRandomWrites, false, m_showStatusBits);		//!< Total Number of Random Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Other Commands", vFarmFrame.at(page).workLoadPage.workLoad.totalNumberofOtherCMDS, false, m_showStatusBits);		//!< Total Number Of Other Commands
    set_json_64_bit_With_Status(pageInfo, "Logical Sectors Written", vFarmFrame.at(page).workLoadPage.workLoad.logicalSecWritten, false, m_showStatusBits);      //!< Logical Sectors Written                    
    set_json_64_bit_With_Status(pageInfo, "Logical Sectors Read", vFarmFrame.at(page).workLoadPage.workLoad.logicalSecRead, false, m_showStatusBits);      //!< Logical Sectors Read
    // found a log where the length of the workload log does not match the spec. Need to check for the 0x50 length
    if (vFarmFrame.at(page).workLoadPage.PageHeader.paramLength > 0x50 || m_showStatic == true)
    {
        set_json_64_bit_With_Status(pageInfo, "Read commands from 0-3.125% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames1, false, m_showStatusBits);		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 3.125-25% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames2, false, m_showStatusBits);		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 25-50% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 50-100% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalReadCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
        set_json_64_bit_With_Status(pageInfo, "Write commands from 0-3.125% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames1, false, m_showStatusBits);	//!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 3.125-25% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames2, false, m_showStatusBits);	//!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 25-50% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 50-100% of LBA space", vFarmFrame.at(page).workLoadPage.workLoad.totalWriteCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames 
    }

    // 4.21
    if ((m_MajorRev >= 4 && m_MinorRev >= 19) || m_showStatic == true)
    {
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length <4kb", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferSmall, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (4kb - 16kb)", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferMid1, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (16kb - 128kb)", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferMid2, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length >= 128kb", vFarmFrame.at(page).workLoadPage.workLoad.numReadTransferLarge, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length <4kb", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferSmall, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (4kb - 16kb)", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferMid1, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (16kb - 128kb)", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferMid2, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length >= 128kb", vFarmFrame.at(page).workLoadPage.workLoad.numWriteTransferLarge, false, m_showStatusBits);
    }

    json_push_back(masterData, pageInfo);
    return eReturnValues::SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Error_Information()
//
//! \brief
//!   Description:  print out the Error log information version 4 and up 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Error_Information(JSONNODE *masterData, uint32_t page)
{
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).errorPage.copyNumber == FACTORYCOPY)
        {
            printf("\nError Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nError Information Log From Farm Log  \n");
        }
        printf("\tUnrecoverable Read Errors:                                    %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalReadECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Read Errors
        printf("\tUnrecoverable Write Errors:                                   %" PRIu64" \n", vFarmFrame.at(page).errorPage.totalWriteECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Write Errors
        printf("\tRead Recovery Attempts:                                       %" PRIu64" \n", vFarmFrame.at(page).errorPage.readRecoveryAtt & UINT64_C(0x00FFFFFFFFFFFFFF));
        if (m_MajorRev >= 4 && m_MinorRev >= 33)
        {
            printf("\tSMART Trip Parameter 1 :                                      %" PRIu64" \n", vFarmFrame.at(page).errorPage.smartTripParam1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tSMART Trip Parameter 2 :                                      %" PRIu64" \n", vFarmFrame.at(page).errorPage.smartTripParam2 & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        printf("\tSMART Trip FRU code:                                          %" PRIu64" \n", vFarmFrame.at(page).errorPage.FRUCode & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tInvalid DWord Count Port A                                    %" PRIu64" \n", vFarmFrame.at(page).errorPage.portAInvalidDwordCount & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tInvalid DWord Count Port B                                    %" PRIu64" \n", vFarmFrame.at(page).errorPage.portBInvalidDwordCount & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDisparity Error Count Port A                                  %" PRIu64" \n", vFarmFrame.at(page).errorPage.portADisparityErrorCount & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tDisparity Error Count Port B                                  %" PRIu64" \n", vFarmFrame.at(page).errorPage.portBDisparityErrorCount & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tLoss Of DWord Sync Port A                                     %" PRIu64" \n", vFarmFrame.at(page).errorPage.portALossDwordSync & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tLoss Of DWord Sync Port B                                     %" PRIu64" \n", vFarmFrame.at(page).errorPage.portBLossDwordSync & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPhy Reset Problem Port A                                      %" PRIu64" \n", vFarmFrame.at(page).errorPage.portAPhyResetProblem & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tPhy Reset Problem Port B                                      %" PRIu64" \n", vFarmFrame.at(page).errorPage.portBPhyResetProblem & UINT64_C(0x00FFFFFFFFFFFFFF));
    }

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (vFarmFrame.at(page).errorPage.copyNumber == FACTORYCOPY)
    {
        temp << "Error Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Error Information Log From Farm Log ";
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame.at(page).errorPage.totalReadECC, false, m_showStatusBits);							//!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame.at(page).errorPage.totalWriteECC, false, m_showStatusBits);							//!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Read Recovery Attempts", vFarmFrame.at(page).errorPage.readRecoveryAtt, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame.at(page).errorPage.totalMechanicalFails, false, m_showStatusBits);			        //!< Number of Mechanical Start Failures
    if ((m_MajorRev >= 4 && m_MinorRev >= 30) || m_showStatic == true)
    {
        set_json_64_bit_With_Status(pageInfo, "SMART Trip Parameter 1", vFarmFrame.at(page).errorPage.smartTripParam1, false, m_showStatusBits);
        if ((opensea_parser::check_For_Active_Status(&vFarmFrame.at(page).errorPage.smartTripParam1)) && (((vFarmFrame.at(page).errorPage.smartTripParam1 & UINT64_C(0x00FFFFFFFFFFFFFF)) != 0)) && (m_showStatic == false))
        {
            if (M_Byte0(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x05)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x05", true);
            }
            if (M_Byte1(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x10)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x10", true);
            }
            if (M_Byte2(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x12)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x12", true);
            }
            if (M_Byte3(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x14)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x14", true);
            }
            if (M_Byte4(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x16)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x16", true);
            }
            if (M_Byte5(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x30)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x30", true);
            }
            if (M_Byte6(vFarmFrame.at(page).errorPage.smartTripParam1) == 0x32)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x32", true);
            }
        }
        set_json_64_bit_With_Status(pageInfo, "SMART Trip Parameter 2", vFarmFrame.at(page).errorPage.smartTripParam2, false, m_showStatusBits);
        if ((opensea_parser::check_For_Active_Status(&vFarmFrame.at(page).errorPage.smartTripParam2)) && (((vFarmFrame.at(page).errorPage.smartTripParam2 & UINT64_C(0x00FFFFFFFFFFFFFF)) != 0)) && (m_showStatic == false))
        {
            if (M_Byte0(vFarmFrame.at(page).errorPage.smartTripParam2) == 0x42)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x42", true);
            }
            if (M_Byte1(vFarmFrame.at(page).errorPage.smartTripParam2) == 0x43)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x43", true);
            }
            if (M_Byte2(vFarmFrame.at(page).errorPage.smartTripParam2) == 0x5B)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x5B", true);
            }
            if (M_Byte3(vFarmFrame.at(page).errorPage.smartTripParam2) == 0x92)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x92", true);
            }
            if (M_Byte4(vFarmFrame.at(page).errorPage.smartTripParam2) == 0x93)
            {
                opensea_parser::set_Json_Bool(pageInfo, "SMART trip Attribute 0x93", true);
            }
        }
    }
    set_json_64_bit_With_Status(pageInfo, "SMART Trip FRU code", vFarmFrame.at(page).errorPage.FRUCode, false, m_showStatusBits);		                                //!< FRU code if smart trip from most recent SMART Frame
    set_json_64_bit_With_Status(pageInfo, "Invalid DWord Count Port A ", vFarmFrame.at(page).errorPage.portAInvalidDwordCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Invalid DWord Count Port B", vFarmFrame.at(page).errorPage.portBInvalidDwordCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Disparity Error Count Port A", vFarmFrame.at(page).errorPage.portADisparityErrorCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Disparity Error Count Port B", vFarmFrame.at(page).errorPage.portBDisparityErrorCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Loss Of DWord Sync Port A", vFarmFrame.at(page).errorPage.portALossDwordSync, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Loss Of DWord Sync Port B", vFarmFrame.at(page).errorPage.portBLossDwordSync, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Phy Reset Problem Port A", vFarmFrame.at(page).errorPage.portAPhyResetProblem, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Phy Reset Problem Port B", vFarmFrame.at(page).errorPage.portBPhyResetProblem, false, m_showStatusBits);

    json_push_back(masterData, pageInfo);
  
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Enviroment_Information()
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
eReturnValues CSCSI_Farm_Log::print_Enviroment_Information(JSONNODE *masterData, uint32_t page)
{
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).environmentPage.copyNumber == FACTORYCOPY)
        {
            printf("Environment Information From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nEnvironment Information From Farm Log \n");
        }
        printf("\tCurrent Temperature:                                          %0.02f     \n", (M_WordInt0(vFarmFrame.at(page).environmentPage.curentTemp) * .10));			        //!< Current Temperature in Celsius
        printf("\tHighest Temperature:                                          %0.02f     \n", (M_WordInt0(vFarmFrame.at(page).environmentPage.highestTemp) * .10));			        //!< Highest Temperature in Celsius
        printf("\tLowest Temperature:                                           %0.02f     \n", (M_WordInt0(vFarmFrame.at(page).environmentPage.lowestTemp) * .10));			        //!< Lowest Temperature
        printf("\tSpecified Max Operating Temperature:                          %0.02f     \n", (M_WordInt0(vFarmFrame.at(page).environmentPage.maxTemp) * 1.00));				    //!< Specified Max Operating Temperature
        printf("\tSpecified Min Operating Temperature:                          %0.02f     \n", (M_WordInt0(vFarmFrame.at(page).environmentPage.minTemp) * 1.00));				    //!< Specified Min Operating Temperature
        printf("\tCurrent Relative Humidity:                                    %" PRId32".%" PRId32"   \n", M_DoubleWord1(vFarmFrame.at(page).environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF)), M_DoubleWord0(vFarmFrame.at(page).environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF)));		//!< Current Relative Humidity (in units of .1%)
        printf("\tCurrent Motor Power:                                          %" PRIu16" \n", (M_Word0(vFarmFrame.at(page).environmentPage.currentMotorPower)));
        printf("\t12v Power Average(mw):                                        %0.03f     \n", (vFarmFrame.at(page).environmentPage.average12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\t12v Power Min(mw):                                            %0.03f     \n", (vFarmFrame.at(page).environmentPage.min12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\t12v Power Max(mw):                                            %0.03f     \n", (vFarmFrame.at(page).environmentPage.max12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\t5v Power Average(mw):                                         %0.03f     \n", (vFarmFrame.at(page).environmentPage.average5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\t5v Power Min(mw):                                             %0.03f     \n", (vFarmFrame.at(page).environmentPage.min5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\t5v Power Max(mw):                                             %0.03f     \n", (vFarmFrame.at(page).environmentPage.max5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
    }

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame.at(page).environmentPage.copyNumber == FACTORYCOPY)
    {
        header.assign("Environment Information From Farm Log copy FACTORY");
    }
    else
    {
        temp << "Environment Information From Farm Log ";
        header.assign(temp.str());
    }
    
    json_set_name(pageInfo, header.c_str());
    double TempValue = static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.curentTemp))) * static_cast<double>(0.10F);
    set_json_float_With_Status(pageInfo, "Current Temperature (Celsius)", TempValue, vFarmFrame.at(page).environmentPage.curentTemp, m_showStatusBits);
    TempValue = static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.highestTemp))) * static_cast<double>(0.10F);
    set_json_float_With_Status(pageInfo, "Highest Temperature", TempValue, vFarmFrame.at(page).environmentPage.highestTemp, m_showStatusBits);
    TempValue = static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.lowestTemp))) * static_cast<double>(0.10F);
    set_json_float_With_Status(pageInfo, "Lowest Temperature", TempValue, vFarmFrame.at(page).environmentPage.lowestTemp, m_showStatusBits);
    TempValue = static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.maxTemp))) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Specified Max Operating Temperature", TempValue, vFarmFrame.at(page).environmentPage.maxTemp, m_showStatusBits);
    TempValue = static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.minTemp))) * static_cast<double>(1.00F);
    set_json_float_With_Status(pageInfo, "Specified Min Operating Temperature", TempValue, vFarmFrame.at(page).environmentPage.minTemp, m_showStatusBits);
    TempValue = (static_cast<double>(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.humidity)) * static_cast<double>(0.1F)));							//!< Current Relative Humidity (in units of .1%)
    set_json_float_With_Status(pageInfo, "Current Relative Humidity", TempValue, vFarmFrame.at(page).environmentPage.humidity, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Current Motor Power",  vFarmFrame.at(page).environmentPage.currentMotorPower,false, m_showStatusBits);					    //!< Current Motor Power, value from most recent SMART Summary Frame6

        if (m_MajorRev >= 4 || m_showStatic == true)
        {
        TempValue = static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.average12v))) * static_cast<double>(0.001F);
        set_json_float_With_Status(pageInfo, "12V Power Average", TempValue, vFarmFrame.at(page).environmentPage.average12v, m_showStatusBits);
            if (m_MinorRev >= 9 || m_showStatic == true)
        {
            TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.min12v))) * static_cast<double>(0.001F);
            set_json_float_With_Status(pageInfo, "12V Power Minimum", TempValue, vFarmFrame.at(page).environmentPage.min12v, m_showStatusBits);
            TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.max12v))) * static_cast<double>(0.001F);
            set_json_float_With_Status(pageInfo, "12V Power Maximum", TempValue, vFarmFrame.at(page).environmentPage.max12v, m_showStatusBits);
        }
        TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.average5v))) * static_cast<double>(0.001F);
        set_json_float_With_Status(pageInfo, "5V Power Average", TempValue, vFarmFrame.at(page).environmentPage.average5v, m_showStatusBits);
            if (m_MinorRev >= 9 || m_showStatic == true)
        {
            TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.min5v))) * static_cast<double>(0.001F);
            set_json_float_With_Status(pageInfo, "5V Power Minimum", TempValue, vFarmFrame.at(page).environmentPage.min5v, m_showStatusBits);
            TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).environmentPage.max5v))) * static_cast<double>(0.001F);
            set_json_float_With_Status(pageInfo, "5V Power Maximum", TempValue, vFarmFrame.at(page).environmentPage.max5v, m_showStatusBits);
        }
    }

    json_push_back(masterData, pageInfo);

    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Enviroment_Statistics_Page_07()
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
eReturnValues CSCSI_Farm_Log::print_Enviroment_Statistics_Page_07(JSONNODE *masterData, uint32_t page)
{
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).envStatPage07.copyNumber == FACTORYCOPY)
        {
            printf("Environment Information Continued From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nEnvironment Information Continued From Farm Log \n");
        }
        printf("\tCurrent 12 volts:                                             %0.03f     \n", (vFarmFrame.at(page).envStatPage07.average12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tMinimum 12 volts:                                             %0.03f     \n", (vFarmFrame.at(page).envStatPage07.min12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tMaximum 12 volts:                                             %0.03f     \n", (vFarmFrame.at(page).envStatPage07.max12v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tCurrent 5 volts:                                              %0.03f     \n", (vFarmFrame.at(page).envStatPage07.average5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tMinimum 5 volts:                                              %0.03f     \n", (vFarmFrame.at(page).envStatPage07.min5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
        printf("\tMaximum 5 volts:                                              %0.03f     \n", (vFarmFrame.at(page).envStatPage07.max5v & UINT64_C(0x00FFFFFFFFFFFFFF)) * 0.001);
    }
    
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (vFarmFrame.at(page).envStatPage07.copyNumber == FACTORYCOPY)
    {
        temp << "Environment Information Continued From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Environment Information Continued From Farm Log ";
    }
    json_set_name(pageInfo, temp.str().c_str());

    double TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.average12v)) * 0.001L);
    set_json_float_With_Status(pageInfo, "Current 12 volts", TempValue, vFarmFrame.at(page).envStatPage07.average12v, m_showStatusBits);
    if (m_MinorRev > 9 || m_showStatic == true)
    {
        TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.min12v)) * 0.001L);
        set_json_float_With_Status(pageInfo, "Minimum 12 volts", TempValue, vFarmFrame.at(page).envStatPage07.min12v, m_showStatusBits);
        TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.max12v)) * 0.001L);
        set_json_float_With_Status(pageInfo, "Maximum 12 volts", TempValue, vFarmFrame.at(page).envStatPage07.max12v, m_showStatusBits);
    }
    TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.average5v)) * 0.001L);
    set_json_float_With_Status(pageInfo, "Current 5 volts", TempValue, vFarmFrame.at(page).envStatPage07.average5v, m_showStatusBits);
    if (m_MinorRev > 9 || m_showStatic == true)
    {
        TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.min5v)) * 0.001L);
        set_json_float_With_Status(pageInfo, "Minimum 5 volts", TempValue, vFarmFrame.at(page).envStatPage07.min5v, m_showStatusBits);
        TempValue = static_cast<double>(M_Word0(check_Status_Strip_Status(vFarmFrame.at(page).envStatPage07.max5v)) * 0.001L);
        set_json_float_With_Status(pageInfo, "Maximum 5 volts", TempValue, vFarmFrame.at(page).envStatPage07.max5v, m_showStatusBits);
    }

    json_push_back(masterData, pageInfo);
  
    return eReturnValues::SUCCESS;
}


//-----------------------------------------------------------------------------
//
//! \fn print_Workload_Statistics_Page_08()
//
//! \brief
//!   Description:  print out the Workload log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Workload_Statistics_Page_08(JSONNODE *masterData, uint32_t page)
{
    if ((m_MajorRev >= 4 && m_MinorRev >= 19) || m_showStatic == true)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);

        if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
        {
            if (vFarmFrame.at(page).workloadStatPage08.copyNumber == FACTORYCOPY)
            {
                printf("Workload Information Continued From Farm Log copy FACTORY \n");
            }
            else
            {
                printf("\nWorkload Information Continued From Farm Log \n");
            }
            printf("\tCount of Queue Depth =1 at 30s intervals:                     %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth1 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth =2 at 30s intervals:                     %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth2 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth 2-4 at 30s intervals:                    %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth3_4 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth 5-8 at 30s intervals:                    %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth5_8 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth 9-16 at 30s intervals:                   %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth9_16 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth 17-32 at 30s intervals:                  %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth17_32 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth 33-64 at 30s intervals:                  %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth33_64 & UINT64_C(0x00FFFFFFFFFFFFFF));
            printf("\tCount of Queue Depth at 64 at 30s intervals:                  %" PRIu64" \n", vFarmFrame.at(page).workloadStatPage08.countQueDepth_gt_64 & UINT64_C(0x00FFFFFFFFFFFFFF));
        }
        std::ostringstream temp;
        if (vFarmFrame.at(page).workloadStatPage08.copyNumber == FACTORYCOPY)
        {
            temp << "Workload Information Continued From Farm Log copy FACTORY";
        }
        else
        {
            temp << "Workload Information Continued From Farm ";
        }
        json_set_name(pageInfo, temp.str().c_str());
        set_json_int_With_Status(pageInfo, "Queue Depth bin = 1", vFarmFrame.at(page).workloadStatPage08.countQueDepth1, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin = 2", vFarmFrame.at(page).workloadStatPage08.countQueDepth2, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin 3-4", vFarmFrame.at(page).workloadStatPage08.countQueDepth3_4, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin 5-8", vFarmFrame.at(page).workloadStatPage08.countQueDepth5_8, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin 9-16", vFarmFrame.at(page).workloadStatPage08.countQueDepth9_16, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin 17-32", vFarmFrame.at(page).workloadStatPage08.countQueDepth17_32, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin 33-64", vFarmFrame.at(page).workloadStatPage08.countQueDepth33_64, m_showStatusBits);
        set_json_int_With_Status(pageInfo, "Queue Depth bin > 64", vFarmFrame.at(page).workloadStatPage08.countQueDepth_gt_64, m_showStatusBits);
        
        json_push_back(masterData, pageInfo);

    }
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
eReturnValues CSCSI_Farm_Log::print_Reli_Information(JSONNODE *masterData, uint32_t page)
{
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    
    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
        {
            printf("Reliability Information From Farm Log copy FACTORY");
        }
        else
        {
            printf("\nReliability Information From Farm Log \n");
        }
        printf("\tNumbert of RAW ops:                                           %" PRIu64" \n", vFarmFrame.at(page).reliPage.numberRAWops & UINT64_C(0x00FFFFFFFFFFFFFF));						
        printf("\tCumulative Lifetime ECC due to ERC:                           %" PRIu64" \n", vFarmFrame.at(page).reliPage.cumECCDueToERC & UINT64_C(0x00FFFFFFFFFFFFFF));					
        printf("\tHigh Priority Unload Events:                                  %" PRIu64" \n", vFarmFrame.at(page).reliPage.unloadEvents & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\t# of Disc Slip Recalibrations Performed :                     %" PRIu64" \n", vFarmFrame.at(page).reliPage.diskSlipRecalPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< Number of disc slip recalibrations performed
        printf("\tHelium Pressure Threshold Trip:                               %" PRIu64" \n", vFarmFrame.at(page).reliPage.heliumPressuretThreshold & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< helium Pressure Threshold Trip		               
    }
    if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
    {
        temp << "Reliability Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Reliability Information From Farm Log ";
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Number of RAW Operations", vFarmFrame.at(page).reliPage.numberRAWops, false, m_showStatusBits);								//!< Number of RAW Operations
    set_json_64_bit_With_Status(pageInfo, "Cumulative Lifetime ECC due to ERC", vFarmFrame.at(page).reliPage.cumECCDueToERC, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "High Priority Unload Events", vFarmFrame.at(page).reliPage.unloadEvents, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame.at(page).reliPage.diskSlipRecalPerformed, false, m_showStatusBits);//!< Number of disc slip recalibrations performed
    set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame.at(page).reliPage.heliumPressuretThreshold, false, m_showStatusBits);			//!< helium Pressure Threshold Trip
   

    json_push_back(masterData, pageInfo);
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Head_Information()
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
eReturnValues CSCSI_Farm_Log::print_Head_Information(eSASLogPageTypes type, JSONNODE *headPage, uint32_t page)
{
    uint32_t loopCount = 0;
    int16_t whole = 0;
    std::string myHeader;

    if (set_Head_Header(myHeader, type) == false)
    {
        return eReturnValues::FAILURE;
    }
    if (type != eSASLogPageTypes::MAX_RESERVED_FOR_FUTURE_EXPANSION)
    {
        if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
        {
            if (vFarmFrame.at(page).reliPage.copyNumber == FACTORYCOPY)
            {
                printf("\nHead Information From Farm Log copy FACTORY");
            }
            else
            {
                printf("\nHead Information From Farm Log \n");
            }
        }
        switch (type)
        {
        case eSASLogPageTypes::FARM_HEADER_PARAMETER:
        case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_PARAMETER:
        case eSASLogPageTypes::WORKLOAD_STATISTICS_PARAMETER:
        case eSASLogPageTypes::ERROR_STATISTICS_PARAMETER:
        case eSASLogPageTypes::ENVIRONMENTAL_STATISTICS_PARAMETER:
        case eSASLogPageTypes::RELIABILITY_STATISTICS_PARAMETER:
        case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_06:
        case eSASLogPageTypes::ENVIRONMENT_STATISTICS_PAMATER_07:
        case eSASLogPageTypes::WORKLOAD_STATISTICS_PAMATER_08:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_4:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_5:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_6:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_7:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_8:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_9:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_10:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_1:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_2:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_3:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_4:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_5:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_6:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_7:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_8:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_9:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_10:
            break;
        case eSASLogPageTypes::MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            if (vFarmFrame.at(page).mrHeadResistanceByHead.headValue[0] & BIT48)
            {
                if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
                {
                    // version 4.34 MR Head Resistance became a percentage. Check bit 48 if set then it is a percentage
                    for (loopCount = 0; loopCount < m_heads; ++loopCount)
                    {
                        int64_t delta = M_IGETBITRANGE((check_Status_Strip_Status(vFarmFrame.at(page).mrHeadResistanceByHead.headValue[loopCount])), 47, 0);
                        whole = M_WordInt2(delta);							                             // get 5:4 whole part of the float
                        double decimal = static_cast<double>(M_DoubleWordInt0(delta));                   // get 3:0 for the Deciaml Part of the float
                        double number = 0.0;
                        if ((vFarmFrame.at(page).mrHeadResistanceByHead.headValue[loopCount] & BIT49) && (whole <= 0))
                        {
                            number = static_cast<double>(whole) - (decimal * static_cast<double>(.0001F));
                        }
                        else
                        {
                            number = static_cast<double>(whole) + (decimal * static_cast<double>(.0001F));
                        }
                        printf("\tMR Head Resistance percentage for Head %2" PRIu32":                    %.4lf \n", loopCount, number);       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                    }
                }
                int_Percent_Dword_Data(headPage, "MR Head Resistance Percentage", reinterpret_cast<int64_t*>(vFarmFrame.at(page).mrHeadResistanceByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            }
            else
            {
                if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
                {
                    for (loopCount = 0; loopCount < m_heads; ++loopCount)
                    {
                        printf("\tMR Head Resistance from Head %2" PRIu32":                              %" PRIu64" \n", loopCount, vFarmFrame.at(page).mrHeadResistanceByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                    }
                }
                int_Data(headPage, "MR Head Resistance", reinterpret_cast<int64_t*>(vFarmFrame.at(page).mrHeadResistanceByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            }
            break;
        case eSASLogPageTypes::NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tServo No Timing Mark Detect for Head %2" PRIu32":                      %" PRIu64" \n", loopCount, vFarmFrame.at(page).numberOfTMDByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
                }
            }
            int_Data(headPage, "Servo No Timing Mark Detect", reinterpret_cast<int64_t*>(vFarmFrame.at(page).numberOfTMDByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tVelocity Observer for Head %2" PRIu32":                                %" PRIu64" \n", loopCount, vFarmFrame.at(page).velocityObserverByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
                }
            }
            int_Data(headPage, "Velocity Observer", reinterpret_cast<int64_t*>(vFarmFrame.at(page).velocityObserverByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tServo Velocity No Timing Mark Detect for Head %2" PRIu32":             %" PRIu64" \n", loopCount, vFarmFrame.at(page).numberOfVelocityObservedByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
                }
            }
            int_Data(headPage, "Servo Velocity No Timing Mark Detect", reinterpret_cast<int64_t*>(vFarmFrame.at(page).numberOfVelocityObservedByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_14:
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            {
                if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
                {
                    for (loopCount = 0; loopCount < m_heads; ++loopCount)
                    {
                        printf("\tCurrent H2SAT amplitude for Head %2" PRIu32":                           %" PRIu64" \n", loopCount, vFarmFrame.at(page).currentH2STAmplituedByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                    }
                }
            int_Data(headPage, "Current H2SAT amplitude", reinterpret_cast<int64_t*>(vFarmFrame.at(page).currentH2STAmplituedByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            }
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT asymmetry by Head %2" PRIu32":                           %0.3f  \n", loopCount, (M_WordInt0(vFarmFrame.at(page).currentH2STAsymmetryByHead.headValue[loopCount])) * 0.10);
                }
            }
            float_Cal_Word_Data(headPage, "Current H2SAT asymmetry", 0.10, reinterpret_cast<int64_t*>(vFarmFrame.at(page).currentH2STAsymmetryByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::NUMBER_OF_RESIDENT_GLIST_ENTRIES:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tReallocated Sectors for Head %2" PRIu32":                               %" PRIu64" \n", loopCount, vFarmFrame.at(page).ResidentGlistEntries.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                }
            }
            int_Data(headPage, "Reallocated Sectors", reinterpret_cast<int64_t*>(vFarmFrame.at(page).ResidentGlistEntries.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::NUMBER_OF_PENDING_ENTRIES:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tReallocated Candidate Sectors for Head %2" PRIu32":                     %" PRIu64" \n", loopCount, vFarmFrame.at(page).ResidentPlistEntries.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                }
            }
            int_Data(headPage, "Reallocated Candidate Sectors", reinterpret_cast<int64_t*>(vFarmFrame.at(page).ResidentPlistEntries.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_15:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_16:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_17:
            break;
        case eSASLogPageTypes::WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tWrite Power On (hrs) for Head %2" PRIu32":                              %0.3f  \n", loopCount, static_cast<double>(M_DoubleWord0(vFarmFrame.at(page).writePowerOnHours.headValue[loopCount])) / 3600.0);
                }
            }
            float_Cal_DoubleWord_Data(headPage, "Write Power On (hrs)", 3600.0, reinterpret_cast<int64_t*>(vFarmFrame.at(page).writePowerOnHours.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_18:
            break;
        case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCum Lifetime Unrecoverable Read for Head %2" PRIu32":                   %" PRIu64" \n", loopCount, vFarmFrame.at(page).cumECCReadRepeat.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                }
            }
            int_Data(headPage, "Cum Lifetime Unrecoverable Read", reinterpret_cast<int64_t*>(vFarmFrame.at(page).cumECCReadRepeat.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCum Lifetime Unrecoverable Read Unique for Head %2" PRIu32":            %" PRIu64" \n", loopCount, vFarmFrame.at(page).cumECCReadUnique.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       
                }
            }
            int_Data(headPage, "Cum Lifetime Unrecoverable Read Unique", reinterpret_cast<int64_t*>(vFarmFrame.at(page).cumECCReadUnique.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_19:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_20:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_21:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_22:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_23:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_24:
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT trimmed mean bits in error Zone 0 for Head %2" PRIu32":   %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STTrimmedbyHeadZone0.headValue[loopCount])), 16)) * 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT trimmed mean bits in error Zone 0", WORD0, 0.10, vFarmFrame.at(page).currentH2STTrimmedbyHeadZone0.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT trimmed mean bits in error Zone 1 for Head %2" PRIu32":   %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STTrimmedbyHeadZone1.headValue[loopCount])), 16)) * 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT trimmed mean bits in error Zone 1", WORD0, 0.10, vFarmFrame.at(page).currentH2STTrimmedbyHeadZone1.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT trimmed mean bits in error Zone 2 for Head %2" PRIu32":   %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STTrimmedbyHeadZone2.headValue[loopCount])), 16)) * 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT trimmed mean bits in error Zone 2", WORD0, 0.10, vFarmFrame.at(page).currentH2STTrimmedbyHeadZone2.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT iterations to converge Test Zone 0 for Head %2" PRIu32":  %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STIterationsByHeadZone0.headValue[loopCount])), 16) )* 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT iterations to converge Test Zone 0", WORD0, 0.10, vFarmFrame.at(page).currentH2STIterationsByHeadZone0.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT iterations to converge Test Zone 1 for Head %2" PRIu32":  %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STIterationsByHeadZone1.headValue[loopCount])), 16)) * 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT iterations to converge Test Zone 1", WORD0, 0.10, vFarmFrame.at(page).currentH2STIterationsByHeadZone1.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tCurrent H2SAT iterations to converge Test Zone 2 for Head %2" PRIu32":  %0.3f \n", loopCount, static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(vFarmFrame.at(page).currentH2STIterationsByHeadZone2.headValue[loopCount])), 16))* 0.10);
                }
            }
            sas_Head_Float_Data(headPage, "Current H2SAT iterations to converge Test Zone 2", WORD0, 0.10, vFarmFrame.at(page).currentH2STIterationsByHeadZone2.headValue, m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_25:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_26:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_27:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_28:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_29:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_30:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_31:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_32:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_33:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_34:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_35:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_36:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_37:
            break;
        case eSASLogPageTypes::SECOND_MR_HEAD_RESISTANCE:
            if (vFarmFrame.at(page).mrHeadResistanceByHead.headValue[0] & BIT48)
            {
                if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
                {
                    // version 4.34 MR Head Resistance became a percentage. Check bit 48 if set then it is a percentage
                    for (loopCount = 0; loopCount < m_heads; ++loopCount)
                    {
                        int64_t delta = M_IGETBITRANGE((check_Status_Strip_Status(vFarmFrame.at(page).secondMRHeadResistanceByHead.headValue[loopCount])), 47, 0);
                        whole = M_WordInt2(delta);							                             // get 5:4 whole part of the float
                        double decimal = static_cast<double>(M_DoubleWordInt0(delta));                   // get 3:0 for the Deciaml Part of the float
                        double number = 0.0;
                        // check bit 49 and whole being less then 0
                        if ((vFarmFrame.at(page).secondMRHeadResistanceByHead.headValue[loopCount] & BIT49) || (whole <= 0))
                        {
                            number = static_cast<double>(whole) - (decimal * static_cast<double>(.0001F));
                        }
                        else
                        {
                            number = static_cast<double>(whole) + (decimal * static_cast<double>(.0001F));
                        }
                        printf("\tSecond MR Head Resistance percentage for Head %2" PRIu32":                    %.4lf \n", loopCount, number);       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                    }
                }
                int_Percent_Dword_Data(headPage, "Second MR Head Resistance Percentage", reinterpret_cast<int64_t*>(vFarmFrame.at(page).secondMRHeadResistanceByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            }
            else
            {
                if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
                {
                    for (loopCount = 0; loopCount < m_heads; ++loopCount)
                    {
                        printf("\tSecond MR Head Resistance for Head %2" PRIu32":                         %" PRIu64" \n", loopCount, vFarmFrame.at(page).secondMRHeadResistanceByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                    }
                }
                int_Data(headPage, "Second MR Head Resistance", reinterpret_cast<int64_t*>(vFarmFrame.at(page).secondMRHeadResistanceByHead.headValue), m_heads, m_showStatusBits, m_showStatic);
            }
            break;
        case eSASLogPageTypes::LUN_0_ACTUATOR:
        case eSASLogPageTypes::LUN_0_FLASH_LED:
        case eSASLogPageTypes::LUN_REALLOCATION_0:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_42:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_43:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_44:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_45:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_46:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_47:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_48:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_49:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_50:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_51:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_52:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_53:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_54:
        case eSASLogPageTypes::LUN_1_ACTUATOR:
        case eSASLogPageTypes::LUN_1_FLASH_LED:
        case eSASLogPageTypes::LUN_REALLOCATION_1:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_61:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_62:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_63:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_64:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_65:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_66:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_67:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_68:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_69:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_70:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_71:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_72:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_73:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_74:
        case eSASLogPageTypes::LUN_2_ACTUATOR:
        case eSASLogPageTypes::LUN_2_FLASH_LED:
        case eSASLogPageTypes::LUN_REALLOCATION_2:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_81:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_82:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_83:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_84:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_85:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_86:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_87:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_88:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_89:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_90:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_91:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_92:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_93:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_94:
        case eSASLogPageTypes::LUN_3_ACTUATOR:
        case eSASLogPageTypes::LUN_3_FLASH_LED:
        case eSASLogPageTypes::LUN_REALLOCATION_3:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_101:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_102:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_103:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_104:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_105:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_106:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_107:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_108:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_109:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_110:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_111:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_112:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_113:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_114:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_115:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_116:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_117:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_118:  //150
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_119:
            break;
        case eSASLogPageTypes::LIFETIME_TERABYTES_WRITTEN:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tLifetime Terabytes Writtens for Head %2" PRIu32":                         %" PRIu64" \n", loopCount, vFarmFrame.at(page).lifeTimeWritten.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                }
            }
            int_Data(headPage, "Lifetime Terabytes Writtens", reinterpret_cast<int64_t*>(vFarmFrame.at(page).lifeTimeWritten.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_120:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_121:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_122:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_123:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_124:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_125:
        case eSASLogPageTypes::UNIQUE_UNRECOVERABLES_SINCE:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tUnique unrecoverable sectors since the last FARM Frame for Head %2" PRIu32":                         %" PRIu64" \n", loopCount, vFarmFrame.at(page).uniqueURESince.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                }
            }
            int_Data(headPage, "Unique unrecoverable sectors since the last FARM Frame", reinterpret_cast<int64_t*>(vFarmFrame.at(page).uniqueURESince.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::UNIQUE_UNRECOVERALBES_BETWEEN:
            if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
            {
                for (loopCount = 0; loopCount < m_heads; ++loopCount)
                {
                    printf("\tUnique unrecoverable sectors previous FARM Frame for Head %2" PRIu32":                         %" PRIu64" \n", loopCount, vFarmFrame.at(page).uniqueURESince.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head9,10
                }
            }
            int_Data(headPage, "Unique unrecoverable sectors previous FARM Frame", reinterpret_cast<int64_t*>(vFarmFrame.at(page).uniqueUREBetween.headValue), m_heads, m_showStatusBits, m_showStatic);
            break;
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_128:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_129:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_130:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_131:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_132:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_133:
        case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_134:
        case eSASLogPageTypes::MAX_RESERVED_FOR_FUTURE_EXPANSION:
            break;
        default:
#if defined _DEBUG
            printf("\tSomething went wrong, ERROR \n");
#endif
            break;
        }
    }

    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_LUN_Acutator_Information()
//
//! \brief
//!   Description:  print out the LUN Acutator information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_Information(JSONNODE *LUNData, uint32_t page, uint16_t actNum)
{
    sLUNStruct *pLUN;
    //pLUN = &vFarmFrame.at(page).vLUN50;
    if (actNum ==  static_cast<uint16_t>(eSASLogPageTypes::LUN_1_ACTUATOR))
        pLUN = &vFarmFrame.at(page).vLUN60;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_2_ACTUATOR))
        pLUN = &vFarmFrame.at(page).vLUN70;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_3_ACTUATOR))
        pLUN = &vFarmFrame.at(page).vLUN80;
    else
        pLUN = &vFarmFrame.at(page).vLUN50;

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (pLUN->copyNumber == FACTORYCOPY)
        {
            printf("LUN Actuator 0x%" PRIx16" Information From Farm Log copy FACTORY", M_Word0(pLUN->LUNID));
        }
        else
        {
            printf("\nLUN Actuator 0x%" PRIx16" Information From Farm Log \n", M_Word0(pLUN->LUNID));
        }
        printf("\tPage Number:                                                  0x%" PRIx64" \n", pLUN->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Page Number                
        printf("\tLUN ID:                                                       %" PRIu64" \n", pLUN->LUNID & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< LUN ID  
        printf("\tHead Load Events:                                             %" PRIu64" \n", pLUN->headLoadEvents & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Head Load Events 
        printf("\tNumber of DOS Scans Performed:                                %" PRIu64" \n", pLUN->dosScansPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of DOS Scans Performed
        printf("\tNumber of LBAs Corrected by ISP:                              %" PRIu64" \n", pLUN->correctedLBAbyISP & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of LBAs Corrected by ISP
        printf("\tNumber of LBAs Corrected by Parity Sector CMR:                %" PRIu64" \n", pLUN->lbasCorrectedByParity & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Number of LBAs Corrected by Parity Sector
        printf("\tNumber of LBAs Corrected by Parity Sector SMR:                %" PRIu64" \n", pLUN->primarySPCovPercentageSMR & UINT64_C(0x00FFFFFFFFFFFFFF));    //!< Number of LBAs Corrected by Parity Sector SMR
    }

    std::ostringstream temp;
    JSONNODE* pageInfo = json_new(JSON_NODE);
    if (pLUN->copyNumber == FACTORYCOPY)
    {
        temp << "LUN Actuator 0x" << std::hex << M_Word0(pLUN->LUNID) << " Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "LUN Actuator Information 0x" << std::hex << M_Word0(pLUN->LUNID) <<" From Farm Log copy " <<std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Page Number", pLUN->pageNumber, true, m_showStatusBits);							                            //!< Page Number 
    set_json_64_bit_With_Status(pageInfo, "LUN Actuator ID", pLUN->LUNID, false, m_showStatusBits);						                                //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Head Load Events", pLUN->headLoadEvents, false, m_showStatusBits);											//!< Head Load Events 
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", pLUN->dosScansPerformed, false, m_showStatusBits);	                        //!< Number of DOS Scans Performed 
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", pLUN->correctedLBAbyISP, false, m_showStatusBits);                         //!< Number of LBAs Corrected by ISP  
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector", pLUN->lbasCorrectedByParity, false, m_showStatusBits);           //!< Number of LBAs Corrected by Parity Sector
    set_json_64_bit_With_Status(pageInfo, "primary super parity coverage CMR",  pLUN->primarySPCovPercentageCMR, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "primary super parity coverage SMR",  pLUN->primarySPCovPercentageSMR, false, m_showStatusBits);
    json_push_back(LUNData, pageInfo);
    
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn print_LUN_Actuator_FLED_Info()
//
//! \brief
//!   Description:  print out the LUN Acutator information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_FLED_Info(JSONNODE *LUNFLED, uint32_t page,  uint16_t actNum)
{

    uint16_t i = 0;
    sActuatorFLEDInfo *pFLED;
    if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_1_FLASH_LED))
        pFLED = &vFarmFrame.at(page).fled61;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_2_FLASH_LED))
        pFLED = &vFarmFrame.at(page).fled71;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_3_FLASH_LED))
        pFLED = &vFarmFrame.at(page).fled81;
    else
        pFLED = &vFarmFrame.at(page).fled51;

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (pFLED->copyNumber == FACTORYCOPY)
        {
            printf("Actuator 0x%" PRIx16" FLED Info From Farm Log copy FACTORY", M_Word0(pFLED->actID));
        }
        else
        {
            printf("\nActuator 0x%" PRIx16" FLED Info From Farm Log \n", M_Word0(pFLED->actID));
        }
        printf("\tPage Number:                                                  0x%" PRIx64" \n", pFLED->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Page Number 
        printf("\tCopy Number:                                                  %" PRIu64" \n", pFLED->copyNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Copy Number 
        printf("\tActuator ID:                                                  %" PRIu64" \n", pFLED->actID & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Actuator ID  
        printf("\tTotal Flash LED Events:                                       %" PRIu64" \n", pFLED->totalFLEDEvents & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< Total Flash LED Events
        printf("\tIndex of Last Flash LED:                                      %" PRIu64" \n", pFLED->index & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Index of last entry in FLED Info array below, in case the array wraps
        for (i = 0; i < FLASH_EVENTS; i++)
        {

            printf("\tInformation of Event %2" PRIu16":                                      0x%" PRIx64" \n", i, pFLED->flashLEDArray[i] & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Info on the last 8 Flash LED (assert) Events, wrapping array
            printf("\tTimeStamp of Event %2" PRIu16":                                        %" PRIu64" \n", i, pFLED->timestampForLED[i] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array
            printf("\tPower Cycle Event %2" PRIu16":                                         %" PRIu64" \n", i, pFLED->powerCycleOfLED[i] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< SPower Cycle of the last 8 Flash LED (assert) Events, wrapping array
        }
    }
    
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (pFLED->copyNumber == FACTORYCOPY)
    {
        temp << "Actuator Flash LED 0x" << std::hex << M_Word0(pFLED->actID) << " Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Actuator Flash LED Information 0x" << std::hex << M_Word0(pFLED->actID) << " From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Page Number", pFLED->pageNumber, true, m_showStatusBits);							                        //!< Page Number 
    set_json_64_bit_With_Status(pageInfo, "Copy Number", pFLED->copyNumber, false, m_showStatusBits);						                            //!< Copy Number 
    set_json_64_bit_With_Status(pageInfo, "Actuator ID", pFLED->actID, false, m_showStatusBits);						                                        //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED Events", pFLED->totalFLEDEvents, false, m_showStatusBits);											//!< Head Load Events 
    set_json_64_bit_With_Status(pageInfo, "Index of Last Flash LED", pFLED->index, false, m_showStatusBits);
    for (i = 0; i < FLASH_EVENTS; i++)
    {
        JSONNODE* eventInfo = json_new(JSON_NODE);
        temp.str("");
        temp.clear();
        temp << "Event " << std::dec << i;
        json_set_name(eventInfo, temp.str().c_str());

        opensea_parser::set_json_64_bit_With_Status(eventInfo, "Event Information", pFLED->flashLEDArray[i], true, m_showStatusBits);	           //!< Info on the last 8 Flash LED (assert) Events, wrapping array
        if (opensea_parser::check_For_Active_Status(&pFLED->flashLEDArray[i]) || g_parseNULL)
        {
            temp.str("");
            temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(opensea_parser::check_Status_Strip_Status(pFLED->flashLEDArray[i]));
            opensea_parser::set_json_string_With_Status(eventInfo, "Flash LED Code", temp.str().c_str(), pFLED->flashLEDArray[i], m_showStatusBits);
            temp.str("");
            temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(opensea_parser::check_Status_Strip_Status(pFLED->flashLEDArray[i]));
            opensea_parser::set_json_string_With_Status(eventInfo, "Flash LED Address", temp.str().c_str(), pFLED->flashLEDArray[i], m_showStatusBits);
            temp.str("");
            temp.clear();
            temp << "TimeStamp of Event(hours) " << std::dec << i;
            double timestamp = static_cast<double>(opensea_parser::check_Status_Strip_Status(pFLED->timestampForLED[i]) / 3600000) * .001;
            opensea_parser::set_json_float_With_Status(eventInfo, temp.str().c_str(), timestamp, pFLED->timestampForLED[i], m_showStatusBits);//!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array
            temp.str("");
            temp.clear();
            temp << "Power Cycle Event " << std::dec << i;
            opensea_parser::set_json_64_bit_With_Status(eventInfo, temp.str(), pFLED->powerCycleOfLED[i], false, m_showStatusBits);	         //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array
        }
        json_push_back(pageInfo, eventInfo);
    }

    json_push_back(LUNFLED, pageInfo);
    
    return eReturnValues::SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_LUN_Actuator_Reallocation
//
//! \brief
//!   Description:  print out the LUN Acutator information for Realalocation
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_Reallocation(JSONNODE* LUNNReallocation, uint32_t page, uint16_t actNum)
{
    sActReallocationData* pReal;
    if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_1))
        pReal = &vFarmFrame.at(page).reall62;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_2))
        pReal = &vFarmFrame.at(page).reall72;
    else if (actNum == static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_3))
        pReal = &vFarmFrame.at(page).reall82;
    else
        pReal = &vFarmFrame.at(page).reall52;

    if (g_verbosity >= eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE)
    {
        if (pReal->copyNumber == FACTORYCOPY)
        {
            printf("LUN Actuator 0x%" PRIx16" Reallocation From Farm Log copy FACTORY", M_Word0(pReal->actID));
        }
        else
        {
            printf("\nLUN Actuator 0x%" PRIx16" Reallocation From Farm Log \n", M_Word0(pReal->actID));
        }
        printf("\tPage Number:                                                  0x%" PRIx64" \n", pReal->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                    //!< Page Number 
        printf("\tActuator ID:                                                  %" PRIu64" \n", pReal->actID & UINT64_C(0x00FFFFFFFFFFFFFF));                           //!< Actuator ID  
        printf("\tNumber of Reallocated Sectors:                                %" PRIu64" \n", pReal->numberReallocatedSectors & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Total Flash LED Events
        printf("\tNumber of Reallocated Candidate Sectors:                      %" PRIu64" \n", pReal->numberReallocatedCandidates & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReallocated sectors since the last FARM Time series Frame:    %" PRIu64" \n", pReal->numReallocatedSince & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReallocated sectors between FARM time series Frame:           %" PRIi64" \n", pReal->numReallocatedBetween & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReallocation candidate since the last FARM Time series Frame: %" PRIu64" \n", pReal->numCandidateSince & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReallocation candidate between FARM time series Frame:        %" PRIi64" \n", pReal->numCandidateBetween & UINT64_C(0x00FFFFFFFFFFFFFF));
    }
 
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (pReal->copyNumber == FACTORYCOPY)
    {
        temp << "LUN Actuator 0x" << std::hex << M_Word0(pReal->actID) << " Reallocation From Farm Log copy FACTORY";
    }
    else
    {
        temp << "LUN Actuator 0x" << std::hex << M_Word0(pReal->actID) << " Reallocation From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Page Number", pReal->pageNumber, true, m_showStatusBits);							                            //!< Page Number
    set_json_64_bit_With_Status(pageInfo, "Actuator ID", pReal->actID, true, m_showStatusBits);						                                        //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", pReal->numberReallocatedSectors, false, m_showStatusBits);						//!< Head Load Events 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors", pReal->numberReallocatedCandidates, false, m_showStatusBits);
    long long reallo = check_for_signed_int(check_Status_Strip_Status(pReal->numReallocatedSince), 8);
    set_json_int_Check_Status(pageInfo, "Reallocated sectors since the last FARM Time series Frame", reallo, pReal->numReallocatedSince, m_showStatusBits);
    reallo = check_for_signed_int(check_Status_Strip_Status(static_cast<uint64_t>(pReal->numReallocatedBetween)), 8);
    set_json_int_Check_Status(pageInfo, "Reallocated sectors between FARM time series Frame", pReal->numReallocatedBetween, false, m_showStatusBits);
    reallo = check_for_signed_int(check_Status_Strip_Status(pReal->numCandidateSince), 8);
    set_json_int_Check_Status(pageInfo, "Reallocation candidate since the last FARM Time series Frame", pReal->numCandidateSince, false, m_showStatusBits);
    reallo = check_for_signed_int(check_Status_Strip_Status(static_cast<uint64_t>(pReal->numCandidateBetween)), 8);
    set_json_int_Check_Status(pageInfo, "Reallocation candidate between FARM time series Frame", pReal->numCandidateBetween, false, m_showStatusBits);

    json_push_back(LUNNReallocation, pageInfo);
    
    return eReturnValues::SUCCESS;
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
void CSCSI_Farm_Log::print_All_Pages(JSONNODE *masterData)
{
    
    if (vFarmFrame.size() > 0)
    {
        print_Header(masterData);
        JSONNODE* headPage;

        headPage = json_new(JSON_NODE);
        json_set_name(headPage, "Head Information From Farm Log");
        
        for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
        {

            for (uint32_t pramCode = 0; pramCode < vFarmFrame.at(index).vFramesFound.size(); pramCode++)
            {
                switch (static_cast<eSASLogPageTypes>(vFarmFrame.at(index).vFramesFound.at(pramCode)))
                {
                case eSASLogPageTypes::FARM_HEADER_PARAMETER:
                    //get the Farm Header information;
                    break;
                case  eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_PARAMETER:
                    print_Drive_Information(masterData, index);                     // get the id drive information at the time.
                    break;
                case  eSASLogPageTypes::WORKLOAD_STATISTICS_PARAMETER:
                    print_WorkLoad(masterData, index);                              // get the work load information
                    break;
                case eSASLogPageTypes::ERROR_STATISTICS_PARAMETER:
                    print_Error_Information(masterData, index);                    // get the error status
                    break;
                case eSASLogPageTypes::ENVIRONMENTAL_STATISTICS_PARAMETER:
                    print_Enviroment_Information(masterData, index);                // get the envirmonent information 
                    break;
                case eSASLogPageTypes::RELIABILITY_STATISTICS_PARAMETER:
                    print_Reli_Information(masterData, index);                      // get the Reliabliity stat
                    break;
                case eSASLogPageTypes::GENERAL_DRIVE_INFORMATION_06:
                    print_General_Drive_Information_Continued(masterData, index);
                    break;
                case eSASLogPageTypes::ENVIRONMENT_STATISTICS_PAMATER_07:
                    print_Enviroment_Statistics_Page_07(masterData, index);
                    break;
                case eSASLogPageTypes::WORKLOAD_STATISTICS_PAMATER_08:
                    print_Workload_Statistics_Page_08(masterData, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_4:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_5:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_6:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_7:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_8:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_9:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_STATISTICS_10:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_1:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_2:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_3:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_4:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_5:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_6:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_7:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_8:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_9:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_10:
                    break;
                case eSASLogPageTypes::MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::NUMBER_OF_TMD_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                case eSASLogPageTypes::VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                case eSASLogPageTypes::NUMBER_OF_VELOCITY_OBSERVER_OVER_LAST_3_SMART_SUMMARY_FRAMES_BY_HEAD:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_14:
                    break;
                case eSASLogPageTypes::CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                case eSASLogPageTypes::CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                case eSASLogPageTypes::NUMBER_OF_RESIDENT_GLIST_ENTRIES:
                case eSASLogPageTypes::NUMBER_OF_PENDING_ENTRIES:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_15:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_16:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_17:
                    break;
                case eSASLogPageTypes::WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_18:
                    break;
                case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
                case eSASLogPageTypes::CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_19:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_20:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_21:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_22:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_23:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_24:
                    break;
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
                case eSASLogPageTypes::CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
                case eSASLogPageTypes::CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_25:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_26:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_27:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_28:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_29:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_30:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_31:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_32:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_33:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_34:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_35:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_36:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_37:
                    break;
                case eSASLogPageTypes::SECOND_MR_HEAD_RESISTANCE:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);   
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_38:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_39:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_40:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_41:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_42:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_43:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_44:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_45:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_46:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_47:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_48:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_HEAD_49:
                    break;
                case eSASLogPageTypes::LUN_0_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_0_ACTUATOR));
                    break;
                case eSASLogPageTypes::LUN_0_FLASH_LED :
                    print_LUN_Actuator_FLED_Info(masterData,index, static_cast<uint16_t>(eSASLogPageTypes::LUN_0_FLASH_LED));
                    break;
                case eSASLogPageTypes::LUN_REALLOCATION_0:
                    print_LUN_Actuator_Reallocation(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_0));
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_42:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_43:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_44:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_45:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_46:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_47:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_48:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_49:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_50:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_51:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_52:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_53:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_54:
                    break;
                case eSASLogPageTypes::LUN_1_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_1_ACTUATOR));
                    break;
                case eSASLogPageTypes::LUN_1_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_1_FLASH_LED));
                    break;
                case eSASLogPageTypes::LUN_REALLOCATION_1:
                    print_LUN_Actuator_Reallocation(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_1));
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_62:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_63:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_64:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_65:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_66:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_67:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_68:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_69:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_70:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_71:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_72:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_73:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_74:
                    break;
                case eSASLogPageTypes::LUN_2_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_2_ACTUATOR));
                    break;
                case eSASLogPageTypes::LUN_2_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_2_FLASH_LED));
                    break;
                case eSASLogPageTypes::LUN_REALLOCATION_2:
                    print_LUN_Actuator_Reallocation(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_2));
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_82:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_83:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_84:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_85:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_86:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_87:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_88:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_89:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_90:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_91:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_92:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_93:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_94:
                    break;
                case eSASLogPageTypes::LUN_3_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_3_ACTUATOR));
                    break;
                case eSASLogPageTypes::LUN_3_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_3_FLASH_LED));
                    break;
                case eSASLogPageTypes::LUN_REALLOCATION_3:
                    print_LUN_Actuator_Reallocation(masterData, index, static_cast<uint16_t>(eSASLogPageTypes::LUN_REALLOCATION_3));
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_101:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_102:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_103:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_104:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_105:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_106:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_107:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_108:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_109:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_110:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_111:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_112:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_113:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_114:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_115:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_116:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_117:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_118:  //150
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_119:
                case eSASLogPageTypes::LIFETIME_TERABYTES_WRITTEN:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_120:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_121:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_122:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_123:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_124:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_125:
                case eSASLogPageTypes::UNIQUE_UNRECOVERABLES_SINCE:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::UNIQUE_UNRECOVERALBES_BETWEEN:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_128:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_129:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_130:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_131:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_132:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_133:
                case eSASLogPageTypes::RESERVED_FOR_FUTURE_EXPANSION_134:
                case eSASLogPageTypes::MAX_RESERVED_FOR_FUTURE_EXPANSION:
                    break;
                default:
                    // No head data. We don't want to go into the head data
                    //print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), masterData, index);
                    break;
                }
            }
        }
        // push all the head data to the masterData Node for Json
        json_push_back(masterData, headPage);
    }
    else
    {
        uint64_t signature = m_pHeader->farmHeader.signature & UINT64_C(0x00FFFFFFFFFFFFFF);

        if (signature != FARMSIGNATURE || signature == UINT64_C(0x00FFFFFFFFFFFFFF))
        {
            json_push_back(masterData, json_new_a("Empty FARM Log", "data has not yet been gathered"));

        }
    }
}

//-----------------------------------------------------------------------------
//
//! \fn print_Page_One_Node()
//
//! \brief
//!   Description:  print out a copy of a the pages, all under one Node
//
//  Entry:
//! \param masterData  =  master data to the json node
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CSCSI_Farm_Log::print_Page_One_Node(JSONNODE *masterData)
{
    if (vFarmFrame.size() > 0)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo,"FARM Log");
        print_All_Pages(pageInfo);
        json_push_back(masterData, pageInfo);
    }
    else
    {
        JSONNODE* pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "FARM Log");
        json_push_back(pageInfo, json_new_a("Empty FARM Log", "data has not yet been gathered"));
        json_push_back(masterData, pageInfo);
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
void CSCSI_Farm_Log::print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page)
{
    if (vFarmFrame.size() > 0)
    {
        if (page < vFarmFrame.size())
        {
            print_WorkLoad(masterData, page);
            print_Error_Information(masterData, page);
            print_Enviroment_Information(masterData, page);
            print_Reli_Information(masterData, page);

            JSONNODE* headInfoPage = json_new(JSON_NODE);
            std::ostringstream temp;
            temp << "Head Information From Farm Log copy: " << std::dec << page;

            json_set_name(headInfoPage, temp.str().c_str());

            for (uint32_t frame = 0; frame < vFarmFrame.at(page).vFramesFound.size(); frame++)
            {
                print_Head_Information(vFarmFrame.at(page).vFramesFound.at(frame), headInfoPage, page);
            }
            json_push_back(masterData, headInfoPage);

        }
    }
}
