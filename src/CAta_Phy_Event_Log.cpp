//
// CAta_Phy_Event_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_Phy_Event_Log.h"


using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CAtaPhyEventLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAtaPhyEventLog::CAtaPhyEventLog()
	: m_name()
	, m_logSize(0)
	, m_status(IN_PROGRESS)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CAtaPhyEventLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAtaPhyEventLog::CAtaPhyEventLog(std::string& filename, JSONNODE* masterData)
	: m_name()
	, m_logSize(0)
	, m_status(IN_PROGRESS)
{
	

	CLog* cCLog;
	cCLog = new CLog(filename);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_logSize = cCLog->get_Size();
			cCLog->get_vBuffer(&v_Buff);

		}
		else
		{

			m_status = FAILURE;
		}
	}
	else
	{
		m_status = cCLog->get_Log_Status();
	}
	delete (cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn CAtaPhyEventLog()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAtaPhyEventLog::CAtaPhyEventLog(uint8_t* bufferData, size_t logSize)
	: m_name()
	, m_logSize(0)
	, m_status(IN_PROGRESS)
	, v_Buff(&bufferData[0], &bufferData[logSize])
{
	if (!v_Buff.empty())
	{
		//is_Device_Scsi();
		//m_status = parse_Media_Cache_Log_with_No_Header(0);
	}
	else
	{
		m_status = FAILURE;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn ~CAtaPhyEventLog()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAtaPhyEventLog::~CAtaPhyEventLog()
{

}