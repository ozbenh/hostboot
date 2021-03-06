/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/bootloader/bootloader.C $                                 */
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

#define __BOOT_LOADER_C

#include <stdint.h>
#include <bootloader/bootloader.H>
#include <bootloader/bootloader_trace.H>
#include <bootloader/hbblreasoncodes.H>
#include <bootloader/bl_pnorAccess.H>

#include <lpc_const.H>
#include <pnor_utils.H>

#include <ecc.H>

#include <stdlib.h>
#include <util/align.H>
#include <string.h>
#include <limits.h>

#include <securerom/ROM.H>
#include <config.h>
#include <secureboot/secure_reasoncodes.H>

extern uint64_t kernel_other_thread_spinlock;
extern PNOR::SectionData_t bootloader_hbbSection;
extern char bootloader_end_address;

namespace Bootloader{
    /**
     * @brief Pointer to bootloader scratch space
     *
     * Pointer to location in main storage which bootloader uses as
     * scratch space
     */
    uint8_t *g_blScratchSpace = NULL;

    /**
     * @brief  Retrieve the internal hardware hash key from secure ROM object.
     * @param[out] o_hash  Reference to the sha2_hash_t array to copy the
     *                     hash to.
     */
    void setHwKeyHash(sha2_hash_t o_hash)
    {
        memcpy(o_hash, reinterpret_cast<void *>(HW_KEYS_HASH_ADDR),
               sizeof(sha2_hash_t));
    }

    // @TODO RTC:167740 remove magic number check once fsp/op signs HBB
    /**
     * @brief  Memcmp a vaddr to the known secureboot magic number
     *
     * @param[in]   i_vaddr: vaddr of secureboot header to check for magic number
     *                       Note: must point to a buffer of size >= 4 bytes
     *
     * @return  bool - True if the magic number and starting bytes of the vaddr
     *                 match. False otherwise.
     */
    bool cmpSecurebootMagicNumber(const uint8_t* i_vaddr)
    {
        return memcmp(&ROM_MAGIC_NUMBER, i_vaddr, sizeof(ROM_MAGIC_NUMBER))==0;
    }

    /**
     * @brief Verify Container against system hash keys
     *
     * @param[in] i_pContainer  Void pointer to effective address
     *                          of container
     * NOTE : no-op if Config Secureboot not enabled.
     *
     * @return N/A
     */
    void verifyContainer(const void * i_pContainer,
                         const sha2_hash_t* i_hwKeyHash)
    {
#ifdef CONFIG_SECUREBOOT
        // @TODO RTC:167740 remove magic number check once fsp/op signs HBB
        if (cmpSecurebootMagicNumber(reinterpret_cast<const uint8_t*>
                                     (i_pContainer)))
        {
            BOOTLOADER_TRACE(BTLDR_TRC_MAIN_VERIFY_HBB_START);

            uint64_t l_rc = 0;

            const void * l_pBootloaderEnd = &bootloader_end_address;

            // Get starting address of ROM code which is the next 8 byte aligned
            // address after the bootloader end.
            uint64_t l_size = 0;
            memcpy (&l_size, l_pBootloaderEnd, sizeof(l_size));
            uint64_t l_rom_startAddr = getHRMOR() + ALIGN_8(l_size);

            // Set startAddr to ROM_verify() function at an offset of Secure ROM
            uint64_t l_rom_verify_startAddr = l_rom_startAddr
                                              + ROM_VERIFY_FUNCTION_OFFSET;

            // Declare local input struct
            ROM_hw_params l_hw_parms;

            // Clear/zero-out the struct since we want 0 ('zero') values for
            // struct elements my_ecid, entry_point and log
            memset(&l_hw_parms, 0, sizeof(ROM_hw_params));


            // Use current hw hash key
            memcpy (&l_hw_parms.hw_key_hash, i_hwKeyHash, sizeof(sha2_hash_t));

            const ROM_container_raw* l_container =
                           reinterpret_cast<const ROM_container_raw*>(i_pContainer);

            l_rc = call_rom_verify(reinterpret_cast<void*>
                                   (l_rom_verify_startAddr),
                                   l_container,
                                   &l_hw_parms);

            if (l_rc != 0)
            {
                // Verification of Container failed.
                BOOTLOADER_TRACE(BTLDR_TRC_MAIN_VERIFY_HBB_FAIL);
                /*@
                 * @errortype
                 * @moduleid     MOD_BOOTLOADER_VERIFY
                 * @reasoncode   SECUREBOOT::RC_ROM_VERIFY
                 * @userdata1    ROM return code
                 * @userdata2    ROM_hw_params log
                 * @devdesc      ROM verification failed
                 * @custdesc     Platform security violation detected
                 */
                bl_terminate(MOD_BOOTLOADER_VERIFY,
                             SECUREBOOT::RC_ROM_VERIFY,
                             l_rc,
                             l_hw_parms.log);

            }

            BOOTLOADER_TRACE(BTLDR_TRC_MAIN_VERIFY_HBB_SUCCESS);
        }
        else
        {
            BOOTLOADER_TRACE(BTLDR_TRC_MAIN_VERIFY_HBB_SKIP);
        }
#endif
    }


    /** Bootloader main function to work with and start HBB.
     *
     * @return 0.
     *
     * @note Should branch to HBB and never return.
     */
    extern "C"
    int main()
    {
        // Initialization
        bootloader_trace_index = 0;
        BOOTLOADER_TRACE(BTLDR_TRC_MAIN_START);

        // Set variables needed for getting location of HB base code
        // @TODO RTC:138268 Support multiple sides of PNOR in bootloader

        //pnorEnd is the end of flash, which is base of lpc, plus
        //the offset of the FW space, plus the TOP memory address in FW space
        uint64_t l_pnorEnd = LPC::LPC_PHYS_BASE + LPC::LPCHC_FW_SPACE
        + PNOR::LPC_TOP_OF_FLASH_OFFSET;

        //We dont know what the start of pnor is because we dont know the size
        uint64_t l_pnorStart = 0;

        uint32_t l_errCode = PNOR::NO_ERROR;
        uint8_t l_tocUsed = 0;
        g_blScratchSpace = reinterpret_cast<uint8_t*>(HBBL_SCRATCH_SPACE_ADDR);

        // Get location of HB base code in PNOR from TOC
        // @TODO RTC:138268 Support multiple sides of PNOR in bootloader
        bl_pnorAccess::getHBBSection(l_pnorEnd,
                                     bootloader_hbbSection,
                                     l_errCode,
                                     l_tocUsed,
                                     l_pnorStart);
        BOOTLOADER_TRACE(BTLDR_TRC_MAIN_GETHBBSECTION_RTN );

        if(PNOR::NO_ERROR == l_errCode)
        {
            // get hbbFlashOffset
            uint64_t l_hbbFlashOffset = bootloader_hbbSection.flashAddr;
            // get hbbLength without size of ECC data
            uint32_t l_hbbLength = bootloader_hbbSection.size;
            // get hbbEcc
            bool l_hbbEcc =
                ( bootloader_hbbSection.integrity == FFS_INTEG_ECC_PROTECT);

            // Copy HB base code from PNOR to working location
            handleMMIO(l_pnorStart + l_hbbFlashOffset,
                       (l_hbbEcc) ? HBB_ECC_WORKING_ADDR : HBB_WORKING_ADDR,
                       (l_hbbEcc) ? (l_hbbLength * LENGTH_W_ECC)/LENGTH_WO_ECC
                                  : l_hbbLength,
                       WORDSIZE);
            BOOTLOADER_TRACE(BTLDR_TRC_MAIN_WORKING_HANDLEMMIO_RTN);

            PNOR::ECC::eccStatus rc = PNOR::ECC::CLEAN;
            if(l_hbbEcc)
            {
                // Remove ECC from HB base code at in working location and
                // store result in new working location
                rc = PNOR::ECC::removeECC(
                    reinterpret_cast<uint8_t*>(HBB_ECC_WORKING_ADDR |
                                               IGNORE_HRMOR_MASK),
                    reinterpret_cast<uint8_t*>(HBB_WORKING_ADDR |
                                               IGNORE_HRMOR_MASK),
                    l_hbbLength);
                if (rc != PNOR::ECC::CLEAN)
                {
                    BOOTLOADER_TRACE(BTLDR_TRC_MAIN_REMOVEECC_RTN);
                }
                else if (rc != PNOR::ECC::CORRECTED)
                {
                    BOOTLOADER_TRACE(BTLDR_TRC_MAIN_REMOVEECC_CORRECTED);
                }
            }

            if (rc != PNOR::ECC::UNCORRECTABLE)
            {
                uint64_t *l_src_addr =
                   reinterpret_cast<uint64_t*>(HBB_WORKING_ADDR |
                                               IGNORE_HRMOR_MASK);

                uint64_t *l_dest_addr =
                   reinterpret_cast<uint64_t*>(HBB_RUNNING_ADDR |
                                               IGNORE_HRMOR_MASK);

                // Get HW keys hash
                sha2_hash_t l_hwKeyHash{0};
                setHwKeyHash(l_hwKeyHash);

                // ROM verification of HBB image
                verifyContainer(l_src_addr, &l_hwKeyHash);

                // Increment past secure header
#ifdef CONFIG_SECUREBOOT
                l_src_addr += PAGE_SIZE/sizeof(uint64_t);
                l_hbbLength -= PAGE_SIZE;
#endif

                // Copy HBB image into address where it executes
                for(uint32_t i = 0;
                    i < l_hbbLength / sizeof(uint64_t);
                    i++)
                {
                    l_dest_addr[i] = l_src_addr[i];
                }
                BOOTLOADER_TRACE(BTLDR_TRC_MAIN_COPY_HBB_DONE);

                // Start executing HBB
                enterHBB(HBB_HRMOR, HBB_RUNNING_OFFSET);
            }
            else
            {
                BOOTLOADER_TRACE_W_BRK(BTLDR_TRC_MAIN_REMOVEECC_FAIL);
                /*@
                 * @errortype
                 * @moduleid     MOD_BOOTLOADER_MAIN
                 * @reasoncode   RC_REMOVE_ECC_FAIL
                 * @userdata1[0:15]   TI_WITH_SRC
                 * @userdata1[16:31]  TI_BOOTLOADER
                 * @userdata1[32:63]  Failing address = 0
                 * @userdata2[0:31]   Word7 = 0
                 * @userdata2[32:63]  Word8 = 0
                 * @devdesc      Uncorrectable ECC error found in HBB
                 * @custdesc     A problem occurred while running processor
                 *               boot code.
                 */
                bl_terminate(MOD_BOOTLOADER_MAIN,
                             RC_REMOVE_ECC_FAIL);
            }
        }
        else
        {
            // Note getHBBSection should have TI'd so won't get here
            BOOTLOADER_TRACE_W_BRK(BTLDR_TRC_MAIN_GETHBBSECTION_FAIL);
        }

        return 0;
    }


    /** Handle MMIO to copy code/data from location to another.
     *
     * @param[in] i_srcAddr - The source location.
     * @param[in] i_destAddr - The destination location.
     * @param[in] i_size - The size of the code/data.
     * @param[in] i_ld_st_size - The size of each load/store operation.
     *
     * @return void.
     */
    void handleMMIO(uint64_t i_srcAddr,
                    uint64_t i_destAddr,
                    uint32_t i_size,
                    MMIOLoadStoreSizes i_ld_st_size)
    {
        BOOTLOADER_TRACE(BTLDR_TRC_HANDLEMMIO_START + i_ld_st_size);

        // Set base addresses, Ignore HRMOR setting
        uint64_t l_srcAddr_base = i_srcAddr | IGNORE_HRMOR_MASK;
        uint64_t l_destAddr_base = i_destAddr | IGNORE_HRMOR_MASK;

        uint32_t l_targetGPR = 0;

        for(uint32_t i = 0;
            i < i_size;
            i += i_ld_st_size)
        {
            // Set addresses
            uint64_t l_srcAddr = l_srcAddr_base + i;
            uint64_t l_destAddr = l_destAddr_base + i;

            if(i_ld_st_size == BYTESIZE)
            {
                // Cache-inhibited load byte from hypervisor state.
                // lbzcix      BOP1,Ref_G0,BOP2
                asm volatile("lbzcix %0, 0, %1"
                             : "=r" (l_targetGPR)    // output, %0
                             : "r" (l_srcAddr)       // input,  %1
                             : );                    // no impacts

                // Store byte.
                // stbx      BOP1,Ref_G0,BOP2
                asm volatile("stbx %0,0,%1"
                             :: "r" (l_targetGPR) , "r" (l_destAddr));
            }
            else if(i_ld_st_size == WORDSIZE)
            {
                // Cache-inhibited load word from hypervisor state.
                // lwzcix      BOP1,Ref_G0,BOP2
                asm volatile("lwzcix %0, 0, %1"
                             : "=r" (l_targetGPR)    // output, %0
                             : "r" (l_srcAddr)       // input,  %1
                             : );                    // no impacts

                // store word.
                // stwx      BOP1,Ref_G0,BOP2
                asm volatile("stwx %0,0,%1"
                             :: "r" (l_targetGPR) , "r" (l_destAddr));
            }
            else
            {
                // Cache-inhibited load double word from hypervisor state.
                // ldcix       BOP1,Ref_G0,BOP2
                asm volatile("ldcix %0, 0, %1"
                             : "=r" (l_targetGPR)    // output, %0
                             : "r" (l_srcAddr)       // input,  %1
                             : );                    // no impacts

                // Store double word.
                // stdx      BOP1,Ref_G0,BOP2
                asm volatile("stdx %0,0,%1"
                             :: "r" (l_targetGPR) , "r" (l_destAddr));
            }
        }
    }
} // end namespace Bootloader
