/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/sbeio/sbe_psudd.H $                           */
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
#ifndef __SBE_PSUDD_H
#define __SBE_PSUDD_H

/**
 * @file sbe_psudd.H
 * @brief SBE psu device driver specifics
 */

#include <stdint.h>
#include <builtins.h>
#include <sys/time.h>

#include <errl/errlentry.H>
#include <util/singleton.H>

namespace SBEIO
{
//-----------------------------------------------------------------------------
//  Interfaces to the SBE PSU device driver to be used by clients
//  within this component.
//-----------------------------------------------------------------------------

/** @class SbePsu
 *  @brief Class for SBE/PSU communication
*/

class SbePsu
{
    public:

        /**
         * @brief get the instance of this class
         *
         * @return the (one and only) instance of SbePsu
         */

        static SbePsu & getTheInstance();

        /**
         * @brief enums for SBE command class
         */

        // BYTE 6 options
        enum psuCommandClass
        {
            SBE_PSU_CLASS_UNKNOWN      = 0,
            SBE_PSU_CLASS_CORE_STATE   = 0xD1,
            SBE_PSU_PUT_RING_FROM_IMAGE_CMD = 0xD3,
            SBE_PSU_GENERIC_MESSAGE         = 0xD7
        };

        /**
         * @brief enums for SBE core state control commands
         */

        // BYTE 7 options
        enum psuCoreStateControlCommands
        {
            SBE_CMD_CONTROL_DEADMAN_LOOP   = 0x01,

        };

        /**
         * @brief enums for SBEPutRing From Image message commands
         */
          // BYTE 7 options
        enum putRingCommandFromImageCommands
        {
            SBE_CMD_CONTROL_PUTRING        = 0x01
        };

        /**
        * @brief enums for SBE generic message commands
        */
        //BYTE 7 options
        enum psuGenericMessageCommands
        {
            SBE_PSU_GENERIC_MSG_QUIESCE      = 0x05,
            SBE_CMD_CONTROL_SYSTEM_CONFIG    = 0x06
        };

        /**
         * @brief enums for SBE core state control flags
         */
        //BYTE 2  & 3, Control Flags
        enum psuDeadManControlFlags
        {
            SBE_DMCONTROL_START              = 0x01,
            SBE_DMCONTROL_STOP               = 0x02,
            SBE_DMCONTROL_RESPONSE_REQUIRED  = 0x0100,
            SBE_DMCONTROL_ACK_REQUIRED       = 0x0200,
        };

        /**
        * @brief enums for SBE system config control flags
        */
        //BYTE 2  & 3, Control Flags
        enum psuSystemConfigControlFlags
        {
            SBE_REQUIRE_RESPONSE             = 0x0100,
            SBE_REQUIRE_ACK                  = 0x0200,
        };

        /**
         * @brief non reserved word enums
         *
         * Shows which of the request and response msg registers are
         * not reserved. Reserved registers do not need to be written
         * or read.
         *
         * This is a 4 bit field:
         * 0x1 - Reg 0 is non-reserved (read or write this reg)
         * 0x2 - Reg 1 is non-reserved (read or write this reg)
         * 0x4 - Reg 2 is non-reserved (read or write this reg)
         * 0x8 - Reg 3 is non-reserved (read or write this reg)
         */
        enum psuCoreStateControlNonReservedMsgs
        {
            SBE_DMCONTROL_START_REQ_USED_REGS     = 0x03,
            SBE_DMCONTROL_START_RSP_USED_REGS     = 0x01,
            SBE_DMCONTROL_STOP_REQ_USED_REGS      = 0x01,
            SBE_DMCONTROL_STOP_RSP_USED_REGS      = 0x01,
        };

        /**
        * @brief non reserved word enums
        *
        * Shows which of the request and response msg registers are
        * not reserved. Reserved registers do not need to be written
        * or read.
        *
        * This is a 4 bit field:
        * 0x1 - Reg 0 is non-reserved (read or write this reg)
        * 0x2 - Reg 1 is non-reserved (read or write this reg)
        * 0x4 - Reg 2 is non-reserved (read or write this reg)
        * 0x8 - Reg 3 is non-reserved (read or write this reg)
        */
        enum psuSystemConfigNonReservedMsgs
        {
            SBE_SYSTEM_CONFIG_REQ_USED_REGS     = 0x03,
            SBE_SYSTEM_CONFIG_RSP_USED_REGS     = 0x01,
        };

        /**
        * @brief non reserved word enums
        *
        * Shows which of the request and response msg registers are
        * not reserved. Reserved registers do not need to be written
        * or read.
        *
        * This is a 4 bit field:
        * 0x1 - Reg 0 is non-reserved (read or write this reg)
        * 0x2 - Reg 1 is non-reserved (read or write this reg)
        * 0x4 - Reg 2 is non-reserved (read or write this reg)
        * 0x8 - Reg 3 is non-reserved (read or write this reg)
        */
        enum psuQuiesceNonReservedMsgs
        {
            SBE_QUIESCE_REQ_USED_REGS     = 0x01,
            SBE_QUIESCE_RSP_USED_REGS     = 0x01,
        };

        /**
         * @brief Struct for PSU command message format
         *
         */
        union psuCommand
        {
            struct //raw
            {
                uint64_t mbxReg0;
                uint64_t mbxReg1;
                uint64_t mbxReg2;
                uint64_t mbxReg3;
            } PACKED;
            struct //common and direct fields
            {
                // mbxReg 0
                uint16_t reserved;
                uint16_t controlFlags;
                uint16_t seqID;
                uint8_t  commandClass;
                uint8_t  command;
                // mbxReg 1
                uint32_t dataWord2;
                uint32_t dataWord3;
                // mbxReg 2
                uint32_t dataWord4;
                uint32_t dataWord5;
                // mbxReg 3
                uint32_t dataWord6;
                uint32_t dataWord7;
            } PACKED;
            struct //indirect
            {
                // mbxReg 0
                uint16_t indirect_reserved;
                uint16_t indirect_controlFlags;
                uint16_t indirect_seqID;
                uint8_t  indirect_commandClass;
                uint8_t  indirect_command;
                // mbxReg 1
                uint32_t indirect_dataWord2;
                uint32_t indirect_dataWord3;
                // mbxReg 2
                uint32_t indirect_dataWord4;
                uint32_t indirect_dataWord5;
                // mbxReg 3
                uint64_t indirect_address;  // Data address (Mainstore/PBA)
            } PACKED;
            struct //controlDeadmanLoop
            {
                uint16_t cd1_ControlDeadmanLoop_Reserved;
                uint16_t cd1_ControlDeadmanLoop_ControlFlags;
                uint16_t cd1_ControlDeadmanLoop_SeqID;
                uint8_t  cd1_ControlDeadmanLoop_CommandClass;
                uint8_t  cd1_ControlDeadmanLoop_Command;
                uint64_t cd1_ControlDeadmanLoop_WaitTime ;
                uint64_t cd1_ControlDeadmanLoop_MbxReg2reserved;
                uint64_t cd1_ControlDeadmanLoop_MbxReg3reserved;
            } PACKED;

            struct //setSystemConfig
            {
                uint16_t cd2_SetSystemConfig_Reserved;
                uint16_t cd2_SetSystemConfig_ControlFlags;
                uint16_t cd2_SetSystemConfig_SeqID;
                uint8_t  cd2_SetSystemConfig_CommandClass;
                uint8_t  cd2_SetSystemConfig_Command;
                uint64_t cd2_SetSystemConfig_SystemFabricIdMap ;
                uint64_t cd2_SetSystemConfig_MbxReg2reserved;
                uint64_t cd2_SetSystemConfig_MbxReg3reserved;
            } PACKED;

            struct //for 'Put Ring from Image' message
            {
                uint16_t cd3_PutRing_Reserved;      //Mbx Reg 0
                uint16_t cd3_PutRing_ControlFlags;  //Mbx Reg 0
                uint16_t cd3_PutRing_SeqID;         //Mbx Reg 0
                uint8_t  cd3_PutRing_CommandClass;    //Mbx Reg 0
                uint8_t  cd3_PutRing_Command;         //Mbx Reg 0
                uint16_t cd3_PutRing_TargetType;  //Mbx Reg 1
                uint8_t  cd3_PutRing_Reserved1;   //Mbx Reg 1
                uint8_t  cd3_PutRing_ChipletID;   //Mbx Reg 1
                uint16_t cd3_PutRing_RingID;      //Mbx Reg 1
                uint16_t cd3_PutRing_RingMode;    //Mbx Reg 1
                uint64_t cd3_PutRing_ReservedMbxReg2; //Mbx Reg 2
                uint64_t cd3_PutRing_ReservedMbxReg3; //Mbx Reg 3
            } PACKED;

            struct //psuQuiesce
            {
                uint16_t cd4_PsuQuiesce_Reserved;
                uint16_t cd4_PsuQuiesce_ControlFlags;
                uint16_t cd4_PsuQuiesce_SeqID;
                uint8_t  cd4_PsuQuiesce_CommandClass;
                uint8_t  cd4_PsuQuiesce_Command;
                uint64_t cd4_PsuQuiesce_MbxReg1reserved;
                uint64_t cd4_PsuQuiesce_MbxReg2reserved;
                uint64_t cd4_PsuQuiesce_MbxReg3reserved;
            } PACKED;


            psuCommand(uint16_t i_controlFlags, //Mbx Reg 0 input
                       uint8_t  i_commandClass, //Mbx Reg 0 input
                       uint8_t  i_command) :    //Mbx Reg 0 input

                       //setting BYTE 0 & 1     Mbx Reg 0
                       reserved (0),
                       //setting BYTE 2 & 3     Mbx Reg 0
                       controlFlags(i_controlFlags),
                       //setting BYTE 4 & 5     Mbx Reg 0
                       seqID(0),
                       //setting BYTE 6         Mbx Reg 0
                       commandClass(i_commandClass),
                       //setting BYTE 7         Mbx Reg 0
                       command(i_command),
                       //setting BYTE 8- Byte 11 - Mbx Reg 1
                       dataWord2(0),
                       //setting BYTE 12- Byte 15 - Mbx Reg 1
                       dataWord3(0),
                       //setting BYTE 16- Byte 19 - Mbx Reg 2
                       dataWord4(0),
                       //setting BYTE 20- Byte 23 - Mbx Reg 2
                       dataWord5(0),
                       //setting BYTE 24- Byte 27 - Mbx Reg 3
                       dataWord6(0),
                       //setting BYTE 28- Byte 31 - Mbx Reg 3
                       dataWord7(0)
            {
            }

        };

        /**
         * @brief Struct for PSU response message format
         *
         */
        union psuResponse
        {
            struct //raw
            {
                uint64_t mbxReg4;
                uint64_t mbxReg5;
                uint64_t mbxReg6;
                uint64_t mbxReg7;
            } PACKED;
            struct //common and direct fields
            {
                // mbxReg 4
                uint16_t primaryStatus;
                uint16_t secondaryStatus;
                uint16_t seqID;
                uint8_t  commandClass;
                uint8_t  command;
                // mbxReg 5
                uint32_t respWord0;
                uint32_t respWord1;
                // mbxReg 6
                uint32_t respWord2;
                uint32_t respWord3;
                // mbxReg 7
                uint32_t respWord4;
                uint32_t respWord5;
            } PACKED;
            struct // indirect fields
            {
                // mbxReg 4
                uint16_t indirect_primaryStatus;
                uint16_t indirect_secondaryStatus;
                uint16_t indirect_seqID;
                uint8_t  indirect_commandClass;
                uint8_t  indirect_command;
                // mbxReg 5
                uint32_t indirect_respWord0;
                uint32_t indirect_respWord1;
                // mbxReg 6
                uint32_t indirect_respWord2;
                uint32_t indirect_respWord3;
                // mbxReg 7
                uint32_t indirect_reserved;
                uint32_t indirect_size;  //Size in dbl words for Indirect data
            } PACKED;
            psuResponse() :
                primaryStatus (0xffff),   //invalid status
                secondaryStatus (0xffff), //invalid status
                seqID (0xffff),           //unlikely seq ID
                commandClass (0xff),      //invalid command class
                command (0xff),           //invalid command
                respWord0 (0),
                respWord1 (0),
                respWord2 (0),
                respWord3 (0),
                respWord4 (0),
                respWord5 (0)
            {
            }

        };

        /**
         * @brief timeout values
         * Making this value big enough to handle being queued up
         *  behind a slow FSP operation (e.g. getmempba)
         */
        static const uint64_t MAX_PSU_SHORT_TIMEOUT_NS=90*NS_PER_SEC; //=90sec

        /**
         * @brief enums for primary SBE response
         *
        */
        enum sbePrimResponse
        {
            SBE_PRI_OPERATION_SUCCESSFUL        = 0x00,
            SBE_PRI_INVALID_COMMAND             = 0x01,
            SBE_PRI_INVALID_DATA                = 0x02,
            SBE_PRI_SEQUENCE_ERROR              = 0x03,
            SBE_PRI_INTERNAL_ERROR              = 0x04,
            SBE_PRI_GENERIC_EXECUTION_FAILURE   = 0xFE,
        };

        /**
         * @brief enums for secondary SBE response
         *         Discuss on SBE_SEC_INVALID_TARGET_ID_PASSED
         *
        */
        enum sbeSecondaryResponse
        {
            SBE_SEC_OPERATION_SUCCESSFUL              = 0x00,
            SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED       = 0x01,
            SBE_SEC_COMMAND_NOT_SUPPORTED             = 0x02,
            SBE_SEC_INVALID_ADDRESS_PASSED            = 0x03,
            SBE_SEC_INVALID_TARGET_TYPE_PASSED        = 0x04,
            SBE_SEC_INVALID_TARGET_ID_PASSED          = 0x05,
            SBE_SEC_SPECIFIED_TARGET_NOT_PRESENT      = 0x06,
            SBE_SEC_SPECIFIED_TARGET_NOT_FUNCTIONAL   = 0x07,
            SBE_SEC_COMMAND_NOT_ALLOWED_IN_THIS_STATE = 0x08,
            SBE_SEC_FUNCTIONALITY_NOT_SUPPORTED       = 0x09,
            SBE_SEC_GENERIC_FAILURE_IN_EXECUTION      = 0x0A,
            SBE_SEC_BACKLISTED_ACCESS                 = 0x0B,
            SBE_SEC_OS_FAILURE                        = 0x0C,
            SBE_SEC_HOST_MBX_REG_ACCESS_FAILURE       = 0x0D,
            SBE_SEC_INSUFFICIENT_DATA_PASSED          = 0x0E,
            SBE_SEC_EXCESS_DATA_PASSED                = 0x0F,
            SBE_SEC_SBE_BUSY_TO_HANDLE_COMMAND        = 0x10,
        };

        enum SBE_TARGET_TYPES {

             SBE_TARGET_TYPE_PROC     = 0x00,
             SBE_TARGET_TYPE_EX       = 0x01,
             SBE_TARGET_TYPE_PERV     = 0x02,
             SBE_TARGET_TYPE_MCS      = 0x03,
             SBE_TARGET_TYPE_TOTAL          ,
             SBE_TARGET_TYPE_UNKNOWN  = 0xFF
        };

        /**
         * @brief perform SBE PSU chip-op
         *
         * @param[in]  i_target       Proc target to use for PSU Request
         * @param[in]  i_pPsuRequest  Pointer to PSU request commands
         * @param[out] o_pPsuResponse Pointer to PSU response
         * @param[in]  i_timeout      Time out for response
         * @param[in]  i_reqMsgs      4 bit mask telling which regs to write
         * @param[in]  i_rspMsgs      4 bit mask telling which regs to read
         */
        errlHndl_t performPsuChipOp(TARGETING::Target * i_target,
                                    psuCommand        * i_pPsuRequest,
                                    psuResponse       * o_pPsuResponse,
                                    const uint64_t      i_timeout,
                                    uint8_t             i_reqMsgs,
                                    uint8_t             i_rspMsgs);

    protected:

        /**
         * @Brief Constructor
         */

        SbePsu();

        /**
         * @Brief Destructor
         */

        ~SbePsu();

        /**
         * @brief populate the iv_ffdcPackageBuffer
         * @param[in]  i_data        FFDC error data
         * @param[in]  i_len         data buffer len to copy
         */

        void writeFFDCBuffer(void * i_data, uint8_t i_len);

    private:
    //---------------------------------------------------------------------
    // Local definitions for the device driver
    //---------------------------------------------------------------------

        /**
         * @brief Write FFDC package buffer - holds information exchanged
         * between SBE and HB
         */
        void * iv_ffdcPackageBuffer;

        /**
         * @brief FFDC package needs to be 2 pages
         */
        const uint8_t ffdcPackageSize = 2;

        /**
         * @brief zero out the FFDC package buffer
         */
        void initFFDCPackageBuffer();

        /**
         * @brief Write request to PSU
         *
         * @param[in]  i_target       Master proc to use for scoms
         * @param[in]  i_pPsuRequest  Pointer to PSU request commands
         * @param[in]  i_reqMsgs      4 bit mask telling which regs to write
         *
         * @return errlHndl_t Error log handle on failure.
         */
        errlHndl_t writeRequest(TARGETING::Target  * i_target,
                                psuCommand         * i_pPsuRequest,
                                uint8_t              i_reqMsgs);
        /**
         * @brief Read response from PSU
         *
         * @param[in]  i_target       Master proc to use for scoms
         * @param[in]  i_pPsuRequest  Pointer to PSU request commands
         * @param[out] o_pPsuResponse Pointer to PSU response
         * @param[in]  i_timeout      Time out for response
         * @param[in]  i_rspMsgs      4 bit mask telling which regs to read
         *
         * @return errlHndl_t Error log handle on failure.
         */
        errlHndl_t readResponse(TARGETING::Target  * i_target,
                                psuCommand         * i_pPsuRequest,
                                psuResponse        * o_pPsuResponse,
                                const uint64_t       i_timeout,
                                uint8_t              i_rspMsgs);
        /**
         * @brief Poll for response ready to be read
         *
         * @param[in]  i_target       Master proc to use for scoms
         * @param[in]  i_timeout      Time out for response
         *
         * @return errlHndl_t Error log handle on failure.
         */
        errlHndl_t pollForPsuComplete(TARGETING::Target * i_target,
                                      const uint64_t i_timeout);
        /**
         * @brief Read Scom wrapper
         *
         * @param[in]  i_target       Master proc to use for scoms
         * @param[in]  i_addr         Scom address
         * @param[out] o_pData        Pointer to returned data
         * @param[in]  i_trace        Trace control to avoid overruning
         *                            trace buffer when polling for
         *                            response ready to be read
         *
         * @return errlHndl_t Error log handle on failure.
         */
        errlHndl_t readScom(TARGETING::Target * i_target,
                             uint64_t   i_addr,
                             uint64_t * o_pData,
                             bool       i_trace=true);

        /**
         * @brief Write Scom wrapper
         *
         * @param[in]  i_target       Master proc to use for scoms
         * @param[in]  i_addr         Scom address
         * @param[in]  i_pData        Pointer to data to write
         *
         * @return errlHndl_t Error log handle on failure.
         */
        errlHndl_t writeScom(TARGETING::Target * i_target,
                              uint64_t   i_addr,
                              uint64_t * i_pData);

        /**
         * @brief SBE PSU register addresses
         */
        enum psuRegs
        {
            PSU_HOST_SBE_MBOX0_REG    = 0x000D0050,
            PSU_HOST_SBE_MBOX1_REG    = 0x000D0051,
            PSU_HOST_SBE_MBOX2_REG    = 0x000D0052,
            PSU_HOST_SBE_MBOX3_REG    = 0x000D0053,
            PSU_HOST_SBE_MBOX4_REG    = 0x000D0054,
            PSU_HOST_SBE_MBOX5_REG    = 0x000D0055,
            PSU_HOST_SBE_MBOX6_REG    = 0x000D0056,
            PSU_HOST_SBE_MBOX7_REG    = 0x000D0057,
            PSU_SBE_DOORBELL_REG_RW   = 0x000D0060,
            PSU_SBE_DOORBELL_REG_AND  = 0x000D0061,
            PSU_SBE_DOORBELL_REG_OR   = 0x000D0062,
            PSU_HOST_DOORBELL_REG_RW  = 0x000D0063,
            PSU_HOST_DOORBELL_REG_AND = 0x000D0064,
            PSU_HOST_DOORBELL_REG_OR  = 0x000D0065,
        };

        /**
         * @brief SBE PSU door bell register
         */
        enum sbeDoorbellReg
        {
            // Doorbell Register to trigger SBE interrupt
            // psu_sbe_interrupt_msg_available. Set by host firmware to inform
            // the SBE about a waiting message in the Host/SBE Mailbox Registers
            SBE_DOORBELL      =0x8000000000000000,
        };
        enum hostDoorbellReg
        {
            // Doorbell Register for Host Bridge interrupt. Set by the SBE to
            // inform host firmware about a response message in the Host/SBE
            // Mailbox Registers
            HOST_RESPONSE_WAITING       = 0x8000000000000000,
            HOST_CLEAR_RESPONSE_WAITING = 0x7FFFFFFFFFFFFFFF,
        };


}; // End of Class SbePsu

} // End of namespace SBEIO

#endif
