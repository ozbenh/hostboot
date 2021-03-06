/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/common/plugins/prdfParserEnums.H $          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2013,2017                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

/** @file  prdfParserEnums.H
 *  @brief Common enums used for building and parsing capture data.
 */

#ifndef __prdfParserEnums_H
#define __prdfParserEnums_H

#include <prdfMemConst.H>

namespace PRDF
{
#ifdef PRDF_HOSTBOOT_ERRL_PLUGIN
namespace HOSTBOOT
{
#endif

#ifdef PRDF_FSP_ERRL_PLUGIN
namespace FSP
{
#endif

enum PositionBounds
{
    MAX_NODE_PER_SYS    = 8,

    MAX_PROC_PER_NODE   = 8,

    MAX_EQ_PER_PROC     = 6,

    MAX_EX_PER_EQ       = 2,
    MAX_EX_PER_PROC     = MAX_EX_PER_EQ * MAX_EQ_PER_PROC,

    MAX_EC_PER_EX       = 2,
    MAX_EC_PER_EQ       = MAX_EC_PER_EX * MAX_EX_PER_EQ,
    MAX_EC_PER_PROC     = MAX_EC_PER_EX * MAX_EX_PER_EQ * MAX_EQ_PER_PROC,

    MAX_CAPP_PER_PROC   = 2,

    MAX_PEC_PER_PROC    = 3,

    MAX_PHB_PER_PEC0    = 1,
    MAX_PHB_PER_PEC1    = 2,
    MAX_PHB_PER_PEC2    = 3,
    MAX_PHB_PER_PROC    = MAX_PHB_PER_PEC0 +
                          MAX_PHB_PER_PEC1 +
                          MAX_PHB_PER_PEC2,

    MAX_OBUS_PER_PROC   = 4, // Nimbus 0 and 3, Cumulus 0, 1, 2, and 3

    MAX_XBUS_PER_PROC   = 3, // Nimbus 1 and 2, Cumulus 0, 1, and 2

    MAX_MCBIST_PER_PROC = 2,

    MAX_MCS_PER_MCBIST  = 2,
    MAX_MCS_PER_PROC    = MAX_MCS_PER_MCBIST * MAX_MCBIST_PER_PROC,

    MAX_MCA_PER_MCS     = 2,
    MAX_MCA_PER_MCBIST  = MAX_MCA_PER_MCS * MAX_MCS_PER_MCBIST,
    MAX_MCA_PER_PROC    = MAX_MCA_PER_MCS * MAX_MCS_PER_PROC,

    MAX_PORT_PER_MCBIST = MAX_MCA_PER_MCBIST, // MCA == PORT

    MAX_MEMBUF_PER_PROC = MAX_MCS_PER_PROC,

    MAX_MBA_PER_MEMBUF  = 2,
    MAX_PORT_PER_MBA    = 2,
    MAX_DIMM_PER_PORT   = 2,
    MAX_L4_PER_MEMBUF   = 1,

    INVALID_POSITION_BOUND = 0xffffffff,
};

namespace PARSER
{
    enum
    {
        // i_parser.PrintString() has two parts: header and data. These are the
        // maximum string lengths for one line in the error log. Note that the
        // header has a hard stop of one line, however, the data can wrap onto
        // multiple lines (not sure of the line limit).
        HEADER_SIZE  = 25,
        DATA_SIZE    = 50,
    };

} // namespace PARSER

namespace UE_TABLE
{
    enum
    {
        MAX_ENTRIES = 16, ///< Maximum number of entries allow in table.
        ENTRY_SIZE  =  7, ///< Number of bytes per entry.

        MAX_SIZE = MAX_ENTRIES * ENTRY_SIZE, ///< Maximum table size.
    };

    /**
     * @brief Represents the several different types of UEs stored in the table.
     */
    enum Type
    {
        SCRUB_MPE = 1, ///< Chip mark placed by scrub
        FETCH_MPE,     ///< Chip mark placed by fetch
        SCRUB_UE,      ///< Scrub UE
        FETCH_UE,      ///< Fetch UE
    };

} // namespace UE_TABLE

namespace CE_TABLE
{
    enum
    {
        METADATA_SIZE =  8, ///< Number of bytes for the table metadata.

        MAX_ENTRIES = 32, ///< Maximum number of entries allow in table.
        ENTRY_SIZE  =  9, ///< Number of bytes per entry.

        MAX_SIZE = METADATA_SIZE + MAX_ENTRIES * ENTRY_SIZE, ///< Maximum size.
    };

} // namespace CE_TABLE

namespace RCE_TABLE
{
    enum
    {
        ENTRY_SIZE  =  2, ///< Number of bytes per entry.
    };

} // namespace RCE_TABLE

/* TODO: RTC 136126
namespace DQ_BITMAP
{
    enum
    {
        BITMAP_SIZE = 10, // 80-bit bitmap
        ENTRY_SIZE  = sizeof(uint8_t) + MBA_DIMMS_PER_RANK * BITMAP_SIZE,
    };

} // namespace DQ_BITMAP
*/

namespace CEN_SYMBOL
{

    /** Used as a 2-bit mask to display which of the DQs on this symbol are
     *  failing. */
    enum DqMask
    {
        NO_SYMBOL_DQS   = 0x0,
        ODD_SYMBOL_DQ   = 0x1,
        EVEN_SYMBOL_DQ  = 0x2,
        BOTH_SYMBOL_DQS = EVEN_SYMBOL_DQ | ODD_SYMBOL_DQ,
    };

    /** This enum describes the raw card type for buffered DIMMs or the
     *  DIMM plug card type for IS DIMMs. PRD will store the 80 byte translation
     *  map in a separate section of the error log which can be used by the
     *  parser to do the Centaur DQ to DIMM DQ translation. These types are used
     *  for error log parsing which must be supported independent of the
     *  drivers (i.e. eblis tool) so enums can be added, but the values of the
     *  enums cannot be changed */
    enum WiringType
    {
        CEN_TYPE_A          = 0,
        CEN_TYPE_B          = 1,
        CEN_TYPE_D          = 2,
        CEN_TYPE_C          = 3,
        CEN_TYPE_B4         = 4,
        CEN_TYPE_A4         = 5,
        CEN_TYPE_D4         = 6,
        WIRING_INVALID      = 7, // An invalid card type
        CEN_TYPE_C4         = 8,
    };

}//namespace CEN_SYMBOL ends

namespace SLW_FFDC_DATA
{
    static const char * const title = "SLW_FFDC_DATA";

    enum
    {
        MAX_ENTRIES = 39, ///< Maximum number of entries allowed.
        ENTRY_SIZE  = 12, ///< Number of bytes per entry.

        MAX_SIZE = MAX_ENTRIES * ENTRY_SIZE, ///< Maximum table size.
    };

} // namespace SLW_FFDC_DATA

// LineDelete/ColumnRepair FFDC
namespace LD_CR_FFDC
{
    static const char * const L2TITLE = "L2_LD_COLRPR_FFDC";
    static const char * const L3TITLE = "L3_LD_COLRPR_FFDC";

    struct L2LdCrFfdc
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        uint32_t L2LDcnt        : 4;
        uint32_t L2LDallowed    : 1;
        uint32_t L2LDMaxAllowed : 4;
        uint32_t L2CRMaxAllowed : 4;
        uint32_t L2CRPresent    : 4;
        uint32_t L2reserved1    :15;

        uint32_t L2errMember    : 3;
        uint32_t L2errDW        : 3;
        uint32_t L2errMacro     : 2;
        uint32_t L2errBank      : 1;
        uint32_t L2errOWSelect  : 1;
        uint32_t L2errBitLine   : 4;
        uint32_t L2errIsTopSA   : 1;
        uint32_t L2errAddress   :10;
        uint32_t L2reserved2    : 7;

        L2LdCrFfdc():
           L2LDcnt(0), L2LDallowed(0), L2LDMaxAllowed(0),
           L2CRMaxAllowed(0), L2CRPresent(0), L2reserved1(0),
           L2errMember(0), L2errDW(0), L2errMacro(0),
           L2errBank(0), L2errOWSelect(0), L2errBitLine(0),
           L2errIsTopSA(0), L2errAddress(0), L2reserved2(0)
        {}
#else
        uint32_t L2reserved1    :15;
        uint32_t L2CRPresent    : 4;
        uint32_t L2CRMaxAllowed : 4;
        uint32_t L2LDMaxAllowed : 4;
        uint32_t L2LDallowed    : 1;
        uint32_t L2LDcnt        : 4;

        uint32_t L2reserved2    : 7;
        uint32_t L2errAddress   :10;
        uint32_t L2errIsTopSA   : 1;
        uint32_t L2errBitLine   : 4;
        uint32_t L2errOWSelect  : 1;
        uint32_t L2errBank      : 1;
        uint32_t L2errMacro     : 2;
        uint32_t L2errDW        : 3;
        uint32_t L2errMember    : 3;

        L2LdCrFfdc():
           L2reserved1(0), L2CRPresent(0), L2CRMaxAllowed(0),
           L2LDMaxAllowed(0), L2LDallowed(0), L2LDcnt(0),
           L2reserved2(0),L2errAddress(0), L2errIsTopSA(0),
           L2errBitLine(0), L2errOWSelect(0), L2errBank(0),
           L2errMacro(0), L2errDW(0), L2errMember(0)
        {}
#endif

    };

    struct L3LdCrFfdc
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        uint32_t L3LDcnt        : 4;
        uint32_t L3LDallowed    : 1;
        uint32_t L3LDMaxAllowed : 4;
        uint32_t L3CRMaxAllowed : 4;
        uint32_t L3CRPresent    : 4;
        uint32_t L3reserved1    :15;

        uint32_t L3errMember    : 3;
        uint32_t L3errDW        : 3;
        uint32_t L3errBank      : 1;
        uint32_t L3errDataOut   : 1;
        uint32_t L3errAddress   : 4;
        uint32_t L3errIO        : 1;
        uint32_t L3errRow       :10;
        uint32_t L3reserved2    : 9;

        L3LdCrFfdc():
           L3LDcnt(0), L3LDallowed(0), L3LDMaxAllowed(0),
           L3CRMaxAllowed(0), L3CRPresent(0),
           L3reserved1(0), L3errMember(0), L3errDW(0),
           L3errBank(0), L3errDataOut(0), L3errAddress(0),
           L3errIO(0), L3errRow(0), L3reserved2(0)
        {}
#else
        uint32_t L3reserved1    :15;
        uint32_t L3CRPresent    : 4;
        uint32_t L3CRMaxAllowed : 4;
        uint32_t L3LDMaxAllowed : 4;
        uint32_t L3LDallowed    : 1;
        uint32_t L3LDcnt        : 4;

        uint32_t L3reserved2    : 9;
        uint32_t L3errRow       :10;
        uint32_t L3errIO        : 1;
        uint32_t L3errAddress   : 4;
        uint32_t L3errDataOut   : 1;
        uint32_t L3errBank      : 1;
        uint32_t L3errDW        : 3;
        uint32_t L3errMember    : 3;

        L3LdCrFfdc():
           L3reserved1(0), L3CRPresent(0), L3CRMaxAllowed(0),
           L3LDMaxAllowed(0), L3LDallowed(0), L3LDcnt(0),
           L3reserved2(0), L3errRow(0), L3errIO(0),
           L3errAddress(0), L3errDataOut(0), L3errBank(0),
           L3errDW(0), L3errMember(0)
        {}
#endif

    };


} // namespace LD_CR_FFDC

namespace TD_CTLR_DATA
{
    static const char * const START = "TDCTLR_STATE_DATA_START";
    static const char * const END   = "TDCTLR_STATE_DATA_END";

    // We don't use 0 as one of the enum values here so we can ensure our data
    // is non-zero when we add the tdCtlr state to the capture data
    enum Version
    {
        IPL = 1,
        RT  = 2,
    };
} // namespace TD_CTLR_DATA

#ifdef PRDF_HOSTBOOT_ERRL_PLUGIN
} // end namespace HOSTBOOT
#endif

#ifdef PRDF_FSP_ERRL_PLUGIN
} // end namespace FSP
#endif

} // end namespace PRDF

#endif // __prdfParserEnums_H

