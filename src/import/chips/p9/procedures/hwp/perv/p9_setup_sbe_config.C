/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/perv/p9_setup_sbe_config.C $ */
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
//------------------------------------------------------------------------------
/// @file  p9_setup_sbe_config.C
///
/// @brief proc setup sbe config
//------------------------------------------------------------------------------
// *HWP HW Owner        : Anusha Reddy Rangareddygari <anusrang@in.ibm.com>
// *HWP HW Backup Owner : Srinivas V Naga <srinivan@in.ibm.com>
// *HWP FW Owner        : Brian Silver <bsilver@us.ibm.com>
// *HWP Team            : Perv
// *HWP Level           : 2
// *HWP Consumed by     : FSP
//------------------------------------------------------------------------------


//## auto_generated
#include "p9_setup_sbe_config.H"

#include <p9_perv_scom_addresses.H>
#include <p9_perv_scom_addresses_fld.H>

enum P9_SETUP_SBE_CONFIG_Private_Constants
{
    // Scratch_reg_1
    ATTR_EQ_GARD_STARTBIT             = 0,
    ATTR_EQ_GARD_LENGTH               = 6,
    ATTR_EC_GARD_STARTBIT             = 8,
    ATTR_EC_GARD_LENGTH               = 24,

    // Scratch_reg_2
    ATTR_I2C_BUS_DIV_REF_STARTBIT      = 0,
    ATTR_I2C_BUS_DIV_REF_LENGTH        = 16,
    ATTR_OPTICS_CONFIG_MODE_OBUS0_BIT  = 16,
    ATTR_OPTICS_CONFIG_MODE_OBUS1_BIT  = 17,
    ATTR_OPTICS_CONFIG_MODE_OBUS2_BIT  = 18,
    ATTR_OPTICS_CONFIG_MODE_OBUS3_BIT  = 19,

    // Scratch_reg_3
    ATTR_BOOT_FLAGS_STARTBIT           = 0,
    ATTR_BOOT_FLAGS_LENGTH             = 32,

    // Scratch_reg_4
    ATTR_BOOT_FREQ_MULT_STARTBIT       = 0,
    ATTR_BOOT_FREQ_MULT_LENGTH         = 16,
    ATTR_NEST_PLL_BUCKET_STARTBIT      = 24,
    ATTR_NEST_PLL_BUCKET_LENGTH        = 8,
    ATTR_CP_FILTER_BYPASS_BIT          = 16,
    ATTR_SS_FILTER_BYPASS_BIT          = 17,
    ATTR_IO_FILTER_BYPASS_BIT          = 18,
    ATTR_DPLL_BYPASS_BIT               = 19,
    ATTR_NEST_MEM_X_O_PCI_BYPASS_BIT   = 20,
    ATTR_OBUS_RATIO_VALUE_BIT          = 21,

    // Scratch_reg_5
    ATTR_PLL_MUX_STARTBIT              = 12,
    ATTR_PLL_MUX_LENGTH                = 20,
    ATTR_CC_IPL_BIT                    = 0,
    ATTR_INIT_ALL_CORES_BIT            = 1,
    ATTR_RISK_LEVEL_BIT                = 2,
    ATTR_DISABLE_HBBL_VECTORS_BIT      = 3,
    ATTR_MC_SYNC_MODE_BIT              = 4,

    // Scratch_reg_6
    ATTR_PROC_FABRIC_GROUP_ID_STARTBIT = 26,
    ATTR_PROC_FABRIC_GROUP_ID_LENGTH   = 3,
    ATTR_PROC_FABRIC_CHIP_ID_STARTBIT  = 29,
    ATTR_PROC_FABRIC_CHIP_ID_LENGTH    = 3,
    ATTR_PUMP_CHIP_IS_GROUP            = 23,
};


fapi2::ReturnCode p9_setup_sbe_config(const
                                      fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target_chip)
{
    fapi2::buffer<uint32_t> l_read_scratch_reg = 0;
    fapi2::buffer<uint32_t> l_read_scratch8 = 0;
    fapi2::buffer<uint8_t> l_read_1 = 0;
    fapi2::buffer<uint8_t> l_read_2 = 0;
    fapi2::buffer<uint16_t> l_read_4 = 0;
    fapi2::buffer<uint32_t> l_read_5 = 0;
    fapi2::buffer<uint32_t> l_read_6 = 0;
    fapi2::buffer<uint32_t> l_data32_cbs_cs;
    fapi2::buffer<uint8_t> l_attr_read;
    const fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> FAPI_SYSTEM;
    fapi2::ATTR_OPTICS_CONFIG_MODE_Type l_optics_cfg_mode;
    FAPI_INF("p9_setup_sbe_config::  Entering ...");

    FAPI_DBG("Read Scratch8 for validity of Scratch register");
    //Getting SCRATCH_REGISTER_8 register value
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_8_FSI,
                                    l_read_scratch8)); //l_read_scratch8 = CFAM.SCRATCH_REGISTER_8

    //set_scratch1_reg
    {

        FAPI_DBG("Read Scratch_reg1");
        //Getting SCRATCH_REGISTER_1 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_1_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_1

        FAPI_DBG("Reading ATTR_EQ_GARD, ATTR_EC_GARD");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_EQ_GARD, i_target_chip, l_read_1));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_EC_GARD, i_target_chip, l_read_5));

        l_read_1.extractToRight< 0, ATTR_EQ_GARD_LENGTH >(l_read_2);
        l_read_5.extractToRight< 0, ATTR_EC_GARD_LENGTH >(l_read_6);

        l_read_scratch_reg.insertFromRight< ATTR_EQ_GARD_STARTBIT, ATTR_EQ_GARD_LENGTH >(l_read_2);
        l_read_scratch_reg.insertFromRight< ATTR_EC_GARD_STARTBIT, ATTR_EC_GARD_LENGTH >(l_read_6);

        FAPI_DBG("Setting up value of Scratch_reg1");
        //Setting SCRATCH_REGISTER_1 register value
        //CFAM.SCRATCH_REGISTER_1 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_1_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<0>();
    }
    //set_scratch2_reg
    {

        FAPI_DBG("Reading Scratch_reg2");
        //Getting SCRATCH_REGISTER_2 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_2_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_2

        FAPI_DBG("Reading ATTR_I2C_BUS_DIV_REF");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_I2C_BUS_DIV_REF, i_target_chip, l_read_4));

        l_read_scratch_reg.insertFromRight< ATTR_I2C_BUS_DIV_REF_STARTBIT, ATTR_I2C_BUS_DIV_REF_LENGTH >(l_read_4);

        for (auto& targ : i_target_chip.getChildren<fapi2::TARGET_TYPE_OBUS>() )
        {
            // OBUS
            uint32_t l_chipletID = targ.getChipletNumber();
            FAPI_DBG("Reading ATTR_OPTICS_CONFIG_MODE");
            FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_OPTICS_CONFIG_MODE, targ, l_optics_cfg_mode),
                     "Error from FAPI_ATTR_GET(ATTR_OPTICS_CONFIG_MODE)");

            if (l_chipletID == 9)
            {
                l_read_scratch_reg.writeBit<ATTR_OPTICS_CONFIG_MODE_OBUS0_BIT>(((l_optics_cfg_mode ==
                        fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_SMP)
                        || (l_optics_cfg_mode == fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_CAPI)) ? (1) : (0));
            }

            if (l_chipletID == 10)
            {
                l_read_scratch_reg.writeBit<ATTR_OPTICS_CONFIG_MODE_OBUS1_BIT>(((l_optics_cfg_mode ==
                        fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_SMP)
                        || (l_optics_cfg_mode == fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_CAPI)) ? (1) : (0));
            }

            if (l_chipletID == 11)
            {
                l_read_scratch_reg.writeBit<ATTR_OPTICS_CONFIG_MODE_OBUS2_BIT>(((l_optics_cfg_mode ==
                        fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_SMP)
                        || (l_optics_cfg_mode == fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_CAPI)) ? (1) : (0));
            }

            if (l_chipletID == 12)
            {
                l_read_scratch_reg.writeBit<ATTR_OPTICS_CONFIG_MODE_OBUS3_BIT>(((l_optics_cfg_mode ==
                        fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_SMP)
                        || (l_optics_cfg_mode == fapi2::ENUM_ATTR_OPTICS_CONFIG_MODE_CAPI)) ? (1) : (0));
            }
        }

        FAPI_DBG("Setting up value of Scratch_reg2");
        //Setting SCRATCH_REGISTER_2 register value
        //CFAM.SCRATCH_REGISTER_2 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_2_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<1>();
    }
    //set_scratch3_reg
    {
        FAPI_DBG("Reading Scratch_reg3");
        //Getting SCRATCH_REGISTER_3 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_3_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_3

        FAPI_DBG("Reading the BOOT_FLAGS");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_BOOT_FLAGS, FAPI_SYSTEM, l_read_5));

        l_read_scratch_reg.insertFromRight< ATTR_BOOT_FLAGS_STARTBIT, ATTR_BOOT_FLAGS_LENGTH >(l_read_5);

        FAPI_DBG("Setting up value of Scratch_reg3");
        //Setting SCRATCH_REGISTER_3 register value
        //CFAM.SCRATCH_REGISTER_3 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_3_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<2>();
    }
    //set_scratch4_reg
    {
        uint8_t l_cp_filter_bypass;
        uint8_t l_ss_filter_bypass;
        uint8_t l_io_filter_bypass;
        uint8_t l_dpll_bypass;
        uint8_t l_nest_mem_x_o_pci_bypass;
        uint8_t l_attr_obus_ratio = 0;

        FAPI_DBG("Reading Scratch_reg4");
        //Getting SCRATCH_REGISTER_4 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_4_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_4

        FAPI_DBG("Reading PLL bypass attributes");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_FILTER_BYPASS, i_target_chip, l_cp_filter_bypass),
                 "Error from FAPI_ATTR_GET (ATTR_CP_FILTER_BYPASS");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SS_FILTER_BYPASS, i_target_chip, l_ss_filter_bypass),
                 "Error from FAPI_ATTR_GET (ATTR_SS_FILTER_BYPASS");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IO_FILTER_BYPASS, i_target_chip, l_io_filter_bypass),
                 "Error from FAPI_ATTR_GET (ATTR_IO_FILTER_BYPASS");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_DPLL_BYPASS, i_target_chip, l_dpll_bypass),
                 "Error from FAPI_ATTR_GET (ATTR_DPLL_BYPASS");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_NEST_MEM_X_O_PCI_BYPASS, i_target_chip, l_nest_mem_x_o_pci_bypass),
                 "Error from FAPI_ATTR_GET (ATTR_NEST_MEM_X_O_PCI_BYPASS");

        FAPI_DBG("Reading ATTR_BOOT_FREQ_MULT, ATTR_NEST_PLL_BUCKET");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_BOOT_FREQ_MULT, i_target_chip, l_read_4));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_NEST_PLL_BUCKET, FAPI_SYSTEM, l_read_1));

        l_read_scratch_reg.insertFromRight< ATTR_BOOT_FREQ_MULT_STARTBIT, ATTR_BOOT_FREQ_MULT_LENGTH >(l_read_4);
        l_read_scratch_reg.insertFromRight< ATTR_NEST_PLL_BUCKET_STARTBIT, ATTR_NEST_PLL_BUCKET_LENGTH >(l_read_1);

        l_read_scratch_reg.writeBit<ATTR_CP_FILTER_BYPASS_BIT>(l_cp_filter_bypass & 0x1);
        l_read_scratch_reg.writeBit<ATTR_SS_FILTER_BYPASS_BIT>(l_ss_filter_bypass & 0x1);
        l_read_scratch_reg.writeBit<ATTR_IO_FILTER_BYPASS_BIT>(l_io_filter_bypass & 0x1);
        l_read_scratch_reg.writeBit<ATTR_DPLL_BYPASS_BIT>(l_dpll_bypass & 0x1);
        l_read_scratch_reg.writeBit<ATTR_NEST_MEM_X_O_PCI_BYPASS_BIT>(l_nest_mem_x_o_pci_bypass & 0x1);

        // Setting OBUS ratio
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_OBUS_RATIO_VALUE, i_target_chip,
                               l_attr_obus_ratio));
        l_read_scratch_reg.writeBit<ATTR_OBUS_RATIO_VALUE_BIT>(l_attr_obus_ratio & 0x1);

        FAPI_DBG("Setting up value of Scratch_reg4");
        //Setting SCRATCH_REGISTER_4 register value
        //CFAM.SCRATCH_REGISTER_4 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_4_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<3>();
    }
    //set_scratch5_reg
    {
        uint8_t l_system_ipl_phase;
        uint8_t l_force_all_cores;
        uint8_t l_risk_level;
        uint8_t l_disable_hbbl_vectors;
        uint32_t l_pll_mux;
        uint8_t l_mc_sync_mode;

        FAPI_DBG("Reading Scratch_reg5");
        //Getting SCRATCH_REGISTER_5 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_5_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_5

        FAPI_DBG("Reading control flag attributes");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SYSTEM_IPL_PHASE, FAPI_SYSTEM, l_system_ipl_phase));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SYS_FORCE_ALL_CORES, FAPI_SYSTEM, l_force_all_cores));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_RISK_LEVEL, FAPI_SYSTEM, l_risk_level));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_DISABLE_HBBL_VECTORS, FAPI_SYSTEM, l_disable_hbbl_vectors));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_MC_SYNC_MODE, i_target_chip, l_mc_sync_mode));

        // set cache contained flag
        if (l_system_ipl_phase == fapi2::ENUM_ATTR_SYSTEM_IPL_PHASE_CACHE_CONTAINED)
        {
            l_read_scratch_reg.setBit<ATTR_CC_IPL_BIT>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_CC_IPL_BIT>();
        }

        // set all cores flag
        if (l_force_all_cores)
        {
            l_read_scratch_reg.setBit<ATTR_INIT_ALL_CORES_BIT>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_INIT_ALL_CORES_BIT>();
        }

        // set risk level flag
        if (l_risk_level == fapi2::ENUM_ATTR_RISK_LEVEL_TRUE)
        {
            l_read_scratch_reg.setBit<ATTR_RISK_LEVEL_BIT>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_RISK_LEVEL_BIT>();
        }

        // set disable of HBBL exception vector flag
        if (l_disable_hbbl_vectors == fapi2::ENUM_ATTR_DISABLE_HBBL_VECTORS_TRUE)
        {
            l_read_scratch_reg.setBit<ATTR_DISABLE_HBBL_VECTORS_BIT>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_DISABLE_HBBL_VECTORS_BIT>();
        }

        // set MC sync mode
        if (l_mc_sync_mode)
        {
            l_read_scratch_reg.setBit<ATTR_MC_SYNC_MODE_BIT>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_MC_SYNC_MODE_BIT>();
        }

        FAPI_DBG("Reading PLL mux attributes");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CLOCK_PLL_MUX, i_target_chip, l_pll_mux));
        // set PLL MUX bits
        l_read_scratch_reg.insert<ATTR_PLL_MUX_STARTBIT, ATTR_PLL_MUX_LENGTH, 0>(l_pll_mux);

        FAPI_DBG("Setting up value of Scratch_reg5");
        //Setting SCRATCH_REGISTER_5 register value
        //CFAM.SCRATCH_REGISTER_5 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_5_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<4>();
    }
    //set_scratch6_reg
    {
        uint8_t l_pump_mode;

        FAPI_DBG("Reading Scratch_reg6");
        //Getting SCRATCH_REGISTER_6 register value
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_6_FSI,
                                        l_read_scratch_reg)); //l_read_scratch_reg = CFAM.SCRATCH_REGISTER_6

        FAPI_DBG("Reading attribute for Hostboot slave bit");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_SBE_MASTER_CHIP, i_target_chip,
                               l_read_1));

        if ( l_read_1 )
        {
            l_read_scratch_reg.clearBit<24>();
        }
        else
        {
            l_read_scratch_reg.setBit<24>();
        }

        FAPI_DBG("Reading PUMP MODE");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_FABRIC_PUMP_MODE,
                               fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(),
                               l_pump_mode));

        if (l_pump_mode == fapi2::ENUM_ATTR_PROC_FABRIC_PUMP_MODE_CHIP_IS_GROUP)
        {
            l_read_scratch_reg.setBit<ATTR_PUMP_CHIP_IS_GROUP>();
        }
        else
        {
            l_read_scratch_reg.clearBit<ATTR_PUMP_CHIP_IS_GROUP>();
        }

        FAPI_DBG("Reading ATTR_PROC_FABRIC_GROUP and CHIP_ID");
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_FABRIC_GROUP_ID, i_target_chip,
                               l_read_1));
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_FABRIC_CHIP_ID, i_target_chip,
                               l_read_2));

        l_read_scratch_reg.insertFromRight< ATTR_PROC_FABRIC_GROUP_ID_STARTBIT, ATTR_PROC_FABRIC_GROUP_ID_LENGTH >(l_read_1);
        l_read_scratch_reg.insertFromRight< ATTR_PROC_FABRIC_CHIP_ID_STARTBIT, ATTR_PROC_FABRIC_CHIP_ID_LENGTH >(l_read_2);

        FAPI_DBG("Setting up value of Scratch_reg6");
        //Setting SCRATCH_REGISTER_6 register value
        //CFAM.SCRATCH_REGISTER_6 = l_read_scratch_reg
        FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_6_FSI,
                                        l_read_scratch_reg));

        l_read_scratch8.setBit<5>();
    }
    FAPI_DBG("Setting Scratch8 for validity of Scratch register");
    //Setting SCRATCH_REGISTER_8 register value
    //CFAM.SCRATCH_REGISTER_8 = l_read_scratch8
    FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_SCRATCH_REGISTER_8_FSI,
                                    l_read_scratch8));

    //Reading SECURITY_MODE attribute
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SECURITY_MODE, FAPI_SYSTEM, l_attr_read));

    if(l_attr_read.getBit<7>() == 0)
    {
        FAPI_DBG("Reading CBS Control Status register");
        FAPI_TRY(fapi2::getCfamRegister(i_target_chip, PERV_CBS_CS_FSI, l_data32_cbs_cs));

        if(!l_data32_cbs_cs.getBit<PERV_CBS_CS_SAMPLED_SMD_PIN>()) //SMD=0 indicate chip is in secure mode
        {
            FAPI_DBG("Changing SAB bit to unsecure mode");
            l_data32_cbs_cs.clearBit<PERV_CBS_CS_SECURE_ACCESS_BIT>();
            FAPI_TRY(fapi2::putCfamRegister(i_target_chip, PERV_CBS_CS_FSI, l_data32_cbs_cs));
        }
    }

    FAPI_INF("p9_setup_sbe_config: Exiting ...");

fapi_try_exit:
    return fapi2::current_err;

}
