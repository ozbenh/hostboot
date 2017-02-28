/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/pnor/ast_mboxdd.H $                                       */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2016                        */
/* [+] Google Inc.                                                        */
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
#ifndef __AST_MBOXDD_H
#define __AST_MBOXDD_H

#include <limits.h>
#include <config.h>

/** @file ast_mboxdd.H
 *  @brief Provides the interfaces Aspeed MBOX hardware
 */

/**
 *  @brief  MBOX Device Driver Class
 */
class astMbox
{
public:

	enum {
		BMC_MBOX_DATA_REGS	= 14,
		BMC_MBOX_ARGS_REGS	= 11,

		/* Commands */
		MBOX_C_RESET_STATE			= 0x01,
		MBOX_C_GET_MBOX_INFO		= 0x02,
		MBOX_C_GET_FLASH_INFO		= 0x03,
		MBOX_C_CREATE_READ_WINDOW	= 0x04,
		MBOX_C_CLOSE_WINDOW			= 0x05,
		MBOX_C_CREATE_WRITE_WINDOW	= 0x06,
		MBOX_C_MARK_WRITE_DIRTY		= 0x07,
		MBOX_C_WRITE_FLUSH			= 0x08,
		MBOX_C_BMC_EVENT_ACK		= 0x09,

		/* Responses */
		MBOX_R_SUCCESS				= 0x01,
		MBOX_R_PARAM_ERROR			= 0x02,
		MBOX_R_WRITE_ERROR			= 0x03,
		MBOX_R_SYSTEM_ERROR			= 0x04,
		MBOX_R_TIMEOUT				= 0x05,
	};

	class mboxMessage {
	public:
		mboxMessage(uint8_t i_cmd)
		{
			cmd = i_cmd;
		}
		uint8_t cmd;
		uint8_t seq;
		uint8_t args[BMC_MBOX_ARGS_REGS];
		uint8_t resp;
		uint8_t host;
		uint8_t bmc;

		inline uint8_t get8(int i)
		{
			return args[i];
		}

		inline void put8(int i, uint8_t val)
		{
			args[i] = val;
		}

		inline uint16_t get16(int i)
		{
			return args[i] | (args[i+1] << 8);
		}

		inline void put16(int i, uint16_t val)
		{
			args[i] = val & 0xff;
			args[i+1] = val >> 8;
		}

		inline uint32_t get32(int i)
		{
			return args[i] | (args[i+1] << 8) | (args[i+2] << 16) | (args[i+3] << 24);
		}

		inline void put32(int i, uint32_t val)
		{
			args[i] = val & 0xff;
			args[i+1] = (val >> 8) & 0xff;
			args[i+2] = (val >> 16) & 0xff;
			args[i+3] = val >> 24;
		}
	};

	errlHndl_t doMessage(mboxMessage &msg);

    /**
     * @brief Constructor
     *
     * @parm i_target     Processor Target
     *       NOTE: i_target can only be used after targeting is loaded
     */
    astMbox( TARGETING::Target* i_target = NULL );


    /**
     * @brief Destructor
     */
    ~astMbox();

private:

	enum {
		MBOX_FLAG_REG				= 0x0f,
		MBOX_STATUS_0				= 0x10,
		  MBOX_STATUS_ATTN			= (1 << 7),
		MBOX_STATUS_1				= 0x11,
		MBOX_BMC_CTRL				= 0x12,
		  MBOX_CTRL_INT_STATUS 		= (1 << 7),
		  MBOX_CTRL_INT_MASK		= (1 << 1),
		  MBOX_CTRL_INT_SEND		= (1 << 0),
		MBOX_HOST_CTRL				= 0x13,
		MBOX_BMC_INT_EN_0			= 0x14,
		MBOX_BMC_INT_EN_1			= 0x15,
		MBOX_HOST_INT_EN_0			= 0x16,
		MBOX_HOST_INT_EN_1			= 0x17,

		MBOX_IO_BASE				= 0x1000,
		MBOX_LPC_IRQ				= 9,
	};

	errlHndl_t initializeMbox(void);
	errlHndl_t mboxOut(uint64_t i_addr, uint8_t i_byte);
	errlHndl_t mboxIn(uint64_t i_addr, uint8_t& o_byte);

    /**
     * @brief Processor Target used to access LPC device
     *
     */
    TARGETING::Target* iv_target;

	/**
	 * @brief Sequence number for mailbox messages
	 */
	uint8_t iv_mboxMsgSeq;
};

#endif /* __AST_MBOXDD_H */
