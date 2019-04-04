//
// CScsi_Self_Test_Results_Log.cpp  Implementation of DST Results log 
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Self_Test_Results_Log.cpp Implementation of DST Results log 
//
#include "CScsi_Self_Test_Results_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsi_DST_Results()
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
CScsi_DST_Results::CScsi_DST_Results()
	: m_DSTName("Self Test Log")
	, m_DSTStatus(IN_PROGRESS)
	, m_PageLength(0)
    , m_SubPage(0)
	, m_DST()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_DSTName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsi_DST_Results()
//
//! \brief
//!   Description: Class constructor for the CScsi_DST_Results
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param masterData - the pointer to the json node that holds all of the json data
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsi_DST_Results::CScsi_DST_Results(uint8_t * buffer, size_t bufferSize, JSONNODE *masterData)
	: m_DSTName("Self Test Log")
	, m_DSTStatus(IN_PROGRESS)
	, m_PageLength(0)
    , m_SubPage(0)
	, m_DST(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_DSTName.c_str());
	}
	if (buffer != NULL)
	{
		m_DSTStatus = parse_Self_Test_Log(buffer, bufferSize, masterData);
	}
	else
	{
		m_DSTStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsi_DST_Results
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
CScsi_DST_Results::~CScsi_DST_Results()
{

}
//-----------------------------------------------------------------------------
//
//! \fn parse_Self_Test_Log()
//
//! \brief
//!   Description: parse the ext self test dst log 
//
//  Entry:
//! \param buffer -   pointer to the buffer data
//! \param masterData - json node for all the data
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CScsi_DST_Results::parse_Self_Test_Log(uint8_t *buffer, size_t bufferSize, JSONNODE *masterJson)
{
	eReturnValues retStatus = IN_PROGRESS;
	std::string myStr = "";
	myStr.resize(BASIC);
	JSONNODE *DstJson = json_new(JSON_NODE);


	json_set_name(DstJson, "Self Test Results Log");

	m_DST = (sSelfTest *)buffer;
	for (int i = 1; i <= 20; i++)
	{
		byte_Swap_Self_Test();
		if (m_DST->paramCode == i && m_DST->paramLength == 0x10)
		{	
			print_Self_Test_Log(DstJson, i);
			if (i == 20)
			{
				retStatus = SUCCESS;
			}
			else
			{
				if ( (((size_t)i+1) * sizeof(sSelfTest) ) < bufferSize )
				{
					m_DST++;
					retStatus = IN_PROGRESS;
				}
				else
				{
					json_push_back(masterJson, DstJson);
					return BAD_PARAMETER;
				}
			}
		}
		else
		{
			retStatus = BAD_PARAMETER;
			break;
		}
	}

	json_push_back(masterJson, DstJson);


	return retStatus;
}
//-----------------------------------------------------------------------------
//
//! \fn print_Self_Test_Log()
//
//! \brief
//!   Description: takes the data for each loop and produces the data for the json data.
//
//  Entry:
//! \param dstNode - json node each loop of the data
//! \param run - the loop number so we know where we are at
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsi_DST_Results::print_Self_Test_Log(JSONNODE *dstNode, uint16_t run)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	JSONNODE *runInfo = json_new(JSON_NODE);
	snprintf((char*)myStr.c_str(), BASIC, "Run %3d ", run);
	json_set_name(runInfo, (char*)myStr.c_str());
	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_DST->paramCode);
	json_push_back(runInfo, json_new_a("Parameter Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%" PRIx8"", m_DST->paramLength);
	json_push_back(runInfo, json_new_a("Parameter Length", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_DST->paramControlByte);
	json_push_back(runInfo, json_new_a("Control Byte", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", M_GETBITRANGE(m_DST->stCode, 7, 5));
	json_push_back(runInfo, json_new_a("Self Test Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", M_GETBITRANGE( m_DST->stCode,3,0));
	json_push_back(runInfo, json_new_a("Self Test Results", (char*)myStr.c_str()));
	if (M_GETBITRANGE(m_DST->stCode, 7, 5) == DST_NOT_RUN)
	{
		json_push_back(runInfo, json_new_a("Self Test Results Meaning", "Self Test Not Ran"));
	}
	else
	{
		get_Self_Test_Results_String(myStr, M_GETBITRANGE(m_DST->stCode, 3, 0));
		json_push_back(runInfo, json_new_a("Self Test Results Meaning", (char*)myStr.c_str()));
	}
	json_push_back(runInfo, json_new_i("Self Test Number", static_cast<uint32_t>(m_DST->stNumber)));
	snprintf((char*)myStr.c_str(), BASIC, "%" PRIu16"", m_DST->accPOH);
	json_push_back(runInfo, json_new_a("Accumulated Power On Hours", (char*)myStr.c_str()));
	set_json_64bit(runInfo, "Address of First Failure", m_DST->address, true);

	snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", m_DST->senseKey);
	json_push_back(runInfo, json_new_a("Sense Key", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", m_DST->addSenseCode);
	json_push_back(runInfo, json_new_a("Additaional Sense Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", m_DST->addSenseCodeQualifier);
	json_push_back(runInfo, json_new_a("Additional Sense Code Qualifier", (char*)myStr.c_str()));

	json_push_back(dstNode, runInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Self_Test_Results_String()
//
//! \brief
//!   Description: takes the data for each loop and produces the data for the json data.
//
//  Entry:
//! \param meaning - string to add verbage to what happened from the testing 
//! \param result - the result of the dst testing
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsi_DST_Results::get_Self_Test_Results_String( std::string & meaning, uint8_t result)
{
	meaning.resize(BASIC);
	switch (result)
	{
		case DST_COMPLETED_WITHOUT_ERROR:
		{
			meaning.assign( "Self Test completed without error.");
			break;
		}
		case DST_BACKGROUND_ABORTED:
		{
			meaning = "The background Self-Test was aborted by the application.";
			break;
		}
		case DST_ABORTED:
		{
			meaning = "The Self Test routine was aborted by an application.";
			break;
		}
		case DST_UNKNOWN_ERROR:
		{
			meaning = "An unknown error occurred.";
			break;
		}
		case DST_FAILURE_UNKNOWN_SEGMENT:
		{
			meaning = "Completed with a failure, segment that failed unknow.";
			break;
		}
		case DST_FAILURE_FIRST_SEGMENT:
		{
			meaning = "Completed with a failure, First segment failed.";
			break;
		}
		case DST_FAILURE_SECOND_SEGMENT:
		{
			meaning = "Completed with a failure, Second segment failed.";
			break;
		}
		case DST_FAILURE_CHECK_NUMBER_FOR_SEGMENT:
		{
			meaning = "Completed with a failure, indicated in Self Test Number.";
			break;
		}
		case DST_IN_PROGRESS:
		{
			meaning = "Self Test is in progress.";
			break;
		}
		default:
		{
			meaning = "Reserved.";
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn byte_Swap_Self_Test()
//
//! \brief
//!   Description: takes the data for each loop and produces the data for the json data.
//
//  Entry:
//! \param none
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsi_DST_Results::byte_Swap_Self_Test()
{
	byte_Swap_16(&m_DST->paramCode);
	byte_Swap_16(&m_DST->accPOH);
	byte_Swap_64(&m_DST->address);
}