/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/sbeio/sbe_systemConfig.C $                            */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2017                        */
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
/**
* @file sbe_systemConfig.C
* @brief System Configuartion Setup Messages to inform the SBE of other
         procs in the system.
*/

#include <config.h>
#include <trace/interface.H>
#include <errl/errlmanager.H>
#include <sbeio/sbeioif.H>
#include <sbeio/sbe_psudd.H>
#include <targeting/common/targetservice.H>

extern trace_desc_t* g_trac_sbeio;

#define SBE_TRACD(printf_string,args...) \
TRACDCOMP(g_trac_sbeio,"sendSystemConfig: " printf_string,##args)

#define SBE_TRACF(printf_string,args...) \
TRACFCOMP(g_trac_sbeio,"sendSystemConfig: " printf_string,##args)

namespace SBEIO
{

    /**
    * @brief Set the system configuration on the SBE so it is aware of
    *        the other procs in the system
    *
    * @param[in] i_systemConfig uint64 where each bit represents a proc in that position
    *        Bit position ATTR_PROC_FABRIC_CHIP_ID + (8 * ATTR_PROC_FABRIC_GROUP_ID) = 1
    *        if that proc is present and functional.
    *
    * @return errlHndl_t Error log handle on failure.
    *
    */

    errlHndl_t sendSystemConfig(const uint64_t i_systemConfig )
    {
        errlHndl_t errl = NULL;

        SBE_TRACD(ENTER_MRK "sending system configuration from HB -> SBE  i_systemConfig=0x%x",i_systemConfig);

        // Find master proc for target of PSU command
        TARGETING::Target * l_master = nullptr;
        (void)TARGETING::targetService().masterProcChipTargetHandle(l_master);

        SbePsu::psuCommand   l_psuCommand(
                                  SbePsu::SBE_REQUIRE_RESPONSE,  //control flags
                                  SbePsu::SBE_PSU_GENERIC_MESSAGE, //command class
                                  SbePsu::SBE_CMD_CONTROL_SYSTEM_CONFIG); //command
        SbePsu::psuResponse  l_psuResponse;

        // set up PSU command message
        l_psuCommand.cd2_SetSystemConfig_SystemFabricIdMap = i_systemConfig;

        errl =  SBEIO::SbePsu::getTheInstance().performPsuChipOp(l_master,
                                &l_psuCommand,
                                &l_psuResponse,
                                SbePsu::MAX_PSU_SHORT_TIMEOUT_NS,
                                SbePsu::SBE_SYSTEM_CONFIG_REQ_USED_REGS,
                                SbePsu::SBE_SYSTEM_CONFIG_RSP_USED_REGS);

        SBE_TRACD(EXIT_MRK "sendSystemConfig");

        return errl;
    };

} //end namespace SBEIO

