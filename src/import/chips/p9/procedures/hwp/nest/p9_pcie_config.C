/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/nest/p9_pcie_config.C $    */
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
//-----------------------------------------------------------------------------------
///
/// @file p9_pcie_config.C
/// @brief Perform PCIE Phase2 init sequence (FAPI2)
///

// *HWP HWP Owner: Christina Graves clgraves@us.ibm.com
// *HWP FW Owner: Thi Tran thi@us.ibm.com
// *HWP Team: Nest
// *HWP Level: 2
// *HWP Consumed by: HB

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <p9_pcie_config.H>
#include <p9_fbc_utils.H>

#include "p9_misc_scom_addresses.H"
#include "p9_misc_scom_addresses_fld.H"


//-----------------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------------
const uint8_t P9_PCIE_CONFIG_BAR_SHIFT = 8;

//-----------------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------------
const uint64_t PCI_NFIR_ACTION0_REG = 0x5B0F81E000000000ULL;
const uint64_t PCI_NFIR_ACTION1_REG = 0x7F0F81E000000000ULL;
const uint64_t PCI_NFIR_MASK_REG    = 0x0030001C00000000ULL;

const uint64_t PCI_PFIR_ACTION0_REG = 0xB000000000000000ULL;
const uint64_t PCI_PFIR_ACTION1_REG = 0xB000000000000000ULL;
const uint64_t PCI_PFIR_MASK_REG    = 0x0E00000000000000ULL;

//-----------------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------------
fapi2::ReturnCode p9_pcie_config(const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target)
{
    FAPI_INF("Start");
    fapi2::ATTR_PROC_PCIE_MMIO_BAR0_BASE_ADDR_OFFSET_Type l_mmio_bar0_offsets;
    fapi2::ATTR_PROC_PCIE_MMIO_BAR1_BASE_ADDR_OFFSET_Type l_mmio_bar1_offsets;
    fapi2::ATTR_PROC_PCIE_REGISTER_BAR_BASE_ADDR_OFFSET_Type l_register_bar_offsets;
    fapi2::ATTR_PROC_PCIE_BAR_SIZE_Type l_bar_sizes;

    fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> FAPI_SYSTEM;

    fapi2::buffer<uint64_t> l_buf = 0;
    uint64_t l_base_addr_nm0, l_base_addr_nm1, l_base_addr_m, l_base_addr_mmio;
    uint8_t l_pec_id = 0;
    uint8_t l_phb_id = 0;

    auto l_pec_chiplets_vec = i_target.getChildren<fapi2::TARGET_TYPE_PEC>(fapi2::TARGET_STATE_FUNCTIONAL);
    auto l_phb_chiplets_vec = i_target.getChildren<fapi2::TARGET_TYPE_PHB>(fapi2::TARGET_STATE_FUNCTIONAL);
    FAPI_DBG("pec target vec size: %#x\n", l_pec_chiplets_vec.size());
    FAPI_DBG("phb target vec size: %#x\n", l_phb_chiplets_vec.size());

    // read system level BAR MMIO offset/size attributes
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_PCIE_MMIO_BAR0_BASE_ADDR_OFFSET, FAPI_SYSTEM, l_mmio_bar0_offsets),
             "Error from FAPI_ATTR_GET (ATTR_PROC_PCIE_MMIO_BAR0_BASE_ADDR_OFFSET)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_PCIE_MMIO_BAR1_BASE_ADDR_OFFSET, FAPI_SYSTEM, l_mmio_bar1_offsets),
             "Error from FAPI_ATTR_GET (ATTR_PROC_PCIE_MMIO_BAR1_BASE_ADDR_OFFSET)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_PCIE_REGISTER_BAR_BASE_ADDR_OFFSET, FAPI_SYSTEM, l_register_bar_offsets),
             "Error from FAPI_ATTR_GET (ATTR_PROC_PCIE_REGISTER_BAR_BASE_ADDR_OFFSET)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_PCIE_BAR_SIZE, FAPI_SYSTEM, l_bar_sizes),
             "Error from FAPI_ATTR_GET (ATTR_PROC_PCIE_BAR_SIZE)");

    // determine base address of chip MMIO range
    FAPI_TRY(p9_fbc_utils_get_chip_base_address(i_target,
             l_base_addr_nm0,
             l_base_addr_nm1,
             l_base_addr_m,
             l_base_addr_mmio),
             "Error from p9_fbc_utils_get_chip_base_address");

    for (auto l_pec_chiplets : l_pec_chiplets_vec)
    {
        // Get the pec id
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CHIP_UNIT_POS, l_pec_chiplets,
                               l_pec_id));

        // Phase2 init step 1
        // NestBase+0x00
        // Set bits 00:03 = 0b0001 Set hang poll scale
        // Set bits 04:07 = 0b0010 Set data scale
        // Set bits 08:11 = 0b0001 Set hang pe scale
        // Set bit 22 = 0b1 Disable out-of-order store behavior
        // Set bit 33 = 0b1 Enable Channel Tag streaming behavior
        // Set bits 34:35 = 0b11 Set P9 Style cache-inject behavior
        FAPI_TRY(fapi2::getScom(l_pec_chiplets, PEC_PBCQHWCFG_REG, l_buf));
        l_buf.insertFromRight<PEC_PBCQHWCFG_REG_HANG_POLL_SCALE, PEC_PBCQHWCFG_REG_HANG_POLL_SCALE_LEN>(0x1);
        l_buf.insertFromRight<PEC_PBCQHWCFG_REG_HANG_DATA_SCALE, PEC_PBCQHWCFG_REG_HANG_DATA_SCALE_LEN>(0x2);
        l_buf.insertFromRight<PEC_PBCQHWCFG_REG_HANG_PE_SCALE, PEC_PBCQHWCFG_REG_HANG_PE_SCALE_LEN>(0x1);
        l_buf.insertFromRight<PEC_PBCQHWCFG_REG_PE_DISABLE_OOO_MODE, 1>(0x1);
        l_buf.setBit<PEC_PBCQHWCFG_REG_PE_CHANNEL_STREAMING_EN>();
        l_buf.insertFromRight<PEC_PBCQHWCFG_REG_PE_WR_CACHE_INJECT_MODE, PEC_PBCQHWCFG_REG_PE_WR_CACHE_INJECT_MODE_LEN>(0x3);
        FAPI_DBG("pec%i: %#lx", l_pec_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_pec_chiplets, PEC_PBCQHWCFG_REG, l_buf));

        // Phase2 init step 2
        // NestBase+0x01
        // N/A Modify Drop Priority Control Register (DrPriCtl)

        // Phase2 init step 3
        // NestBase+0x03
        // Set bits 00:03 = 0b1001 Enable trace, and select
        // Inbound operations with addr information
        FAPI_TRY(fapi2::getScom(l_pec_chiplets, PEC_NESTTRC_REG, l_buf));
        // TODO: no register bit field defined.
        l_buf.insertFromRight<0, 4>(0x9);
        FAPI_DBG("pec%i: %#lx", l_pec_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_pec_chiplets, PEC_NESTTRC_REG, l_buf));

        // Phase2 init step 4
        // NestBase+0x05
        // N/A For use of atomics/asb_notify

        // Phase2 init step 5
        // NestBase+0x06
        // N/A To override scope prediction

        // Phase2 init step 6
        // PCIBase +0x00
        // Set bits 30 = 0b1 Enable Trace
        l_buf = 0;
        l_buf.setBit<PEC_PBAIBHWCFG_REG_PE_PCIE_CLK_TRACE_EN>();
        FAPI_DBG("pec%i: %#lx", l_pec_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_pec_chiplets, PEC_PBAIBHWCFG_REG, l_buf));
    }

    for (auto l_phb_chiplets : l_phb_chiplets_vec)
    {
        fapi2::ATTR_PROC_PCIE_BAR_ENABLE_Type l_bar_enables;
        fapi2::buffer<uint64_t> l_mmio0_bar = l_base_addr_mmio;
        fapi2::buffer<uint64_t> l_mmio1_bar = l_base_addr_mmio;
        fapi2::buffer<uint64_t> l_register_bar = l_base_addr_mmio;

        // Get the phb id
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CHIP_UNIT_POS, l_phb_chiplets,
                               l_phb_id));

        // Phase2 init step 12_a
        // NestBase+StackBase+0xA
        // 0x00000000_00000000
        // Clear any spurious cerr_rpt0 bits (cerr_rpt0)
        // TODO: HW363246 this step appears in the v1.0 doc but failed PCB address
        // error in model e9034
        //l_buf = (uint64_t)0x0;
        //FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        //FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_CERR_RPT0_REG, l_buf));

        // Phase2 init step 12_b
        // NestBase+StackBase+0xB
        // 0x00000000_00000000
        // Clear any spurious cerr_rpt1 bits (cerr_rpt1)
        // TODO: HW363246 this step appears in the v1.0 doc but failed PCB address
        // error in model e9034
        //l_buf = (uint64_t)0x0;
        //FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        //FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_CERR_RPT1_REG, l_buf));

        // Phase2 init step 7_c
        // NestBase+StackBase+0x0
        // 0x00000000_00000000
        // Clear any spurious FIR
        // bits (NFIR)NFIR
        l_buf = (uint64_t)0x0;
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_NFIR_REG, l_buf));

        // Phase2 init step 8
        // NestBase+StackBase+0x8
        // 0x00000000_00000000
        // Clear any spurious WOF
        // bits (NFIRWOF)
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_NFIRWOF_REG, l_buf));

        // Phase2 init step 9
        // NestBase+StackBase+0x6
        // Set the per FIR Bit Action 0 register
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_NFIRACTION0_REG, PCI_NFIR_ACTION0_REG));

        // Phase2 init step 10
        // NestBase+StackBase+0x7
        // Set the per FIR Bit Action 1 register
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_NFIRACTION1_REG, PCI_NFIR_ACTION1_REG));

        // Phase2 init step 11
        // NestBase+StackBase+0x3
        // Set FIR Mask Bits to allow errors (NFIRMask)
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_NFIRMASK_REG, PCI_NFIR_MASK_REG));

        // Phase2 init step 12
        // NestBase+StackBase+0x15
        // 0x00000000_00000000
        // Set Data Freeze Type Register for SUE handling (DFREEZE)
        l_buf = 0x0000000000000000;
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PE_DFREEZE_REG, l_buf));

        // Phase2 init step 13_a
        // PCIBase+StackBase+0xB
        // 0x00000000_00000000
        // Clear any spurious pbaib_cerr_rpt bits
        l_buf = (uint64_t)0x0;
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PBAIB_CERR_RPT_REG, l_buf));

        // Phase2 init step 13_b
        // PCIBase+StackBase+0x0
        // 0x00000000_00000000
        // Clear any spurious FIR
        // bits (PFIR)PFIR
        l_buf = (uint64_t)0x0;
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PFIR_REG, l_buf));

        // Phase2 init step 14
        // PCIBase+StackBase+0x8
        // 0x00000000_00000000
        // Clear any spurious WOF
        // bits (PFIRWOF)
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PFIRWOF_REG, l_buf));

        // Phase2 init step 15
        // PCIBase+StackBase+0x6
        // Set the per FIR Bit Action 0 register
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PFIRACTION0_REG, PCI_PFIR_ACTION0_REG));

        // Phase2 init step 16
        // PCIBase+StackBase+0x7
        // Set the per FIR Bit Action 1 register
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PFIRACTION1_REG, PCI_PFIR_ACTION1_REG));

        // Phase2 init step 17
        // PCIBase+StackBase+0x3
        // Set FIR Mask Bits to allow errors (PFIRMask)
        FAPI_DBG("phb%i: %#lx", l_phb_id,  l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PFIRMASK_REG, PCI_PFIR_MASK_REG));

        // Get the BAR enable attribute
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_PCIE_BAR_ENABLE, l_phb_chiplets, l_bar_enables),
                 "Error from FAPI_ATTR_GET (ATTR_PROC_PCIE_BAR_ENABLE)");

        // step 18: NestBase+StackBase+0xE<software programmed>Set MMIO Base
        l_mmio0_bar += l_mmio_bar0_offsets[l_phb_id];
        FAPI_DBG("phb%i bar0 addr: %#lx", l_phb_id, l_mmio0_bar());
        l_mmio0_bar = l_mmio0_bar << P9_PCIE_CONFIG_BAR_SHIFT;
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_MMIOBAR0_REG, l_mmio0_bar),
                 "Error from putScom (PHB_MMIOBAR0_REG)");

        // step 19: NestBase+StackBase+0xF<software programmed>Set MMIO BASE
        // Address Register Mask 0 (MMIOBAR0_MASK)
        FAPI_DBG("phb%i bar0 size: %#lx", l_phb_id, l_bar_sizes[0]);
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_MMIOBAR0_MASK_REG, l_bar_sizes[0]));

        // step 20: NestBase+StackBase+0x10<software programmed>Set MMIO Base
        // Address Register 1 (MMIOBAR1)
        l_mmio1_bar += l_mmio_bar1_offsets[l_phb_id];
        FAPI_DBG("phb%i bar1 addr: %#lx", l_phb_id, l_mmio1_bar());
        l_mmio1_bar = l_mmio1_bar << P9_PCIE_CONFIG_BAR_SHIFT;
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_MMIOBAR1_REG, l_mmio1_bar),
                 "Error from putScom (PHB_MMIOBAR1_REG)");

        // step 21: NestBase+StackBase+0x11<software programmed>Set MMIO Base
        // Address Register Mask 1 (MMIOBAR1_MASK)
        FAPI_DBG("phb%i bar1 size: %#lx", l_phb_id, l_bar_sizes[1]);
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_MMIOBAR1_MASK_REG, l_bar_sizes[1]));

        // step 22: NestBase+StackBase+0x12<software programmed>Set PHB
        // Regsiter Base address Register (PHBBAR)
        l_register_bar += l_register_bar_offsets[l_phb_id];
        FAPI_DBG("phb%i bar1 addr: %#lx", l_phb_id, l_register_bar());
        l_register_bar = l_register_bar << P9_PCIE_CONFIG_BAR_SHIFT;
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PHBBAR_REG, l_register_bar),
                 "Error from putScom (PHB_PHBBAR_REG)");

        // step 23: NestBase+StackBase+0x14<software programmed>Set Base
        // addressress Enable Register (BARE)
        l_buf = (uint64_t)0x0;
        l_buf.insertFromRight<PHB_BARE_REG_PE_MMIO_BAR0_EN, 1>(l_bar_enables[0]); // bit 0 for BAR0
        l_buf.insertFromRight<PHB_BARE_REG_PE_MMIO_BAR1_EN, 1>(l_bar_enables[1]); // bit 1 for BAR1
        l_buf.insertFromRight<PHB_BARE_REG_PE_PHB_BAR_EN, 1>(l_bar_enables[2]);   // bit 2 for PHB
        FAPI_DBG("phb%i bar enable: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_BARE_REG, l_buf));

        // Phase2 init step 24
        // PCIBase+StackBase +0x0A
        // 0x00000000_00000000
        // Remove ETU/AIB bus from reset (PHBReset)
        l_buf = (uint64_t)0x0;
        FAPI_DBG("phb%i: %#lx", l_phb_id, l_buf());
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_PHBRESET_REG, l_buf));

        // Configure ETU FIR (all masked)
        l_buf = (uint64_t)0x0;
        FAPI_DBG("phb%i ETU FIR setup", l_phb_id);
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_ACT0_REG, l_buf));
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_ACTION1_REG, l_buf));
        l_buf = (uint64_t)0xFFFFFFFFFFFFFFFFULL;
        FAPI_TRY(fapi2::putScom(l_phb_chiplets, PHB_MASK_REG, l_buf));
    }

    FAPI_INF("End");

fapi_try_exit:
    return fapi2::current_err;
}
