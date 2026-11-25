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

#include "CAta_sense_code_data.h"
#include <stdexcept>
#include <utility>

using namespace opensea_parser;

constexpr uint64_t SupportedBit = uint64_t{1} << 63;
constexpr uint64_t ValidBit = uint64_t{1} << 62;

std::uint8_t Byte(uint64_t raw, unsigned index) 
{
    return static_cast<uint8_t>(raw >> (index * 8));
}

std::string HexByte(uint8_t value) 
{
    constexpr char digits[] = "0123456789ABCDEF";
    std::string result = "0x00";
    result[2] = digits[value >> 4];
    result[3] = digits[value & 0x0F];
    return result;
}
//-----------------------------------------------------------------------------
//
/// @brief Creates and initializes a SenseAttribute object from a raw attribute value.
///
/// This function extracts the supported and valid status flags from the provided
/// raw attribute value and populates a SenseAttribute structure with the parsed data.
///
/// The resulting object's `raw` field contains only the bits defined by
/// SupportedBit and ValidBit.
///
/// @param raw 64-bit raw attribute value containing status flag bits.
/// @param name Human-readable name associated with the attribute.
///
/// @return SenseAttribute Initialized SenseAttribute structure containing:
///         - Filtered raw value
///         - Attribute name
///         - Supported status flag
///         - Valid status flag
///
//-----------------------------------------------------------------------------
SenseAttribute Attribute(uint64_t raw, const std::string& name) 
{
    SenseAttribute result;
    result.raw = raw & (SupportedBit | ValidBit);
    result.name = name;
    result.supported = (raw & SupportedBit) != 0;
    result.valid = (raw & ValidBit) != 0;
    return result;
}
//-----------------------------------------------------------------------------
/// @brief Builds an ATA sense code entry from decoded sense data.
///
/// Creates and populates a SenseCodeEntry object using ATA sense data fields,
/// including command information, error details, actuator information,
/// logical head, error LBA, power cycle count, and timestamp values.
///
/// The function extracts supported and valid attributes from the provided
/// SenseData structure and formats selected fields as hexadecimal strings
/// for display purposes.
///
/// @param data
///     SenseData structure containing raw ATA sense data fields.
///
/// @param classNumber
///     Sense code classification identifier.
///
/// @param classIndex
///     Index value associated with the sense code class.
///
/// @return
///     A populated SenseCodeEntry containing parsed ATA sense attributes.
//-----------------------------------------------------------------------------
SenseCodeTable::SenseCodeEntry SenseCodeTable::BuildATASenseCodeEntry(const SenseData& data, size_t classNumber, size_t classIndex)
{
    SenseCodeTable::SenseCodeEntry entry;
    auto addNumber = [&](const std::string& key, const std::string& name, uint64_t raw, uint64_t value) 
        {
        auto attribute = Attribute(raw, name);
        if (attribute.supported && attribute.valid) {
            attribute.value = value;
        }
        entry.emplace(key, std::move(attribute));
    };
    addNumber("Class", "Sense Code Class", SupportedBit | ValidBit, static_cast<uint64_t>(classNumber));
    addNumber("Class Count", "Class Count", SupportedBit | ValidBit, static_cast<uint64_t>(classIndex));
    entry.at("Class").raw = entry.at("Class").raw | static_cast<uint64_t>(classNumber);
    entry.at("Class Count").raw = entry.at("Class Count").raw | static_cast<uint64_t>(classIndex);

    const std::array<const char*, 7> keys = {
        "CDB", "StatusRegister", "ErrorRegister", "ErrorCode", "SenseKey", "SenseQualifier", "Code"};
    const std::array<const char*, 7> names = {
        "CDB Operation Code", "Status Register", "Error Register", "Error Code",
        "Additional Sense Key", "Additional Sense Key Qualifier", "Unit Code"};
    for (size_t index = 0; index < keys.size(); ++index) {
        auto attribute = Attribute(data.entry0, names[index]);
        attribute.isString = true;
        if (attribute.supported && attribute.valid) {
            attribute.value = Byte(data.entry0, static_cast<unsigned>(6 - index));
            attribute.strValue = HexByte(static_cast<uint8_t>(attribute.value));
        }
        entry.emplace(keys[index], std::move(attribute));
    }

    addNumber("Actuator", "Actuator", data.entry1, M_Byte6(data.entry1) >> 6);
    addNumber("LogicalHead", "Logical Head", data.entry1, M_Byte6(data.entry1) & 0x3Fu);
    addNumber("ErrorLBA", "Error LBA", data.entry1, data.entry1 & UINT64_C(0x0000FFFFFFFFFFFF));  
    addNumber("PowerCycle", "Power Cycle", data.entry2, M_Word2(data.entry2));                   
    addNumber("TimeStamp", "Time Stamp in seconds", data.entry2, M_DoubleWord0(data.entry2));   ;
    return entry;
}
//-----------------------------------------------------------------------------
/// @brief Parses ATA sense code data into grouped sense code classes.
///
/// Processes a collection of ATA SenseData records and organizes them into
/// predefined sense code classes. Each entry is converted into a
/// SenseCodeEntry using BuildATASenseCodeEntry() and stored within the
/// corresponding class container.
///
/// The function divides the input data into the following class sizes:
/// - Class 1: 16 entries
/// - Class 2: 16 entries
/// - Class 3: 4 entries
/// - Class 4: 4 entries
/// - Class 5: 4 entries
///
/// Each generated class is stored using the key format:
/// "senseCodeClassN", where N is the class number.
///
/// @param data
///     Vector containing ATA sense data records to parse.
///
/// @return
///     A SenseCodeClasses container populated with parsed ATA sense code
///     entries grouped by class.
//-----------------------------------------------------------------------------
SenseCodeTable::SenseCodeClasses SenseCodeTable::ParseATASenseCodeData(const std::vector<SenseData>& data)
{
    SenseCodeTable::SenseCodeClasses classes;
    if (data.empty()) {
        return classes;
    }

    const std::vector<size_t> counts{ 16, 16, 4, 4, 4 };
    size_t start = 0;
    for (size_t classIndex = 0; classIndex < counts.size(); ++classIndex) 
    {
        auto& entries = classes["senseCodeClass" + std::to_string(classIndex + 1)];
        entries.reserve(counts[classIndex]);
        size_t myclass = classIndex + 1;
        for (std::size_t index = 0; index < counts[classIndex] && start + index < data.size(); ++index) 
        {
            entries.push_back(BuildATASenseCodeEntry(data[start + index], myclass, static_cast<size_t>(index)));
        }
        start += counts[classIndex];
    }
    return classes;
}
//-----------------------------------------------------------------------------
/// @brief Returns the ordered list of ATA sense code field keys.
///
/// Provides the predefined field ordering used for ATA sense code printing
/// and display formatting.
///
/// @return
///     Constant reference to the ordered array of ATA sense code field keys.
//-----------------------------------------------------------------------------
const std::array<std::string, 15>& SenseCodeTable::CreateOrderATASenseCodePrinting() 
{
    static const std::array<std::string, 15> keys = {
        "Class", "Class Count", "CDB", "StatusRegister", "ErrorRegister", "ErrorCode",
        "SenseKey", "SenseQualifier", "FRU", "CategoryL1", "Actuator", "LogicalHead",
        "ErrorLBA", "PowerCycle", "TimeStamp"};
    return keys;
}

