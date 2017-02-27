/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep14/call_mss_memdiag.C $                   */
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
#include <errl/errlentry.H>
#include <errl/errlmanager.H>
#include <isteps/hwpisteperror.H>
#include <initservice/isteps_trace.H>
#include <targeting/common/utilFilter.H>
#include <diag/attn/attn.H>
#include <diag/mdia/mdia.H>
#include <targeting/common/targetservice.H>
#include <devicefw/driverif.H>

using   namespace   ISTEP;
using   namespace   ISTEP_ERROR;
using   namespace   ERRORLOG;

namespace ISTEP_14
{
void* call_mss_memdiag (void* io_pArgs)
{
    errlHndl_t l_errl = NULL;

    IStepError l_stepError;

    TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
              "call_mss_memdiag entry");

    TARGETING::Target* masterproc = nullptr;
    TARGETING::targetService().masterProcChipTargetHandle(masterproc);

#ifdef CONFIG_IPLTIME_CHECKSTOP_ANALYSIS
    // @TODO-RTC: 155065
    // update firdata inputs for OCC
    l_errl = HBOCC::loadHostDataToSRAM(masterproc,
                                        PRDF::ALL_PROC_MEM_MASTER_CORE);
    assert(l_errl==NULL,
           "Error returned from call to HBOCC::loadHostDataToSRAM");
#endif

    TARGETING::TargetHandleList l_targetList;
    TARGETING::TYPE targetType;

    // we need to check the model of the master proc
    // if it is Cumulus then we will use TYPE_MBA for targetType
    // else it is Nimbus so then we will use TYPE_MCBIST for targetType
    if ( TARGETING::MODEL_CUMULUS ==
         masterproc->getAttr<TARGETING::ATTR_MODEL>() )
    {
        targetType = TARGETING::TYPE_MBA;
    }
    else
    {
        targetType = TARGETING::TYPE_MCBIST;
    }

    getAllChiplets(l_targetList, targetType);

    do
    {
        l_errl = ATTN::startService();
        if( NULL != l_errl )
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                      "ATTN startService failed");
            break;
        }

        l_errl = MDIA::runStep(l_targetList);
        if( NULL != l_errl )
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace, "MDIA subStep failed");
            break;
        }

        l_errl = ATTN::stopService();
        if( NULL != l_errl )
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                      "ATTN stopService failed");
            break;
        }

        //Mask IPOLL Interrupts
        TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                  "Mask IPOLL Interrupts");

        // Get all functional proc chip targets
        //Use targeting code to get a list of all processors
        TARGETING::TargetHandleList l_procChips;
        getAllChips( l_procChips, TARGETING::TYPE_PROC   );

        for (auto l_procChip: l_procChips)
        {
            uint64_t l_data = 0xfff0000000000000;
            size_t l_data_sz = sizeof(uint64_t);

            l_errl = deviceWrite( l_procChip, &l_data, l_data_sz,
                  DEVICE_SCOM_ADDRESS(0x000F0033) );

            if( NULL != l_errl )
            {
                TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                   "IPOLL MASK ERROR: deviceWrite on 0x%08X failed HUID:0x%08X",
                   0x000F0033, get_huid(l_procChip));
                break;
            }

            //Disable the nest pulse to the bottom 4 bits of the TB
            l_data = 0x0000000000000000;

            l_errl = deviceWrite( l_procChip, &l_data, l_data_sz,
                  DEVICE_SCOM_ADDRESS(0x010F0023) );

            if( NULL != l_errl )
            {
                TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                   "Disable nest pulse: deviceWrite on 0x%08X failed HUID:0x%08X",
                   0x010F0023, get_huid(l_procChip));
                break;
            }
        }

    }while( 0 );

    if( NULL != l_errl )
    {
        // Create IStep error log and cross reference to error that occurred
        l_stepError.addErrorDetails(l_errl);

        // Commit Error
        errlCommit(l_errl, HWPF_COMP_ID);
    }

    TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
              "call_mss_memdiag exit");

    // end task, returning any errorlogs to IStepDisp
    return l_stepError.getErrorHandle();
}

};
