/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/lib/mc/port.H $     */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2017                        */
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
/// @file port.H
/// @brief Code to support ports (phy _ mc for certain operations)
///
// *HWP HWP Owner: Brian Silver <bsilver@us.ibm.com>
// *HWP HWP Backup: Andre Marin <aamarin@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: HB:FSP

#ifndef _MSS_PORT_H_
#define _MSS_PORT_H_

#include <fapi2.H>

#include <p9_mc_scom_addresses.H>
#include <p9_mc_scom_addresses_fld.H>
#include <lib/mss_attribute_accessors.H>

#include <lib/shared/mss_const.H>
#include <lib/utils/scom.H>
#include <lib/dimm/rank.H>
#include <c_str.H>

namespace mss
{

// I have a dream that port code can be shared among controllers. So, I drive the
// engine from a set of traits. This might be folly. Allow me to dream. BRS

template< fapi2::TargetType T >
class portTraits;

// Centaur port traits
template<>
class portTraits<fapi2::TARGET_TYPE_MBA>
{
    public:
};

// Nimbus port traits
template<>
class portTraits<fapi2::TARGET_TYPE_MCA>
{
    public:
        static constexpr uint64_t FARB5Q_REG = MCA_MBA_FARB5Q;
        static constexpr uint64_t FARB0Q_REG = MCA_MBA_FARB0Q;
        static constexpr uint64_t REFRESH_REG = MCA_MBAREF0Q;
        static constexpr uint64_t ECC_REG = MCA_RECR;
        static constexpr uint64_t CAL0Q_REG = MCA_MBA_CAL0Q;
        static constexpr uint64_t CAL1Q_REG = MCA_MBA_CAL1Q;
        static constexpr uint64_t CAL2Q_REG = MCA_MBA_CAL2Q;
        static constexpr uint64_t CAL3Q_REG = MCA_MBA_CAL3Q;
        static constexpr uint64_t DSM0Q_REG = MCA_MBA_DSM0Q;

        // Danger Will Robinson <wave robot arms> MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0 uses PHY rank ordinal numbers
        // which are different between PHYs. So if you're playing with this register, be sure to map rank numbers.
        static constexpr uint64_t PHY_ZQCAL_REG = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0;

        static constexpr uint64_t PHY_PERIODIC_CAL_CONFIG_REG = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0;
        static constexpr uint64_t PHY_PERIODIC_CAL_RELOAD_REG = MCA_DDRPHY_PC_RELOAD_VALUE0_P0;
        static constexpr uint64_t PHY_CAL_TIMER_RELOAD_REG = MCA_DDRPHY_PC_CAL_TIMER_RELOAD_VALUE_P0;
        static constexpr uint64_t PHY_ZCAL_TIMER_RELOAD_REG = MCA_DDRPHY_PC_ZCAL_TIMER_RELOAD_VALUE_P0;
        static constexpr uint64_t RRQ_REG = MCA_MBA_RRQ0Q;
        static constexpr uint64_t WRQ_REG = MCA_MBA_WRQ0Q;

        static constexpr uint64_t MAGIC_NUMBER_SIM = 765;
        static constexpr uint64_t MAGIC_NUMBER_NOT_SIM = 196605;

        // DPHY01_DDRPHY_PC_RELOAD_VALUE0_P0   0x005   0x8000c0050301143f
        // scom 0x800(0,1)C0050301143F {   # _P[0:1]
        // bits  , scom_data      , expr ;
        // 0:47  , 0x000000000000 , any  ; # reserved
        // 48    , 0b0            , any  ; # PERIODIC_CAL_REQ_EN
        // 49:63 , 0x0001         , any  ; # PERIODIC_RELOAD_VALUE0
        // }
        static constexpr uint64_t PHY_PERIODIC_CAL_RELOAD_VALUE = 0x1;

        enum
        {
            CFG_DDR_DPHY_NCLK =     MCA_MBA_FARB5Q_CFG_DDR_DPHY_NCLK,
            CFG_DDR_DPHY_NCLK_LEN = MCA_MBA_FARB5Q_CFG_DDR_DPHY_NCLK_LEN,
            CFG_DDR_DPHY_PCLK =     MCA_MBA_FARB5Q_CFG_DDR_DPHY_PCLK,
            CFG_DDR_DPHY_PCLK_LEN = MCA_MBA_FARB5Q_CFG_DDR_DPHY_PCLK_LEN,
            CFG_CCS_INST_RESET_ENABLE = MCA_MBA_FARB5Q_CFG_CCS_INST_RESET_ENABLE,
            CFG_DDR_RESETN = MCA_MBA_FARB5Q_CFG_DDR_RESETN,
            CFG_CCS_ADDR_MUX_SEL = MCA_MBA_FARB5Q_CFG_CCS_ADDR_MUX_SEL,

            REFRESH_ENABLE = MCA_MBAREF0Q_CFG_REFRESH_ENABLE,

            ECC_CHECK_DISABLE = MCA_RECR_MBSECCQ_DISABLE_MEMORY_ECC_CHECK_CORRECT,
            ECC_CORRECT_DISABLE = MCA_RECR_MBSECCQ_DISABLE_MEMORY_ECC_CORRECT,
            ECC_USE_ADDR_HASH = MCA_RECR_MBSECCQ_USE_ADDRESS_HASH,

            PORT_FAIL_DISABLE = MCA_MBA_FARB0Q_CFG_PORT_FAIL_DISABLE,
            OE_ALWAYS_ON = MCA_MBA_FARB0Q_CFG_OE_ALWAYS_ON,
            RCD_RECOVERY_DISABLE = MCA_MBA_FARB0Q_CFG_DISABLE_RCD_RECOVERY,

            CAL0Q_CAL_INTERVAL_TMR0_ENABLE = MCA_MBA_CAL0Q_CFG_CAL_INTERVAL_TMR0_ENABLE,
            CAL0Q_TIME_BASE_TMR0 = MCA_MBA_CAL0Q_CFG_TIME_BASE_TMR0,
            CAL0Q_TIME_BASE_TMR0_LEN = MCA_MBA_CAL0Q_CFG_TIME_BASE_TMR0_LEN,
            CAL0Q_INTERVAL_COUNTER_TMR0 = MCA_MBA_CAL0Q_CFG_INTERVAL_COUNTER_TMR0,
            CAL0Q_INTERVAL_COUNTER_TMR0_LEN = MCA_MBA_CAL0Q_CFG_INTERVAL_COUNTER_TMR0_LEN,
            CAL0Q_CAL_TMR0_CAL1_ENABLE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL1_ENABLE,
            CAL0Q_CAL_TMR0_CAL1_TYPE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL1_TYPE,
            CAL0Q_CAL_TMR0_CAL1_TYPE_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL1_TYPE_LEN,
            CAL0Q_CAL_TMR0_CAL1_DDR_DONE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL1_DDR_DONE,
            CAL0Q_CAL_TMR0_CAL2_ENABLE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL2_ENABLE,
            CAL0Q_CAL_TMR0_CAL2_TYPE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL2_TYPE,
            CAL0Q_CAL_TMR0_CAL2_TYPE_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL2_TYPE_LEN,
            CAL0Q_CAL_TMR0_CAL2_DDR_DONE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL2_DDR_DONE,
            CAL0Q_CAL_TMR0_CAL3_ENABLE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL3_ENABLE,
            CAL0Q_CAL_TMR0_CAL3_TYPE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL3_TYPE,
            CAL0Q_CAL_TMR0_CAL3_TYPE_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL3_TYPE_LEN,
            CAL0Q_CAL_TMR0_CAL3_DDR_DONE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_CAL3_DDR_DONE,
            CAL0Q_CAL_TMR0_Z_SYNC = MCA_MBA_CAL0Q_CFG_CAL_TMR0_Z_SYNC,
            CAL0Q_CAL_TMR0_Z_SYNC_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_Z_SYNC_LEN,
            CAL0Q_CAL_TMR0_DDR_RESET_TMR = MCA_MBA_CAL0Q_CFG_CAL_TMR0_DDR_RESET_TMR,
            CAL0Q_CAL_TMR0_DDR_RESET_TMR_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_DDR_RESET_TMR_LEN,
            CAL0Q_CAL_TMR0_DDR_RESET_TMR_TB = MCA_MBA_CAL0Q_CFG_CAL_TMR0_DDR_RESET_TMR_TB,
            CAL0Q_CAL_TMR0_DDR_RESET_TMR_TB_LEN = MCA_MBA_CAL0Q_CFG_CAL_TMR0_DDR_RESET_TMR_TB_LEN,
            CAL0Q_CAL_TMR0_DDR_RESET_ENABLE = MCA_MBA_CAL0Q_CFG_CAL_TMR0_DDR_RESET_ENABLE,
            CAL0Q_CAL_TMR0_SINGLE_RANK = MCA_MBA_CAL0Q_CFG_CAL_TMR0_SINGLE_RANK,
            CAL0Q_RVED_51 = MCA_MBA_CAL0Q_RESERVED_51,
            CAL0Q_CT_1HOT_SM_ERROR = MCA_MBA_CAL0Q_INJECT_1HOT_SM_ERROR,
            CAL0Q_CAL_SINGLE_PORT_MODE = MCA_MBA_CAL0Q_CFG_CAL_SINGLE_PORT_MODE,
            CAL0Q_CAL_SINGLE_PORT_MODE_LEN = MCA_MBA_CAL0Q_CFG_CAL_SINGLE_PORT_MODE_LEN,
            CAL0Q_BUS_BIT = MCA_MBA_CAL0Q_DBG_BUS_BIT,
            CAL0Q_T_RECOVER = MCA_MBA_CAL0Q_RESET_RECOVER,
            CAL0Q_RANK_SM_STALL_DISABLE = MCA_MBA_CAL0Q_CFG_RANK_SM_STALL_DISABLE,
            CAL0Q_ENABLE_SPEC_ATTN = MCA_MBA_CAL0Q_CFG_ENABLE_SPEC_ATTN,
            CAL0Q_ENABLE_HOST_ATTN = MCA_MBA_CAL0Q_CFG_ENABLE_HOST_ATTN,

            CAL1Q_CAL_INTERVAL_TMR1_ENABLE = MCA_MBA_CAL1Q_CFG_CAL_INTERVAL_TMR1_ENABLE,
            CAL1Q_TIME_BASE_TMR1 = MCA_MBA_CAL1Q_CFG_TIME_BASE_TMR1,
            CAL1Q_TIME_BASE_TMR1_LEN = MCA_MBA_CAL1Q_CFG_TIME_BASE_TMR1_LEN,
            CAL1Q_INTERVAL_COUNTER_TMR1 = MCA_MBA_CAL1Q_CFG_INTERVAL_COUNTER_TMR1,
            CAL1Q_INTERVAL_COUNTER_TMR1_LEN = MCA_MBA_CAL1Q_CFG_INTERVAL_COUNTER_TMR1_LEN,
            CAL1Q_CAL_TMR1_CAL1_ENABLE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL1_ENABLE,
            CAL1Q_CAL_TMR1_CAL1_TYPE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL1_TYPE,
            CAL1Q_CAL_TMR1_CAL1_TYPE_LEN = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL1_TYPE_LEN,
            CAL1Q_CAL_TMR1_CAL1_DDR_DONE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL1_DDR_DONE,
            CAL1Q_CAL_TMR1_CAL2_ENABLE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL2_ENABLE,
            CAL1Q_CAL_TMR1_CAL2_TYPE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL2_TYPE,
            CAL1Q_CAL_TMR1_CAL2_TYPE_LEN = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL2_TYPE_LEN,
            CAL1Q_CAL_TMR1_CAL2_DDR_DONE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL2_DDR_DONE,
            CAL1Q_CAL_TMR1_CAL3_ENABLE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL3_ENABLE,
            CAL1Q_CAL_TMR1_CAL3_TYPE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL3_TYPE,
            CAL1Q_CAL_TMR1_CAL3_TYPE_LEN = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL3_TYPE_LEN,
            CAL1Q_CAL_TMR1_CAL3_DDR_DONE = MCA_MBA_CAL1Q_CFG_CAL_TMR1_CAL3_DDR_DONE,
            CAL1Q_CAL_TMR1_Z_SYNC = MCA_MBA_CAL1Q_CFG_CAL_TMR1_Z_SYNC,
            CAL1Q_CAL_TMR1_Z_SYNC_LEN = MCA_MBA_CAL1Q_CFG_CAL_TMR1_Z_SYNC_LEN,
            CAL1Q_CAL_TMR1_SINGLE_RANK = MCA_MBA_CAL1Q_CFG_CAL_TMR1_SINGLE_RANK,
            CAL1Q_CAL_RANK_ENABLE = MCA_MBA_CAL1Q_CFG_CAL_RANK_ENABLE,
            CAL1Q_CAL_RANK_ENABLE_LEN = MCA_MBA_CAL1Q_CFG_CAL_RANK_ENABLE_LEN,

            CAL2Q_CAL_INTERVAL_TMR2_ENABLE = MCA_MBA_CAL2Q_CFG_CAL_INTERVAL_TMR2_ENABLE,
            CAL2Q_TIME_BASE_TMR2 = MCA_MBA_CAL2Q_CFG_TIME_BASE_TMR2,
            CAL2Q_TIME_BASE_TMR2_LEN = MCA_MBA_CAL2Q_CFG_TIME_BASE_TMR2_LEN,
            CAL2Q_INTERVAL_COUNTER_TMR2 = MCA_MBA_CAL2Q_CFG_INTERVAL_COUNTER_TMR2,
            CAL2Q_INTERVAL_COUNTER_TMR2_LEN = MCA_MBA_CAL2Q_CFG_INTERVAL_COUNTER_TMR2_LEN,
            CAL2Q_CAL_TMR2_CAL1_ENABLE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL1_ENABLE,
            CAL2Q_CAL_TMR2_CAL1_TYPE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL1_TYPE,
            CAL2Q_CAL_TMR2_CAL1_TYPE_LEN = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL1_TYPE_LEN,
            CAL2Q_CAL_TMR2_CAL1_DDR_DONE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL1_DDR_DONE,
            CAL2Q_CAL_TMR2_CAL2_ENABLE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL2_ENABLE,
            CAL2Q_CAL_TMR2_CAL2_TYPE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL2_TYPE,
            CAL2Q_CAL_TMR2_CAL2_TYPE_LEN = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL2_TYPE_LEN,
            CAL2Q_CAL_TMR2_CAL2_DDR_DONE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL2_DDR_DONE,
            CAL2Q_CAL_TMR2_CAL3_ENABLE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL3_ENABLE,
            CAL2Q_CAL_TMR2_CAL3_TYPE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL3_TYPE,
            CAL2Q_CAL_TMR2_CAL3_TYPE_LEN = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL3_TYPE_LEN,
            CAL2Q_CAL_TMR2_CAL3_DDR_DONE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_CAL3_DDR_DONE,
            CAL2Q_CAL_TMR2_Z_SYNC = MCA_MBA_CAL2Q_CFG_CAL_TMR2_Z_SYNC,
            CAL2Q_CAL_TMR2_Z_SYNC_LEN = MCA_MBA_CAL2Q_CFG_CAL_TMR2_Z_SYNC_LEN,
            CAL2Q_CAL_TMR2_SINGLE_RANK = MCA_MBA_CAL2Q_CFG_CAL_TMR2_SINGLE_RANK,
            CAL2Q_CAL_TMR2_WAT_EVENT_ENABLE = MCA_MBA_CAL2Q_CFG_CAL_TMR2_WAT_EVENT_ENABLE,

            CAL3Q_INTERNAL_ZQ_TB = MCA_MBA_CAL3Q_CFG_INTERNAL_ZQ_TB,
            CAL3Q_INTERNAL_ZQ_TB_LEN = MCA_MBA_CAL3Q_CFG_INTERNAL_ZQ_TB_LEN,
            CAL3Q_INTERNAL_ZQ_LENGTH = MCA_MBA_CAL3Q_CFG_INTERNAL_ZQ_LENGTH,
            CAL3Q_INTERNAL_ZQ_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_INTERNAL_ZQ_LENGTH_LEN,
            CAL3Q_EXTERNAL_ZQ_TB = MCA_MBA_CAL3Q_CFG_EXTERNAL_ZQ_TB,
            CAL3Q_EXTERNAL_ZQ_TB_LEN = MCA_MBA_CAL3Q_CFG_EXTERNAL_ZQ_TB_LEN,
            CAL3Q_EXTERNAL_ZQ_LENGTH = MCA_MBA_CAL3Q_CFG_EXTERNAL_ZQ_LENGTH,
            CAL3Q_EXTERNAL_ZQ_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_EXTERNAL_ZQ_LENGTH_LEN,
            CAL3Q_RDCLK_SYSCLK_TB = MCA_MBA_CAL3Q_CFG_RDCLK_SYSCLK_TB,
            CAL3Q_RDCLK_SYSCLK_TB_LEN = MCA_MBA_CAL3Q_CFG_RDCLK_SYSCLK_TB_LEN,
            CAL3Q_RDCLK_SYSCLK_LENGTH = MCA_MBA_CAL3Q_CFG_RDCLK_SYSCLK_LENGTH,
            CAL3Q_RDCLK_SYSCLK_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_RDCLK_SYSCLK_LENGTH_LEN,
            CAL3Q_DQS_ALIGNMENT_TB = MCA_MBA_CAL3Q_CFG_DQS_ALIGNMENT_TB,
            CAL3Q_DQS_ALIGNMENT_TB_LEN = MCA_MBA_CAL3Q_CFG_DQS_ALIGNMENT_TB_LEN,
            CAL3Q_DQS_ALIGNMENT_LENGTH = MCA_MBA_CAL3Q_CFG_DQS_ALIGNMENT_LENGTH,
            CAL3Q_DQS_ALIGNMENT_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_DQS_ALIGNMENT_LENGTH_LEN,
            CAL3Q_MPR_READEYE_TB = MCA_MBA_CAL3Q_CFG_MPR_READEYE_TB,
            CAL3Q_MPR_READEYE_TB_LEN = MCA_MBA_CAL3Q_CFG_MPR_READEYE_TB_LEN,
            CAL3Q_MPR_READEYE_LENGTH = MCA_MBA_CAL3Q_CFG_MPR_READEYE_LENGTH,
            CAL3Q_MPR_READEYE_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_MPR_READEYE_LENGTH_LEN,
            CAL3Q_ALL_PERIODIC_TB = MCA_MBA_CAL3Q_CFG_ALL_PERIODIC_TB,
            CAL3Q_ALL_PERIODIC_TB_LEN = MCA_MBA_CAL3Q_CFG_ALL_PERIODIC_TB_LEN,
            CAL3Q_ALL_PERIODIC_LENGTH = MCA_MBA_CAL3Q_CFG_ALL_PERIODIC_LENGTH,
            CAL3Q_ALL_PERIODIC_LENGTH_LEN = MCA_MBA_CAL3Q_CFG_ALL_PERIODIC_LENGTH_LEN,
            CAL3Q_FREEZE_ON_PARITY_ERROR_DIS = MCA_MBA_CAL3Q_CFG_FREEZE_ON_PARITY_ERROR_DIS,

            DSM0Q_CFG_RDTAG_DLY = MCA_MBA_DSM0Q_CFG_RDTAG_DLY,
            DSM0Q_CFG_RDTAG_DLY_LEN = MCA_MBA_DSM0Q_CFG_RDTAG_DLY_LEN,

            PER_ZCAL_ENA_RANK = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0_ENA_RANK,
            PER_ZCAL_ENA_RANK_LEN = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0_ENA_RANK_LEN,
            PER_ZCAL_NEXT_RANK = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0_NEXT_RANK,
            PER_ZCAL_NEXT_RANK_LEN = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0_NEXT_RANK_LEN,
            PER_ZCAL_START = MCA_DDRPHY_PC_PER_ZCAL_CONFIG_P0_START,

            ZCAL_TIMER_RELOAD_VALUE = MCA_DDRPHY_PC_ZCAL_TIMER_RELOAD_VALUE_P0_PERIODIC,
            ZCAL_TIMER_RELOAD_VALUE_LEN = MCA_DDRPHY_PC_ZCAL_TIMER_RELOAD_VALUE_P0_PERIODIC_LEN,

            PC_CAL_TIMER_RELOAD_VALUE = MCA_DDRPHY_PC_CAL_TIMER_RELOAD_VALUE_P0_PERIODIC,
            PC_CAL_TIMER_RELOAD_VALUE_LEN = MCA_DDRPHY_PC_CAL_TIMER_RELOAD_VALUE_P0_PERIODIC_LEN,

            PER_ENA_RANK_PAIR = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_RANK_PAIR,
            PER_ENA_RANK_PAIR_LEN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_RANK_PAIR_LEN,
            PER_ENA_ZCAL = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_ZCAL,
            PER_ENA_SYSCLK_ALIGN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_SYSCLK_ALIGN,
            PER_ENA_READ_CTR = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_READ_CTR,
            PER_ENA_RDCLK_ALIGN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_RDCLK_ALIGN,
            PER_ENA_DQS_ALIGN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ENA_DQS_ALIGN,
            PER_NEXT_RANK_PAIR = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_NEXT_RANK_PAIR,
            PER_NEXT_RANK_PAIR_LEN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_NEXT_RANK_PAIR_LEN,
            PER_FAST_SIM_CNTR = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_FAST_SIM_CNTR,
            PER_START_INIT = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_START_INIT,
            PER_START = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_START,
            PER_ABORT_ON_ERR_EN = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_ABORT_ON_ERR_EN,
            PER_DD2_FIX_DIS = MCA_DDRPHY_PC_PER_CAL_CONFIG_P0_DD2_FIX_DIS,

            RRQ_FIFO_MODE = MCA_MBA_RRQ0Q_CFG_RRQ_FIFO_MODE,
            WRQ_FIFO_MODE = MCA_MBA_WRQ0Q_CFG_WRQ_FIFO_MODE,
        };
};

///
/// @brief Read the rdtag delay value
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[out] o_delay RDTAG_DLY value (in cycles)
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode read_rdtag_delay( const fapi2::Target<T>& i_target, uint64_t& o_delay )
{

    fapi2::buffer<uint64_t> l_data;
    FAPI_TRY( mss::getScom(i_target, TT::DSM0Q_REG, l_data) );
    l_data.template extractToRight<TT::DSM0Q_CFG_RDTAG_DLY, TT::DSM0Q_CFG_RDTAG_DLY_LEN>(o_delay);

    FAPI_INF( "RDTAG_DLY %d for %s", uint64_t(o_delay), mss::c_str(i_target) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Change the rdtag delay value
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_delay new RDTAG_DLY value (in cycles) to set
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_rdtag_delay( const fapi2::Target<T>& i_target, const uint64_t i_delay )
{
    // RDTAG is only 6 bits in length, input could be bigger
    // so we just let the user know of an invalid input.
    // This is a programming error so we assert out.
    // 6 bits - 0b0011 1111 (right aligned)
    constexpr uint64_t MAX_DELAY = 0x3F;

    if( i_delay > MAX_DELAY )
    {
        FAPI_ERR("Invalid delay received: %d, largest possible: %d", i_delay, MAX_DELAY);
        fapi2::Assert(false);
    }

    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG( "Change RDTAG_DLY to %d for %s",  i_delay, mss::c_str(i_target) );

    FAPI_TRY( mss::getScom(i_target, TT::DSM0Q_REG, l_data) );
    l_data.insertFromRight<TT::DSM0Q_CFG_RDTAG_DLY, TT::DSM0Q_CFG_RDTAG_DLY_LEN>(i_delay);
    FAPI_TRY( mss::putScom(i_target, TT::DSM0Q_REG, l_data) );

    FAPI_INF( "DSM0Q_REG 0x%016lx for %s", uint64_t(l_data), mss::c_str(i_target) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Change the state of the port_fail_disable bit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_state the state
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_port_fail_disable( const fapi2::Target<T>& i_target, states i_state )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG("Change port fail disable to %s %s", (i_state == HIGH ? "high" : "low"), mss::c_str(i_target));
    FAPI_TRY( mss::getScom(i_target, TT::FARB0Q_REG, l_data) );
    l_data.writeBit<TT::PORT_FAIL_DISABLE>(i_state);
    FAPI_TRY( mss::putScom(i_target, TT::FARB0Q_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Change the state of the RCD recovery bit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_state the state
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_rcd_recovery_disable( const fapi2::Target<T>& i_target, states i_state )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG("Change rcd recovery disable to %s %s", (i_state == HIGH ? "high" : "low"), mss::c_str(i_target));
    FAPI_TRY( mss::getScom(i_target, TT::FARB0Q_REG, l_data) );
    l_data.writeBit<TT::RCD_RECOVERY_DISABLE>(i_state);
    FAPI_TRY( mss::putScom(i_target, TT::FARB0Q_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Change the state of the output enable always-on bit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_state the state
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_oe_always_on( const fapi2::Target<T>& i_target, states i_state )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG("Change OE always on to %s %s", (i_state == HIGH ? "high" : "low"), mss::c_str(i_target));
    FAPI_TRY( mss::getScom(i_target, TT::FARB0Q_REG, l_data) );
    l_data.writeBit<TT::OE_ALWAYS_ON>(i_state);
    FAPI_TRY( mss::putScom(i_target, TT::FARB0Q_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Change the state of the addr_mux_sel bit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_state the state
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_addr_mux_sel( const fapi2::Target<T>& i_target, states i_state )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG("Change addr_mux_sel to %s %s", (i_state == HIGH ? "high" : "low"), mss::c_str(i_target));
    FAPI_TRY( mss::getScom(i_target, TT::FARB5Q_REG, l_data) );
    l_data.writeBit<TT::CFG_CCS_ADDR_MUX_SEL>(i_state);
    FAPI_TRY( mss::putScom(i_target, TT::FARB5Q_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}


///
/// @brief Change the state of the MC Refresh enable bit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @param[in] i_state the state
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode change_refresh_enable( const fapi2::Target<T>& i_target, states i_state )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_DBG("Change refresh enable to %s %s", (i_state == HIGH ? "high" : "low"), mss::c_str(i_target));
    FAPI_TRY( mss::getScom(i_target, TT::REFRESH_REG, l_data) );
    l_data.writeBit<TT::REFRESH_ENABLE>(i_state);
    FAPI_TRY( mss::putScom(i_target, TT::REFRESH_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enable the MC Periodic calibration functionality
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode enable_periodic_cal( const fapi2::Target<T>& i_target );

///
/// @brief Enable the MC Periodic calibration functionality - MCA specialization
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template<>
fapi2::ReturnCode enable_periodic_cal( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target );

///
/// @brief Enable Read ECC checking
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode enable_read_ecc( const fapi2::Target<T>& i_target )
{
    fapi2::buffer<uint64_t> l_data;
    uint8_t is_sim = 0;

    FAPI_DBG("Enable Read ECC %s", mss::c_str(i_target));

    FAPI_TRY( mss::getScom(i_target, TT::ECC_REG, l_data) );
    l_data.clearBit<TT::ECC_CHECK_DISABLE>();
    l_data.clearBit<TT::ECC_CORRECT_DISABLE>();
    l_data.setBit<TT::ECC_USE_ADDR_HASH>();

    // The preferred operating mode is 11 (INVERT_DATA_TOGGLE_CHECKS)   which stores data complemented
    // (because most bits are '0', and the dram bus pulls up, so transmitting 1s is least power)  but
    // still flips the inversion of check bits to aid RAS. Per Brad Michael 12/15
    // Leave un-inverted for sim. This allows the DIMM loader to write 0's and effect good ECC
    FAPI_TRY( FAPI_ATTR_GET(fapi2::ATTR_IS_SIMULATION, fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(), is_sim) );
    l_data.insertFromRight<MCA_RECR_MBSECCQ_DATA_INVERSION, MCA_RECR_MBSECCQ_DATA_INVERSION_LEN>(is_sim ? 0b00 : 0b11);

    // bits: 60 MBSTRQ_CFG_MAINT_RCE_WITH_CE
    // cfg_maint_rce_with_ce - not implemented. Need to investigate if needed for nimbus.

    FAPI_TRY( mss::putScom(i_target, TT::ECC_REG, l_data) );

fapi_try_exit:
    return fapi2::current_err;
}

//
// We expect to come in to draminit with the following setup:
// 1. ENABLE_RESET_N (FARB5Q(6)) 0
// 2. RESET_N (FARB5Q(4)) 1 - out of reset (done in draminit as a separate step)
// 3. CCS_ADDR_MUX_SEL (FARB5Q(5)) - 1
// 4. CKE out of high impedence
// Note: Ignore resetn as it's taken care of as a separate step
//
///
/// @brief Secure the entry criteria for draminit
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target A target representing a port
/// @return FAPI2_RC_SUCCESS if and only if ok
// This is in this header as it's hoped to be able to be shared. Seems to make more
// Might make more sense in p9_mss_draminit.C ... BRS
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
inline fapi2::ReturnCode draminit_entry_invariant( const fapi2::Target<T>& i_target )
{
    fapi2::buffer<uint64_t> l_data;
    FAPI_TRY( mss::getScom(i_target, TT::FARB5Q_REG, l_data) );

    if ((l_data.getBit<TT::CFG_CCS_ADDR_MUX_SEL>() != HIGH) || (l_data.getBit<TT::CFG_CCS_INST_RESET_ENABLE>() != LOW))
    {
        // We have some bits not set correctly. Lets try to reset the register.
        FAPI_INF("FARB5Q: 0x%llx, setting MUX_SEL, clearing RESET_ENABLE", uint64_t(l_data));
        l_data.setBit<TT::CFG_CCS_ADDR_MUX_SEL>();
        l_data.clearBit<TT::CFG_CCS_INST_RESET_ENABLE>();
        FAPI_TRY( mss::putScom(i_target, TT::FARB5Q_REG, l_data) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Drive memory clocks
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target A target representing a port
/// @param[in] i_pclk phy p clock - right most 2 bits
/// @param[in] i_nclk phy n clock - right most 2 bits
/// @return FAPI2_RC_SUCCESS if and only if ok
/// @note this might need a port id added for Centaur/MBA controllers
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode drive_mem_clks( const fapi2::Target<T>& i_target, const uint64_t i_pclk, const uint64_t i_nclk )
{
    fapi2::buffer<uint64_t> l_data;

    FAPI_INF("Drive mem clocks pclk: 0x%x nclk 0x%x", i_pclk, i_nclk);
    FAPI_TRY( mss::getScom(i_target, TT::FARB5Q_REG, l_data) );

    l_data.insertFromRight<TT::CFG_DDR_DPHY_NCLK, TT::CFG_DDR_DPHY_NCLK_LEN>(i_nclk);
    l_data.insertFromRight<TT::CFG_DDR_DPHY_PCLK, TT::CFG_DDR_DPHY_PCLK_LEN>(i_pclk);

    FAPI_TRY( mss::putScom(i_target, TT::FARB5Q_REG, l_data) );

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    FAPI_ERR("Unable to drive mem clocks: %s", mss::c_str(i_target));
    return fapi2::current_err;
}

///
/// @brief Set DDR resetn
/// @tparam T the fapi2 target type of the target
/// @tparam TT the class traits for the port
/// @param[in] i_target A target representing a port
/// @param[in] i_state high or low
/// @return FAPI2_RC_SUCCESS if and only if ok
/// @note this might need a port id added for Centaur/MBA controllers
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode ddr_resetn( const fapi2::Target<T>& i_target, const bool i_state )
{
    fapi2::buffer<uint64_t> l_data;
    FAPI_TRY( mss::getScom(i_target, TT::FARB5Q_REG, l_data) );

    if (l_data.getBit<TT::CFG_DDR_RESETN>() != i_state)
    {
        l_data.writeBit<TT::CFG_DDR_RESETN>(i_state);
        FAPI_DBG("ddr_resetn transitioning to %d (0x%llx)", i_state, l_data);
        FAPI_TRY( mss::putScom(i_target, TT::FARB5Q_REG, l_data) );
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    FAPI_ERR("Unable to change resetn: %s (%d)", mss::c_str(i_target), i_state);
    return fapi2::current_err;
}

///
/// @brief Apply mark store bits from module VPD
/// @tparam T, the fapi2 target type of the target
/// @tparam TT, the class traits for the port
/// @param[in] i_target A target representing a port
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode apply_mark_store( const fapi2::Target<T>& i_target )
{
    FAPI_INF("Enable marks from MVPD");

    uint32_t l_fwms[MARK_STORE_COUNT];

    FAPI_TRY( mss::mvpd_fwms(i_target, &(l_fwms[0])) );

    for (size_t l_mark = 0; l_mark < MARK_STORE_COUNT; ++l_mark)
    {
        if (l_fwms[l_mark] != 0)
        {
            fapi2::buffer<uint64_t> l_fwms_data;
            l_fwms_data.insertFromRight < MCA_FWMS0_MARK, MCA_FWMS0_EXIT_1 - MCA_FWMS0_MARK + 1 > (l_fwms[l_mark]);
            FAPI_TRY( mss::putScom(i_target, MCA_FWMS0 + l_mark, l_fwms_data) );
        }
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Reset the ZCAL config register.
/// @warning This maps PHY rank numbers per target
/// @tparam T, the fapi2 target type of the target
/// @param[in] i_target A target representing a port
/// @return FAPI2_RC_SUCCESS if and only if ok
///
template< fapi2::TargetType T, typename TT = portTraits<T> >
fapi2::ReturnCode reset_zqcal_config( const fapi2::Target<T>& i_target )
{
    fapi2::buffer<uint64_t> l_phy_zqcal_config;
    std::vector<uint64_t> l_ranks;

    FAPI_TRY( mss::rank::ranks(i_target, l_ranks) );

    for (const auto r : l_ranks)
    {
        l_phy_zqcal_config.setBit(TT::PER_ZCAL_ENA_RANK + rank::map_rank_ordinal_to_phy(i_target, r));
    }

    // Write the ZQCAL periodic config
    FAPI_INF("zcal periodic config: 0x%016lx", l_phy_zqcal_config);
    FAPI_TRY( mss::putScom(i_target, TT::PHY_ZQCAL_REG, l_phy_zqcal_config) );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Configures the write reorder queue for MCBIST operations
/// @param[in] i_target the target to effect
/// @param[in] i_state to set the bit too
/// @return FAPI2_RC_SUCCSS iff ok
///
inline fapi2::ReturnCode configure_wrq(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target, const mss::states i_state)
{
    typedef portTraits<fapi2::TARGET_TYPE_MCA> TT;

    fapi2::buffer<uint64_t> l_data;

    // Gets the reg
    FAPI_TRY(mss::getScom(i_target, TT::WRQ_REG, l_data), "%s failed to getScom from MCA_MBA_WRQ0Q", mss::c_str(i_target));

    // Sets the bit
    l_data.writeBit<TT::WRQ_FIFO_MODE>(i_state == mss::states::ON);

    // Sets the regs
    FAPI_TRY(mss::putScom(i_target, TT::WRQ_REG, l_data), "%s failed to putScom to MCA_MBA_WRQ0Q", mss::c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}


///
/// @brief Configures the write reorder queue bit
/// @param[in] i_target the target to effect
/// @param[in] i_state to set the bit too
/// @return FAPI2_RC_SUCCSS iff ok
///
inline fapi2::ReturnCode configure_wrq(const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                                       const mss::states i_state)
{
    // Loops through all MCA targets, hitting all the registers
    for( const auto& l_mca : mss::find_targets<fapi2::TARGET_TYPE_MCA>(i_target) )
    {
        FAPI_TRY(configure_wrq(l_mca, i_state));
    }

    // In case we don't have any MCA's
    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Configures the read reorder queue for MCBIST operations
/// @param[in] i_target the target to effect
/// @param[in] i_state to set the bit too
/// @return FAPI2_RC_SUCCSS iff ok
///
inline fapi2::ReturnCode configure_rrq(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target, const mss::states i_state)
{
    typedef portTraits<fapi2::TARGET_TYPE_MCA> TT;

    fapi2::buffer<uint64_t> l_data;

    // Gets the reg
    FAPI_TRY(mss::getScom(i_target, TT::RRQ_REG, l_data), "%s failed to getScom from MCA_MBA_RRQ0Q", mss::c_str(i_target));

    // Sets the bit
    l_data.writeBit<TT::RRQ_FIFO_MODE>(i_state == mss::states::ON);

    // Sets the regs
    FAPI_TRY(mss::putScom(i_target, TT::RRQ_REG, l_data), "%s failed to putScom to MCA_MBA_RRQ0Q", mss::c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Configures the read reorder queue bit
/// @param[in] i_target the target to effect
/// @param[in] i_state to set the bit too
/// @return FAPI2_RC_SUCCSS iff ok
///
inline fapi2::ReturnCode configure_rrq(const fapi2::Target<fapi2::TARGET_TYPE_MCBIST>& i_target,
                                       const mss::states i_state)
{
    // Loops through all MCA targets, hitting all the registers
    for( const auto& l_mca : mss::find_targets<fapi2::TARGET_TYPE_MCA>(i_target) )
    {
        FAPI_TRY(configure_rrq(l_mca, i_state));
    }

    // In case we don't have any MCA's
    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Resets the write/read reorder queue values - needs to be called after MCBIST execution
/// @tparam T, the fapi2 target type of the target
/// @param[in] i_target the target to effect
/// @return FAPI2_RC_SUCCSS iff ok
///
template< fapi2::TargetType T>
fapi2::ReturnCode reset_reorder_queue_settings(const fapi2::Target<T>& i_target)
{
    uint8_t l_reorder_queue = 0;
    FAPI_TRY(reorder_queue_setting(i_target, l_reorder_queue));

    // Changes the reorder queue settings
    {
        // Two settings are FIFO and REORDER.  FIFO is a 1 in the registers, while reorder is a 0 state
        const mss::states l_state = ((l_reorder_queue == fapi2::ENUM_ATTR_MSS_REORDER_QUEUE_SETTING_FIFO) ?
                                     mss::states::ON : mss::states::OFF);
        FAPI_TRY(configure_rrq(i_target, l_state), "%s failed to reset read reorder queue settings", mss::c_str(i_target));
        FAPI_TRY(configure_wrq(i_target, l_state), "%s failed to reset read reorder queue settings", mss::c_str(i_target));
    }


fapi_try_exit:
    return fapi2::current_err;
}

}// mss

#endif
