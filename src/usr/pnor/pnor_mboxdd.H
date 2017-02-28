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
#ifndef __PNOR_MBOXDD_H
#define __PNOR_MBOXDD_H

#include <limits.h>
#include <config.h>
namespace PNOR { class UdPnorDDParms; }

/** @file pnor_mboxdd.H
 *  @brief Provides the interfaces to the PNOR via the
 *         MBOX protocol
 */

/**
 *  @brief  PNOR Device Driver Class
 *     Provides access to the PNOR flash via the LPC MBOX hardware
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

	astMbox *iv_mbox;
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

#endif /* __PNOR_MBOXDD_H */
