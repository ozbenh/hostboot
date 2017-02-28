/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/pnor/ast_mboxdd.C $                                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2015                        */
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
/**
 *  @file ast_mboxdd.C
 *
 *  @brief Implementation of the PNOR Device Driver on top of AST MBOX protocol
 */

/*****************************************************************************/
// I n c l u d e s
/*****************************************************************************/
#include <sys/mmio.h>
#include <sys/task.h>
#include <sys/sync.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <devicefw/driverif.H>
#include <trace/interface.H>
#include <errl/errlentry.H>
#include <errl/errlmanager.H>
#include <errl/errludlogregister.H>
#include <errl/errludstring.H>
#include <targeting/common/targetservice.H>
#include <sio/sio.H>
#include "ast_mboxdd.H"
#include <pnor/pnor_reasoncodes.H>
#include <sys/time.h>
#include <initservice/initserviceif.H>
#include <util/align.H>
#include <lpc/lpcif.H>
#include <config.h>

// Initialized in pnorrp.C
extern trace_desc_t* g_trac_pnor;

errlHndl_t astMbox::mboxOut(uint64_t i_addr, uint8_t i_byte)
{
	size_t len = sizeof(i_byte);
	return deviceWrite(iv_target, /*TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL, */
					   &i_byte,
					   len,
					   DEVICE_LPC_ADDRESS(LPC::TRANS_IO,
										  i_addr + MBOX_IO_BASE));
}

errlHndl_t astMbox::mboxIn(uint64_t i_addr, uint8_t &o_byte)
{
	size_t len = sizeof(o_byte);
	return deviceRead(iv_target, /* TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL, */
					  static_cast<uint8_t*>(&o_byte),
					  len,
					  DEVICE_LPC_ADDRESS(LPC::TRANS_IO,
										 i_addr + MBOX_IO_BASE));
}

errlHndl_t astMbox::doMessage(struct mboxMessage &msg)
{
	uint8_t *data = reinterpret_cast <uint8_t *>((char *)&msg);
	errlHndl_t l_err = NULL;
	uint8_t status;
	int i;

	TRACFCOMP( g_trac_pnor, ENTER_MRK"astMboxDD::doMessage(0x%02x)", msg.cmd );
	msg.seq = iv_mboxMsgSeq++;

	do {
		/* Clear status */
		mboxOut(MBOX_HOST_CTRL, MBOX_CTRL_INT_STATUS);

		l_err = mboxIn(MBOX_HOST_CTRL, status);
		TRACFCOMP( g_trac_pnor, "Initial status=%02x...", status);

		/* Write message out */
		for (i = 0; i < BMC_MBOX_DATA_REGS && !l_err; i++)
			l_err = mboxOut(i, data[i]);
		if (l_err)
			break;

		/* Ping BMC */
		l_err = mboxOut(MBOX_HOST_CTRL, MBOX_CTRL_INT_SEND);
		if (l_err)
			break;

		TRACFCOMP( g_trac_pnor, "Sending cmd...");

		/* Wait for response */
		do {
			l_err = mboxIn(MBOX_HOST_CTRL, status);
			if (l_err)
				break;
			if (status & MBOX_CTRL_INT_STATUS)
				break;
			/* XXX Add timeout */
			/* XXX Add attentions from BMC */
			nanosleep(0, 1000);
		} while(1);

		if (l_err)
			break;

		/* Clear status */
		mboxOut(MBOX_HOST_CTRL, MBOX_CTRL_INT_STATUS);

		/* Check sequence number */
		uint8_t old_seq = msg.seq;
		for (i = 0; i < BMC_MBOX_DATA_REGS && !l_err; i++)
			l_err = mboxIn(i, data[i]);
		if (l_err)
			break;
		TRACFCOMP( g_trac_pnor, "Message: cmd:%02x seq:%02x a:%02x %02x %02x %02x %02x..resp:%02x\n",
				   msg.cmd, msg.seq, msg.args[0], msg.args[1], msg.args[2], msg.args[3],
				   msg.args[4], msg.resp);
		if (old_seq != msg.seq) {
			TRACFCOMP( g_trac_pnor, "bad sequence number in mbox message, got %d want %d",
					   msg.seq, old_seq);
			// XXX Do something about it
		}
		if (msg.resp != MBOX_R_SUCCESS) {
			TRACFCOMP( g_trac_pnor, "BMC mbox command failed with err %d", msg.resp);
			// XXX Do something about it
		}

	} while(0);

	if (l_err) {
		/* XXX */
        TRACFCOMP( g_trac_pnor, "LPC Error writing to mbox :: RC=%.4X",
				   ERRL_GETRC_SAFE(l_err) );
	}
    TRACFCOMP(g_trac_pnor, EXIT_MRK "astMboxDD::doMessage() resp=0x%02x", msg.resp );
	return l_err;
}

errlHndl_t astMbox::initializeMbox(void)
{
	errlHndl_t l_errl = NULL;
	uint8_t l_data;
	size_t l_len = sizeof(uint8_t);

	TRACFCOMP(g_trac_pnor, ENTER_MRK"PnorDD::initializeMBOX()");
	do {
		l_data = SIO::DISABLE_DEVICE;
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x30));
		if (l_errl) { break; }

		// Set SUART1 addr to g_uartBase
		l_data = (MBOX_IO_BASE >> 8) & 0xFF;
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x60));
		if (l_errl) { break; }

		l_data = MBOX_IO_BASE & 0xFF;
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x61));
		if (l_errl) { break; }

		l_data = MBOX_LPC_IRQ;
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x70));
		if (l_errl) { break; }

		l_data = 1; /* Low level trigger */
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x71));
		if (l_errl) { break; }

		l_data = SIO::ENABLE_DEVICE;
		l_errl = deviceOp( DeviceFW::WRITE,
						   TARGETING::MASTER_PROCESSOR_CHIP_TARGET_SENTINEL,
						   &(l_data),
						   l_len,
						   DEVICE_SIO_ADDRESS(SIO::MB, 0x30));
		if (l_errl) { break; }

	} while(0);

	return l_errl;
}

/**
 * @brief  Constructor
 */
astMbox::astMbox( TARGETING::Target* i_target )
	: iv_target(i_target)
	, iv_mboxMsgSeq(1)
{
    TRACFCOMP(g_trac_pnor, ENTER_MRK "astMbox::astMbox()" );
    errlHndl_t l_err = NULL;

	l_err = initializeMbox();
	if (l_err)
	{
        TRACFCOMP( g_trac_pnor, "Failure to initialize the MBOX logic, shutting down :: RC=%.4X", ERRL_GETRC_SAFE(l_err) );
        l_err->collectTrace(PNOR_COMP_NAME); /* XX change comp name/ID ? */
        ERRORLOG::errlCommit(l_err,PNOR_COMP_ID);
        INITSERVICE::doShutdown( PNOR::RC_PNOR_INIT_FAILURE );
    }

    TRACFCOMP(g_trac_pnor, EXIT_MRK "astMbox::astMbox()" );
}

/**
 * @brief  Destructor
 */
astMbox::~astMbox()
{
}
