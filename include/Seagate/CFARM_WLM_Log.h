//
// CFARM_WLM_Log.h
//
// Do NOT modify or remove this copyright and confidentiality notice.
//
// Copyright 2012 - 2023 Seagate Technology LLC.
//
// The code contained herein is CONFIDENTIAL to Seagate Technology LLC
// and may be covered under one or more Non-Disclosure Agreements.
// All or portions are also trade secret.
// Any use, modification, duplication, derivation, distribution or disclosure
// of this code, for any reason, not expressly authorized is prohibited.
// All other rights are expressly reserved by Seagate Technology LLC.
//
// *****************************************************************************

// \file CFARM_WLM_Log.h
// \brief Defines the function calls and structures for parsing Seagate logs

#pragma once
#include <vector>
#include "common.h"
#include "CLog.h"
#include "libjson.h"
#include "Farm_Helper.h"

namespace opensea_parser {

#define TRACESIZE       0x80000
#define FARMWLM         0x3c485457      // FARM Frame Header Signature
#define FARMFOOTWLM     0x08465457      // FARM Frame Footer Signature
#define FARMFOOTWLMSWAP 0x57544608      // FARM Frame Footer byteSwapped
#define WLM3            0x0003
#define WLM6            0x0006

    class CFARMWLM
    {
    protected:

        typedef enum _eLogPageType
        {
            LOG_PAGE_WITH_HEADER,
            LOG_PAGE_WITHOUT_HEADER,
        } logPageType;

#pragma pack(push, 1)
 
        typedef struct _sWLMDataHeader
        {
            uint32_t                signature;                                      //<! signature for the farm wlm
            uint16_t                farmeNumber;                                    //<! index number of the trame starts at 0 goes to 0xffff
            uint16_t                rev;                                            //<! Revision of the Workload trace feature
            uint64_t                startTimestamp;                                 //<! Timestamp when the frame began to be built.
            uint64_t                endTimestamp;                                   //<! Timestamp when this frame was completed
            uint32_t                preFrameOffset;                                 //<! byte offset of the SIM file where the last trace frame was saved
            uint16_t                cycleCount;                                     //<! workload trace cycle to which this frame belongs
            uint16_t                pad1;                                           //<! unused
            uint32_t                frameSize;                                      //<! Size of the frame in the bytes
            uint32_t                duration;                                       //<! Duration of this frame in seconds
            uint32_t                readOps;                                        //<! number of read operations in this trace frame
            uint32_t                writeOps;                                       //<! number of write operations in this trace frame
            uint32_t                markers;                                        //<! number of one second markers in the trace frame
            uint32_t                reserved;
            uint32_t                reserved2;

            _sWLMDataHeader() : signature(0), farmeNumber(0), rev(0), startTimestamp(0), endTimestamp(0), \
             preFrameOffset(0), cycleCount(0), frameSize (0), duration(0), readOps(0), writeOps(0), markers(0) {};
        }sWLMDataHeader;

        typedef struct _sWLM_Trace
        {
            
            uint64_t                LBA;                                            //<! The starting LBAs of read and write operations
            uint16_t                transLength;                                    //<! The Transfer length
            uint16_t                streamEvent;                                    //<! Read and write hardware streaming events, 
            uint64_t                markers;                                        //<! One Second Markers. 
            
            _sWLM_Trace() : LBA(0), transLength(0), streamEvent(0), markers(0) {};

        }sWLM_Trace;

#pragma pack(pop)

        std::vector <sWLM_Trace > vFrame;
        std::vector <sWLM_Trace > vBlankFrame;

        uint8_t                     *pData;                                         //<! pointer data structure
        std::string                 m_name;                                         //<! name of the class
        uint32_t                    m_logSize;                                      //<! log size in bytes
        uint16_t                    m_version;                                      //<! version of the frame to use
        eReturnValues               m_WLMstatus;                                    //<! holds the status so 
        sWLMDataHeader              m_DataHeader;                                   //<! Data Header sturcture
        sWLMDataHeader              *m_DataHeaderPTR;


        
        eReturnValues parse_WLM_Summary(JSONNODE *masterData);
        eReturnValues print_Empty_WLM_Log(JSONNODE *wlmJson);
        eReturnValues print_Summary_Log(size_t* offset, JSONNODE *wlm);

        bool is_FARM(uint32_t signature);
        bool validate_WLM();
        bool get_WLM_Header_Data(size_t offset);
        uint16_t get_transferSize(uint8_t tLen,size_t* offset);
        bool get_Trace_Data(size_t* offset, JSONNODE* wlmJson);


    public:
        CFARMWLM();
        CFARMWLM( uint8_t *BufferData, uint32_t dataSize, JSONNODE *masterData);
        ~CFARMWLM();

        eReturnValues get_WLM_Status()
        {
            if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
            {
                printf("\nstatus = %d", m_WLMstatus);
            }
            return m_WLMstatus;
        };

    };
}
