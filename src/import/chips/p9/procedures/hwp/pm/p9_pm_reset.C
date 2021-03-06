/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/pm/p9_pm_reset.C $         */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2017                        */
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
/// @file p9_pm_reset.C
/// @brief Wrapper that calls underlying HWPs to perform a Power Management
///        Reset function when needing to restart the OCC complex.
///
// *HWP HWP Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP HWP Backup Owner :
// *HWP FW Owner         : Sangeetha T S <sangeet2@in.ibm.com>
// *HWP Team             : PM
// *HWP Level            : 2
// *HWP Consumed by      : HS

///
/// High-level procedure flow:
///
/// @verbatim
///
///     - Mask the PM FIRs
///     - Halt and then Reset the PPC405
///     - Put all EX chiplets in special wakeup
///     - Reset Stop,Pstate and OCC GPEs
///     - Reset the Cores and Quads
///     - Reset PBA
///     - Reset SRAM Controller
///     - Reset OCB
///     - Reset PSS
///     - Clear special wakeups
///
/// @endverbatim
///
// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <p9_pm_reset.H>
#include <p9_pm_utils.H>

// -----------------------------------------------------------------------------
// Constant definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constant Defintions
// -----------------------------------------------------------------------------
enum PPM_MASK
{
    CME_FIRMASK = 0xFFFFFFFF,
    CORE_ERRMASK = 0xFFFFFFFF,
    QUAD_ERRMASK = 0xFFFFFFFF
};

// -----------------------------------------------------------------------------
// Function prototypes
// -----------------------------------------------------------------------------

///
/// @brief Clear the Deep Exit Masks
///
/// @param[in] i_target Chip target
///
/// @return FAPI2_RC_SUCCESS If the special wake-up is successful,
///         else error code.
///
fapi2::ReturnCode clear_deep_exit_mask(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target);

// -----------------------------------------------------------------------------
// Function definitions
// -----------------------------------------------------------------------------

fapi2::ReturnCode p9_pm_reset(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target)
{
    FAPI_IMP("Entering p9_pm_reset ...");

    fapi2::buffer<uint64_t> l_data64;
    fapi2::ReturnCode l_rc;

    //  ************************************************************************
    //  Mask the FIRs as errors can occur in what follows
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_firinit for masking errors in reset operation.");
    FAPI_EXEC_HWP(l_rc, p9_pm_firinit, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to mask OCC,PBA & CME FIRs.");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After masking FIRs"));

    // Clear the OCC's PIB I2C engine locks.
    // All other OCC owned flag bits are retained.
    l_data64.setBit<17>().setBit<19>().setBit<21>();
    FAPI_TRY(fapi2::putScom(i_target, PU_OCB_OCI_OCCFLG_SCOM1, l_data64),
             "ERROR: Failed to write to OCC FLAG");

    //  ************************************************************************
    //  Halt the OCC PPC405 and reset it safely
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_occ_control to put OCC PPC405 into reset safely.");
    FAPI_EXEC_HWP(l_rc, p9_pm_occ_control,
                  i_target,
                  p9occ_ctrl::PPC405_RESET_SEQUENCE, //Operation on PPC405
                  p9occ_ctrl::PPC405_BOOT_NULL); // Boot instruction location
    FAPI_TRY(l_rc, "ERROR: Failed to reset OCC PPC405");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After safe reset of OCC PPC405"));

    //  ************************************************************************
    //  Put all EX chiplets in special wakeup
    //  ************************************************************************
    FAPI_DBG("Enable special wakeup for all functional  EX targets.");
    FAPI_TRY(special_wakeup_all(i_target,
                                true),//Enable splwkup
             "ERROR: Failed to remove EX chiplets from special wakeup");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After EX in special wakeup"));

    //  ************************************************************************
    //  Reset the STOP GPE (Bring it to HALT)
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_stop_gpe_init to reset SGPE");
    FAPI_EXEC_HWP(l_rc, p9_pm_stop_gpe_init, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to reset SGPE");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of SGPE"));

    //  ************************************************************************
    //  Reset the PSTATE GPE (Bring it to HALT)
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_pstate_gpe_init to reset PGPE");
    /* TODO: RTC 157096 - Enable once the procedure is available
    FAPI_EXEC_HWP(l_rc, p9_pm_pstate_gpe_init, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to reset PGPE");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of PGPE"));
    */

    //  ************************************************************************
    //  Issue reset to OCC GPEs ( GPE0 and GPE1) (Bring them to HALT)
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_occ_gpe_init to reset OCC GPE");
    FAPI_EXEC_HWP(l_rc, p9_pm_occ_gpe_init,
                  i_target,
                  p9pm::PM_RESET,
                  p9occgpe::GPEALL // Apply to both OCC GPEs
                 );
    FAPI_TRY(l_rc, "ERROR: Failed to reset the OCC GPEs");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of OCC GPEs"));

    //  ************************************************************************
    //  Reset Cores and Quads
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_corequad_init to reset cores & quads");
    FAPI_EXEC_HWP(l_rc, p9_pm_corequad_init,
                  i_target,
                  p9pm::PM_RESET,
                  CME_FIRMASK, // CME FIR MASK
                  CORE_ERRMASK,// Core Error Mask
                  QUAD_ERRMASK // Quad Error Mask
                 );
    FAPI_TRY(l_rc, "ERROR: Failed to reset cores & quads");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of core quad"));

    //  ************************************************************************
    //  Issue reset to PBA
    //  Note:  this voids the channel used by the GPEs
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_pba_init to reset PBA");
    FAPI_EXEC_HWP(l_rc, p9_pm_pba_init, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to reset PBA BUS");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of PBA"));

    //  ************************************************************************
    //  Issue reset to OCC-SRAM
    //  ************************************************************************
    FAPI_DBG("Executing p8_occ_sram_init to reset OCC-SRAM");
    FAPI_EXEC_HWP(l_rc, p9_pm_occ_sram_init, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to reset OCC SRAM");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of OCC SRAM"));

    //  ************************************************************************
    //  Issue reset to OCB
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_ocb_init to reset OCB");
    FAPI_EXEC_HWP(l_rc, p9_pm_ocb_init,
                  i_target,
                  p9pm::PM_RESET,
                  p9ocb::OCB_CHAN0, // Channel
                  p9ocb::OCB_TYPE_NULL, // Channel type
                  0, // Base address
                  0, // Length of circular push/pull queue
                  p9ocb::OCB_Q_OUFLOW_NULL, // Channel flow control
                  p9ocb::OCB_Q_ITPTYPE_NULL // Channel interrupt control
                 );
    FAPI_TRY(l_rc, "ERROR: Failed to reset OCB");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of OCB channels"));

    //  ************************************************************************
    //  Resets P2S and HWC logic
    //  ************************************************************************
    FAPI_DBG("Executing p9_pm_pss_init to reset P2S and HWC logic");
    FAPI_EXEC_HWP(l_rc, p9_pm_pss_init, i_target, p9pm::PM_RESET);
    FAPI_TRY(l_rc, "ERROR: Failed to reset PSS & HWC");
    FAPI_TRY(p9_pm_glob_fir_trace(i_target, "After reset of PSS"));

fapi_try_exit:
    FAPI_IMP("Exiting p9_pm_reset ...");
    return fapi2::current_err;
}

///
/// @brief Clear deep exit mask
///
/// @param[in] i_target Chip target
///
/// @return FAPI2_RC_SUCCESS on success, else error.
///
fapi2::ReturnCode clear_deep_exit_mask(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target)
{
    FAPI_INF("clear_deep_exit_mask Enter");

    return fapi2::FAPI2_RC_SUCCESS;
}
