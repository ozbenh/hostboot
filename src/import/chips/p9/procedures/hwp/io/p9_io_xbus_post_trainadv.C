/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/io/p9_io_xbus_post_trainadv.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2016                        */
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
///
/// @file p9_io_xbus_post_trainadv.H
/// @brief Post-Training PHY Status Function.
///
///-----------------------------------------------------------------------------
/// *HWP HWP Owner        : Chris Steffen <cwsteffen@us.ibm.com>
/// *HWP HWP Backup Owner : Gary Peterson <garyp@us.ibm.com>
/// *HWP FW Owner         : Jamie Knight <rjknight@us.ibm.com>
/// *HWP Team             : IO
/// *HWP Level            : 2
/// *HWP Consumed by      : FSP:HB
///-----------------------------------------------------------------------------
///
/// @verbatim
/// High-level procedure flow:
///
/// Post-Training PHY Status Function.
///
/// Procedure Prereq:
///   - System clocks are running.
///   - Scominit Procedure is completed.
///   - IO DCCAL Procedure is completed.
///   - IO Run Training Procedure is completed.
/// @endverbatim
///----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "p9_io_xbus_post_trainadv.H"

// ----------------------------------------------------------------------------
// Procedure Function
// ----------------------------------------------------------------------------

/**
 * @brief A simple HWP that runs after io_run_trainig.
 *  This function is called on every Xbus.
 * @param[in] i_target   Fapi2 Target
 * @param[in] i_group    Clock Group
 * @param[in] i_ctarget  Fapi2 Connected Target
 * @param[in] i_cgroup   Connected Clock Group
 * @retval ReturnCode
 */
fapi2::ReturnCode p9_io_xbus_post_trainadv(
    const fapi2::Target < fapi2::TARGET_TYPE_XBUS >& i_target,
    const uint8_t& i_group,
    const fapi2::Target < fapi2::TARGET_TYPE_XBUS >& i_ctarget,
    const uint8_t& i_cgroup)
{
    FAPI_IMP("Entering...");
    uint8_t l_status = 0x0;
    char target_string[fapi2::MAX_ECMD_STRING_LEN];

    fapi2::toString(i_target, target_string, fapi2::MAX_ECMD_STRING_LEN);

    FAPI_INF("Checking %s:g%d Debug Status.", target_string, i_group);
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IO_X_DEBUG, i_target, l_status));

    if(l_status == fapi2::ENUM_ATTR_IO_X_DEBUG_TRUE)
    {
        FAPI_INF("Debug True.");
    }
    else
    {
        FAPI_INF("Debug False.");
    }

fapi_try_exit:
    FAPI_IMP("Exiting...");
    return fapi2::current_err;
}

