// Opensea_Parser_Helper.cpp   Definition of SeaParser specific functions, structures
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file Opensea_Parser_Helper.cpp   Definition of SeaParser specific functions, structures
#include "Opensea_Parser_Helper.h"

eVerbosityLevels    g_verbosity = VERBOSITY_DEFAULT;
eDataFormat         g_dataformat = JSON_DATA;
bool                g_parseUnknown = true;
time_t              g_currentTime;
char                g_currentTimeString[64];
char                *g_currentTimeStringPtr = g_currentTimeString;


//-----------------------------------------------------------------------------
//
//! \fn get_SMART_Save_Flages()
//
//! \brief
//!   Description:  gets the command timeout values from the raw data
//
//  Entry:
//! \param headerNode - the json node that we will be adding the data to
//! \param flag - the uint8 value from the buffer
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
void opensea_parser::get_SMART_Save_Flages(JSONNODE *headerNode, uint8_t flag)
{
    switch (flag)
    {
    case SMART_FRAME_FLAG_SAVED_AT_MSI_PERIOD:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "MSI PERIOD"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_COLD_START:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Cold Start"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI Rezero Command"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_03_XX_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "03 XX Error"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_04_XX_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "04 XX Errror"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_1_5D_ERROR_FOR_PREDICTIVE_FAILURE:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "5D Error for Predictive Failure"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Factory Command"));
        break;
    case  SMART_FRAME_FLAG_SAVED_DUE_TO_SEND_DIAGNOSTIC_COMMAND_WITH_SELFTEST_BIT_SET:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Diag Command with Selftest bit set"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_0B_08_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI 0B 08 Error"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_09_XX_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI 09 XX Error"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND_WITH_HARD_RESET:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI Rezero Command with hard reset"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_0B_01_WARNING:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "0B 01 Warning"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_POWER_DOWN:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Power Down"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_BEGINNING_OF_SCSI_FORMAT_OPERATION:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI Format Operation"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_RESET_FLASH_LED_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Reset Flash Led Error"));
        break;
    case SMART_FRAME_FLAG_RESERVED:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Reserved"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_DOWNLOAD_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI Download Command"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_TRIP_ALGORITHM:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCSI Trip Algorithm"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_NHT_HEALTH_TRIP:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "NHT Health Trip"));
        break;
    case SMART_FRAME_FLAG_DUE_TO_ISSRA_WARNING:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "ISSRA Warning"));
        break;
    case SMART_FRAME_FLAG_DUE_TO_FACTORY_COMMAND_SAVE:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Factory Command Save"));
        break;
    case SMART_FRAME_FLAG_DUE_TO_EXIT_REMANUFACTURING:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Exit Remanufacturing"));
        break;
    case SMART_FRAME_FLAG_DUE_TO_HITACHI_ORM_OR_HD_TEST:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Hitachi ORM or HD test"));
        break;
    case SMART_FRAME_FLAG_STOP_UNIT_FRAME:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Unit Frame"));
        break;
    case SMART_FRAME_FLAG_INTERIM_FRAME_10MIN_IDLE_TIME:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "10 minute IDLE Time"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_WRITE_PROTECT_INVOCATION:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Write Protect Invocation"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND_TO_INVOCATE_A_FRAME_THAT_CAN_BE_INTERRUPTABLE:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Invocate a frame that can be interruptable"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SMART_SCT_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "SCT Command"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_HEAD_UNLOADING:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Head Unloading"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_START_OF_SANITIZE_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Start of Sanitize Command"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_END_OF_SANITIZE_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "End of Sanitize Command"));
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FORMAT_COMPLETION:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Format Completion"));
        break;
    case SMART_FRAME_FLAG_UNRECOVERED_WRITE_NVC_RESET_FRAME:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Unrecovered write NVC Reset Frame"));
        break;
    case SMART_FRAME_FLAG_QUICK_FORMAT_FINISH:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Quick Format Finish (SAS)"));
        break;
    case SMART_FRAME_FLAG_SATA_DEVICE_FAULT:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Sata Device Fault"));
        break;
    case SMART_FRAME_FLAG_SATA_UNCORRECTABLE_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Uncorrectable Error"));
        break;
    case SMART_FRAME_FLAG_SATA_ERC_UNCORRETABLE_ERROR:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "ERC Uncorrectable Error"));
        break;
    case SMART_FRAME_FLAG_DRIVE_SELF_TEST_COMPLETE_WITH_FAILURE:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "DST Test Completed with Failure"));
        break;
    case SMART_FRAME_FLAG_DEFECT_LIST_COMMAND:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Defect List Command"));
        break;
    case SMART_FRAME_FLAG_F7_CDB_TRIGGER :
        json_push_back(headerNode, json_new_a("SMART Save Flag", "F7 CDB Trigger"));
        break;
    default:
        json_push_back(headerNode, json_new_a("SMART Save Flag", "Did not match any value"));
        break;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_SMART_Save_Flages()
//
//! \brief
//!   Description:  gets the command timeout values from the raw data
//
//  Entry:
//! \param headerNode - the json node that we will be adding the data to
//! \param flag - the uint8 value from the buffer
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
void opensea_parser::get_SMART_Save_Flages_String(std::string& reason, uint8_t flag)
{
    switch (flag)
    {
    case SMART_FRAME_FLAG_SAVED_AT_MSI_PERIOD:
        reason =  "MSI PERIOD";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_COLD_START:
        reason = "Cold Start";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND:
        reason = "SCSI Rezero Command";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_03_XX_ERROR:
        reason = "03 XX Error";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_04_XX_ERROR:
        reason = "04 XX Errror";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_1_5D_ERROR_FOR_PREDICTIVE_FAILURE:
        reason = "5D Error for Predictive Failure";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND:
        reason = "Factory Command";
        break;
    case  SMART_FRAME_FLAG_SAVED_DUE_TO_SEND_DIAGNOSTIC_COMMAND_WITH_SELFTEST_BIT_SET:
        reason = "Diag Command with Selftest bit set";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_0B_08_ERROR:
        reason = "SCSI 0B 08 Error";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_09_XX_ERROR:
        reason = "SCSI 09 XX Error";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND_WITH_HARD_RESET:
        reason = "SCSI Rezero Command with hard reset";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_0B_01_WARNING:
        reason = "0B 01 Warning";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_POWER_DOWN:
        reason = "Power Down";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_BEGINNING_OF_SCSI_FORMAT_OPERATION:
        reason = "SCSI Format Operation";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_RESET_FLASH_LED_ERROR:
        reason = "Reset Flash Led Error";
        break;
    case SMART_FRAME_FLAG_RESERVED:
        reason = "Reserved";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_DOWNLOAD_COMMAND:
        reason = "SCSI Download Command";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_TRIP_ALGORITHM:
        reason = "SCSI Trip Algorithm";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_NHT_HEALTH_TRIP:
        reason = "NHT Health Trip";
        break;
    case SMART_FRAME_FLAG_DUE_TO_ISSRA_WARNING:
        reason = "ISSRA Warning";
        break;
    case SMART_FRAME_FLAG_DUE_TO_FACTORY_COMMAND_SAVE:
        reason = "Factory Command Save";
        break;
    case SMART_FRAME_FLAG_DUE_TO_EXIT_REMANUFACTURING:
        reason = "Exit Remanufacturing";
        break;
    case SMART_FRAME_FLAG_DUE_TO_HITACHI_ORM_OR_HD_TEST:
        reason = "Hitachi ORM or HD test";
        break;
    case SMART_FRAME_FLAG_STOP_UNIT_FRAME:
        reason = "Unit Frame";
        break;
    case SMART_FRAME_FLAG_INTERIM_FRAME_10MIN_IDLE_TIME:
        reason = "10 minute IDLE Time";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_WRITE_PROTECT_INVOCATION:
        reason = "Write Protect Invocation";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND_TO_INVOCATE_A_FRAME_THAT_CAN_BE_INTERRUPTABLE:
        reason = "Invocate a frame that can be interruptable";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_SMART_SCT_COMMAND:
        reason = "SCT Command";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_HEAD_UNLOADING:
        reason = "Head Unloading";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_START_OF_SANITIZE_COMMAND:
        reason = "Start of Sanitize Command";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_END_OF_SANITIZE_COMMAND:
        reason = "End of Sanitize Command";
        break;
    case SMART_FRAME_FLAG_SAVED_DUE_TO_FORMAT_COMPLETION:
        reason = "Format Completion";
        break;
    case SMART_FRAME_FLAG_UNRECOVERED_WRITE_NVC_RESET_FRAME:
        reason = "Unrecovered write NVC Reset Frame";
        break;
    case SMART_FRAME_FLAG_QUICK_FORMAT_FINISH:
        reason = "Quick Format Finish (SAS)";
        break;
    case SMART_FRAME_FLAG_SATA_DEVICE_FAULT:
        reason = "Sata Device Fault";
        break;
    case SMART_FRAME_FLAG_SATA_UNCORRECTABLE_ERROR:
        reason = "Uncorrectable Error";
        break;
    case SMART_FRAME_FLAG_SATA_ERC_UNCORRETABLE_ERROR:
        reason = "ERC Uncorrectable Error";
        break;
    case SMART_FRAME_FLAG_DRIVE_SELF_TEST_COMPLETE_WITH_FAILURE:
        reason = "DST Test Completed with Failure";
        break;
    case SMART_FRAME_FLAG_DEFECT_LIST_COMMAND:
        reason = "Defect List Command";
        break;
    case SMART_FRAME_FLAG_F7_CDB_TRIGGER:
        reason = "F7 CDB Trigger";
        break;
    default:
        reason = "Did not match any value";
        break;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn prePython_uknown_params()
//
//! \brief
//!   Description:  process prepython unknow or vendor spcific information
//
//  Entry:
//! \param  value - the 64 bit value  
//! \param  logPage - the log page number of the data
//! \param  subPage - the sub page number of the data
//! \param  paramCode for the data 
//! \param  offset - offset with in the buffer where the data is coming from
//! \param  parseUnknowns - do we need to show the data or just skip it all
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void opensea_parser::prePython_unknown_params(JSONNODE* masterData, uint64_t value, uint16_t logPage, uint8_t subPage, uint16_t paramCode, uint32_t offset)
{
    if (g_parseUnknown)
    {
        std::string myStr = "";
        myStr.resize(BASIC);
        JSONNODE* data = json_new(JSON_NODE);
        json_push_back(data, json_new_a("name", "unknown"));
        JSONNODE* label = json_new(JSON_NODE);
        json_set_name(label, "labels");
        json_push_back(label, json_new_a("units", "unknown"));

        snprintf(&*myStr.begin(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", logPage, subPage, paramCode, offset);
        json_push_back(label, json_new_a("metric_source", &*myStr.begin()));
        json_push_back(data, label);
        json_push_back(data, json_new_i("value", value));
        snprintf(&*myStr.begin(), BASIC, "%" PRIu64"", value);

        json_push_back(masterData, data);
    }

}
//-----------------------------------------------------------------------------
//
//! \fn prePython_int()
//
//! \brief
//!   Description:  format the data for the prepython data int type
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void opensea_parser::prePython_int(JSONNODE* masterData, const char* name, const char* statType, const char* unit, uint64_t value, uint16_t logPage, uint8_t subPage, uint16_t paramCode, uint32_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    JSONNODE* data = json_new(JSON_NODE);
    json_push_back(data, json_new_a("name", name));
    JSONNODE* label = json_new(JSON_NODE);
    json_set_name(label, "labels");
    if (statType != NULL)
    {
        json_push_back(label, json_new_a("stat_type", statType));
    }
    json_push_back(label, json_new_a("units", unit));

    snprintf(&*myStr.begin(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", logPage, subPage, paramCode, offset);
    json_push_back(label, json_new_a("metric_source", &*myStr.begin()));
    json_push_back(data, label);
    json_push_back(data, json_new_i("value", value));
    snprintf(&*myStr.begin(), BASIC, "%" PRIu64"", value);

    json_push_back(masterData, data);
}
//-----------------------------------------------------------------------------
//
//! \fn prePython_float()
//
//! \brief
//!   Description:  format the data for the prepython data float type
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void opensea_parser::prePython_float(JSONNODE* masterData, const char* name, const char* statType, const char* unit, double value, uint16_t logPage,uint8_t subPage, uint16_t paramCode, uint32_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    JSONNODE* data = json_new(JSON_NODE);
    json_push_back(data, json_new_a("name", name));
    JSONNODE* label = json_new(JSON_NODE);
    json_set_name(label, "labels");
    if (statType != NULL)
    {
        json_push_back(label, json_new_a("stat_type", statType));
    }
    json_push_back(label, json_new_a("units", unit));
    snprintf(&*myStr.begin(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", logPage, subPage, paramCode, offset);
    json_push_back(label, json_new_a("metric_source", &*myStr.begin()));
    json_push_back(data, label);
    json_push_back(data, json_new_f("value", value));

    json_push_back(masterData, data);
}