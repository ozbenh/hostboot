/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep13/call_mss_draminit_mc.C $               */
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

//Error handling and tracing
#include <errl/errlentry.H>
#include <errl/errlmanager.H>
#include <errl/errludtarget.H>
#include <isteps/hwpisteperror.H>
#include <initservice/isteps_trace.H>

//  targeting support
#include <targeting/common/commontargeting.H>
#include <targeting/common/util.H>
#include <targeting/common/utilFilter.H>

// Istep 13 framework
#include "istep13consts.H"

// fapi2 HWP invoker
#include  <fapi2/plat_hwp_invoker.H>

//From Import Directory (EKB Repository)
#include  <config.h>
#include  <fapi2.H>
#include  <p9_mss_draminit_mc.H>


using namespace ERRORLOG;
using namespace ISTEP;
using namespace ISTEP_ERROR;
using namespace TARGETING;

namespace ISTEP_13
{
void* call_mss_draminit_mc (void *io_pArgs)
{
    errlHndl_t l_err = NULL;

    IStepError l_stepError;

    TARGETING::Target * sys = NULL;
    TARGETING::targetService().getTopLevelTarget( sys );

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,"call_mss_draminit_mc entry" );

    // Get all MCBIST
    TARGETING::TargetHandleList l_mcbistTargetList;
    getAllChiplets(l_mcbistTargetList, TYPE_MCBIST);

    for (const auto & l_mcbist_target : l_mcbistTargetList)
    {
        // Dump current run on target
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                "Running p9_mss_draminit_mc HWP on "
                "target HUID %.8X", TARGETING::get_huid(l_mcbist_target));

        fapi2::Target<fapi2::TARGET_TYPE_MCBIST> l_fapi_mcbist_target
            (l_mcbist_target);
        FAPI_INVOKE_HWP(l_err, p9_mss_draminit_mc, l_fapi_mcbist_target);

        if (l_err)
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                    "ERROR 0x%.8X : p9_mss_draminit_mc HWP returns error",
                    l_err->reasonCode());

            // capture the target data in the elog
            ErrlUserDetailsTarget(l_mcbist_target).addToLog( l_err );

            // Create IStep error log and cross reference to error that occurred
            l_stepError.addErrorDetails( l_err );

            // Commit Error
            errlCommit( l_err, HWPF_COMP_ID );
        }
        else
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                       "SUCCESS running p9_mss_draminit_mc HWP on "
                       "target HUID %.8X", TARGETING::get_huid(l_mcbist_target));
        }

    } // End; memBuf loop


    //TODO RTC:167292 Remove this workaround that is clearing this scom register
    uint64_t l_data64 = 0x0;
    size_t l_size = sizeof(uint64_t);
    // Get all MCAs
    TARGETING::TargetHandleList l_mcaTargetList;
    getAllChiplets(l_mcaTargetList, TYPE_MCA);

    //Clear out the MBA_FARB3Q reg(0x7010916) on all MCAs
    for (const auto & l_mca_target : l_mcaTargetList)
    {
        l_err = deviceOp( DeviceFW::WRITE,
                          l_mca_target,
                          &l_data64,
                          l_size,
                          DEVICE_SCOM_ADDRESS(0x7010916));
        if (l_err)
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                      "Failed clearing MBA_FARB3Q register on MCA %d",
                      l_mca_target->getAttr<TARGETING::ATTR_CHIPLET_ID>());
            errlCommit(l_err, HWPF_COMP_ID);
        }
    }

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace, "call_mss_draminit_mc exit" );

    return l_stepError.getErrorHandle();
}

};
