/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/utils/stopreg/p9_stop_data_struct.H $ */
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

///
/// @file   p9_stop_data_struct.H
/// @brief  describes data structures internal to STOP API.
///
// *HWP HW Owner    :  Greg Still <stillgs@us.ibm.com>
// *HWP FW Owner    :  Prem Shanker Jha <premjha2@in.ibm.com>
// *HWP Team        :  PM
// *HWP Level       :  2
// *HWP Consumed by :  HB:HYP
#ifndef __STOP_DATA_STRUCT_
#define __STOP_DATA_STRUCT_

#ifndef  _AIX
    #include <endian.h>
#endif

#include "p9_stop_section_defines.H"

#ifdef __FAPI_2_
    #include <fapi2.H>
#endif


#ifdef __cplusplus
extern "C" {
namespace stopImageSection
{
#endif

typedef struct
{
    uint32_t scomEntryHeader;
    uint32_t scomEntryAddress;
    uint64_t scomEntryData;
} ScomEntry_t;

/**
 * @brief models a CPU register restoration area in STOP section of homer image.
 */
typedef struct
{
    uint8_t threadArea[THREAD_AREA_SIZE];
    uint8_t coreArea[CORE_SPR_SECTN_SIZE];
} SprRestoreArea_t;

/**
 * @brief   models homer image of a chip.
 * @note    sections not relevant for CPU register restoration have been
 * abstracted using field 'reserve'.
 */
typedef struct
{
    uint8_t occ_host_sgpe_area[ TWO_MB ];  // CPU restore area starts at an offset of 2MB from chip HOMER
    uint8_t interrruptHandler[INTERRUPT_HANDLER_SIZE];
    uint8_t threadLauncher[THREAD_LAUNCHER_SIZE];
    SprRestoreArea_t coreThreadRestore[MAX_CORE_ID_SUPPORTED + 1][MAX_THREAD_ID_SUPPORTED + 1];
    uint8_t reserve[(ONE_KB * ONE_KB) - SPR_RESTORE_PER_CHIP];
} HomerSection_t;

/**
 * @brief models cache subsection in STOP section of a given homer image.
 * @note    given the start of cache subsection associated with a given core,
 *          the structure below represents what a cache subsection would look
 *          like. Based on known start address, quick traversing can be done
 *          within the cache subsection.
 */
typedef struct
{
    ScomEntry_t  nonCacheArea[MAX_EQ_SCOM_ENTRIES];
    ScomEntry_t  l2CacheArea[MAX_L2_SCOM_ENTRIES];
    ScomEntry_t  l3CacheArea[MAX_L3_SCOM_ENTRIES];
} StopCacheSection_t;

/**
 * @brief summarizes attributes associated with a SPR register.
 */
typedef struct
{
    uint32_t sprId;
    bool isThreadScope;
} StopSprReg_t;

enum
{
    SIZE_SCOM_ENTRY = sizeof( ScomEntry_t ),
    SCOM_ENTRY_START = 0xDEADDEAD,
};

#ifdef __FAPI_2_
    #define MY_ERR( _fmt_, _args_...)  FAPI_ERR(_fmt_, ##_args_)
    #define MY_INF(_fmt_, _args_...) FAPI_INF(_fmt_, ##_args_)
#else
    #define MY_ERR( _fmt_, _args_...)
    #define MY_INF(_fmt_, _args_...)
#endif

#define CORE_ID_SCOM_START(io_image,\
                           i_chipletId) \
((ScomEntry_t*)(((uint8_t*)(io_image))+ CORE_SCOM_SECTN_START +\
                ((i_chipletId - CORE_CHIPLET_ID_MIN) * \
                 SCOM_SIZE_PER_CORE)));

#define CACHE_SECTN_START(io_image,\
                          i_chipletId) \
((StopCacheSection_t *)(((uint8_t *)(io_image))+CACHE_SCOM_SECTN_START \
                        + ( i_chipletId - CACHE_CHIPLET_ID_MIN ) * \
                        SCOM_SIZE_PER_CACHE_CHIPLET ));
#ifdef __cplusplus
}  // extern "C"

} //namespace stopImageSection ends
#endif //__cplusplus

#endif
