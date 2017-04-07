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
	uint8_t *l_data = reinterpret_cast <uint8_t *>((char *)&msg);
	errlHndl_t l_err = NULL;
	uint8_t l_stat1;
	uint8_t l_flags;
	uint32_t l_loops = 0;
	bool l_prot_error = false;
	int i;

	TRACFCOMP( g_trac_pnor, ENTER_MRK"astMboxDD::doMessage(0x%02x)", msg.cmd );
	msg.seq = iv_mboxMsgSeq++;

	do {
		/* Write message out */
		for (i = 0; i < BMC_MBOX_DATA_REGS && !l_err; i++)
			l_err = mboxOut(i, l_data[i]);
		if ( l_err )
			break;

		/* Clear status1 response bit as we just set it writing the req. */
		l_err = mboxOut(MBOX_STATUS_1, MBOX_STATUS1_RESP);
		if ( l_err )
			break;

		/* Ping BMC */
		l_err = mboxOut(MBOX_HOST_CTRL, MBOX_CTRL_INT_SEND);
		if ( l_err )
			break;

		TRACFCOMP( g_trac_pnor, "Sending cmd...");

		/* Wait for response */
		while ( l_loops++ < MBOX_MAX_RESP_WAIT_US && !l_err ) {
			l_err = mboxIn(MBOX_STATUS_1, l_stat1);
			if ( l_err )
				break;
			l_err = mboxIn(MBOX_FLAG_REG, l_flags);
			if ( l_err )
				break;
			if ( l_stat1 & MBOX_STATUS1_RESP )
				break;
			nanosleep(0, 1000);
		}

		TRACFCOMP( g_trac_pnor, "status=%02x flags=%02x", l_stat1, l_flags);

		if ( l_err ) {
			TRACFCOMP( g_trac_pnor, "Got error waiting for response !");
			break;
		}

		if ( !(l_stat1 & MBOX_STATUS1_RESP) ) {
			TRACFCOMP( g_trac_pnor, "Timeout waiting for response !");

			// Don't try to interrupt the BMC anymore
			mboxOut(MBOX_HOST_CTRL, 0);

            /*@
             * @errortype
             * @moduleid     PNOR::MOD_ASTMBOXDD_DO_MESSAGE
             * @reasoncode   PNOR::RC_SFC_TIMEOUT
             * @userdata1[48:55] mbox status 1 reg
             * @userdata1[56:63] mbox flag reg
             * @devdesc      astMbox::doMessage> Timeout waiting for
             *               message response
             * @custdesc     BMC not responding while accessing the flash
             */
            l_err = new ERRORLOG::ErrlEntry(ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                            PNOR::MOD_ASTMBOXDD_DO_MESSAGE,
                                            PNOR::RC_SFC_TIMEOUT,
                                            TWO_UINT8_TO_UINT16(l_stat1, l_flags),
                                            0);

            // Limited in callout: no PNOR target, so calling out processor
            l_err->addHwCallout( iv_target,
								 HWAS::SRCI_PRIORITY_HIGH,
								 HWAS::NO_DECONFIG,
								 HWAS::GARD_NULL );

            l_err->collectTrace(PNOR_COMP_NAME);

			// Tell the code below that we generated the error (not an LPC error)
			l_prot_error = true;
		}

		if ( l_err )
			break;

		/* Clear status */
		mboxOut(MBOX_STATUS_1, MBOX_STATUS1_RESP);

		// Remember some message fields before they get overwritten
		// by the response
		uint8_t old_seq = msg.seq;
		uint8_t old_cmd = msg.cmd;

		// Read response
		TRACFCOMP( g_trac_pnor, "Reading response data...");
		for (i = 0; i < BMC_MBOX_DATA_REGS && !l_err; i++)
			l_err = mboxIn(i, l_data[i]);
		if ( l_err ) {
			TRACFCOMP( g_trac_pnor, "Got error reading response !");
			break;
		}
		TRACFCOMP( g_trac_pnor, "Message: cmd:%02x seq:%02x a:%02x %02x %02x %02x %02x..resp:%02x",
				   msg.cmd, msg.seq, msg.args[0], msg.args[1], msg.args[2], msg.args[3],
				   msg.args[4], msg.resp);
		if (old_seq != msg.seq) {
			TRACFCOMP( g_trac_pnor, "bad sequence number in mbox message, got %d want %d",
					   msg.seq, old_seq);

            /*@
             * @errortype
             * @moduleid     PNOR::MOD_ASTMBOXDD_DO_MESSAGE
             * @reasoncode   PNOR::RC_MBOX_BAD_SEQUENCE
             * @userdata1[48:55] mbox status 1 reg
             * @userdata1[56:63] mbox flag reg
             * @userdata2[32:39] original command code
             * @userdata2[40:47] response command code
             * @userdata2[48:55] sequence wanted
             * @userdata2[56:63] sequence obtained
             * @devdesc      astMbox::doMessage> Timeout waiting for
             *               message response
             * @custdesc     BMC not responding while accessing the flash
             */
            l_err = new ERRORLOG::ErrlEntry(ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                            PNOR::MOD_ASTMBOXDD_DO_MESSAGE,
                                            PNOR::RC_MBOX_BAD_SEQUENCE,
                                            TWO_UINT8_TO_UINT16(l_stat1, l_flags),
                                            FOUR_UINT8_TO_UINT32(old_cmd,
																 msg.cmd,
																 old_seq,
																 msg.seq));

            // Limited in callout: no PNOR target, so calling out processor
            l_err->addHwCallout( iv_target,
								 HWAS::SRCI_PRIORITY_HIGH,
								 HWAS::NO_DECONFIG,
								 HWAS::GARD_NULL );

            l_err->collectTrace(PNOR_COMP_NAME);

			// Tell the code below that we generated the error (not an LPC error)
			l_prot_error = true;
		}
		if (msg.resp != MBOX_R_SUCCESS) {
			TRACFCOMP( g_trac_pnor, "BMC mbox command failed with err %d", msg.resp);

            /*@
             * @errortype
             * @moduleid     PNOR::MOD_ASTMBOXDD_DO_MESSAGE
             * @reasoncode   PNOR::RC_MBOX_ERROR_STATUS
             * @userdata1[48:55] mbox status 1 reg
             * @userdata1[56:63] mbox flag reg
             * @userdata2[32:39] original command code
             * @userdata2[40:47] response command code
             * @userdata2[48:55] sequence number
             * @userdata2[56:63] status code
             * @devdesc      astMbox::doMessage> Timeout waiting for
             *               message response
             * @custdesc     BMC not responding while accessing the flash
             */
            l_err = new ERRORLOG::ErrlEntry(ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                            PNOR::MOD_ASTMBOXDD_DO_MESSAGE,
                                            PNOR::RC_MBOX_ERROR_STATUS,
                                            TWO_UINT8_TO_UINT16(l_stat1, l_flags),
                                            FOUR_UINT8_TO_UINT32(old_cmd,
																 msg.cmd,
																 old_seq,
																 msg.resp));

            // Limited in callout: no PNOR target, so calling out processor
            l_err->addHwCallout( iv_target,
								 HWAS::SRCI_PRIORITY_HIGH,
								 HWAS::NO_DECONFIG,
								 HWAS::GARD_NULL );

            l_err->collectTrace(PNOR_COMP_NAME);

			// Tell the code below that we generated the error (not an LPC error)
			l_prot_error = true;
		}

	} while(0);

	// If we got an LPC error, commit it and generate our own
	if ( l_err && !l_prot_error ) {
		errlHndl_t l_lpc_err = l_err;

		TRACFCOMP( g_trac_pnor, "LPC Error writing to mbox :: RC=%.4X",
				   ERRL_GETRC_SAFE(l_err) );

          /*@
             * @errortype
             * @moduleid     PNOR::MOD_ASTMBOXDD_DO_MESSAGE
             * @reasoncode   PNOR::RC_LPC_ERROR
             * @devdesc      astMbox::doMessage> LPC Error communicating
                             with the mailbox
             * @custdesc     LPC bus error communicating with the BMC
             */
            l_err = new ERRORLOG::ErrlEntry(ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                            PNOR::MOD_ASTMBOXDD_DO_MESSAGE,
                                            PNOR::RC_LPC_ERROR, 0, 0);

            // Limited in callout: no PNOR target, so calling out processor
            l_err->addHwCallout( iv_target,
								 HWAS::SRCI_PRIORITY_HIGH,
								 HWAS::NO_DECONFIG,
								 HWAS::GARD_NULL );

            l_err->collectTrace(PNOR_COMP_NAME);

            l_err->plid(l_lpc_err->plid());
            l_lpc_err->setSev(ERRORLOG::ERRL_SEV_INFORMATIONAL);
            ERRORLOG::errlCommit(l_lpc_err, PNOR_COMP_ID);
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
