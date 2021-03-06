/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/sbe/p9_sbe_ext_defs.H $    */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016                             */
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
/// @file p9_sbe_ext_defs.H
/// @brief This file will has structures and constants that can be treated as
//         the external interfaces of the SBE.

#ifndef _P9_SBE_EXT_DEFS_H_
#define _P9_SBE_EXT_DEFS_H_

/// @brief A structure (bitfield) representing the SBE messaging register
typedef union sbeMsgReg
{
    struct
    {
#ifdef _BIG_ENDIAN
        uint32_t sbeBooted : 1; ///< SBE control loop initialized
        uint32_t reserved1 : 3; ///< Reserved
        uint32_t prevState : 4; ///< Previous SBE state
        uint32_t currState : 4; ///< Current SBE state
        uint32_t majorStep : 4; ///< Last major istep executed by the SBE
        uint32_t minorStep : 8; ///< Last minor istep executed by the SBE
        uint32_t reserved2 : 8; ///< Reserved
#else
        uint32_t reserved2 : 8; ///< Reserved
        uint32_t minorStep : 8; ///< Last minor istep executed by the SBE
        uint32_t majorStep : 4; ///< Last major istep executed by the SBE
        uint32_t currState : 4; ///< Current SBE state
        uint32_t prevState : 4; ///< Previous SBE state
        uint32_t reserved1 : 3; ///< Reserved
        uint32_t sbeBooted : 1; ///< SBE control loop initialized
#endif
    };
    uint32_t reg; ///< The complete SBE messaging register as a uint32
} sbeMsgReg_t;

/**
 * @brief Enumeration of SBE states
*/
typedef enum sbeState
{
    SBE_STATE_UNKNOWN = 0x0, // Unkown, initial state
    SBE_STATE_FFDC_COLLECT = 0x1, // Waiting for FFDC collection after a reset
    SBE_STATE_IPLING = 0x2, // IPL'ing - autonomous mode (transient)
    SBE_STATE_ISTEP = 0x3, // ISTEP - Running IPL by steps (transient)
    SBE_STATE_RUNTIME = 0x4, // SBE Runtime
    SBE_STATE_MPIPL = 0x5, // MPIPL
    SBE_STATE_DMT = 0x6, // Dead Man Timer State (transient)
    SBE_STATE_DUMP = 0x7, // Dumping
    SBE_STATE_FAILURE = 0x8, // Internal SBE failure
    SBE_STATE_QUIESCE = 0x9, // Final state - needs SBE reset to get out
    SBE_STATE_ABORT = 0xA, // SBE was asked to abort - need reset to get out

    // Max States, Always keep it at the last of the enum and sequential
    SBE_MAX_STATE = 0xB,
    // Don't count this in the state, just to intialize the state variables
    SBE_INVALID_STATE = 0xF,
} sbeState_t;

#endif //_P9_SBE_EXT_DEFS_H_
