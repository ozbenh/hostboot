/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/pnor/pnordd.H $                                       */
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
#ifndef __PNOR_AST_MBOXDD_H
#define __PNOR_AST_MBOXDD_H

#include <limits.h>
#include <config.h>
namespace PNOR { class UdPnorDDParms; }
class SfcDD;

/** @file pnordd.H
 *  @brief Provides the interfaces to the PNOR Device Driver
 */

/**
 *  @brief  PNOR Device Driver Class
 *     Provides access to the PNOR flash via the ECCB/LPC hardware
 */
class PnorDD
{

  public:
    /**
     * @brief Performs a PNOR Read Operation
     *
     * @parm o_buffer  Buffer to read data into
     * @parm io_buflen  Input: Number of bytes to read,
     *       Output: Number of bytes actually read
     * @parm i_address  Offset into flash to read
     *
     * @return Error from operation
     */
    errlHndl_t readFlash(void* o_buffer,
                         size_t& io_buflen,
                         uint64_t i_address);

    /**
     * @brief Performs a PNOR Write Operation
     *
     * @parm i_buffer  Buffer to write data from
     * @parm io_buflen  Input: Number of bytes to write,
     *       Output: Number of bytes actually written
     * @parm i_address  Offset into flash to write
     *
     * @return Error from operation
     */
    errlHndl_t writeFlash(const void* i_buffer,
                          size_t& io_buflen,
                          uint64_t i_address);

    /**
     * @brief Informs caller if PNORDD is using
     *        L3 Cache for fake PNOR or not.
     *
     * @return Indicate state of fake PNOR
     *         true = PNOR DD is using L3 Cache for fake PNOR
     *         false = PNOR DD not using L3 Cache for fake PNOR
     */
    bool usingL3Cache( );


    /**
     * @brief Retrieve bitstring of NOR workarounds
     * @return NOR workarounds (see VendorWorkarounds in norflash.H)
     */
    uint32_t getNorWorkarounds( void );

    /**
     * @brief Retrieve size of NOR flash
     * @return Size of PNOR in bytes
     */
    uint32_t getNorSize( void );


    // Enumeration values must match those in debug framework.
    enum PnorMode_t {
        MODEL_UNKNOWN,   /**< Invalid */
        MODEL_MEMCPY,    /**< No LPC logic, just do memcpy into cache area */
        MODEL_LPC_MEM,   /**< Break into 32-bit LPC ops but use fake-PNOR */
        MODEL_REAL_CMD,  /**< Code for real HW using Command based reads */
        MODEL_REAL_MMIO, /**< Code for real hardware using MMIO reads */
    };

    /**
     * @brief Constructor
     *
     * @parm i_target     Processor Target connected to PNOR
     *       NOTE: i_target can only be used after targeting is loaded
     */
    PnorDD( TARGETING::Target* i_target = NULL );


    /**
     * @brief Destructor
     */
    ~PnorDD();

  protected:

    /**
     * @brief Call SFC to write data to the PNOR flash, doing retries
     *        as needed
     * @pre Mutex should already be locked before calling
     *
     * @parm[in] i_addr  PNOR flash Address to write
     * @parm[in] i_size  Amount of data to write, in bytes.
     * @parm[in] i_data  Buffer containing data to write
     *
     * @return Error from operation
     */
    errlHndl_t _writeFlash( uint32_t i_addr,
                            size_t i_size,
                            const void* i_data );

    /**
     * @brief Call SFC to read data from the PNOR flash, doing retries
     *        as needed
     * @pre Mutex should already be locked before calling
     *
     * @parm[in] i_addr  PNOR flash Address to read
     * @parm[in] i_size  Amount of data to read, in bytes.
     * @parm[out] o_data  Buffer to read data into
     *
     * @return Error from operation
     */
    errlHndl_t _readFlash( uint32_t i_addr,
                           size_t i_size,
                           void* o_data );



/* Not 16 because the last two are interrupt based status regs */
#define BMC_MBOX_DATA_REGS 14
#define BMC_MBOX_ARGS_REGS 11

#define MBOX_C_RESET_STATE 0x01
#define MBOX_C_GET_MBOX_INFO 0x02
#define MBOX_C_GET_FLASH_INFO 0x03
#define MBOX_C_CREATE_READ_WINDOW 0x04
#define MBOX_C_CLOSE_WINDOW 0x05
#define MBOX_C_CREATE_WRITE_WINDOW 0x06
#define MBOX_C_MARK_WRITE_DIRTY 0x07
#define MBOX_C_WRITE_FLUSH 0x08
#define MBOX_C_BMC_EVENT_ACK 0x09

#define MBOX_R_SUCCESS 0x01
#define MBOX_R_PARAM_ERROR 0x02
#define MBOX_R_WRITE_ERROR 0x03
#define MBOX_R_SYSTEM_ERROR 0x04
#define MBOX_R_TIMEOUT 0x05

#define MBOX_FLAG_REG 0x0f
#define MBOX_STATUS_0 0x10
#define   MBOX_STATUS_ATTN (1 << 7)
#define MBOX_STATUS_1 0x11
#define MBOX_BMC_CTRL 0x12
#define   MBOX_CTRL_INT_STATUS (1 << 7)
#define   MBOX_CTRL_INT_MASK (1 << 1)
#define   MBOX_CTRL_INT_SEND (1 << 0)
#define MBOX_HOST_CTRL 0x13
#define MBOX_BMC_INT_EN_0 0x14
#define MBOX_BMC_INT_EN_1 0x15
#define MBOX_HOST_INT_EN_0 0x16
#define MBOX_HOST_INT_EN_1 0x17

#define MBOX_MAX_QUEUE_LEN 5

#define BMC_RESET 1
#define BMC_COMPLETE 2

#define MBOX_IO_BASE 0x1000

	errlHndl_t mboxOut(uint64_t i_addr, uint8_t i_byte);
	errlHndl_t mboxIn(uint64_t i_addr, uint8_t& o_byte);

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

	errlHndl_t readLpcFw(uint32_t i_offset, uint32_t i_size, void *o_buf);
	errlHndl_t writeLpcFw(uint32_t i_offset, uint32_t i_size, const void *i_buf);

	typedef struct lpcWindow {
		uint32_t lpcAddr;
		uint32_t flashOffset;
		uint32_t size;
		bool open;
	} lpcWindow;

	errlHndl_t adjustMboxWindow(lpcWindow &i_win, uint8_t i_cmd,
								uint32_t i_reqAddr, size_t i_reqSize,
								uint32_t &o_lpcAddr, size_t &o_chunkLen);

  private: // Variables

	uint32_t iv_blockShift;
	uint32_t iv_flashSize;
	uint32_t iv_flashEraseSize;
	lpcWindow iv_readWindow;
	lpcWindow iv_writeWindow;

   /**
    * @brief Global Mutex to prevent concurrent PNOR accesses to Master Proc
    *        This needs to be static so we can mutex across multiple
    *        instances of PnorDD
    */
   static mutex_t cv_mutex;

    /**
     * @brief Class Mutex used to prevent concurrent PNOR accesses
     */
    mutex_t iv_mutex;

    /**
     * @brief Mutex pointer to either class-specific or global mutex to prevent
     *        concurrent PNOR accesses.
     *        Each class uses a mutex; some share the static cv_mutex
     */
    mutex_t* iv_mutex_ptr;

    /**
     * @brief Processor Target used to access PNOR device
     *
     */
    TARGETING::Target* iv_target;

    // Needed for testcases
    friend class PnorDdTest;

    // let the UserDetails classes see internal structures
    friend class PNOR::UdPnorDDParms;
};

#endif /* __PNOR_AST_MBOXDD_H */
