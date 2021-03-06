/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/lib/p9_pstates_cmeqm.h $   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2016                        */
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
/// @file  p9_pstates_cmeqm.h
/// @brief Pstate structures and support routines for CME Hcode
///
// *HWP HW Owner        : Rahul Batra <rbatra@us.ibm.com>
// *HWP HW Owner        : Michael Floyd <mfloyd@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 1
// *HWP Consumed by     : CME:PGPE

#ifndef __P9_PSTATES_CME_H__
#define __P9_PSTATES_CME_H__


/// \defgroup QM Flags
///
/// These are flag bits for the \a Quad Manager field.
///
/// @{

/// qmflag() - Disable Resonant Clock use.
#define PSTATE_RESCLK_DISABLE               0x8000

/// qmflag() - Disable IVRM use.
#define PSTATE_IVRMS_DISABLE                0x4000

/// qmflag() - Disable VDM use.
#define PSTATE_VDM_DISABLE                  0x2000

/// qmflag() - Disable WOF.
#define PSTATE_WOF_DISABLE                  0x1000

/// qmflag() - dpll_dynamic_fmax_enable
#define PSTATE_DPLL_DYNAMIC_FMAX_ENABLE     0x0800

/// qmflag() - dpll_dynamic_fmin_enable
#define PSTATE_DPLL_DYNAMIC_FMIN_ENABLE     0x0400

/// qmflag() - dpll_droop_protect_enable
#define PSTATE_DPLL_DROOP_PROTECT_ENABLE    0x0200

/// @}


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/// Quad Manager Flags
///

typedef union
{
    uint16_t value;
    struct
    {
#ifdef _BIG_ENDIAN
        uint16_t    resclk_enable               : 1;
        uint16_t    ivrm_enable                 : 1;
        uint16_t    wof_enable                  : 1;
        uint16_t    dpll_dynamic_fmax_enable    : 1;
        uint16_t    dpll_dynamic_fmin_enable    : 1;
        uint16_t    dpll_droop_protect_enable   : 1;
        uint16_t    reserved                    : 10;
#else
        uint16_t    reserved                    : 10;
        uint16_t    dpll_droop_protect_enable   : 1;
        uint16_t    dpll_dynamic_fmin_enable    : 1;
        uint16_t    dpll_dynamic_fmax_enable    : 1;
        uint16_t    wof_enable                  : 1;
        uint16_t    ivrm_enable                 : 1;
        uint16_t    resclk_enable               : 1;
#endif // _BIG_ENDIAN
    } fields;

} QuadManagerFlags;

/// Resonant Clock Stepping Entry
///
typedef union
{
    uint16_t value;
    struct
    {
#ifdef _BIG_ENDIAN
        uint16_t    sector_buffer   : 4;
        uint16_t    spare1          : 1;
        uint16_t    pulse_enable    : 1;
        uint16_t    pulse_mode      : 2;
        uint16_t    resonant_switch : 4;
        uint16_t    spare4          : 4;
#else
        uint16_t    spare4          : 4;
        uint16_t    resonant_switch : 4;
        uint16_t    pulse_mode      : 2;
        uint16_t    pulse_enable    : 1;
        uint16_t    spare1          : 1;
        uint16_t    sector_buffer   : 4;
#endif // _BIG_ENDIAN
    } fields;

} ResonantClockingStepEntry;

#define RESCLK_FREQ_REGIONS 8
#define RESCLK_STEPS        64
#define RESCLK_L3_STEPS     4

typedef struct ResonantClockControl
{
    uint8_t resclk_freq[RESCLK_FREQ_REGIONS];    // Lower frequency of Resclk Regions

    uint8_t resclk_index[RESCLK_FREQ_REGIONS];   // Index into value array for the
    // respective Resclk Region

    /// Array containing the transition steps
    ResonantClockingStepEntry steparray[RESCLK_STEPS];

    /// Delay between steps (in nanoseconds)
    /// Maximum delay: 65.536us
    uint16_t    step_delay_ns;

    /// L3 Clock Stepping Array
    uint8_t     l3_steparray[RESCLK_L3_STEPS];

    /// Resonant Clock Voltage Threshold (in millivolts)
    /// This value is used to choose the appropriate L3 clock region setting.
    uint16_t l3_threshold_mv;

} ResonantClockingSetup;


/// VDM/Droop Parameter Block
///
typedef struct
{
    uint8_t  vid_compare_override_mv_enable;
    uint8_t  vid_compare_override_mv[VPD_PV_POINTS];
    uint8_t  vdm_response;

    // For the following *_enable fields, bits are defined to indicate
    // which of the respective *override* array entries are valid.
    // bit 0: UltraTurbo; bit 1: Turbo; bit 2: Nominal; bit 3: PowSave
    uint8_t  droop_small_override_enable;
    uint8_t  droop_large_override_enable;
    uint8_t  droop_extreme_override_enable;
    uint8_t  overvolt_override_enable;
    uint16_t fmin_override_khz_enable;
    uint16_t fmax_override_khz_enable;

    // The respecitve *_enable above indicate which index values are valid
    uint8_t  droop_small_override[VPD_PV_POINTS];
    uint8_t  droop_large_override[VPD_PV_POINTS];
    uint8_t  droop_extreme_override[VPD_PV_POINTS];
    uint8_t  overvolt_override[VPD_PV_POINTS];
    uint16_t fmin_override_khz[VPD_PV_POINTS];
    uint16_t fmax_override_khz[VPD_PV_POINTS];

    /// Pad structure to 8-byte alignment
    /// @todo pad once fully structure is complete.
    // uint8_t pad[1];

} VDMParmBlock;


/// The layout of the data created by the Pstate table creation firmware for
/// comsumption by the Pstate GPE.  This data will reside in the Quad
/// Power Management Region (QPMR).
///

/// Standard options controlling Pstate setup procedures

/// System Power Distribution Paramenters
///
/// Parameters set by system design that influence the power distribution
/// for a rail to the processor module.  This values are typically set in the
/// system machine readable workbook and are used in the generation of the
/// Global Pstate Table.  This values are carried in the Pstate SuperStructure
/// for use and/or reference by OCC firmware (eg the WOF algorithm)


/// IVRM Parameter Block
///
/// @todo Major work item.  Largely will seed the CME Quad Manager to perform
/// iVRM voltage calculations

#define IVRM_ARRAY_SIZE 64
typedef struct iVRMInfo
{

    /// Pwidth from 0.03125 to 1.96875 in 1/32 increments at Vin=Vin_Max
    uint8_t strength_lookup[IVRM_ARRAY_SIZE];   // Each entry is a six bit value, right justified

    /// Scaling factor for the Vin_Adder calculation.
    uint8_t vin_multiplier[IVRM_ARRAY_SIZE];     // Each entry is from 0 to 255.

    /// Vin_Max used in Vin_Adder calculation (in millivolts)
    uint16_t    vin_max_mv;

    /// Delay between steps (in nanoseconds)
    /// Maximum delay: 65.536us
    uint16_t    step_delay_ns;

    /// Stabilization delay once target voltage has been reached (in nanoseconds)
    /// Maximum delay: 65.536us
    uint16_t    stablization_delay_ns;

    /// Deadzone (in millivolts)
    /// Maximum: 255mV.  If this value is 0, 50mV is assumed.
    uint8_t    deadzone_mv;

    /// Pad to 8B
    uint8_t    pad;

} IvrmParmBlock;


/// The layout of the data created by the Pstate table creation firmware for
/// comsumption by the CME Quad Manager.  This data will reside in the Core
/// Power Management Region (CPMR).
///
typedef struct
{

    /// Magic Number
    uint64_t magic;     // the last byte of this number the structure's version.

    // QM Flags
    QuadManagerFlags qmflags;

    /// Operating points
    ///
    /// VPD operating points are stored without load-line correction.  Frequencies
    /// are in MHz, voltages are specified in units of 5mV, and currents are
    /// in units of 500mA.
    VpdOperatingPoint operating_points[VPD_PV_POINTS];

    /// Loadlines and Distribution values for the VDD rail
    SysPowerDistParms vdd_sysparm;

    /// External Biases
    ///
    /// Biases applied to the VPD operating points prior to load-line correction
    /// in setting the external voltages.  This is used to recompute the Vin voltage
    /// based on the Global Actual Pstate .
    /// Values in 0.5%
    VpdBias ext_biases[VPD_PV_POINTS];

    /// Internal Biases
    ///
    /// Biases applied to the VPD operating points that are used for interpolation
    /// in setting the internal voltages (eg Vout to the iVRMs) as part of the
    /// Local Actual Pstate.
    /// Values in 0.5%
    VpdBias int_biases[VPD_PV_POINTS];

    /// IVRM Data
    IvrmParmBlock ivrm;

    /// Resonant Clock Grid Management Setup
    ResonantClockingSetup resclk;

    /// VDM Data
    VDMParmBlock vdm;

} LocalPstateParmBlock;

#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __P9_PSTATES_CME_H__ */
