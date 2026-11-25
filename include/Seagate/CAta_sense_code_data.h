//
// Ata_sense_code_data.h   Structures of ATA FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "common_types.h"
#include "bit_manip.h"


namespace opensea_parser {

#ifndef SEASAW_SENSECODEDATA_H
#define SEASAW_SENSECODEDATA_H

    struct SenseAttribute {
        uint64_t raw = 0;
        std::string name;
        bool supported = false;
        bool valid = false;
        uint64_t value = 0;
        std::string strValue;
        bool isString = false;
    };

    struct SenseData {
        uint64_t entry0 = 0;
        uint64_t entry1 = 0;
        uint64_t entry2 = 0;
    };

    constexpr size_t ATASenseDataOffset = 3184;
    constexpr size_t ATASenseEntrySize = 24;
    constexpr size_t ATASenseEntryCount = 44;

    class SenseCodeTable {
    public:
        using SenseCodeEntry = std::map<std::string, SenseAttribute>;
        using SenseCodeClasses = std::map<std::string, std::vector<SenseCodeEntry>>;

        SenseCodeClasses ParseATASenseCodeData(const std::vector<SenseData>& data);

    private:
        SenseCodeEntry BuildATASenseCodeEntry(const SenseData& data, size_t classNumber, size_t classIndex);
        const std::array<std::string, 15>& CreateOrderATASenseCodePrinting();
    };
 
#endif
}