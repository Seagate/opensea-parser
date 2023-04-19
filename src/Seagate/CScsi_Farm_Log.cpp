//
// CScsi_Farm_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
	: m_totalPages()                                       
	, m_logSize(0)                                             
	, m_pageSize(0)                                      
	, m_heads(0)                                   
	, m_MaxHeads(0)                                
	, m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf()
    , m_status(IN_PROGRESS)                                
	, m_logParam()   
	, m_pageParam()
    , m_pHeader()
    , m_pDriveInfo() 
	, m_alreadySet(false)          
	, m_showStatusBits(false)
    , m_fromScsiLogPages(false)
    , m_farmSubPage(0)
{                                                  
	m_status = IN_PROGRESS;

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

CSCSI_Farm_Log::CSCSI_Farm_Log(uint8_t* bufferData, size_t bufferSize, uint8_t subPage, bool fromLogPage)
    : m_totalPages()
    , m_logSize(static_cast<uint16_t>(bufferSize))
    , m_pageSize(0)
    , m_heads(0)
    , m_MaxHeads(0)
    , m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf(NULL)
    , m_status(IN_PROGRESS)
    , m_logParam()
    , m_pageParam()
    , m_pHeader()
    , m_pDriveInfo()
    , m_alreadySet(false)
    , m_showStatusBits(false)
    , m_fromScsiLogPages(fromLogPage)
    , m_farmSubPage(subPage)
{
    m_status = IN_PROGRESS;
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
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
    if (pBuf != NULL)
    {
        if (init_Header_Data() == SUCCESS)							// init the data for getting the log
        {
            m_status = parse_Farm_Log();
        }
        else
        {
            m_status = FAILURE;
        }
    }
    else
    {
        m_status = FAILURE;
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

CSCSI_Farm_Log::CSCSI_Farm_Log( uint8_t *bufferData, size_t bufferSize, uint8_t subPage, bool fromLogPage, bool showStatus)
	: m_totalPages()                                       
	, m_logSize(static_cast<uint16_t>(bufferSize))
	, m_pageSize(0)                                      
	, m_heads(0)                                   
	, m_MaxHeads(0)                                
	, m_copies(0)
    , m_MajorRev(0)
    , m_MinorRev(0)
    , pBuf(NULL)
    , m_status(IN_PROGRESS)                                
	, m_logParam()
	, m_pageParam()
    , m_pHeader()
    , m_pDriveInfo() 
	, m_alreadySet(false)          
	, m_showStatusBits(showStatus)
    , m_fromScsiLogPages(fromLogPage)
    , m_farmSubPage(subPage)
{
    m_status = IN_PROGRESS;
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
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
    if (pBuf != NULL)
    {
        if (init_Header_Data() == SUCCESS)							// init the data for getting the log
        {
            m_status = parse_Farm_Log();
        }
        else
        {
            m_status = FAILURE;
        }
    }
    else
    {
        m_status = FAILURE;
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
//!   \return eReturnValues - MEMORY_FAILURE - if the buffer is NULL
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::init_Header_Data()
{
	if (pBuf == NULL)
	{
		return MEMORY_FAILURE;
	}
	else
	{
        
        if (m_fromScsiLogPages == true)
        {
            m_pHeader->pPageHeader = *reinterpret_cast<sLogParams*>(&pBuf[0]);
            swap_Bytes_sFarmHeader(m_pHeader, &pBuf[4]);											// swap all the bytes for the header
        }
        else
        {
            m_logParam = reinterpret_cast<sLogPageStruct*>(&pBuf[0]);
            m_logSize = m_logParam->pageLength;									    // set the class log size 
            byte_Swap_16(&m_logSize);
            
            m_pHeader->pPageHeader = *reinterpret_cast<sLogParams*>(&pBuf[4]);
            swap_Bytes_sFarmHeader(m_pHeader, &pBuf[8]);											// swap all the bytes for the header
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
		m_copies = M_DoubleWord0(m_pHeader->farmHeader.copies);						// finish up with the number of copies (not supported "YET" in SAS)
	}
	return SUCCESS;
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
    case FARM_HEADER_PARAMETER:
    case GENERAL_DRIVE_INFORMATION_PARAMETER:
    case WORKLOAD_STATISTICS_PARAMETER:
    case ERROR_STATISTICS_PARAMETER:
    case ENVIRONMENTAL_STATISTICS_PARAMETER:
    case RELIABILITY_STATISTICS_PARAMETER:
    case GENERAL_DRIVE_INFORMATION_06:
    case ENVIRONMENT_STATISTICS_PAMATER_07:
    case WORKLOAD_STATISTICS_PAMATER_08:
    case RESERVED_FOR_FUTURE_STATISTICS_4:
    case RESERVED_FOR_FUTURE_STATISTICS_5:
    case RESERVED_FOR_FUTURE_STATISTICS_6:
    case RESERVED_FOR_FUTURE_STATISTICS_7:
    case RESERVED_FOR_FUTURE_STATISTICS_8:
    case RESERVED_FOR_FUTURE_STATISTICS_9:
    case RESERVED_FOR_FUTURE_STATISTICS_10:
        break;
    case RESERVED_FOR_FUTURE_HEAD_1:
    case RESERVED_FOR_FUTURE_HEAD_2:
    case RESERVED_FOR_FUTURE_HEAD_3:
    case RESERVED_FOR_FUTURE_HEAD_4:
    case RESERVED_FOR_FUTURE_HEAD_5:
    case RESERVED_FOR_FUTURE_HEAD_6:
    case RESERVED_FOR_FUTURE_HEAD_7:
    case RESERVED_FOR_FUTURE_HEAD_8:
    case RESERVED_FOR_FUTURE_HEAD_9:
    case RESERVED_FOR_FUTURE_HEAD_10:
        break;
    case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
        headerName = "MR Head Resistance from most recent SMART Summary Frame";
        break;
    case RESERVED_FOR_FUTURE_HEAD_11:
    case RESERVED_FOR_FUTURE_HEAD_12:
    case RESERVED_FOR_FUTURE_HEAD_13:
    case RESERVED_FOR_FUTURE_HEAD_14:
        break;
    case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT amplitude, averaged across Test Zone";
        break;
    case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
        headerName = "Current H2SAT asymmetry, averaged across Test Zone";
        break;
    case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
        headerName = "Resident Glist Entries";
        break;
    case NUMBER_OF_PENDING_ENTRIES:
        headerName = "Resident Pending list Entries";
        break;
    case RESERVED_FOR_FUTURE_HEAD_15:
    case RESERVED_FOR_FUTURE_HEAD_16:
    case RESERVED_FOR_FUTURE_HEAD_17:
        break;
    case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
        headerName = "Write Power On Hours from most recent SMART";
        break;
    case RESERVED_FOR_FUTURE_HEAD_18:
		break;
    case CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
        headerName = "Cumlative Lifetime Unrecoverable Read Repeat by head";
        break;
    case CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
        headerName = "Cumlative Lifetime Unrecoverable Read Unique by head";
        break;
    case RESERVED_FOR_FUTURE_HEAD_19:
    case RESERVED_FOR_FUTURE_HEAD_20:
    case RESERVED_FOR_FUTURE_HEAD_21:
    case RESERVED_FOR_FUTURE_HEAD_22:
    case RESERVED_FOR_FUTURE_HEAD_23:
    case RESERVED_FOR_FUTURE_HEAD_24:
        headerName = "Reserved Head information";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 0";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 1";
        break;
    case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT trimmed mean bits in error Test Zone 2";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
        headerName = "Current H2SAT iterations to converge Test Zone 0";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
        headerName = "Current H2SAT iterations to converge Test Zone 1";
        break;
    case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
        headerName = "Current H2SAT iterations to converge Test Zone 2";
        break;
    case RESERVED_FOR_FUTURE_HEAD_25:
    case RESERVED_FOR_FUTURE_HEAD_26:
    case RESERVED_FOR_FUTURE_HEAD_27:
    case RESERVED_FOR_FUTURE_HEAD_28:
    case RESERVED_FOR_FUTURE_HEAD_29:
    case RESERVED_FOR_FUTURE_HEAD_30:
    case RESERVED_FOR_FUTURE_HEAD_31:
    case RESERVED_FOR_FUTURE_HEAD_32:
    case RESERVED_FOR_FUTURE_HEAD_33:
    case RESERVED_FOR_FUTURE_HEAD_34:
    case RESERVED_FOR_FUTURE_HEAD_35:
    case RESERVED_FOR_FUTURE_HEAD_36:
    case RESERVED_FOR_FUTURE_HEAD_37:
        headerName = "Reserved Head information";
        break;
    case SECOND_MR_HEAD_RESISTANCE:
        headerName = "Second Head MR Head Resistance";
        break;
    case RESERVED_FOR_FUTURE_HEAD_38:
    case RESERVED_FOR_FUTURE_HEAD_39:
    case RESERVED_FOR_FUTURE_HEAD_40:
    case RESERVED_FOR_FUTURE_HEAD_41:
    case RESERVED_FOR_FUTURE_HEAD_42:
    case RESERVED_FOR_FUTURE_HEAD_43:
    case RESERVED_FOR_FUTURE_HEAD_44:
    case RESERVED_FOR_FUTURE_HEAD_45:
    case RESERVED_FOR_FUTURE_HEAD_46:
    case RESERVED_FOR_FUTURE_HEAD_47:
    case RESERVED_FOR_FUTURE_HEAD_48:
    case RESERVED_FOR_FUTURE_HEAD_49:
        headerName = "Reserved Head information";
        break;
    case LUN_0_ACTUATOR:
    case LUN_0_FLASH_LED:
    case LUN_REALLOCATION_0:
    case RESERVED_FOR_FUTURE_EXPANSION_42:
    case RESERVED_FOR_FUTURE_EXPANSION_43:
    case RESERVED_FOR_FUTURE_EXPANSION_44:
    case RESERVED_FOR_FUTURE_EXPANSION_45:
    case RESERVED_FOR_FUTURE_EXPANSION_46:
    case RESERVED_FOR_FUTURE_EXPANSION_47:
    case RESERVED_FOR_FUTURE_EXPANSION_48:
    case RESERVED_FOR_FUTURE_EXPANSION_49:
    case RESERVED_FOR_FUTURE_EXPANSION_50:
    case RESERVED_FOR_FUTURE_EXPANSION_51:
    case RESERVED_FOR_FUTURE_EXPANSION_52:
    case RESERVED_FOR_FUTURE_EXPANSION_53:
    case RESERVED_FOR_FUTURE_EXPANSION_54:
    case LUN_1_ACTUATOR:
    case LUN_1_FLASH_LED:
    case LUN_REALLOCATION_1:
    case RESERVED_FOR_FUTURE_EXPANSION_61:
    case RESERVED_FOR_FUTURE_EXPANSION_62:
    case RESERVED_FOR_FUTURE_EXPANSION_63:
    case RESERVED_FOR_FUTURE_EXPANSION_64:
    case RESERVED_FOR_FUTURE_EXPANSION_65:
    case RESERVED_FOR_FUTURE_EXPANSION_66:
    case RESERVED_FOR_FUTURE_EXPANSION_67:
    case RESERVED_FOR_FUTURE_EXPANSION_68:
    case RESERVED_FOR_FUTURE_EXPANSION_69:
    case RESERVED_FOR_FUTURE_EXPANSION_70:
    case RESERVED_FOR_FUTURE_EXPANSION_71:
    case RESERVED_FOR_FUTURE_EXPANSION_72:
    case RESERVED_FOR_FUTURE_EXPANSION_73:
    case RESERVED_FOR_FUTURE_EXPANSION_74:
    case LUN_2_ACTUATOR:
    case LUN_2_FLASH_LED:
    case LUN_REALLOCATION_2:
    case RESERVED_FOR_FUTURE_EXPANSION_81:
    case RESERVED_FOR_FUTURE_EXPANSION_82:
    case RESERVED_FOR_FUTURE_EXPANSION_83:
    case RESERVED_FOR_FUTURE_EXPANSION_84:
    case RESERVED_FOR_FUTURE_EXPANSION_85:
    case RESERVED_FOR_FUTURE_EXPANSION_86:
    case RESERVED_FOR_FUTURE_EXPANSION_87:
    case RESERVED_FOR_FUTURE_EXPANSION_88:
    case RESERVED_FOR_FUTURE_EXPANSION_89:
    case RESERVED_FOR_FUTURE_EXPANSION_90:
    case RESERVED_FOR_FUTURE_EXPANSION_91:
    case RESERVED_FOR_FUTURE_EXPANSION_92:
    case RESERVED_FOR_FUTURE_EXPANSION_93:
    case RESERVED_FOR_FUTURE_EXPANSION_94:
    case LUN_3_ACTUATOR:
    case LUN_3_FLASH_LED:
    case LUN_REALLOCATION_3:
    case RESERVED_FOR_FUTURE_EXPANSION_101:
    case RESERVED_FOR_FUTURE_EXPANSION_102:
    case RESERVED_FOR_FUTURE_EXPANSION_103:
    case RESERVED_FOR_FUTURE_EXPANSION_104:
    case RESERVED_FOR_FUTURE_EXPANSION_105:
    case RESERVED_FOR_FUTURE_EXPANSION_106:
    case RESERVED_FOR_FUTURE_EXPANSION_107:
    case RESERVED_FOR_FUTURE_EXPANSION_108:
    case RESERVED_FOR_FUTURE_EXPANSION_109:
    case RESERVED_FOR_FUTURE_EXPANSION_110:
    case RESERVED_FOR_FUTURE_EXPANSION_111:
    case RESERVED_FOR_FUTURE_EXPANSION_112:
    case RESERVED_FOR_FUTURE_EXPANSION_113:
    case RESERVED_FOR_FUTURE_EXPANSION_114:
    case RESERVED_FOR_FUTURE_EXPANSION:
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
    offset += SIZEPARAM;  //reserved
    offset += SIZEPARAM;  //reserved1
    offset += SIZEPARAM;  //reserved2
    di->poh = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    offset += SIZEPARAM;  //reserved3
    offset += SIZEPARAM;  //reserved4
    di->headLoadEvents = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->powerCycleCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    di->resetCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
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
bool CSCSI_Farm_Log::Get_sDrive_Info_Page_06(sGeneralDriveInfoPage06 *gd, uint64_t offset)
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
    gd->productID[0] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->productID[1] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->productID[2] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    gd->productID[3] = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
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
    //offset += SIZEPARAM;
    
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

    if (m_MajorRev < 4)
    {
        es->errorStat.pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        es->errorStat.pPageHeader.paramControlByte = pBuf[offset];
        offset++;
        es->errorStat.pPageHeader.paramLength = pBuf[offset];
        offset++;
        es->errorStat.pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.totalReadECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.totalWriteECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.totalReallocations = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 2); //one reserved
        es->errorStat.totalMechanicalFails = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.totalReallocatedCanidates = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 6); // 5 reseved
        es->errorStat.attrIOEDCErrors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 4); //3 reseved
        es->errorStat.totalFlashLED = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 3);
        es->errorStat.FRUCode = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorStat.parity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;

    }
    else
    {
        es->errorV4.pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        es->errorV4.pPageHeader.paramControlByte = pBuf[offset];
        offset++;
        es->errorV4.pPageHeader.paramLength = pBuf[offset];
        offset++;
        es->errorV4.pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.totalReadECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.totalWriteECC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 3); // 2 reserved
        es->errorV4.totalMechanicalFails = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 7);  // 6 reserved
        es->errorV4.attrIOEDCErrors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 7); // 6 reserved
        es->errorV4.FRUCode = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portAInvalidDwordCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portBInvalidDwordCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portADisparityErrorCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portBDisparityErrorCount = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portALossDwordSync = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portBLossDwordSync = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portAPhyResetProblem = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        es->errorV4.portBPhyResetProblem = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;

    }
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
    offset += (SIZEPARAM*9);
    es->maxTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->minTemp = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += (SIZEPARAM*3);
    es->humidity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    es->humidityRatio = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
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
    //offset += SIZEPARAM;

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
    if (m_MajorRev < 4)
    {

        ss->reli.pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        ss->reli.pPageHeader.paramControlByte = pBuf[offset];
        offset++;
        ss->reli.pPageHeader.paramLength = pBuf[offset];
        offset++;
        ss->reli.pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.lastIDDTest = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.cmdLastIDDTest = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.gListReclamed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.servoStatus = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.altsBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.altsAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.gListBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.gListAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.scrubsBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.scrubsAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.numberDOSScans = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.numberLBACorrect = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.numberValidParitySec = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.numberRAWops = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 8); //6 reserved
        ss->reli.microActuatorLockOut = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.diskSlipRecalPerformed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.heliumPressuretThreshold = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.rvAbsoluteMean = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.maxRVAbsoluteMean = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli.idleTime = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;

    }
    else
    {
        ss->reli4.pPageHeader.paramCode = M_BytesTo2ByteValue(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        ss->reli4.pPageHeader.paramControlByte = pBuf[offset];
        offset++;
        ss->reli4.pPageHeader.paramLength = pBuf[offset];
        offset++;
        ss->reli4.pageNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli4.copyNumber = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 14);
        ss->reli4.numberRAWops = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli4.cumECCDueToERC = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += (SIZEPARAM * 7);
        ss->reli4.microActuatorLockOut = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli4.diskSlipRecalPerformed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        offset += SIZEPARAM;
        ss->reli4.heliumPressuretThreshold = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
        //offset += SIZEPARAM;

    }
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
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CSCSI_Farm_Log::swap_Bytes_sFarmHeader(sScsiFarmHeader *fh, uint8_t* pData)
{
    fh->farmHeader.signature = M_BytesTo8ByteValue(pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7] );
    fh->farmHeader.majorRev = M_BytesTo8ByteValue(pData[8], pData[9], pData[10], pData[11], pData[12], pData[13], pData[14], pData[15]);
    fh->farmHeader.minorRev = M_BytesTo8ByteValue(pData[16], pData[17], pData[18], pData[19], pData[20], pData[21], pData[22], pData[23]);
    fh->farmHeader.pagesSupported = M_BytesTo8ByteValue(pData[24], pData[25], pData[26], pData[27], pData[28], pData[29], pData[30], pData[31]);
    fh->farmHeader.logSize  = M_BytesTo8ByteValue(pData[32], pData[33], pData[34], pData[35], pData[36], pData[37], pData[38], pData[39]);
    fh->farmHeader.pageSize = M_BytesTo8ByteValue(pData[40], pData[41], pData[42], pData[43], pData[44], pData[45], pData[46], pData[47]);
    fh->farmHeader.headsSupported  = M_BytesTo8ByteValue(pData[48], pData[49], pData[50], pData[51], pData[52], pData[53], pData[54], pData[55]);
    fh->farmHeader.copies = M_BytesTo8ByteValue(pData[56], pData[57], pData[58], pData[59], pData[60], pData[61], pData[62], pData[63]);
    fh->farmHeader.reasonForFrameCapture = M_BytesTo8ByteValue(pData[64], pData[65], pData[66], pData[67], pData[68], pData[69], pData[70], pData[71]);

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
    LUN->reallocatedSectors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->reallocatedCandidates = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->timeStampOfIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->subCmdOfIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->reclamedGlist = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->servoStatus = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->slippedSectorsBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->slippedSectorsAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->residentReallocatedBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->residentReallocatedAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->successScrubbedBeforeIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->successScrubbedAfterIDD = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->dosScansPerformed = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->correctedLBAbyISP = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->paritySectors = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->RVabsolue = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->maxRVabsolue = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->idleTime = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->lbasCorrectedByParity = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->currentLowFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->currentMidFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->currentHighFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->worstLowFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->worstMidFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->worstHighFrequencyVibe = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->primarySPCovPercentage = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    offset += SIZEPARAM;
    LUN->primarySPCovPercentageSMR = M_BytesTo8ByteValue(pBuf[offset], pBuf[offset + 1], pBuf[offset + 2], pBuf[offset + 3], pBuf[offset + 4], pBuf[offset + 5], pBuf[offset + 6], pBuf[offset + 7]);
    //offset += SIZEPARAM;

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
    //for both 32 bit and 64 bit Linux
    //.. / .. / src//Seagate/CScsi_Farm_Log.cpp: In member function ‘bool opensea_parser::CSCSI_Farm_Log::get_Head_Info(opensea_parser::sHeadInformation*, uint8_t*)’:
    //    .. / .. / src//Seagate/CScsi_Farm_Log.cpp:1293:11: warning: ‘void* memcpy(void*, const void*, size_t)’ copying an object of non-trivial type ‘opensea_parser::sLogParams’ {aka ‘struct opensea_parser::_sLogPageParamStruct’} from an array of ‘opensea_parser::sLogPageStruct’ {aka ‘struct opensea_parser::_sLogPageStruct’} [-Wclass-memaccess]
    //    1293 | memcpy(&phead->pageHeader, reinterpret_cast<sLogPageStruct *>(&buffer[0]), sizeof(sLogPageStruct));
    //| ~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
//!   \return SUCCESS or FAILURE
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

    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("SCSI parse FARM Log\n");
    }

    if (pBuf == NULL)
    {
        return FAILURE;
    }
    uint64_t signature = m_pHeader->farmHeader.signature & UINT64_C(0x00FFFFFFFFFFFFFF);
    m_MajorRev = M_DoubleWord0(m_pHeader->farmHeader.majorRev);
    m_MinorRev = M_DoubleWord0(m_pHeader->farmHeader.minorRev);
    if ((signature != FARMSIGNATURE && signature != FACTORYCOPY) || signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
    {
        if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
            return SUCCESS;
        else
            return VALIDATION_FAILURE;
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
                switch (m_pageParam->paramCode)
                {
                    
                case FARM_HEADER_PARAMETER:
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
 
                case  GENERAL_DRIVE_INFORMATION_PARAMETER:
                    {
                        Get_sDriveInfo(&pFarmFrame->driveInfo, offset);       // get the id drive information at the time.
                        create_Serial_Number(pFarmFrame->identStringInfo.serialNumber, M_DoubleWord0(pFarmFrame->driveInfo.serialNumber), M_DoubleWord0(pFarmFrame->driveInfo.serialNumber2), m_MajorRev, true);
                        create_World_Wide_Name(pFarmFrame->identStringInfo.worldWideName, pFarmFrame->driveInfo.worldWideName, pFarmFrame->driveInfo.worldWideName2, true);
                        create_Firmware_String(pFarmFrame->identStringInfo.firmwareRev, M_DoubleWord0(pFarmFrame->driveInfo.firmware), M_DoubleWord0(pFarmFrame->driveInfo.firmwareRev), true);
                        create_Device_Interface_String(pFarmFrame->identStringInfo.deviceInterface, M_DoubleWord0(pFarmFrame->driveInfo.deviceInterface), true);

                        offset += (m_pageParam->paramLength + sizeof(sLogParams));
                    }
                    break;
                   
                case  WORKLOAD_STATISTICS_PARAMETER:
                    {                    
                        Get_sWorkLoadStat(&pFarmFrame->workLoadPage,offset);
                        offset += (pFarmFrame->workLoadPage.PageHeader.paramLength + sizeof(sLogParams));
                    }
                    break;
                   
                case ERROR_STATISTICS_PARAMETER:   
                {
                    if (m_MajorRev < 4)
                    {
                        Get_sErrorStat(&pFarmFrame->errorPage,offset);
                        offset += (pFarmFrame->errorPage.errorStat.pPageHeader.paramLength + sizeof(sLogParams));
                    }
                    else
                    {
                        Get_sErrorStat(&pFarmFrame->errorPage,offset);
                        offset += (pFarmFrame->errorPage.errorV4.pPageHeader.paramLength + sizeof(sLogParams));
                    }
                }
                break;
                    
                case ENVIRONMENTAL_STATISTICS_PARAMETER:     
                {
                    Get_sEnvironmentStat(&pFarmFrame->environmentPage,offset);
                    offset += (pFarmFrame->environmentPage.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                   
                case RELIABILITY_STATISTICS_PARAMETER:    
                {
                    if (m_MajorRev < 4)
                    {
                        Get_sScsiReliabilityStat(&pFarmFrame->reliPage,offset);
                        offset += (pFarmFrame->reliPage.reli.pPageHeader.paramLength + sizeof(sLogParams));
                    }
                    else
                    {
                        Get_sScsiReliabilityStat(&pFarmFrame->reliPage, offset);
                        offset += (pFarmFrame->reliPage.reli4.pPageHeader.paramLength + sizeof(sLogParams));
                    }
                }
                break; 
                case  GENERAL_DRIVE_INFORMATION_06:
                {
                    Get_sDrive_Info_Page_06(&pFarmFrame->gDPage06,offset);
                    create_Model_Number_String(pFarmFrame->identStringInfo.modelNumber, pFarmFrame->gDPage06.productID,true);
                    offset += (pFarmFrame->gDPage06.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  ENVIRONMENT_STATISTICS_PAMATER_07:
                {
                    Get_EnvironmentPage07(&pFarmFrame->envStatPage07,offset);
                    offset += (pFarmFrame->envStatPage07.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  WORKLOAD_STATISTICS_PAMATER_08:
                {
                    Get_WorkloadPage08(&pFarmFrame->workloadStatPage08,offset);
                    offset += (pFarmFrame->workloadStatPage08.pPageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case  RESERVED_FOR_FUTURE_STATISTICS_4:
                case  RESERVED_FOR_FUTURE_STATISTICS_5:
                case  RESERVED_FOR_FUTURE_STATISTICS_6:
                case  RESERVED_FOR_FUTURE_STATISTICS_7:
                case  RESERVED_FOR_FUTURE_STATISTICS_8:
                case  RESERVED_FOR_FUTURE_STATISTICS_9:
                case  RESERVED_FOR_FUTURE_STATISTICS_10:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->discSlipPerHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->mrHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAmplituedByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STAsymmetryByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case NUMBER_OF_RESIDENT_GLIST_ENTRIES:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentGlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case     NUMBER_OF_PENDING_ENTRIES:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->ResidentPlistEntries, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->writePowerOnHours, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->cumECCReadRepeat, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->cumECCReadUnique, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:    
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STTrimmedbyHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone0, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:  
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone1, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:     
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation(); 
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->currentH2STIterationsByHeadZone2, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;  
                }
                break;
                case SECOND_MR_HEAD_RESISTANCE:
                {
                    sHeadInformation *pHeadInfo = new sHeadInformation();
                    get_Head_Info(pHeadInfo, &pBuf[offset]);
                    memcpy(&pFarmFrame->secondMRHeadResistanceByHead, pHeadInfo, sizeof(*pHeadInfo));
                    offset += (pHeadInfo->pageHeader.paramLength + sizeof(sLogParams));
                    delete pHeadInfo;
                }
                break;
                case LUN_0_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN50,offset);
                    offset += (pFarmFrame->vLUN50.pageHeader.paramLength + sizeof(sLogParams));

                }
                break;
                case LUN_0_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled51, offset);
                    offset += (pFarmFrame->fled51.pageHeader.paramLength + sizeof(sLogParams));

                }
                break;
                case LUN_REALLOCATION_0:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall52, offset);
                    offset += (pFarmFrame->reall52.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_1_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN60,offset);
                    offset += (pFarmFrame->vLUN60.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_1_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled61,offset);
                    offset += (pFarmFrame->fled61.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_REALLOCATION_1:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall62, offset);
                    offset += (pFarmFrame->reall62.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_2_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN70,offset);
                    offset += (pFarmFrame->vLUN70.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_2_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled71,offset);
                    offset += (pFarmFrame->fled71.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_REALLOCATION_2:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall72, offset);
                    offset += (pFarmFrame->reall72.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_3_ACTUATOR:
                {
                    Get_sLUNStruct(&pFarmFrame->vLUN80,offset);
                    offset += (pFarmFrame->vLUN80.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_3_FLASH_LED:
                {
                    Get_Flash_LED(&pFarmFrame->fled81,offset);
                    offset += (pFarmFrame->fled81.pageHeader.paramLength + sizeof(sLogParams));
                }
                break;
                case LUN_REALLOCATION_3:
                {
                    Get_Reallocation_Data(&pFarmFrame->reall82, offset);
                    offset += (pFarmFrame->reall82.pageHeader.paramLength + sizeof(sLogParams));
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
        return SUCCESS;
    }
    delete pFarmFrame;
    return FAILURE;
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
eReturnValues CSCSI_Farm_Log::print_Header(JSONNODE *masterData)
{
	uint32_t page = 0;
#if defined _DEBUG
    printf("\tLog Signature:                      0x%" PRIX64" \n", vFarmFrame[page].farmHeader.farmHeader.signature );                                  //!< Log Signature = 0x00004641524D4552
    printf("\tMajor Revision:                      %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.majorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< Log Major rev
    printf("\tMinor Revision:                      %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.minorRev & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< minor rev 
    printf("\tPages Supported:                     %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.pagesSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                             //!< number of pages supported
    printf("\tLog Size:                            %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.logSize & UINT64_C(0x00FFFFFFFFFFFFFF));                                    //!< log size in bytes
    if (m_MajorRev < MAJORVERSION4)
    {
        printf("\tPage Size:                           %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.pageSize & UINT64_C(0x00FFFFFFFFFFFFFF));                                   //!< page size in bytes
    }
    printf("\tHeads Supported:                     %" PRIu64"  \n", vFarmFrame[page].farmHeader.farmHeader.headsSupported & UINT64_C(0x00FFFFFFFFFFFFFF));                             //!< Maximum Drive Heads Supported
    printf("\tReason for Frame Capture(debug):     %" PRId64"  \n", vFarmFrame[page].farmHeader.farmHeader.reasonForFrameCapture & 0x00FFFFFFFFFFFFF);	      //!< Reason for Frame Capture
#endif

    JSONNODE* pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "FARM Log Header");
    std::ostringstream temp;
    temp << "0x" <<std::hex << std::uppercase << check_Status_Strip_Status(vFarmFrame[page].farmHeader.farmHeader.signature);
    json_push_back(pageInfo, json_new_a("Log Signature", temp.str().c_str()));
    set_json_64_bit_With_Status(pageInfo, "Major Revision", vFarmFrame[page].farmHeader.farmHeader.majorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Minor Revision", vFarmFrame[page].farmHeader.farmHeader.minorRev, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Pages Supported", vFarmFrame[page].farmHeader.farmHeader.pagesSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Log Size", vFarmFrame[page].farmHeader.farmHeader.logSize, false, m_showStatusBits);
    if (m_MajorRev < MAJORVERSION4)
    {
        set_json_64_bit_With_Status(pageInfo, "Page Size", vFarmFrame[page].farmHeader.farmHeader.pageSize, false, m_showStatusBits);
    }
    set_json_64_bit_With_Status(pageInfo, "Heads Supported", vFarmFrame[page].farmHeader.farmHeader.headsSupported, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Reason for Frame Capture", vFarmFrame[page].farmHeader.farmHeader.reasonForFrameCapture, false, m_showStatusBits);
    std::string meaning;
    Get_FARM_Reason_For_Capture(&meaning, M_Byte0(vFarmFrame[page].farmHeader.farmHeader.reasonForFrameCapture));

    json_push_back(pageInfo, json_new_a("Reason meaning", meaning.c_str()));
    json_push_back(masterData, pageInfo);

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

eReturnValues CSCSI_Farm_Log::print_Drive_Information(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        printf("\nDrive Information From Farm Log copy FACTORY\n");
    }
    else
    {
        printf("\nDrive Information From Farm Log copy %d:\n", page);
    }
    printf("\tDevice Interface:                         %s         \n", vFarmFrame[page].identStringInfo.deviceInterface.c_str());
    printf("\tDevice Capcaity in sectors:               %" PRId64" \n", vFarmFrame[page].driveInfo.deviceCapacity & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tPhysical Sector size:                     %" PRIX64" \n", vFarmFrame[page].driveInfo.psecSize & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Physical Sector Size in Bytes
    printf("\tLogical Sector Size:                      %" PRIX64" \n", vFarmFrame[page].driveInfo.lsecSize & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Logical Sector Size in Bytes
    printf("\tDevice Buffer Size:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.deviceBufferSize & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< Device Buffer Size in Bytes
    printf("\tNumber of heads:                          %" PRId64" \n", vFarmFrame[page].driveInfo.heads & UINT64_C(0x00FFFFFFFFFFFFFF));										//!< Number of Heads
    printf("\tDevice form factor:                       %" PRIX64" \n", vFarmFrame[page].driveInfo.factor & UINT64_C(0x00FFFFFFFFFFFFFF));										//!< Device Form Factor (ID Word 168)
    printf("\tserial number:                            %s         \n", vFarmFrame[page].identStringInfo.serialNumber.c_str());
    printf("\tworkd wide name:                          %s         \n", vFarmFrame[page].identStringInfo.worldWideName.c_str());
    printf("\tfirmware Rev:                             %s         \n", vFarmFrame[page].identStringInfo.firmwareRev.c_str());											//!< Firmware Revision [0:3]
    printf("\tRotation Rate:                            %" PRIu64" \n", vFarmFrame[page].driveInfo.rotationRate & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Rotational Rate of Device 
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< reserved
    printf("\tPower on Hours:                           %" PRIu64" \n", vFarmFrame[page].driveInfo.poh & UINT64_C(0x00FFFFFFFFFFFFFF));											//!< Power-on Hour
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< reserved
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< reserved
    if (m_MajorRev < MAJORVERSION4)
    {
        printf("\tHead Load Events:                         %" PRIu64" \n", vFarmFrame[page].driveInfo.headLoadEvents & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Head Load Events
    }
    printf("\tPower Cycle count:                        %" PRIu64" \n", vFarmFrame[page].driveInfo.powerCycleCount & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Power Cycle Count
    printf("\tHardware Reset count:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.resetCount & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< Hardware Reset Count
    printf("\treserved:                                 %" PRIu64" \n", vFarmFrame[page].driveInfo.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));									//!< treserved
    printf("\tNVC Status @ power on:                    %" PRIu64" \n", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< NVC Status on Power-on
    printf("\tTime Available to save:                   %" PRIu64" \n", vFarmFrame[page].driveInfo.timeAvailable & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    printf("\tTime of latest frame:                     %" PRIu64" \n", vFarmFrame[page].driveInfo.firstTimeStamp & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Timestamp of first SMART Summary Frame in Power-On Hours microseconds (spec is wrong)
    printf("\tTime of latest frame (milliseconds):      %" PRIu64" \n", vFarmFrame[page].driveInfo.lastTimeStamp & UINT64_C(0x00FFFFFFFFFFFFFF));								//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds1

#endif
    
    std::ostringstream temp;
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::string header;
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        header.assign("Drive Information From Farm Log copy FACTORY");
    }
    else
    {
        temp.str("");temp.clear();
        temp << "Drive Information From Farm Log copy " << std::dec << page;
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());
        
    json_push_back(pageInfo, json_new_a("Serial Number", vFarmFrame[page].identStringInfo.serialNumber.c_str()));
    json_push_back(pageInfo, json_new_a("World Wide Name", vFarmFrame[page].identStringInfo.worldWideName.c_str()));
    json_push_back(pageInfo, json_new_a("Firmware Rev", vFarmFrame[page].identStringInfo.firmwareRev.c_str()));

    if (vFarmFrame[page].identStringInfo.modelNumber == "")
    {
        vFarmFrame[page].identStringInfo.modelNumber = "ST12345678";
    }
    json_push_back(pageInfo, json_new_a("Model Number", vFarmFrame[page].identStringInfo.modelNumber.c_str()));

    json_push_back(pageInfo, json_new_a("Device Interface", vFarmFrame[page].identStringInfo.deviceInterface.c_str()));
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].driveInfo.deviceCapacity & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Device Capacity in Sectors", temp.str().c_str(), vFarmFrame[page].driveInfo.deviceCapacity, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Physical Sector size", vFarmFrame[page].driveInfo.psecSize, false, m_showStatusBits);									//!< Physical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Logical Sector Size", vFarmFrame[page].driveInfo.lsecSize, false, m_showStatusBits);										//!< Logical Sector Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Device Buffer Size", vFarmFrame[page].driveInfo.deviceBufferSize, false, m_showStatusBits);								//!< Device Buffer Size in Bytes
    set_json_64_bit_With_Status(pageInfo, "Number of heads", vFarmFrame[page].driveInfo.heads, false, m_showStatusBits);											//!< Number of Heads
    if (check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads) != 0)
    {
        m_heads = static_cast<uint64_t>(check_Status_Strip_Status(vFarmFrame[page].driveInfo.heads));
    }
    set_json_64_bit_With_Status(pageInfo, "Device form factor", vFarmFrame[page].driveInfo.factor, false, m_showStatusBits);										//!< Device Form Factor (ID Word 168)

    set_json_64_bit_With_Status(pageInfo, "Rotation Rate", vFarmFrame[page].driveInfo.rotationRate, false, m_showStatusBits);										//!< Rotational Rate of Device (ID Word 217)
    set_json_64_bit_With_Status(pageInfo, "Power on Hour", vFarmFrame[page].driveInfo.poh, false, m_showStatusBits);                                                //!< Power-on Hour
    if (m_MajorRev < MAJORVERSION4)
    {
        set_json_64_bit_With_Status(pageInfo, "Head Load Events", vFarmFrame[page].driveInfo.headLoadEvents, false, m_showStatusBits);									//!< Head Load Events
    }
    set_json_64_bit_With_Status(pageInfo, "Power Cycle count", vFarmFrame[page].driveInfo.powerCycleCount, false, m_showStatusBits);								//!< Power Cycle Count
    set_json_64_bit_With_Status(pageInfo, "Hardware Reset count", vFarmFrame[page].driveInfo.resetCount, false, m_showStatusBits);									//!< Hardware Reset Count
    set_json_64_bit_With_Status(pageInfo, "NVC Status @ power on", vFarmFrame[page].driveInfo.NVC_StatusATPowerOn, false, m_showStatusBits);						//!< NVC Status on Power-on

    //Get_NVC_Status(pageInfo, vFarmFrame[page].driveInfo.NVC_StatusATPowerOn);


    set_json_64_bit_With_Status(pageInfo, "NVC Time Available to save (in 100us)", vFarmFrame[page].driveInfo.timeAvailable, false, m_showStatusBits);					//!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    set_json_64_bit_With_Status(pageInfo, "Timestamp of First SMART Summary Frame (ms)", vFarmFrame[page].driveInfo.firstTimeStamp, false, m_showStatusBits);		//!< Timestamp of first SMART Summary Frame in Power-On Hours Milliseconds
    set_json_64_bit_With_Status(pageInfo, "TimeStamp of Last SMART Summary Frame (ms)", vFarmFrame[page].driveInfo.lastTimeStamp, false, m_showStatusBits);			//!< Timestamp of latest SMART Summary Frame in Power-On Hours Milliseconds

    if (check_For_Active_Status(&vFarmFrame[page].driveInfo.dateOfAssembly) || \
        (vFarmFrame[page].driveInfo.dateOfAssembly < 0x40000000 && vFarmFrame[page].driveInfo.dateOfAssembly > 0x3030))
    {
        std::string dataAndTime;
        uint16_t year = M_Word1(vFarmFrame[page].driveInfo.dateOfAssembly);
        uint16_t week = M_Word0(vFarmFrame[page].driveInfo.dateOfAssembly);

        create_Year_Assembled_String(dataAndTime, year, true);
        json_push_back(pageInfo, json_new_a("Year of Assembled", dataAndTime.c_str()));
        dataAndTime.clear();
        create_Year_Assembled_String(dataAndTime, week, true);
        json_push_back(pageInfo, json_new_a("Week of Assembled", dataAndTime.c_str()));
    }
    else
    {
        json_push_back(pageInfo, json_new_a("Year of Assembled", "00"));
        json_push_back(pageInfo, json_new_a("Week of Assembled", "00"));
    }
    json_push_back(masterData, pageInfo);

    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_General_Drive_Information_Continued(JSONNODE* masterData, uint32_t page)
{
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame[page].driveInfo.copyNumber == FACTORYCOPY)
    {
        header.assign("General Drive Informatio From Farm Log copy FACTORY");
    }
    else
    {
        temp << "General Drive Information From Farm Log copy " << std::dec << page;
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());

    set_json_64_bit_With_Status(pageInfo, "Depopulation Head Mask", vFarmFrame[page].gDPage06.Depop, false, m_showStatusBits);                                   //!< Depopulation Head Mask

    std::string type = "not supported";
    uint64_t pageDriveType = vFarmFrame[page].gDPage06.driveType;
    if (check_For_Active_Status( &pageDriveType))
    {
        type = "CMR";
        if (vFarmFrame[page].gDPage06.driveType & BIT0)
        {
            type = "SMR";
        }
    }

    set_json_string_With_Status(pageInfo, "Drive Recording Type", type.c_str(), vFarmFrame[page].gDPage06.driveType, m_showStatusBits);

    if (check_Status_Strip_Status(vFarmFrame[page].gDPage06.Depop) != 0)
    {
        set_Json_Bool(pageInfo, "Has Drive been Depopped", true);
    }
    else
    {
        set_Json_Bool(pageInfo, "Has Drive been Depopped", false);
    }

    set_json_64_bit_With_Status(pageInfo, "Max Number of Available Sectors for Reassignment", vFarmFrame[page].gDPage06.maxNumAvaliableSectors, false, m_showStatusBits);          //!< Max Number of Available Sectors for Reassignment � Value in disc sectors(started in 3.3 )
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(3) << (static_cast<float>(M_Word0(vFarmFrame[page].gDPage06.timeToReady)) * .001F);
    set_json_string_With_Status(pageInfo, "Time to ready of the last power cycle (sec)", temp.str().c_str(), vFarmFrame[page].gDPage06.timeToReady, m_showStatusBits);			//!< time to ready of the last power cycle
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(3) << (static_cast<float>(M_Word0(vFarmFrame[page].gDPage06.holdTime)) * .001F);
    set_json_string_With_Status(pageInfo, "Time drive is held in staggered spin (sec)", temp.str().c_str(), vFarmFrame[page].gDPage06.holdTime, m_showStatusBits);                //!< time drive is held in staggered spin during the last power on sequence
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(3) << (static_cast<float>(M_Word0(vFarmFrame[page].gDPage06.servoSpinUpTime)) * .001F);
    set_json_string_With_Status(pageInfo, "Last Servo Spin up Time (sec)", temp.str().c_str(), vFarmFrame[page].gDPage06.servoSpinUpTime, m_showStatusBits);			//!< time to ready of the last power cycle

    json_push_back(masterData, pageInfo);
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
eReturnValues CSCSI_Farm_Log::print_WorkLoad(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        printf("\nWork Load From Farm Log copy FACTORY");
    }
    else
    {
        printf("\nWork Load From Farm Log copy %d: \n", page);
    }
    printf("\tRated Workload Percentage:                          %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.workloadPercentage & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< rated Workload Percentage
	printf("\tTotal Number of Read Commands:                       %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCommands & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Total Number of Read Commands
	printf("\tTotal Number of Write Commands:                      %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCommands & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Total Number of Write Commands
	printf("\tTotal Number of Random Read Cmds:                    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalRandomReads & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Total Number of Random Read Commands
	printf("\tTotal Number of Random Write Cmds:                   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalRandomWrites & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Total Number of Random Write Commands
    printf("\tTotal Number of Other Commands:                      %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalNumberofOtherCMDS & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Total Number Of Other Commands
    printf("\tLogical Sectors Written:                             %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));          //!< Logical Sectors Written
    printf("\tLogical Sectors Read:                                %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));             //!< Logical Sectors Read
    printf("\tNumber of Read commands from 0-3.125%% of LBA space   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames1 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Read commands from 3.125-25%% of LBA space  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Read commands from 25-50%% of LBA space     %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames3 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Read commands from 50-100%% of LBA space    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames4 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
    printf("\tNumber of Write commands from 0-3.125%% of LBA space  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames1 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Write commands from 3.125-25%% of LBA space %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames2 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Write commands from 25-50%% of LBA space    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames3 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
    printf("\tNumber of Write commands from 50-100%% of LBA space   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames4 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames
    //4.21
    if (m_MajorRev >= 4 && m_MinorRev > 20) {
        printf("\tNumber of Read Commands of transfer length <=16KB for last 3 SMART Summary Frames   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numReadTransferSmall & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read Commands of transfer length (16KB � 512KB] for last 3 SMART Summary Frames  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numReadTransferMid1 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read Commands of transfer length (512KB � 2MB] for last 3 SMART Summary Frames     %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numReadTransferMid2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numReadTransferLarge & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
        printf("\tNumber of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames  %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferSmall & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write Commands of transfer length (16KB � 512KB] for last 3 SMART Summary Frames %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferMid1 & UINT64_C(0x00FFFFFFFFFFFFFF));	    //!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write Commands of transfer length (512KB � 2MB] for last 3 SMART Summary Frames    %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferMid2 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
        printf("\tNumber of Write Commands of transfer length > 2MB for last 3 SMART Summary Frames   %" PRIu64"  \n", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferLarge & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames
    
    }
    

#endif

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame[page].workLoadPage.workLoad.copyNumber == FACTORYCOPY)
    {
        header.assign("Workload From Farm Log copy FACTORY");
    }
    else
    {
        temp << "Workload From Farm Log copy " << std::dec << page;  
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());
    set_json_64_bit_With_Status(pageInfo, "Rated Workload Percentage", vFarmFrame[page].workLoadPage.workLoad.workloadPercentage, false, m_showStatusBits);				//!< rated Workload Percentage
    set_json_64_bit_With_Status(pageInfo, "Total Read Commands", vFarmFrame[page].workLoadPage.workLoad.totalReadCommands, false, m_showStatusBits);			//!< Total Number of Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Write Commands", vFarmFrame[page].workLoadPage.workLoad.totalWriteCommands, false, m_showStatusBits);			//!< Total Number of Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Read Cmds", vFarmFrame[page].workLoadPage.workLoad.totalRandomReads, false, m_showStatusBits);			//!< Total Number of Random Read Commands
    set_json_64_bit_With_Status(pageInfo, "Total Random Write Cmds", vFarmFrame[page].workLoadPage.workLoad.totalRandomWrites, false, m_showStatusBits);		//!< Total Number of Random Write Commands
    set_json_64_bit_With_Status(pageInfo, "Total Other Commands", vFarmFrame[page].workLoadPage.workLoad.totalNumberofOtherCMDS, false, m_showStatusBits);		//!< Total Number Of Other Commands
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Logical Sectors Written", temp.str().c_str(), vFarmFrame[page].workLoadPage.workLoad.logicalSecWritten, m_showStatusBits);					//!< Logical Sectors Written
    temp.str("");temp.clear();
    temp << std::dec << (vFarmFrame[page].workLoadPage.workLoad.logicalSecRead & UINT64_C(0x00FFFFFFFFFFFFFF));
    set_json_string_With_Status(pageInfo, "Logical Sectors Read", temp.str().c_str(), vFarmFrame[page].workLoadPage.workLoad.logicalSecRead, m_showStatusBits);						//!< Logical Sectors Read
    // found a log where the length of the workload log does not match the spec. Need to check for the 0x50 length
    if (vFarmFrame[page].workLoadPage.PageHeader.paramLength > 0x50)
    {
        set_json_64_bit_With_Status(pageInfo, "Read commands from 0-3.125% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames1, false, m_showStatusBits);		//!< Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 3.125-25% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames2, false, m_showStatusBits);		//!< Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 25-50% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Read commands from 25-50% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Read commands from 50-100% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalReadCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Read commands from 50-100% of LBA space for last 3 SMART Summary Frames 
        set_json_64_bit_With_Status(pageInfo, "Write commands from 0-3.125% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames1, false, m_showStatusBits);	//!< Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 3.125-25% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames2, false, m_showStatusBits);	//!< Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 25-50% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames3, false, m_showStatusBits);		//!< Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames
        set_json_64_bit_With_Status(pageInfo, "Write commands from 50-100% of LBA space", vFarmFrame[page].workLoadPage.workLoad.totalWriteCmdsFromFrames4, false, m_showStatusBits);		//!< Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames 
    }

    // 4.21
    if (m_MajorRev >= 4 && m_MinorRev >= 19)
    {
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length <4kb", vFarmFrame[page].workLoadPage.workLoad.numReadTransferSmall, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (4kb - 16kb)", vFarmFrame[page].workLoadPage.workLoad.numReadTransferMid1, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length (16kb - 128kb)", vFarmFrame[page].workLoadPage.workLoad.numReadTransferMid2, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Read Commands of transfer length >= 128kb", vFarmFrame[page].workLoadPage.workLoad.numReadTransferLarge, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length <4kb", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferSmall, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (4kb - 16kb)", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferMid1, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length (16kb - 128kb)", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferMid2, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Write Commands of transfer length >= 128kb", vFarmFrame[page].workLoadPage.workLoad.numWriteTransferLarge, false, m_showStatusBits);
    }

    json_push_back(masterData, pageInfo);
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Error_Information()
//
//! \brief
//!   Description:  print out the Error log information 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Error_Information(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].errorPage.errorStat.copyNumber == FACTORYCOPY)
    {
        printf("\nError Information From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nError Information Log From Farm Log copy %d: \n", page);
    }
    printf("\tUnrecoverable Read Errors:                %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalReadECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors:               %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalWriteECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Write Errors
    printf("\tNumber of Reallocated Sectors:            %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalReallocations & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< Number of Reallocated Sectors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tNumber of Mechanical Start Failures:      %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalMechanicalFails & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Mechanical Start Failures
    printf("\tNumber of Reallocated Candidate Sectors:  %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalReallocatedCanidates & UINT64_C(0x00FFFFFFFFFFFFFF));	//!< Number of Reallocated Candidate Sectors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tIOEDC Errors (not supported in sas):      %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.attrIOEDCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of IOEDC Errors 
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved6 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved7 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved8 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tTotal Flash LED (Assert) Events:          %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.totalFlashLED & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Total Flash LED (Assert) Events
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.reserved9 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tFRU code if smart trip:                   %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.FRUCode & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tSuper Parity on the Fly Recovery          %" PRIu64" \n", vFarmFrame[page].errorPage.errorStat.parity & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame[page].errorPage.errorStat.copyNumber == FACTORYCOPY)
    {
        header.assign("Error Information From Farm Log copy FACTORY");
    }
    else
    {
        temp << "Error Information Log From Farm Log copy " << std::dec << page;
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.errorStat.totalReadECC, false, m_showStatusBits);							//!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.errorStat.totalWriteECC, false, m_showStatusBits);							//!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Reallocated Sectors", vFarmFrame[page].errorPage.errorStat.totalReallocations, false, m_showStatusBits);					//!< Number of Reallocated Sectors
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame[page].errorPage.errorStat.totalMechanicalFails, false, m_showStatusBits);			//!< Number of Mechanical Start Failures
    set_json_64_bit_With_Status(pageInfo, "Reallocated Candidate Sectors", vFarmFrame[page].errorPage.errorStat.totalReallocatedCanidates, false, m_showStatusBits); //!< Number of Reallocated Candidate Sectors
    set_json_64_bit_With_Status(pageInfo, "Number of IOEDC Errors (Raw)", vFarmFrame[page].errorPage.errorStat.attrIOEDCErrors, false, m_showStatusBits);						//!< Number of IOEDC Errors (SMART Attribute 184 Raw)
    set_json_64_bit_With_Status(pageInfo, "Total Flash LED (Assert) Events", vFarmFrame[page].errorPage.errorStat.totalFlashLED, false, m_showStatusBits);						//!< Total Flash LED (Assert) Events
    set_json_64_bit_With_Status(pageInfo, "SMART Trip FRU code", vFarmFrame[page].errorPage.errorStat.FRUCode, false, m_showStatusBits);		//!< FRU code if smart trip from most recent SMART Frame
    set_json_64_bit_With_Status(pageInfo, "Super Parity on the Fly Recovery", vFarmFrame[page].errorPage.errorStat.parity, false, m_showStatusBits);                      //!< Super Parity on the Fly Recovery

    json_push_back(masterData, pageInfo);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn print_Error_Information_Version_4()
//
//! \brief
//!   Description:  print out the Error log information version 4 and up 
//
//  Entry:
//! \param masterData - pointer to the json data that will be printed or passed on
//! \param page  = the page copy number of the data we want to print. 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Error_Information_Version_4(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].errorPage.errorV4.copyNumber == FACTORYCOPY)
    {
        printf("\nError Information From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nError Information Log From Farm Log copy %d: \n", page);
    }
    printf("\tUnrecoverable Read Errors:                %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.totalReadECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Read Errors
    printf("\tUnrecoverable Write Errors:               %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.totalWriteECC & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of Unrecoverable Write Errors
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.totalMechanicalFails & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< Number of Mechanical Start Failures
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tIOEDC Errors (not supported in sas):      %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.attrIOEDCErrors & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of IOEDC Errors 
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved6 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved7 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved8 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.reserved9 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Reserved
    printf("\tSMART Trip FRU code:                      %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.FRUCode & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tInvalid DWord Count Port A                %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portAInvalidDwordCount & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tInvalid DWord Count Port B                %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portBInvalidDwordCount & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tDisparity Error Count Port A              %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portADisparityErrorCount & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tDisparity Error Count Port B              %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portBDisparityErrorCount & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tLoss Of DWord Sync Port A                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portALossDwordSync & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tLoss Of DWord Sync Port B                 %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portBLossDwordSync & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tPhy Reset Problem Port A                  %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portAPhyResetProblem & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tPhy Reset Problem Port B                  %" PRIu64" \n", vFarmFrame[page].errorPage.errorV4.portBPhyResetProblem & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (vFarmFrame[page].errorPage.errorV4.copyNumber == FACTORYCOPY)
    {
        temp << "Error Information From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Error Information Log From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Read Errors", vFarmFrame[page].errorPage.errorV4.totalReadECC, false, m_showStatusBits);							//!< Number of Unrecoverable Read Errors
    set_json_64_bit_With_Status(pageInfo, "Unrecoverable Write Errors", vFarmFrame[page].errorPage.errorV4.totalWriteECC, false, m_showStatusBits);							//!< Number of Unrecoverable Write Errors
    set_json_64_bit_With_Status(pageInfo, "Mechanical Start Failures", vFarmFrame[page].errorPage.errorV4.totalMechanicalFails, false, m_showStatusBits);			//!< Number of Mechanical Start Failures
    set_json_64_bit_With_Status(pageInfo, "IOEDC Errors (Raw)", vFarmFrame[page].errorPage.errorV4.attrIOEDCErrors, false, m_showStatusBits);						//!< Number of IOEDC Errors (SMART Attribute 184 Raw)   
    set_json_64_bit_With_Status(pageInfo, "SMART Trip FRU code", vFarmFrame[page].errorPage.errorV4.FRUCode, false, m_showStatusBits);		//!< FRU code if smart trip from most recent SMART Frame
    set_json_64_bit_With_Status(pageInfo, "Invalid DWord Count Port A ", vFarmFrame[page].errorPage.errorV4.portAInvalidDwordCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Invalid DWord Count Port B", vFarmFrame[page].errorPage.errorV4.portBInvalidDwordCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Disparity Error Count Port A", vFarmFrame[page].errorPage.errorV4.portADisparityErrorCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Disparity Error Count Port B", vFarmFrame[page].errorPage.errorV4.portBDisparityErrorCount, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Loss Of DWord Sync Port A", vFarmFrame[page].errorPage.errorV4.portALossDwordSync, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Loss Of DWord Sync Port B", vFarmFrame[page].errorPage.errorV4.portBLossDwordSync, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Phy Reset Problem Port A", vFarmFrame[page].errorPage.errorV4.portAPhyResetProblem, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "Phy Reset Problem Port B", vFarmFrame[page].errorPage.errorV4.portBPhyResetProblem, false, m_showStatusBits);

    json_push_back(masterData, pageInfo);
  
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Enviroment_Information(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        printf( "Environment Information From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nEnvironment Information From Farm Log copy %d: \n", page);
    }

    printf("\tCurrent Temperature:                      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.curentTemp)*.10));			        //!< Current Temperature in Celsius
    printf("\tHighest Temperature:                      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.highestTemp)*.10));			        //!< Highest Temperature in Celsius
    printf("\tLowest Temperature:                       %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.lowestTemp)*.10));			        //!< Lowest Temperature
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved6 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved7 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tSpecified Max Operating Temperature:      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.maxTemp) * 1.00));				    //!< Specified Max Operating Temperature
    printf("\tSpecified Min Operating Temperature:      %0.02f     \n", static_cast<float>(M_WordInt0(vFarmFrame[page].environmentPage.minTemp) * 1.00));				    //!< Specified Min Operating Temperature
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved8 & UINT64_C(0x00FFFFFFFFFFFFFF));				            //!< Reserved
    printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].environmentPage.reserved9 & UINT64_C(0x00FFFFFFFFFFFFFF));	                        //!< Reserved
    printf("\tCurrent Relative Humidity:                %" PRId32".%" PRId32"   \n", M_DoubleWord1(vFarmFrame[page].environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF)), M_DoubleWord0(vFarmFrame[page].environmentPage.humidity & UINT64_C(0x00FFFFFFFFFFFFFF)));		//!< Current Relative Humidity (in units of .1%)
    printf("\tHumidity Mixed Ratio:                     %0.02f     \n", static_cast<double>(M_WordInt0(vFarmFrame[page].environmentPage.humidityRatio & UINT64_C(0x00FFFFFFFFFFFFFF)) / 8.0)); //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    printf("\tCurrent Motor Power:                      %" PRIu16" \n", (M_Word0(vFarmFrame[page].environmentPage.currentMotorPower)));	
    printf("\t12v Power Average(mw):                    %" PRIu64" \n", vFarmFrame[page].environmentPage.average12v & UINT64_C(0x00FFFFFFFFFFFFFF));				
    printf("\t12v Power Min(mw):                        %" PRIu64" \n", vFarmFrame[page].environmentPage.min12v & UINT64_C(0x00FFFFFFFFFFFFFF));				
    printf("\t12v Power Max(mw):                        %" PRIu64" \n", vFarmFrame[page].environmentPage.max12v & UINT64_C(0x00FFFFFFFFFFFFFF));				
    printf("\t5v Power Average(mw):                     %" PRIu64" \n", vFarmFrame[page].environmentPage.average5v & UINT64_C(0x00FFFFFFFFFFFFFF));				
    printf("\t5v Power Min(mw):                         %" PRIu64" \n", vFarmFrame[page].environmentPage.min5v & UINT64_C(0x00FFFFFFFFFFFFFF));				
    printf("\t5v Power Max(mw):                         %" PRIu64" \n", vFarmFrame[page].environmentPage.max5v & UINT64_C(0x00FFFFFFFFFFFFFF));	
#endif

    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    std::string header;
    if (vFarmFrame[page].environmentPage.copyNumber == FACTORYCOPY)
    {
        header.assign("Environment Information From Farm Log copy FACTORY");
    }
    else
    {
        temp << "Environment Information From Farm Log copy " << std::dec << page;
        header.assign(temp.str());
    }
    json_set_name(pageInfo, header.c_str());
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.curentTemp)) * .10));							//!< Current Temperature in Celsius
    set_json_string_With_Status(pageInfo, "Current Temperature (Celsius)", temp.str().c_str(), vFarmFrame[page].environmentPage.curentTemp, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.highestTemp)) * .10));						//!< Highest Average Long Term Temperature
    set_json_string_With_Status(pageInfo, "Highest Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.highestTemp, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.lowestTemp)) * .10));							//!< Lowest Average Long Term Temperature
    set_json_string_With_Status(pageInfo, "Lowest Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.lowestTemp, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.maxTemp)) * 1.00));							//!< Specified Max Operating Temperature
    set_json_string_With_Status(pageInfo, "Specified Max Operating Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.maxTemp, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.minTemp)) * 1.00));							//!< Specified Min Operating Temperature
    set_json_string_With_Status(pageInfo, "Specified Min Operating Temperature", temp.str().c_str(), vFarmFrame[page].environmentPage.minTemp, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidity)) * 0.1));							//!< Current Relative Humidity (in units of .1%)
    set_json_string_With_Status(pageInfo, "Current Relative Humidity", temp.str().c_str(), vFarmFrame[page].environmentPage.humidity, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << (static_cast<float>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.humidityRatio)) / 8.0));						//!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    set_json_string_With_Status(pageInfo, "Humidity Mixed Ratio", temp.str().c_str(), vFarmFrame[page].environmentPage.humidityRatio, m_showStatusBits);
    temp.str("");temp.clear();
    temp << std::dec << M_Word0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.currentMotorPower));
    set_json_string_With_Status(pageInfo, "Current Motor Power", temp.str().c_str(), vFarmFrame[page].environmentPage.currentMotorPower, m_showStatusBits);					    //!< Current Motor Power, value from most recent SMART Summary Frame6

    if (m_MajorRev >= 4)
    {
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.average12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.average12v)) % 1000);
        set_json_string_With_Status(pageInfo, "12V Power Average", temp.str().c_str(), vFarmFrame[page].environmentPage.average12v, m_showStatusBits);
        if (m_MinorRev <= 9)
        {
            temp.str(""); temp.clear();
            temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min12v)) % 1000);
            set_json_string_With_Status(pageInfo, "12V Power Minimum", temp.str().c_str(), vFarmFrame[page].environmentPage.min12v, m_showStatusBits);
            temp.str(""); temp.clear();
            temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max12v)) % 1000);
            set_json_string_With_Status(pageInfo, "12V Power Maximum", temp.str().c_str(), vFarmFrame[page].environmentPage.max12v, m_showStatusBits);
        }
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.average5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.average5v)) % 1000);
        set_json_string_With_Status(pageInfo, "5V Power Average", temp.str().c_str(), vFarmFrame[page].environmentPage.average5v, m_showStatusBits);
        if (m_MinorRev <= 9)
        {
            temp.str(""); temp.clear();
            temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.min5v)) % 1000);
            set_json_string_With_Status(pageInfo, "5V Power Minimum", temp.str().c_str(), vFarmFrame[page].environmentPage.min5v, m_showStatusBits);
            temp.str(""); temp.clear();
            temp << std::dec << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_WordInt0(check_Status_Strip_Status(vFarmFrame[page].environmentPage.max5v)) % 1000);
            set_json_string_With_Status(pageInfo, "5V Power Maximum", temp.str().c_str(), vFarmFrame[page].environmentPage.max5v, m_showStatusBits);
        }
    }

    json_push_back(masterData, pageInfo);

    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Enviroment_Statistics_Page_07(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (vFarmFrame[page].envStatPage07.copyNumber == FACTORYCOPY)
    {
        printf("Environment Information Continued From Farm Log copy FACTORY \n");
    }
    else
    {
        printf("\nEnvironment Information Continued From Farm Log copy %d: \n", page);
    }
    printf("\tCurrent 12 volts:                         %" PRIu64" \n", vFarmFrame[page].envStatPage07.average12v & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tMinimum 12 volts:                         %" PRIu64" \n", vFarmFrame[page].envStatPage07.min12v & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tMaximum 12 volts:                         %" PRIu64" \n", vFarmFrame[page].envStatPage07.max12v & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tCurrent 5 volts:                          %" PRIu64" \n", vFarmFrame[page].envStatPage07.average5v & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tMinimum 5 volts:                          %" PRIu64" \n", vFarmFrame[page].envStatPage07.min5v & UINT64_C(0x00FFFFFFFFFFFFFF));
    printf("\tMaximum 5 volts:                          %" PRIu64" \n", vFarmFrame[page].envStatPage07.max5v & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
    
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (vFarmFrame[page].envStatPage07.copyNumber == FACTORYCOPY)
    {
        temp << "Environment Information Continued From Farm Log copy FACTORY";
    }
    else
    {
        temp << "Environment Information Continued From Farm Log copy " << std::dec << page;
    }
    json_set_name(pageInfo, temp.str().c_str());

    temp.str("");temp.clear();
    temp << std::dec << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.average12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.average12v)) % 1000);
    set_json_string_With_Status(pageInfo, "Current 12 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.average12v, m_showStatusBits);
    if (m_MinorRev > 9)
    {
        temp.str(""); temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.min12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.min12v)) % 1000);
        set_json_string_With_Status(pageInfo, "Minimum 12 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.min12v, m_showStatusBits);
        temp.str(""); temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.max12v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.max12v)) % 1000);
        set_json_string_With_Status(pageInfo, "Maximum 12 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.max12v, m_showStatusBits);
    }
    temp.str("");temp.clear();
    temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.average5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << static_cast<uint16_t>(M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.average5v)) % 1000);
    set_json_string_With_Status(pageInfo, "Current 5 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.average5v, m_showStatusBits);
    if (m_MinorRev > 9)
    {
        temp.str(""); temp.clear();
        temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.min5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.min5v)) % 1000;
        set_json_string_With_Status(pageInfo, "Minimum 5 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.min5v, m_showStatusBits);
        temp.str(""); temp.clear();
        temp << std::dec << (M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.max5v)) / 1000) << "." << std::dec << std::setfill('0') << std::setw(3) << M_Word0(check_Status_Strip_Status(vFarmFrame[page].envStatPage07.max5v)) % 1000;
        set_json_string_With_Status(pageInfo, "Maximum 5 volts", temp.str().c_str(), vFarmFrame[page].envStatPage07.max5v, m_showStatusBits);
    }

    json_push_back(masterData, pageInfo);
  
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Workload_Statistics_Page_08(JSONNODE *masterData, uint32_t page)
{
    if (m_MajorRev >= 4 && m_MinorRev >= 19)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);

    #if defined _DEBUG
        if (vFarmFrame[page].workloadStatPage08.copyNumber == FACTORYCOPY)
        {
            printf("Workload Information Continued From Farm Log copy FACTORY \n");
        }
        else
        {
            printf("\nWorkload Information Continued From Farm Log copy %d: \n", page);
        }
        printf("\tCount of Queue Depth =1 at 30s intervals:    %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth1 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth =2 at 30s intervals:    %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth2 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth 2-4 at 30s intervals:   %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth3_4 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth 5-8 at 30s intervals:   %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth5_8 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth 9-16 at 30s intervals:  %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth9_16 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth 17-32 at 30s intervals: %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth17_32 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth 33-64 at 30s intervals: %" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth33_64 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCount of Queue Depth at 64 at 30s intervals:%" PRIu64" \n", vFarmFrame[page].workloadStatPage08.countQueDepth_gt_64 & UINT64_C(0x00FFFFFFFFFFFFFF));
    

    #endif
        std::ostringstream temp;
        if (vFarmFrame[page].workloadStatPage08.copyNumber == FACTORYCOPY)
        {
            temp << "Workload Information Continued From Farm Log copy FACTORY";
        }
        else
        {
            temp << "Workload Information Continued From Farm Log copy " << std::dec << page;
        }
        json_set_name(pageInfo, temp.str().c_str());
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 1", vFarmFrame[page].workloadStatPage08.countQueDepth1,false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin = 2", vFarmFrame[page].workloadStatPage08.countQueDepth2, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 3-4", vFarmFrame[page].workloadStatPage08.countQueDepth3_4, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 5-8", vFarmFrame[page].workloadStatPage08.countQueDepth5_8, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 9-16", vFarmFrame[page].workloadStatPage08.countQueDepth9_16, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 17-32", vFarmFrame[page].workloadStatPage08.countQueDepth17_32, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin 33-64", vFarmFrame[page].workloadStatPage08.countQueDepth33_64, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Queue Depth bin > 64", vFarmFrame[page].workloadStatPage08.countQueDepth_gt_64, false, m_showStatusBits);
        json_push_back(masterData, pageInfo);

    }
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
eReturnValues CSCSI_Farm_Log::print_Reli_Information(JSONNODE *masterData, uint32_t page)
{
#if defined _DEBUG
    if (m_MajorRev < 4)
    {
        if (vFarmFrame[page].reliPage.reli.copyNumber == FACTORYCOPY)
        {
            printf("Reliability Information From Farm Log copy FACTORY");
        }
        else
        {
            printf("\nReliability Information From Farm Log copy: %d\n", page);
        }
        printf("\tTimeStamp of last IDD test:               %" PRIu64" \n", vFarmFrame[page].reliPage.reli.lastIDDTest & UINT64_C(0x00FFFFFFFFFFFFFF));                     //!< Timestamp of last IDD test
        printf("\tSub-command of last IDD test:             %" PRIu64" \n", vFarmFrame[page].reliPage.reli.cmdLastIDDTest & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Sub-command of last IDD test
        printf("\tNumber of Reclamations Sectors:           %" PRIu64" \n", vFarmFrame[page].reliPage.reli.gListReclamed & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Number of G-List Reclamations 
        printf("\tServo Status:                             %" PRIu64" \n", vFarmFrame[page].reliPage.reli.servoStatus & UINT64_C(0x00FFFFFFFFFFFFFF));                     //!< Servo Status (follows standard DST error code definitions)
        printf("\tNumber of Slipped Secotrs Before IDD Scan:%" PRIu64" \n", vFarmFrame[page].reliPage.reli.altsBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Number of Alt List Entries Before IDD Scan
        printf("\tNumber of Slipped Secotrs After IDD Scan: %" PRIu64" \n", vFarmFrame[page].reliPage.reli.altsAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                    //!< Number of Alt List Entries After IDD Scan
        printf("\tResident G-List Entries Before IDD Scan:  %" PRIu64" \n", vFarmFrame[page].reliPage.reli.gListBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Number of Resident G-List Entries Before IDD Scan
        printf("\tResident G-List Entries After IDD Scan:   %" PRIu64" \n", vFarmFrame[page].reliPage.reli.gListAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Number of Resident G-List Entries After IDD Scan
        printf("\tScrubs List Entries Before IDD Scan:      %" PRIu64" \n", vFarmFrame[page].reliPage.reli.scrubsBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                 //!< Number of Scrub List Entries Before IDD Scan
        printf("\tScrubs List Entries After IDD Scan:       %" PRIu64" \n", vFarmFrame[page].reliPage.reli.scrubsAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Number of Scrub List Entries After IDD Scan
        printf("\tNumber of DOS Scans Performed:            %" PRIu64" \n", vFarmFrame[page].reliPage.reli.numberDOSScans & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Number of DOS Scans Performed
        printf("\tNumber of LBAs Corrected by ISP:          %" PRIu64" \n", vFarmFrame[page].reliPage.reli.numberLBACorrect & UINT64_C(0x00FFFFFFFFFFFFFF));                //!< Number of LBAs Corrected by ISP
        printf("\tNumber of Valid Parity Sectors:           %" PRIu64" \n", vFarmFrame[page].reliPage.reli.numberValidParitySec & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of Valid Parity Sectors
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.reserved6 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tNumber of RAW Operations:                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli.numberRAWops & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tMicro Actuator Lock-out accumulated:      %" PRIu64" \n", vFarmFrame[page].reliPage.reli.microActuatorLockOut & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
        printf("\t# of Disc Slip Recalibrations Performed : %" PRIu64" \n", vFarmFrame[page].reliPage.reli.diskSlipRecalPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< Number of disc slip recalibrations performed
        printf("\tHelium Pressure Threshold Trip:           %" PRIu64" \n", vFarmFrame[page].reliPage.reli.heliumPressuretThreshold & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< helium Pressure Threshold Trip
        printf("\tRV Absolute Mean:                         %" PRIu64" \n", vFarmFrame[page].reliPage.reli.rvAbsoluteMean & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< RV Absulute Mean
        printf("\tMax RV Absolute Mean:                     %" PRIu64" \n", vFarmFrame[page].reliPage.reli.maxRVAbsoluteMean & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Max RV absulute Mean
        printf("\tIdle Time value from the most recent SMART Summary Frame:     %" PRIu64" \n", vFarmFrame[page].reliPage.reli.idleTime & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< idle Time value from the most recent SMART Summary Frame
    }
#endif
   
    JSONNODE* pageInfo = json_new(JSON_NODE);
    std::ostringstream temp;
    if (m_MajorRev < 4)
    {
        if (vFarmFrame[page].reliPage.reli.copyNumber == FACTORYCOPY)
        {
            temp << "Reliability Information From Farm Log copy FACTORY";
        }
        else
        {
            temp << "Reliability Information From Farm Log copy " << std::dec << page;
        }
        json_set_name(pageInfo, temp.str().c_str());

        set_json_64_bit_With_Status(pageInfo, "TimeStamp of last IDD test", vFarmFrame[page].reliPage.reli.lastIDDTest, false, m_showStatusBits);							//!< Timestamp of last IDD test
        set_json_64_bit_With_Status(pageInfo, "Sub-Command of Last IDD Test", vFarmFrame[page].reliPage.reli.cmdLastIDDTest, false, m_showStatusBits);						//!< Sub-command of last IDD test
        set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sector Reclamations", vFarmFrame[page].reliPage.reli.gListReclamed, false, m_showStatusBits);						//!< Number of G-List Reclamations 
        set_json_64_bit_With_Status(pageInfo, "Servo Status", vFarmFrame[page].reliPage.reli.servoStatus, false, m_showStatusBits);											//!< Servo Status (follows standard DST error code definitions)
        set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors Before IDD Scan", vFarmFrame[page].reliPage.reli.altsBeforeIDD, false, m_showStatusBits);					//!< Number of Alt List Entries Before IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors After IDD Scan", vFarmFrame[page].reliPage.reli.altsAfterIDD, false, m_showStatusBits);						//!< Number of Alt List Entries After IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors Before IDD Scan", vFarmFrame[page].reliPage.reli.gListBeforeIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries Before IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors After IDD Scan", vFarmFrame[page].reliPage.reli.gListAfterIDD, false, m_showStatusBits);				//!< Number of Resident G-List Entries After IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors Before IDD Scan", vFarmFrame[page].reliPage.reli.scrubsBeforeIDD, false, m_showStatusBits);				//!< Number of Scrub List Entries Before IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors After IDD Scan", vFarmFrame[page].reliPage.reli.scrubsAfterIDD, false, m_showStatusBits);					//!< Number of Scrub List Entries After IDD Scan
        set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", vFarmFrame[page].reliPage.reli.numberDOSScans, false, m_showStatusBits);						//!< Number of DOS Scans Performed
        set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", vFarmFrame[page].reliPage.reli.numberLBACorrect, false, m_showStatusBits);					//!< Number of LBAs Corrected by ISP
        set_json_64_bit_With_Status(pageInfo, "Number of Valid Parity Sectors", vFarmFrame[page].reliPage.reli.numberValidParitySec, false, m_showStatusBits);				//!< Number of Valid Parity Sectors
        set_json_64_bit_With_Status(pageInfo, "Number of RAW Operations", vFarmFrame[page].reliPage.reli.numberRAWops, false, m_showStatusBits);								//!< Number of RAW Operations
        set_json_64_bit_With_Status(pageInfo, "Micro Actuator Lock-out accumulated", vFarmFrame[page].reliPage.reli.microActuatorLockOut, false, m_showStatusBits);			//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
        set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame[page].reliPage.reli.diskSlipRecalPerformed, false, m_showStatusBits);	            //!< Number of disc slip recalibrations performed
        set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame[page].reliPage.reli.heliumPressuretThreshold, false, m_showStatusBits);			//!< helium Pressure Threshold Trip
        set_json_64_bit_With_Status(pageInfo, "RV Absolute Mean", vFarmFrame[page].reliPage.reli.rvAbsoluteMean, false, m_showStatusBits);									//!< RV Absulute Mean
        set_json_64_bit_With_Status(pageInfo, "Max RV Absolute Mean", vFarmFrame[page].reliPage.reli.maxRVAbsoluteMean, false, m_showStatusBits);							//!< Max RV absulute Mean
        set_json_64_bit_With_Status(pageInfo, "Idle Time (hours)", vFarmFrame[page].reliPage.reli.idleTime, false, m_showStatusBits);	//!< idle Time value from the most recent SMART Summary Frame
    }
    else
    {
#if defined _DEBUG
        if (vFarmFrame[page].reliPage.reli.copyNumber == FACTORYCOPY)
        {
            printf("Reliability Information From Farm Log copy FACTORY");
        }
        else
        {
            printf("\nReliability Information From Farm Log copy: %d\n", page);
        }
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved1 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved2 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved3 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved4 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved5 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved6 & UINT64_C(0x00FFFFFFFFFFFFFF));                       //!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved7 & UINT64_C(0x00FFFFFFFFFFFFFF));							//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved8 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved9 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved10 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved11 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved12 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tNumber of RAW Operations:                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.numberRAWops & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tCumulative Lifetime ECC due to ERC:       %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.cumECCDueToERC & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved13 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved14 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved15 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved16 & UINT64_C(0x00FFFFFFFFFFFFFF));						//!< Reserved
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved17 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved18 & UINT64_C(0x00FFFFFFFFFFFFFF));
        printf("\tMicro Actuator Lock-out accumulated:      %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.microActuatorLockOut & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
        printf("\t# of Disc Slip Recalibrations Performed : %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.diskSlipRecalPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< Number of disc slip recalibrations performed
        printf("\tHelium Pressure Threshold Trip:           %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.heliumPressuretThreshold & UINT64_C(0x00FFFFFFFFFFFFFF));			//!< helium Pressure Threshold Trip
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved19 & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< RV Absulute Mean
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved20 & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Max RV absulute Mean
        printf("\tReserved:                                 %" PRIu64" \n", vFarmFrame[page].reliPage.reli4.reserved21 & UINT64_C(0x00FFFFFFFFFFFFFF));		//!< idle Time value from the most recent SMART Summary Frame

#endif
        if (vFarmFrame[page].reliPage.reli.copyNumber == FACTORYCOPY)
        {
            temp << "Reliability Information From Farm Log copy FACTORY";
        }
        else
        {
            temp << "Reliability Information From Farm Log copy " << std::dec << page;
        }
        json_set_name(pageInfo, temp.str().c_str());

        set_json_64_bit_With_Status(pageInfo, "Number of RAW Operations", vFarmFrame[page].reliPage.reli4.numberRAWops, false, m_showStatusBits);								//!< Number of RAW Operations
        set_json_64_bit_With_Status(pageInfo, "Cumulative Lifetime ECC due to ERC", vFarmFrame[page].reliPage.reli4.cumECCDueToERC, false, m_showStatusBits);
        set_json_64_bit_With_Status(pageInfo, "Micro Actuator Lock-out accumulated", vFarmFrame[page].reliPage.reli4.microActuatorLockOut, false, m_showStatusBits);			//!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames8
        set_json_64_bit_With_Status(pageInfo, "Number of Disc Slip Recalibrations Performed", vFarmFrame[page].reliPage.reli4.diskSlipRecalPerformed, false, m_showStatusBits);	            //!< Number of disc slip recalibrations performed
        set_json_64_bit_With_Status(pageInfo, "Helium Pressure Threshold Tripped", vFarmFrame[page].reliPage.reli4.heliumPressuretThreshold, false, m_showStatusBits);			//!< helium Pressure Threshold Trip
    }

    json_push_back(masterData, pageInfo);
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_Head_Information(eSASLogPageTypes type, JSONNODE *headPage, uint32_t page)
{
    uint32_t loopCount = 0;
    std::string myHeader;

    if (set_Head_Header(myHeader, type) == false)
    {
        return FAILURE;
    }
    if (type != RESERVED_FOR_FUTURE_EXPANSION)
    {
        switch (type)
        {
        case FARM_HEADER_PARAMETER:
        case GENERAL_DRIVE_INFORMATION_PARAMETER:
        case WORKLOAD_STATISTICS_PARAMETER:
        case ERROR_STATISTICS_PARAMETER:
        case ENVIRONMENTAL_STATISTICS_PARAMETER:
        case RELIABILITY_STATISTICS_PARAMETER:
        case GENERAL_DRIVE_INFORMATION_06:
        case ENVIRONMENT_STATISTICS_PAMATER_07:
        case WORKLOAD_STATISTICS_PAMATER_08:
        case RESERVED_FOR_FUTURE_STATISTICS_4:
        case RESERVED_FOR_FUTURE_STATISTICS_5:
        case RESERVED_FOR_FUTURE_STATISTICS_6:
        case RESERVED_FOR_FUTURE_STATISTICS_7:
        case RESERVED_FOR_FUTURE_STATISTICS_8:
        case RESERVED_FOR_FUTURE_STATISTICS_9:
        case RESERVED_FOR_FUTURE_STATISTICS_10:
        case RESERVED_FOR_FUTURE_HEAD_1:
        case RESERVED_FOR_FUTURE_HEAD_2:
        case RESERVED_FOR_FUTURE_HEAD_3:
        case RESERVED_FOR_FUTURE_HEAD_4:
        case RESERVED_FOR_FUTURE_HEAD_5:
        case RESERVED_FOR_FUTURE_HEAD_6:
        case RESERVED_FOR_FUTURE_HEAD_7:
        case RESERVED_FOR_FUTURE_HEAD_8:
        case RESERVED_FOR_FUTURE_HEAD_9:
        case RESERVED_FOR_FUTURE_HEAD_10:
            break;
        case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tMR Head Resistance for Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< MR Head Resistance from most recent SMART Summary Frame
#endif
                std::ostringstream header;
                header << "MR Head Resistance for Head " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].mrHeadResistanceByHead.headValue[loopCount], false, m_showStatusBits);  //!< MR Head Resistance from most recent SMART Summary Frame
            }
            break;
        case RESERVED_FOR_FUTURE_HEAD_11:
        case RESERVED_FOR_FUTURE_HEAD_12:
        case RESERVED_FOR_FUTURE_HEAD_13:
        case RESERVED_FOR_FUTURE_HEAD_14:
            break;
        case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT amplitude by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT amplitude, averaged across Test Zone
#endif
                std::ostringstream header;
                header << "Current H2SAT amplitude by Head " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].currentH2STAmplituedByHead.headValue[loopCount], false, m_showStatusBits);  //!< Current H2SAT amplitude, averaged across Test Zone
            }
            break;
        case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT asymmetry by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT asymmetry, averaged across Test Zone
#endif
                std::ostringstream header;
                header << "Current H2SAT asymmetry by Head " << std::dec << loopCount;// Head count
                set_json_float_With_Status(headPage, header.str().c_str(), static_cast<double>(M_WordInt0(vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount]) * 0.1F), vFarmFrame[page].currentH2STAsymmetryByHead.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT asymmetry, averaged across Test Zone
            }
            break;
        case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tNumber of Reallocated Sectors by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentGlistEntries.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
                std::ostringstream header;
                header << "Reallocated Sectors by Head " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].ResidentGlistEntries.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case NUMBER_OF_PENDING_ENTRIES:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tReallocated Candidate Sectors by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].ResidentPlistEntries.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
                std::ostringstream header;
                header << "Reallocated Candidate Sectors by Head " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].ResidentPlistEntries.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case RESERVED_FOR_FUTURE_HEAD_15:
        case RESERVED_FOR_FUTURE_HEAD_16:
        case RESERVED_FOR_FUTURE_HEAD_17:
            break;
        case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tWrite Power On (hrs) value by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].writePowerOnHours.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
                std::ostringstream header;
                header << "Write Power On (hrs) by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_DoubleWord0(vFarmFrame[page].writePowerOnHours.headValue[loopCount]) / 3600.0);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].discSlipPerHead.headValue[loopCount], m_showStatusBits);
            }
            break;
        case RESERVED_FOR_FUTURE_HEAD_18:
            break;
        case CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCum Lifetime Unrecoverable Read Repeat %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].cumECCReadRepeat.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
                std::ostringstream header;
                header << "Cum Lifetime Unrecoverable Read Repeating " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].cumECCReadRepeat.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCumlative Lifetime Unrecoverable Read Unique per head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].cumECCReadUnique.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));
#endif
                std::ostringstream header;
                header << "Cum Lifetime Unrecoverable Read Unique " << std::dec << loopCount; // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].cumECCReadUnique.headValue[loopCount], false, m_showStatusBits);
            }
            break;
        case RESERVED_FOR_FUTURE_HEAD_19:
        case RESERVED_FOR_FUTURE_HEAD_20:
        case RESERVED_FOR_FUTURE_HEAD_21:
        case RESERVED_FOR_FUTURE_HEAD_22:
        case RESERVED_FOR_FUTURE_HEAD_23:
        case RESERVED_FOR_FUTURE_HEAD_24:
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32":  by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
#endif
                std::ostringstream header;
                header << "Current H2SAT trimmed mean bits in error Zone 0 by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STTrimmedbyHeadZone0.headValue[loopCount], m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 0
            }
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32": by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
#endif
                std::ostringstream header;
                header << "Current H2SAT trimmed mean bits in error Zone 1 by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STTrimmedbyHeadZone1.headValue[loopCount], m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 1
            }
            break;
        case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT trimmed mean bits in error by Head %" PRIu32" , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
#endif
                std::ostringstream header;
                header << "Current H2SAT trimmed mean bits in error Zone 2 by Head " << std::dec << loopCount;// Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STTrimmedbyHeadZone2.headValue[loopCount], m_showStatusBits); //!< Current H2SAT trimmed mean bits in error by Head, by Test Zone 2
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 0:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
#endif
                std::ostringstream header;
                header << "Current H2SAT iterations to converge Test Zone 0 by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STIterationsByHeadZone0.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 0
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 1:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
#endif
                std::ostringstream header;
                header << "Current H2SAT iterations to converge Test Zone 1 by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 1
            }
            break;
        case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tCurrent H2SAT iterations to converge by Head %" PRIu32" , by Test Zone 2:      %" PRIu64" \n", loopCount, vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
#endif
                std::ostringstream header;
                header << "Current H2SAT iterations to converge Test Zone 2 by Head " << std::dec << loopCount; // Head count
                double number = static_cast<double>(M_Word0(vFarmFrame[page].currentH2STIterationsByHeadZone1.headValue[loopCount]) * 0.10F);
                set_json_float_With_Status(headPage, header.str().c_str(), number, vFarmFrame[page].currentH2STIterationsByHeadZone2.headValue[loopCount], m_showStatusBits);  //!< Current H2SAT iterations to cnverge by Head, by Test Zone 2
            }
            break;
        case RESERVED_FOR_FUTURE_HEAD_25:
        case RESERVED_FOR_FUTURE_HEAD_26:
        case RESERVED_FOR_FUTURE_HEAD_27:
        case RESERVED_FOR_FUTURE_HEAD_28:
        case RESERVED_FOR_FUTURE_HEAD_29:
        case RESERVED_FOR_FUTURE_HEAD_30:
        case RESERVED_FOR_FUTURE_HEAD_31:
        case RESERVED_FOR_FUTURE_HEAD_32:
        case RESERVED_FOR_FUTURE_HEAD_33:
        case RESERVED_FOR_FUTURE_HEAD_34:
        case RESERVED_FOR_FUTURE_HEAD_35:
        case RESERVED_FOR_FUTURE_HEAD_36:
        case RESERVED_FOR_FUTURE_HEAD_37:
            break;
        case SECOND_MR_HEAD_RESISTANCE:
        {
            for (loopCount = 0; loopCount < m_heads; ++loopCount)
            {
#if defined _DEBUG
                printf("\tSecond MR Head Resistance by Head %" PRIu32":      %" PRIu64" \n", loopCount, vFarmFrame[page].secondMRHeadResistanceByHead.headValue[loopCount] & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Second MR Head Resistance
#endif
                std::ostringstream header;
                header << "Second MR Head Resistance by Head " << std::dec << loopCount;     // Head count
                set_json_64_bit_With_Status(headPage, header.str().c_str(), vFarmFrame[page].secondMRHeadResistanceByHead.headValue[loopCount], false, m_showStatusBits);  //!< Second MR Head Resistance
            }
        }
        break;

        case RESERVED_FOR_FUTURE_HEAD_38:
        case RESERVED_FOR_FUTURE_HEAD_39:
        case RESERVED_FOR_FUTURE_HEAD_40:
        case RESERVED_FOR_FUTURE_HEAD_41:
        case RESERVED_FOR_FUTURE_HEAD_42:
        case RESERVED_FOR_FUTURE_HEAD_43:
        case RESERVED_FOR_FUTURE_HEAD_44:
        case RESERVED_FOR_FUTURE_HEAD_45:
        case RESERVED_FOR_FUTURE_HEAD_46:
        case RESERVED_FOR_FUTURE_HEAD_47:
        case RESERVED_FOR_FUTURE_HEAD_48:
        case RESERVED_FOR_FUTURE_HEAD_49:
        case LUN_0_ACTUATOR:
        case LUN_0_FLASH_LED:
        case LUN_REALLOCATION_0:
        case RESERVED_FOR_FUTURE_EXPANSION_42:
        case RESERVED_FOR_FUTURE_EXPANSION_43:
        case RESERVED_FOR_FUTURE_EXPANSION_44:
        case RESERVED_FOR_FUTURE_EXPANSION_45:
        case RESERVED_FOR_FUTURE_EXPANSION_46:
        case RESERVED_FOR_FUTURE_EXPANSION_47:
        case RESERVED_FOR_FUTURE_EXPANSION_48:
        case RESERVED_FOR_FUTURE_EXPANSION_49:
        case RESERVED_FOR_FUTURE_EXPANSION_50:
        case RESERVED_FOR_FUTURE_EXPANSION_51:
        case RESERVED_FOR_FUTURE_EXPANSION_52:
        case RESERVED_FOR_FUTURE_EXPANSION_53:
        case RESERVED_FOR_FUTURE_EXPANSION_54:
        case LUN_1_ACTUATOR:
        case LUN_1_FLASH_LED:
        case LUN_REALLOCATION_1:
        case RESERVED_FOR_FUTURE_EXPANSION_61:
        case RESERVED_FOR_FUTURE_EXPANSION_62:
        case RESERVED_FOR_FUTURE_EXPANSION_63:
        case RESERVED_FOR_FUTURE_EXPANSION_64:
        case RESERVED_FOR_FUTURE_EXPANSION_65:
        case RESERVED_FOR_FUTURE_EXPANSION_66:
        case RESERVED_FOR_FUTURE_EXPANSION_67:
        case RESERVED_FOR_FUTURE_EXPANSION_68:
        case RESERVED_FOR_FUTURE_EXPANSION_69:
        case RESERVED_FOR_FUTURE_EXPANSION_70:
        case RESERVED_FOR_FUTURE_EXPANSION_71:
        case RESERVED_FOR_FUTURE_EXPANSION_72:
        case RESERVED_FOR_FUTURE_EXPANSION_73:
        case RESERVED_FOR_FUTURE_EXPANSION_74:
        case LUN_2_ACTUATOR:
        case LUN_2_FLASH_LED:
        case LUN_REALLOCATION_2:
        case RESERVED_FOR_FUTURE_EXPANSION_81:
        case RESERVED_FOR_FUTURE_EXPANSION_82:
        case RESERVED_FOR_FUTURE_EXPANSION_83:
        case RESERVED_FOR_FUTURE_EXPANSION_84:
        case RESERVED_FOR_FUTURE_EXPANSION_85:
        case RESERVED_FOR_FUTURE_EXPANSION_86:
        case RESERVED_FOR_FUTURE_EXPANSION_87:
        case RESERVED_FOR_FUTURE_EXPANSION_88:
        case RESERVED_FOR_FUTURE_EXPANSION_89:
        case RESERVED_FOR_FUTURE_EXPANSION_90:
        case RESERVED_FOR_FUTURE_EXPANSION_91:
        case RESERVED_FOR_FUTURE_EXPANSION_92:
        case RESERVED_FOR_FUTURE_EXPANSION_93:
        case RESERVED_FOR_FUTURE_EXPANSION_94:
        case LUN_3_ACTUATOR:
        case LUN_3_FLASH_LED:
        case LUN_REALLOCATION_3:
        case RESERVED_FOR_FUTURE_EXPANSION_101:
        case RESERVED_FOR_FUTURE_EXPANSION_102:
        case RESERVED_FOR_FUTURE_EXPANSION_103:
        case RESERVED_FOR_FUTURE_EXPANSION_104:
        case RESERVED_FOR_FUTURE_EXPANSION_105:
        case RESERVED_FOR_FUTURE_EXPANSION_106:
        case RESERVED_FOR_FUTURE_EXPANSION_107:
        case RESERVED_FOR_FUTURE_EXPANSION_108:
        case RESERVED_FOR_FUTURE_EXPANSION_109:
        case RESERVED_FOR_FUTURE_EXPANSION_110:
        case RESERVED_FOR_FUTURE_EXPANSION_111:
        case RESERVED_FOR_FUTURE_EXPANSION_112:
        case RESERVED_FOR_FUTURE_EXPANSION_113:
        case RESERVED_FOR_FUTURE_EXPANSION_114:
        case RESERVED_FOR_FUTURE_EXPANSION:
            break;
        default:
#if defined _DEBUG
            printf("\tSomething went wrong, ERROR \n");
#endif
            break;
        }
    }
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_Information(JSONNODE *LUNData, uint32_t page, uint16_t actNum)
{
    sLUNStruct *pLUN;
    //pLUN = &vFarmFrame[page].vLUN50;
    if (actNum == LUN_1_ACTUATOR)
        pLUN = &vFarmFrame[page].vLUN60;
    else if (actNum == LUN_2_ACTUATOR)
        pLUN = &vFarmFrame[page].vLUN70;
    else if (actNum == LUN_3_ACTUATOR)
        pLUN = &vFarmFrame[page].vLUN80;
    else
        pLUN = &vFarmFrame[page].vLUN50;

#if defined _DEBUG
    if (pLUN->copyNumber == FACTORYCOPY)
    {
        printf("LUN Actuator 0x%" PRIx16" Information From Farm Log copy FACTORY", M_Word0(pLUN->LUNID));
    }
    else
    {
        printf("\nLUN Actuator 0x%" PRIx16" Information From Farm Log copy: %" PRIu32"\n", M_Word0(pLUN->LUNID), page);
    }
    printf("\tPage Number:                                0x%" PRIx64" \n", pLUN->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Page Number 
    printf("\tCopy Number:                                  %" PRIu64" \n", pLUN->copyNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Copy Number 
    printf("\tLUN ID:                                       %" PRIu64" \n", pLUN->LUNID & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< LUN ID  
    printf("\tHead Load Events:                             %" PRIu64" \n", pLUN->headLoadEvents & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Head Load Events 
    if (pLUN->reallocatedSectors != 0)
    {
        printf("\tNumber of Reallocated Sectors:                %" PRIu64" \n", pLUN->reallocatedSectors & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Number of Reallocated Sectors 
    }
    if (pLUN->reallocatedCandidates != 0)
    {
        printf("\tNumber of Reallocated Candidate Sectors:      %" PRIu64" \n", pLUN->reallocatedCandidates & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Number of Reallocated Candidate Sectors 
    }
    printf("\tNumber of Reallocated Candidate Sectors:      %" PRIu64" \n", pLUN->reallocatedCandidates & UINT64_C(0x00FFFFFFFFFFFFFF));        //!< Number of Reallocated Candidate Sectors 
    printf("\tTimeStamp of last IDD test:                   %" PRIu64" \n", pLUN->timeStampOfIDD & UINT64_C(0x00FFFFFFFFFFFFFF));               //!< Timestamp of last IDD test 
    printf("\tSub-Command of Last IDD Test:                 %" PRIu64" \n", pLUN->subCmdOfIDD & UINT64_C(0x00FFFFFFFFFFFFFF));                  //!< Sub-command of last IDD test 
    printf("\tNumber of Reallocated Sector Reclamations:    %" PRIu64" \n", pLUN->reclamedGlist & UINT64_C(0x00FFFFFFFFFFFFFF));				//!< Number of G-list reclamations 
    printf("\tServo Status:                                 %" PRIu64" \n", pLUN->servoStatus & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< Servo Status 
    printf("\tNumber of Slipped Sectors Before IDD Scan:    %" PRIu64" \n", pLUN->slippedSectorsBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Number of Slipped Sectors Before IDD Scan 
    printf("\tNumber of Slipped Sectors After IDD Scan:     %" PRIu64" \n", pLUN->slippedSectorsAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));       //!< Number of Slipped Sectors After IDD Scan 
    printf("\tResident Reallocated Sectors Before IDD Scan: %" PRIu64" \n", pLUN->residentReallocatedBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Number of Resident Reallocated Sectors Before IDD Scan 
    printf("\tResident Reallocated Sectors After IDD Scan:  %" PRIu64" \n", pLUN->residentReallocatedAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));  //!< Number of Resident Reallocated Sectors After IDD Scan  
    printf("\tSuccessfully Scrubbed Sectors Before IDD Scan:%" PRIu64" \n", pLUN->successScrubbedBeforeIDD & UINT64_C(0x00FFFFFFFFFFFFFF));     //!< Number of Successfully Scrubbed Sectors Before IDD Scan 
    printf("\tSuccessfully Scrubbed Sectors After IDD Scan: %" PRIu64" \n", pLUN->successScrubbedAfterIDD & UINT64_C(0x00FFFFFFFFFFFFFF));      //!< Number of Successfully Scrubbed Sectors After IDD Scan 
    printf("\tNumber of DOS Scans Performed:                %" PRIu64" \n", pLUN->dosScansPerformed & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of DOS Scans Performed
    printf("\tNumber of LBAs Corrected by ISP:              %" PRIu64" \n", pLUN->correctedLBAbyISP & UINT64_C(0x00FFFFFFFFFFFFFF));            //!< Number of LBAs Corrected by ISP
    printf("\tNumber of Valid Parity Sectors:               %" PRIu64" \n", pLUN->paritySectors & UINT64_C(0x00FFFFFFFFFFFFFF));                //!< Number of Valid Parity Sectors
    printf("\tRV Absulute Mean:                             %" PRIu64" \n", pLUN->RVabsolue & UINT64_C(0x00FFFFFFFFFFFFFF));					//!< RV Absulute Mean
    printf("\tMax RV absulute Mean:                         %" PRIu64" \n", pLUN->maxRVabsolue & UINT64_C(0x00FFFFFFFFFFFFFF));				    //!< Max RV absulute Mean
    printf("\tIdle Time:                                    %" PRIu64" \n", pLUN->idleTime & UINT64_C(0x00FFFFFFFFFFFFFF));		                //!< idle Time value from the most recent SMART Summary Frame
    printf("\tNumber of LBAs Corrected by Parity Sector:    %" PRIu64" \n", pLUN->lbasCorrectedByParity & UINT64_C(0x00FFFFFFFFFFFFFF)); //!< Number of LBAs Corrected by Parity Sector
#endif

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

    set_json_64_bit_With_Status(pageInfo, "Page Number", pLUN->pageNumber, true, m_showStatusBits);							                        //!< Page Number 
    set_json_64_bit_With_Status(pageInfo, "Copy Number ", pLUN->copyNumber, false, m_showStatusBits);						                            //!< Copy Number 
    set_json_64_bit_With_Status(pageInfo, "LUN Actuator ID", pLUN->LUNID, false, m_showStatusBits);						                                //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Head Load Events", pLUN->headLoadEvents, false, m_showStatusBits);											//!< Head Load Events 
    if (pLUN->reallocatedSectors != 0)
    {
        set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", pLUN->reallocatedSectors, false, m_showStatusBits);					        //!< Number of Reallocated Sectors 
        set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors Candidate", pLUN->reallocatedCandidates, false, m_showStatusBits);				//!< Number of Reallocated Candidate Sectors  
    }
    set_json_64_bit_With_Status(pageInfo, "TimeStamp of last IDD test", pLUN->timeStampOfIDD, false, m_showStatusBits);						            //!< Timestamp of last IDD test  
    set_json_64_bit_With_Status(pageInfo, "Sub-Command of Last IDD Test", pLUN->subCmdOfIDD, false, m_showStatusBits);				                    //!< Sub-command of last IDD test 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sector Reclamations", pLUN->reclamedGlist, false, m_showStatusBits);				                //!< Number of G-list reclamations  
    set_json_64_bit_With_Status(pageInfo, "Servo Status", pLUN->servoStatus, false, m_showStatusBits);				                                    //!< Servo Status  
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors Before IDD Scan", pLUN->slippedSectorsBeforeIDD, false, m_showStatusBits);			//!< Number of Slipped Sectors Before IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Number of Slipped Sectors After IDD Scan", pLUN->slippedSectorsAfterIDD, false, m_showStatusBits);			//!< Number of Slipped Sectors After IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors Before IDD Scan", pLUN->residentReallocatedBeforeIDD, false, m_showStatusBits);	//!< Number of Resident Reallocated Sectors Before IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Number of Resident Reallocated Sectors After IDD Scan", pLUN->residentReallocatedAfterIDD, false, m_showStatusBits);	//!< Number of Resident Reallocated Sectors After IDD Scan
    set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors Before IDD Scan", pLUN->successScrubbedBeforeIDD, false, m_showStatusBits);	//!< Number of Successfully Scrubbed Sectors Before IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Successfully Scrubbed Sectors After IDD Scan", pLUN->successScrubbedAfterIDD, false, m_showStatusBits);		//!< Number of Successfully Scrubbed Sectors After IDD Scan 
    set_json_64_bit_With_Status(pageInfo, "Number of DOS Scans Performed", pLUN->dosScansPerformed, false, m_showStatusBits);	                        //!< Number of DOS Scans Performed 
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by ISP", pLUN->correctedLBAbyISP, false, m_showStatusBits);                         //!< Number of LBAs Corrected by ISP  
    set_json_64_bit_With_Status(pageInfo, "Number of Valid Parity Sectors", pLUN->paritySectors, false, m_showStatusBits);                              //!< Number of Valid Parity Sectors  
    set_json_64_bit_With_Status(pageInfo, "RV Absolute Mean", pLUN->RVabsolue, false, m_showStatusBits);									            //!< RV Absulute Mean
    set_json_64_bit_With_Status(pageInfo, "Max RV Absolute Mean", pLUN->maxRVabsolue, false, m_showStatusBits);							                //!< Max RV absulute Mean 
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(3) << (static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(pLUN->idleTime)) * 1.0) / 3600);
    set_json_string_With_Status(pageInfo, "Idle Time (hours)", temp.str().c_str(), pLUN->idleTime, m_showStatusBits);                                 //!< idle Time value from the most recent SMART Summary Frame
    set_json_64_bit_With_Status(pageInfo, "Number of LBAs Corrected by Parity Sector", pLUN->lbasCorrectedByParity, false, m_showStatusBits);           //!< Number of LBAs Corrected by Parity Sector
    set_json_64_bit_With_Status(pageInfo, "current low vibe score",   pLUN->currentLowFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "current mid vibe score",   pLUN->currentMidFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "current high vibe score",  pLUN->currentHighFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "worst low vibe score",  pLUN->worstLowFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "worst mid vibe score",  pLUN->worstMidFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "worst high vibe score", pLUN->worstHighFrequencyVibe, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "primary super parity coverage",  pLUN->primarySPCovPercentage, false, m_showStatusBits);
    set_json_64_bit_With_Status(pageInfo, "primary super parity coverage SMR",  pLUN->primarySPCovPercentageSMR, false, m_showStatusBits);
    json_push_back(LUNData, pageInfo);
    
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_FLED_Info(JSONNODE *LUNFLED, uint32_t page,  uint16_t actNum)
{

    uint16_t i = 0;
    sActuatorFLEDInfo *pFLED;
    if (actNum == LUN_1_FLASH_LED)
        pFLED = &vFarmFrame[page].fled61;
    else if (actNum == LUN_2_FLASH_LED)
        pFLED = &vFarmFrame[page].fled71;
    else if (actNum == LUN_3_FLASH_LED)
        pFLED = &vFarmFrame[page].fled81;
    else
        pFLED = &vFarmFrame[page].fled51;

#if defined _DEBUG
    if (pFLED->copyNumber == FACTORYCOPY)
    {
        printf("Actuator 0x%" PRIx16" FLED Info From Farm Log copy FACTORY", M_Word0(pFLED->actID));
    }
    else
    {
        printf("\nActuator 0x%" PRIx16" FLED Info From Farm Log copy %" PRIu32"\n", M_Word0(pFLED->actID), page);
    }
    printf("\tPage Number:                                  0x%" PRIx64" \n", pFLED->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Page Number 
    printf("\tCopy Number:                                  %" PRIu64" \n", pFLED->copyNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Copy Number 
    printf("\tActuator ID:                                  %" PRIu64" \n", pFLED->actID & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Actuator ID  
    printf("\tTotal Flash LED Events:                       %" PRIu64" \n", pFLED->totalFLEDEvents & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< Total Flash LED Events
    printf("\tIndex of Last Flash LED:                      %" PRIu64" \n", pFLED->index & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Index of last entry in FLED Info array below, in case the array wraps
    for (i = 0; i < FLASH_EVENTS; i++)
    {

        printf("\tInformation of Event %" PRIu16":             0x%" PRIx64" \n", i,pFLED->flashLEDArray[i] & UINT64_C(0x00FFFFFFFFFFFFFF));           //!< Info on the last 8 Flash LED (assert) Events, wrapping array
        printf("\tTimeStamp of Event%" PRIu16":                  %" PRIu64" \n", i,pFLED->timestampForLED[i] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array
        printf("\tPower Cycle Event %" PRIu16":                  %" PRIu64" \n", i,pFLED->powerCycleOfLED[i] & UINT64_C(0x00FFFFFFFFFFFFFF));         //!< SPower Cycle of the last 8 Flash LED (assert) Events, wrapping array
    }


#endif
    
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

        set_json_64_bit_With_Status(eventInfo, "Address of Event", pFLED->flashLEDArray[i], true, m_showStatusBits);	           //!< Info on the last 8 Flash LED (assert) Events, wrapping array
        temp.str("");
        temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) <<  M_Word2(check_Status_Strip_Status(pFLED->flashLEDArray[i]));
        json_push_back(eventInfo, json_new_a("Flash LED Code", temp.str().c_str()));
        std::string meaning;
        //get_Assert_Code_Meaning(meaning, M_Word2(check_Status_Strip_Status(pFLED->flashLEDArray[i])));
        //json_push_back(eventInfo, json_new_a("Flash LED Code Meaning", meaning.c_str()));
        temp.str("");
        temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(check_Status_Strip_Status(pFLED->flashLEDArray[i]));
        json_push_back(eventInfo, json_new_a("Flash LED Address", temp.str().c_str()));
        temp.str("");
        temp.clear();
        temp << "TimeStamp of Event(hours) " << std::dec << i;
        std::ostringstream time;
        time << std::setprecision(3) << (static_cast<double>(M_DoubleWord0(check_Status_Strip_Status(pFLED->timestampForLED[i])) / 3600000) * .001);
        set_json_string_With_Status(eventInfo, temp.str().c_str(), time.str().c_str(), pFLED->timestampForLED[i], m_showStatusBits);//!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array
        temp.str("");
        temp.clear();
        temp << "Power Cycle Event " << std::dec << i;
        set_json_64_bit_With_Status(eventInfo, temp.str(), pFLED->powerCycleOfLED[i], false, m_showStatusBits);	         //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array

        json_push_back(pageInfo, eventInfo);
    }

    json_push_back(LUNFLED, pageInfo);
    
    return SUCCESS;
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
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSCSI_Farm_Log::print_LUN_Actuator_Reallocation(JSONNODE* LUNNReallocation, uint32_t page, uint16_t actNum)
{
    uint16_t i = 0;
    sActReallocationData* pReal;
    if (actNum == LUN_REALLOCATION_1)
        pReal = &vFarmFrame[page].reall62;
    else if (actNum == LUN_REALLOCATION_2)
        pReal = &vFarmFrame[page].reall72;
    else if (actNum == LUN_REALLOCATION_3)
        pReal = &vFarmFrame[page].reall82;
    else
        pReal = &vFarmFrame[page].reall52;

#if defined _DEBUG
    if (pReal->copyNumber == FACTORYCOPY)
    {
        printf("LUN Actuator 0x%" PRIx16" Reallocation From Farm Log copy FACTORY", M_Word0(pReal->actID));
    }
    else
    {
        printf("\nLUN Actuator 0x%" PRIx16" Reallocation From Farm Log copy %" PRIu32"\n", M_Word0(pReal->actID), page);
    }
    printf("\tPage Number:                                  0x%" PRIx64" \n", pReal->pageNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Page Number 
    printf("\tCopy Number:                                  %" PRIu64" \n", pReal->copyNumber & UINT64_C(0x00FFFFFFFFFFFFFF));                   //!< Copy Number 
    printf("\tActuator ID:                                  %" PRIu64" \n", pReal->actID & UINT64_C(0x00FFFFFFFFFFFFFF));                        //!< Actuator ID  
    printf("\tNumber of Reallocated Sectors:                %" PRIu64" \n", pReal->numberReallocatedSectors & UINT64_C(0x00FFFFFFFFFFFFFF));              //!< Total Flash LED Events
    printf("\tNumber of Reallocated Candidate Sectors:      %" PRIu64" \n", pReal->numberReallocatedCandidates & UINT64_C(0x00FFFFFFFFFFFFFF));
    for (i = 0; i < REALLOCATIONEVENTS; i++)
    {
        //std::string meaning;
        //get_Reallocation_Cause_Meanings(meaning, i);
        //printf("\t%-33s:            %" PRIu64" \n", meaning.c_str(), pReal->reallocatedCauses[i] & UINT64_C(0x00FFFFFFFFFFFFFF));
    }
#endif
 
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

    set_json_64_bit_With_Status(pageInfo, "Page Number", pReal->pageNumber, true, m_showStatusBits);							                        //!< Page Number 
    set_json_64_bit_With_Status(pageInfo, "Copy Number", pReal->copyNumber, false, m_showStatusBits);						                            //!< Copy Number 
    set_json_64_bit_With_Status(pageInfo, "Actuator ID", pReal->actID, true, m_showStatusBits);						                                        //!< LUN ID 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Sectors", pReal->numberReallocatedSectors, false, m_showStatusBits);											//!< Head Load Events 
    set_json_64_bit_With_Status(pageInfo, "Number of Reallocated Candidate Sectors", pReal->numberReallocatedCandidates, false, m_showStatusBits);

    for (i = 0; i < REALLOCATIONEVENTS; i++)
    {
        //std::string meaning;
        //get_Reallocation_Cause_Meanings(meaning, i);
        //set_json_64_bit_With_Status(pageInfo, meaning, pReal->reallocatedCauses[i], false, m_showStatusBits);
    }
    json_push_back(LUNNReallocation, pageInfo);
    
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
void CSCSI_Farm_Log::print_All_Pages(JSONNODE *masterData)
{
    
    if (vFarmFrame.size() > 0)
    {
        print_Header(masterData);
        JSONNODE* headPage;
        if (g_dataformat == PREPYTHON_DATA)
        {
            headPage = masterData;
        }
        else
        {
            headPage = json_new(JSON_NODE);
            json_set_name(headPage, "Head Information From Farm Log copy 0");
        }
        
        for (uint32_t index = 0; index < vFarmFrame.size(); ++index)
        {

            for (uint32_t pramCode = 0; pramCode < vFarmFrame.at(index).vFramesFound.size(); pramCode++)
            {
                switch (vFarmFrame.at(index).vFramesFound.at(pramCode))
                {
                case FARM_HEADER_PARAMETER:
                    //get the Farm Header information;
                    break;
                case  GENERAL_DRIVE_INFORMATION_PARAMETER:
                    print_Drive_Information(masterData, index);                   // get the id drive information at the time.
                    break;
                case  WORKLOAD_STATISTICS_PARAMETER:
                    print_WorkLoad(masterData, index);                           // get the work load information
                    break;
                case ERROR_STATISTICS_PARAMETER:
                {
                    if (m_MajorRev < 4)
                    {
                        print_Error_Information(masterData, index);                    // get the error status
                    }
                    else
                    {
                        print_Error_Information_Version_4(masterData, index);
                    }
                }
                break;
                case ENVIRONMENTAL_STATISTICS_PARAMETER:
                    print_Enviroment_Information(masterData, index);               // get the envirmonent information 
                    break;
                case RELIABILITY_STATISTICS_PARAMETER:
                    print_Reli_Information(masterData, index);         // get the Reliabliity stat
                    break;
                case GENERAL_DRIVE_INFORMATION_06:
                    print_General_Drive_Information_Continued(masterData, index);
                    break;
                case ENVIRONMENT_STATISTICS_PAMATER_07:
                    print_Enviroment_Statistics_Page_07(masterData, index);
                    break;
                case WORKLOAD_STATISTICS_PAMATER_08:
                    print_Workload_Statistics_Page_08(masterData, index);
                    break;
                case RESERVED_FOR_FUTURE_STATISTICS_4:
                case RESERVED_FOR_FUTURE_STATISTICS_5:
                case RESERVED_FOR_FUTURE_STATISTICS_6:
                case RESERVED_FOR_FUTURE_STATISTICS_7:
                case RESERVED_FOR_FUTURE_STATISTICS_8:
                case RESERVED_FOR_FUTURE_STATISTICS_9:
                case RESERVED_FOR_FUTURE_STATISTICS_10:
                case RESERVED_FOR_FUTURE_HEAD_1:
                case RESERVED_FOR_FUTURE_HEAD_2:
                case RESERVED_FOR_FUTURE_HEAD_3:
                case RESERVED_FOR_FUTURE_HEAD_4:
                case RESERVED_FOR_FUTURE_HEAD_5:
                case RESERVED_FOR_FUTURE_HEAD_6:
                case RESERVED_FOR_FUTURE_HEAD_7:
                case RESERVED_FOR_FUTURE_HEAD_8:
                case RESERVED_FOR_FUTURE_HEAD_9:
                case RESERVED_FOR_FUTURE_HEAD_10:
                    break;
                case MR_HEAD_RESISTANCE_FROM_MOST_RECENT_SMART_SUMMARY_FRAME_BY_HEAD:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case RESERVED_FOR_FUTURE_HEAD_11:
                case RESERVED_FOR_FUTURE_HEAD_12:
                case RESERVED_FOR_FUTURE_HEAD_13:
                case RESERVED_FOR_FUTURE_HEAD_14:
                    break;
                case CURRENT_H2SAT_AMPLITUDE_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                case CURRENT_H2SAT_ASYMMETRY_BY_HEAD_AVERAGED_ACROSS_TEST_ZONES:
                case NUMBER_OF_RESIDENT_GLIST_ENTRIES:
                case NUMBER_OF_PENDING_ENTRIES:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case RESERVED_FOR_FUTURE_HEAD_15:
                case RESERVED_FOR_FUTURE_HEAD_16:
                case RESERVED_FOR_FUTURE_HEAD_17:
                    break;
                case WRITE_POWERON_HOURS_FROM_MOST_RECENT_SMART:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case RESERVED_FOR_FUTURE_HEAD_18:
                    break;
                case CUM_LIFETIME_UNRECOVERALBE_READ_REPET_PER_HEAD:
                case CUM_LIFETIME_UNRECOVERABLE_READ_UNIQUE_PER_HEAD:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case RESERVED_FOR_FUTURE_HEAD_19:
                case RESERVED_FOR_FUTURE_HEAD_20:
                case RESERVED_FOR_FUTURE_HEAD_21:
                case RESERVED_FOR_FUTURE_HEAD_22:
                case RESERVED_FOR_FUTURE_HEAD_23:
                case RESERVED_FOR_FUTURE_HEAD_24:
                    break;
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_0:
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_1:
                case CURRENT_H2SAT_TRIMMED_MEAN_BITS_IN_ERROR_BY_HEAD_BY_TEST_ZONE_2:
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_0:
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_1:
                case CURRENT_H2SAT_ITERATIONS_TO_CONVERGE_BY_HEAD_BY_TEST_ZONE_2:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);
                    break;
                case RESERVED_FOR_FUTURE_HEAD_25:
                case RESERVED_FOR_FUTURE_HEAD_26:
                case RESERVED_FOR_FUTURE_HEAD_27:
                case RESERVED_FOR_FUTURE_HEAD_28:
                case RESERVED_FOR_FUTURE_HEAD_29:
                case RESERVED_FOR_FUTURE_HEAD_30:
                case RESERVED_FOR_FUTURE_HEAD_31:
                case RESERVED_FOR_FUTURE_HEAD_32:
                case RESERVED_FOR_FUTURE_HEAD_33:
                case RESERVED_FOR_FUTURE_HEAD_34:
                case RESERVED_FOR_FUTURE_HEAD_35:
                case RESERVED_FOR_FUTURE_HEAD_36:
                case RESERVED_FOR_FUTURE_HEAD_37:
                    break;
                case SECOND_MR_HEAD_RESISTANCE:
                    print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), headPage, index);   
                    break;
                case RESERVED_FOR_FUTURE_HEAD_38:
                case RESERVED_FOR_FUTURE_HEAD_39:
                case RESERVED_FOR_FUTURE_HEAD_40:
                case RESERVED_FOR_FUTURE_HEAD_41:
                case RESERVED_FOR_FUTURE_HEAD_42:
                case RESERVED_FOR_FUTURE_HEAD_43:
                case RESERVED_FOR_FUTURE_HEAD_44:
                case RESERVED_FOR_FUTURE_HEAD_45:
                case RESERVED_FOR_FUTURE_HEAD_46:
                case RESERVED_FOR_FUTURE_HEAD_47:
                case RESERVED_FOR_FUTURE_HEAD_48:
                case RESERVED_FOR_FUTURE_HEAD_49:
                    break;
                case LUN_0_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, LUN_0_ACTUATOR);
                    break;
                case LUN_0_FLASH_LED :
                    print_LUN_Actuator_FLED_Info(masterData,index,LUN_0_FLASH_LED);
                    break;
                case LUN_REALLOCATION_0:
                    print_LUN_Actuator_Reallocation(masterData, index, LUN_REALLOCATION_0);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_42:
                case RESERVED_FOR_FUTURE_EXPANSION_43:
                case RESERVED_FOR_FUTURE_EXPANSION_44:
                case RESERVED_FOR_FUTURE_EXPANSION_45:
                case RESERVED_FOR_FUTURE_EXPANSION_46:
                case RESERVED_FOR_FUTURE_EXPANSION_47:
                case RESERVED_FOR_FUTURE_EXPANSION_48:
                case RESERVED_FOR_FUTURE_EXPANSION_49:
                case RESERVED_FOR_FUTURE_EXPANSION_50:
                case RESERVED_FOR_FUTURE_EXPANSION_51:
                case RESERVED_FOR_FUTURE_EXPANSION_52:
                case RESERVED_FOR_FUTURE_EXPANSION_53:
                case RESERVED_FOR_FUTURE_EXPANSION_54:
                    break;
                case LUN_1_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, LUN_1_ACTUATOR);
                    break;
                case LUN_1_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index, LUN_1_FLASH_LED);
                    break;
                case LUN_REALLOCATION_1:
                    print_LUN_Actuator_Reallocation(masterData, index,LUN_REALLOCATION_1);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_61:
                case RESERVED_FOR_FUTURE_EXPANSION_62:
                case RESERVED_FOR_FUTURE_EXPANSION_63:
                case RESERVED_FOR_FUTURE_EXPANSION_64:
                case RESERVED_FOR_FUTURE_EXPANSION_65:
                case RESERVED_FOR_FUTURE_EXPANSION_66:
                case RESERVED_FOR_FUTURE_EXPANSION_67:
                case RESERVED_FOR_FUTURE_EXPANSION_68:
                case RESERVED_FOR_FUTURE_EXPANSION_69:
                case RESERVED_FOR_FUTURE_EXPANSION_70:
                case RESERVED_FOR_FUTURE_EXPANSION_71:
                case RESERVED_FOR_FUTURE_EXPANSION_72:
                case RESERVED_FOR_FUTURE_EXPANSION_73:
                case RESERVED_FOR_FUTURE_EXPANSION_74:
                    break;
                case LUN_2_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index, LUN_2_ACTUATOR);
                    break;
                case LUN_2_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index,  LUN_2_FLASH_LED);
                    break;
                case LUN_REALLOCATION_2:
                    print_LUN_Actuator_Reallocation(masterData, index, LUN_REALLOCATION_2);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_81:
                case RESERVED_FOR_FUTURE_EXPANSION_82:
                case RESERVED_FOR_FUTURE_EXPANSION_83:
                case RESERVED_FOR_FUTURE_EXPANSION_84:
                case RESERVED_FOR_FUTURE_EXPANSION_85:
                case RESERVED_FOR_FUTURE_EXPANSION_86:
                case RESERVED_FOR_FUTURE_EXPANSION_87:
                case RESERVED_FOR_FUTURE_EXPANSION_88:
                case RESERVED_FOR_FUTURE_EXPANSION_89:
                case RESERVED_FOR_FUTURE_EXPANSION_90:
                case RESERVED_FOR_FUTURE_EXPANSION_91:
                case RESERVED_FOR_FUTURE_EXPANSION_92:
                case RESERVED_FOR_FUTURE_EXPANSION_93:
                case RESERVED_FOR_FUTURE_EXPANSION_94:
                    break;
                case LUN_3_ACTUATOR:
                    print_LUN_Actuator_Information(masterData, index,LUN_3_ACTUATOR); 
                    break;
                case LUN_3_FLASH_LED:
                    print_LUN_Actuator_FLED_Info(masterData, index, LUN_3_FLASH_LED);
                    break;
                case LUN_REALLOCATION_3:
                    print_LUN_Actuator_Reallocation(masterData, index, LUN_REALLOCATION_3);
                    break;
                case RESERVED_FOR_FUTURE_EXPANSION_101:
                case RESERVED_FOR_FUTURE_EXPANSION_102:
                case RESERVED_FOR_FUTURE_EXPANSION_103:
                case RESERVED_FOR_FUTURE_EXPANSION_104:
                case RESERVED_FOR_FUTURE_EXPANSION_105:
                case RESERVED_FOR_FUTURE_EXPANSION_106:
                case RESERVED_FOR_FUTURE_EXPANSION_107:
                case RESERVED_FOR_FUTURE_EXPANSION_108:
                case RESERVED_FOR_FUTURE_EXPANSION_109:
                case RESERVED_FOR_FUTURE_EXPANSION_110:
                case RESERVED_FOR_FUTURE_EXPANSION_111:
                case RESERVED_FOR_FUTURE_EXPANSION_112:
                case RESERVED_FOR_FUTURE_EXPANSION_113:
                case RESERVED_FOR_FUTURE_EXPANSION_114:
                case RESERVED_FOR_FUTURE_EXPANSION:
                    break;
                default:
                    // No head data. We don't want to go into the head data
                    //print_Head_Information(vFarmFrame.at(index).vFramesFound.at(pramCode), masterData, index);
                    break;
                }
            }
        }
        if (g_dataformat != PREPYTHON_DATA)
        {
            json_push_back(masterData, headPage);
        }
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
            if (g_dataformat == PREPYTHON_DATA)
            {
                for (uint32_t frame = 0; frame < vFarmFrame.at(page).vFramesFound.size(); frame++)
                {
                    print_Head_Information(vFarmFrame.at(page).vFramesFound.at(frame), masterData, page);
                }
            }
            else
            {

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
}
