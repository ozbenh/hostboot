# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/build/mkrules/hbfw/img/makefile $
#
# OpenPOWER HostBoot Project
#
# Contributors Listed Below - COPYRIGHT 2012,2017
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG

#
# FSP Destination: src/hbfw/img/makefile
#
# NOTE: Do NOT modify this file in CMVC directly!  It comes from the Hostboot
#       repository and will be overwritten.

.include <${RULES_MK}>

.if($(CONTEXT:R) == "ppc")

VPATH += ../fsp

DEFAULT_PATH = ${.PATH}
SRCPATH = ${DEFAULT_PATH:M*src*}

build_all: cp_hbfiles gen_default_images
install_all: gen_system_specific_images build_sbe_partitions build_pnor_images

#Some useful search paths
HBFW_OBJPATH = ${.PATH:M*obj*}
ENGD_OBJPATH = ${HBFW_OBJPATH:S/hbfw\/img/engd\/href/g}
ENGD_SRCPATH = ${SRCPATH:S/hbfw\/img/engd\/href/g}

#################################################
# Copy Hostboot binary images to obj dir to be grabbed
# during build flash pass and consumption by HWSV.
# Having external users acquire from OBJ dir as there are packaging
# changes expected in the future which will require using OBJ version.
#################################################

# Input default images
BOOTLDR_IMG = hostboot_bootloader.bin
HBBL_IMG = hbbl.bin
HB_SECROM_IMG = hostboot_securerom.bin
HBB_IMG = hostboot.bin
HBB_ECC_IMG = hostboot.bin.ecc
HBI_IMG =  hostboot_extended.bin
HBRT_IMG = hostboot_runtime.bin
BASE_IMAGES = ${BOOTLDR_IMG} ${HB_SECROM_IMG} ${HBB_IMG} ${HBI_IMG} ${HBRT_IMG}

# Input fake images
HBI_FAKE_IMG = hostboot_extended.bin.fake
VPO_FAKE_MVPD = vpo_sysmvpd.dat
VPO_FAKE_DJVPD = vpo_djvpd.dat
VPO_FAKE_DVPD = dvpd.dat
FAKE_IMAGES = ${HBI_FAKE_IMG} ${VPO_FAKE_MVPD} ${VPO_FAKE_DJVPD} ${VPO_FAKE_DVPD}
DEFAULT_INPUT_IMAGES = ${BASE_IMAGES} ${HBB_ECC_IMG} ${HBBL_IMG} ${FAKE_IMAGES}

# Output final images
HBBL_FINAL_IMG = HBBL.bin
HBB_FINAL_IMG = HBB.bin
HBI_FINAL_IMG = HBI.bin
HBRT_FINAL_IMG = HBRT.bin
TEST_FINAL_IMG = TEST.bin
TESTRO_FINAL_IMG = TESTRO.bin
HBEL_FINAL_IMG = HBEL.bin
GUARD_FINAL_IMG = GUARD.bin
GLOBAL_FINAL_IMG = GLOBAL.bin
DJVPD_FINAL_IMG = DJVPD.bin
MVPD_FINAL_IMG = MVPD.bin
CVPD_FINAL_IMG = CVPD.bin
PAYLOAD_FINAL_IMG = PAYLOAD.bin
RINGOVD_FINAL_IMG = RINGOVD.bin
SBKT_FINAL_IMG = SBKT.bin
WOFDATA_FINAL_IMG = WOFDATA.bin
FINAL_OUTPUT_IMAGES = ${HBBL_FINAL_IMG} ${HBB_FINAL_IMG} ${HBI_FINAL_IMG} \
			  ${HBRT_FINAL_IMG} ${TEST_FINAL_IMG} ${TESTRO_FINAL_IMG} \
			  ${HBEL_FINAL_IMG} ${GUARD_FINAL_IMG} ${GLOBAL_FINAL_IMG} \
			  ${DJVPD_FINAL_IMG} ${MVPD_FINAL_IMG} ${CVPD_FINAL_IMG} \
			  ${PAYLOAD_FINAL_IMG} ${RINGOVD_FINAL_IMG} ${SBKT_FINAL_IMG} \
			  ${WOFDATA_FINAL_IMG}

# Temp images
HB_TEMP_IMG = hb_temp.bin

# Aggregate
ALL_DEFAULT_IMAGES = ${DEFAULT_INPUT_IMAGES} ${FINAL_OUTPUT_IMAGES}

# Imprint hw keys' hash
IMPRINT_HW_KEY_HASH = ${imprintHwKeyHash:P}

# Script to manipulate bin files to prepare for buildpnor (install_all phase)
# Note: sections with no input files are zero filled images and pass EMPTY as
#       their input file name. This is so the script knows it needs to generate
#       them, rather than use an input.
# Note: HBI depends on HBB for sw signatures. Ensure that both are passed into
#       the same --systemBinFiles parameter for genPnorImages
GEN_PNOR_IMAGE_SCRIPT = ${genPnorImages.pl:P}

# Decide which PNOR to build
.if(${FAKEPNOR} == "")
    DEFAULT_PNOR_LAYOUT = ${defaultPnorLayout.xml:P}
    # Parameters passed into GEN_PNOR_IMAGE_SCRIPT.
    GEN_DEFAULT_BIN_FILES = HBBL=${HBBL_IMG},HBB=${HBB_IMG},HBI=${HBI_IMG},HBRT=${HBRT_IMG},TEST=EMPTY,TESTRO=EMPTY,HBEL=EMPTY,GUARD=EMPTY,GLOBAL=EMPTY,PAYLOAD=EMPTY,CVPD=EMPTY,MVPD=EMPTY,DJVPD=EMPTY,RINGOVD=EMPTY,SBKT=EMPTY,WOFDATA=EMPTY
    DEFAULT_PARAMS = --build-all ${TARGET_TEST:b--test} \
                     ${CONFIG_SECUREBOOT:b--secureboot} --systemBinFiles ${GEN_DEFAULT_BIN_FILES} \
                     --pnorLayout ${DEFAULT_PNOR_LAYOUT} ${KEY_TRANSITION_PARAMS} ${CORRUPT_PARAMS} \
                     --hwKeyHashFile ${IMPRINT_HW_KEY_HASH}
.else
    DEFAULT_PNOR_LAYOUT = ${pnorLayoutFake.xml:P}
    # Parameters passed into GEN_PNOR_IMAGE_SCRIPT.
    GEN_DEFAULT_BIN_FILES = HBI=${HBI_IMG},HBEL=EMPTY,MVPD=${${VPO_FAKE_MVPD}:P},DJVPD=${${VPO_FAKE_DJVPD}:P}
    DEFAULT_PARAMS = --systemBinFiles ${GEN_DEFAULT_BIN_FILES} --pnorLayout ${DEFAULT_PNOR_LAYOUT}
.endif

cp_hbfiles: .SPECTARG
	${BASE_IMAGES:@image@cp -f ${SRCPATH:F${image}} ${image};@}

gen_default_images: cp_hbfiles
	currentsb -chain
	ecc --inject ${HBB_IMG} --output ${HBB_ECC_IMG} --p8

    # Remove offset from start of Bootloader image for HBBL partition
    # Actual code is offset from HRMOR by 12k = 12 1k-blocks (space
    # reserved for exception vectors)
	dd if=${BOOTLDR_IMG} of=${HBBL_IMG} ibs=8 skip=1536 conv=sync

	# Append Hostboot securerom code to the HBBL section
	cat ${HB_SECROM_IMG} >> ${HBBL_IMG}

	# Call script to generate final bin files for default images
	${GEN_PNOR_IMAGE_SCRIPT} ${DEFAULT_PARAMS}

clobber_cp_hbfiles:
	rm -f ${ALL_DEFAULT_IMAGES}

#################################################
###  SAMPLE for building an SBE Partition with multiple ECs
#################################################
#S1_EC10_BIN = ${ENGD_OBJPATH:Fs1_10.sbe_seeprom.bin}
#s1SbePartition.bin: ${SBE_BUILD_SCRIPT} ${S1_EC10_BIN}
#	${buildSbePart.pl:P} --sbeOutBin s1SbePartition.bin \
#		--ecImg_10 ${S1_EC10_BIN}
#################################################
SBE_BUILD_SCRIPT = ${buildSbePart.pl:P}

P9N_EC10_BIN = ${ENGD_OBJPATH:Fp9n_10.sbe_seeprom.hdr.bin}

SBE_PART_INFO = \
  p9nSbePartition.bin:10=${P9N_EC10_BIN}


__SBE_PART_BUILD/% : .SPECTARG .PMAKE
	@${MAKE:T:R} BUILD_SPECIFIC_SBEPART \
	    "SBE_PART_PARAMS=${.TARGET:s/__SBE_PART_BUILD\///:s/:/ /g}"

.ifdef SBE_PART_PARAMS

SBEPART_TARGET = ${SBE_PART_PARAMS:xs/ .*//}
SBEPART_SECTIONS = ${SBE_PART_PARAMS:xs/[^ ]* //:xs/ .*//}
SBEPART_BINS = ${SBEPART_SECTIONS:s/,/ /g:xS/[^=]*=//g}
SBEPART_BIN_OPTION = ${SBEPART_SECTIONS:s/,/ /g:S/^/--ecImg_/g:s/=/ /g:p}

BUILD_SPECIFIC_SBEPART: .SPECTARG ${SBEPART_TARGET}
	#@echo TARGET ${SBEPART_TARGET}
	#@echo LAYOUT ${SBEPART_LAYOUT}
	#@echo BINARIES ${SBEPART_BINS}
	#@echo BIN_OPTION ${SBEPART_BIN_OPTION}

${SBEPART_TARGET}: ${SBEPART_LAYOUT} ${SBEPART_BINS} ${PNOR_BUILD_SCRIPT}
	 ${SBE_BUILD_SCRIPT}  --sbeOutBin ${SBEPART_TARGET} \
		${SBEPART_BIN_OPTION}

.endif

build_sbe_partitions: .SPECTARG ${SBE_PART_INFO:S/^/__SBE_PART_BUILD\//g}

clobber_build_sbe_partitions:
	${SBE_PART_INFO:@image@${pnorimg:!rm -f ${image:s/:/ /g:xs/ .*//};!e}@}

#################################################
###  SAMPLE for building a PNOR image
#################################################
#MURANO_TARGETING = simics_MURANO_targeting.bin
#MURANO_LAYOUT = defaultPnorLayout.xml
#murano.pnor: ${MURANO_TARGETING} ${MURANO_LAYOUT} hostboot_extended.bin hostboot.bin
#	${buildpnor.pl:P} --pnorLayout ${.PATH:F${MURANO_LAYOUT}} \
#	   --pnorOutBin ${.TARGET} \
#	   --binFile_HBI ${.PATH:Fhostboot_extended.bin} \
#	   --binFile_HBD ${.PATH:F${MURANO_TARGETING}} \
#	   --binFile_HBB ${.PATH:Fhostboot.bin} \
#	   --fpartCmd "${FPARTCMD}" --fcpCmd "${FCPCMD}"
##################################################
PNOR_BUILD_SCRIPT = ${buildpnor.pl:P}

#early hostboot use of this makefile will be against FSP drivers lacking fips_pnor.mk
#so need to use tryinclude for now.
.tryinclude <${.PATH:Ffips_pnor.mk}>

HBFW_OBJPATH = ${.PATH:M*obj*}
ENGD_OBJPATH = ${HBFW_OBJPATH:S/hbfw\/img/engd\/href/g}

## Define system name strings
NIMBUS = NIMBUS

# Input system specific images
NIMBUS_HBD_IMG = simics_${NIMBUS}_targeting.bin
NIMBUS_HCODE_IMG = ${ENGD_SRCPATH:Fp9n.hw_ref_image.bin}
NIMBUS_SBE_IMG = p9nSbePartition.bin
NIMBUS_OCC_IMG = ${bb}/images/ppc/lab/fs/p9le/rootfs/opt/extucode/81e00430.lid
SBEC_IMG = centSbePartition.bin

# Input fake images
NIMBUS_VPO_HBD_IMG = vbu_${NIMBUS}_targeting.bin

# Output final images
NIMBUS_HBD_FINAL_IMG = ${NIMBUS}.HBD.bin
NIMBUS_HCODE_FINAL_IMG = ${NIMBUS}.HCODE.bin
NIMBUS_SBE_FINAL_IMG = ${NIMBUS}.SBE.bin
NIMBUS_OCC_FINAL_IMG = ${NIMBUS}.OCC.bin
SBEC_FINAL_IMG = SBEC.bin
TEMP_IMG = temp.bin

# Decide which PNOR to build
.if(${FAKEPNOR} == "")
    # Paramemters passed into GEN_PNOR_IMAGE_SCRIPT.
    #@TODO securebootp9 - Put `HBD=${${NIMBUS_HBD_IMG}:P},` back into genPnorImage parameters
    GEN_NIMBUS_BIN_FILES = ${NIMBUS}:SBE=${${NIMBUS_SBE_IMG}:P},HCODE=${${NIMBUS_HCODE_IMG}:P},OCC=${${NIMBUS_OCC_IMG}:P}
    #@TODO RTC:163810 - Put SBEC back "--systemBinFile  SBEC=${${SBEC_IMG}:P}"
    SYSTEM_SPECIFIC_PARAMS = ${TARGET_TEST:b--test} ${CONFIG_SECUREBOOT:b--secureboot} \
                             --pnorLayout ${DEFAULT_PNOR_LAYOUT} ${CORRUPT_PARAMS} \
                             --systemBinFiles ${GEN_NIMBUS_BIN_FILES} --hwKeyHashFile ${IMPRINT_HW_KEY_HASH}
.else
    # Parameters passed into GEN_PNOR_IMAGE_SCRIPT.
    #@TODO securebootp9 - Put `HBD=${${NIMBUS_VPO_HBD_IMG}:P}` back into genPnorImage parameters
    GEN_NIMBUS_BIN_FILES = ${NIMBUS}:HCODE=${${NIMBUS_HCODE_IMG}:P}
    #@TODO RTC:163810 - Put SBEC back "--systemBinFile  SBEC=${${SBEC_IMG}:P}"
    SYSTEM_SPECIFIC_PARAMS = --pnorLayout ${DEFAULT_PNOR_LAYOUT} \
                             --systemBinFiles ${GEN_NIMBUS_BIN_FILES}
.endif

gen_system_specific_images: build_sbe_partitions
	# Call script to generate final bin file for system specific images
	${GEN_PNOR_IMAGE_SCRIPT} ${SYSTEM_SPECIFIC_PARAMS}
	#@TODO securebootp9 - Remove all dd/ecc commands and add HBD back into genPnorImage
	# HBD partition
	dd if=${${NIMBUS_HBD_IMG}:P} of=${TEMP_IMG} ibs=1024k conv=sync
	ecc --inject ${TEMP_IMG} --output ${NIMBUS_HBD_FINAL_IMG} --p8
	#for NIMBUS fake pnor skip version header
	.if(${FAKEPNOR} != "")
	dd if=${${NIMBUS_VPO_HBD_IMG}:P} of=${NIMBUS_HBD_FINAL_IMG} ibs=4k skip=1 conv=sync
	.endif
	rm -f ${TEMP_IMG}

HOSTBOOT_DEFAULT_SECTIONS = HBBL=${HBBL_FINAL_IMG},HBB=${HBB_FINAL_IMG},HBI=${HBI_FINAL_IMG},HBRT=${HBRT_FINAL_IMG},TEST=${TEST_FINAL_IMG},TESTRO=${TESTRO_FINAL_IMG},HBEL=${HBEL_FINAL_IMG},GUARD=${GUARD_FINAL_IMG},GLOBAL=${GLOBAL_FINAL_IMG},PAYLOAD=${PAYLOAD_FINAL_IMG},CVPD=${CVPD_FINAL_IMG},MVPD=${MVPD_FINAL_IMG},DJVPD=${DJVPD_FINAL_IMG},RINGOVD=${RINGOVD_FINAL_IMG},SBKT=${SBKT_FINAL_IMG},WOFDATA=${WOFDATA_FINAL_IMG}
#@TODO RTC:163810 - Put SBEC back "SBEC=${SBEC_FINAL_IMG},"
NIMBUS_SECT = HBD=${NIMBUS_HBD_FINAL_IMG},SBE=${NIMBUS_SBE_FINAL_IMG},HCODE=${NIMBUS_HCODE_FINAL_IMG},OCC=${NIMBUS_OCC_FINAL_IMG}
PNOR_IMG_INFO = \
   nimbus.pnor:${DEFAULT_PNOR_LAYOUT}:${NIMBUS_SECT},${HOSTBOOT_DEFAULT_SECTIONS} \
   ${FIPS_PNOR_INFO}

# To build fake PNOR, set FAKEPNOR to filename of file to build,
#    ie, 'export FAKEPNOR=fake8m.pnor'
# To not build fake PNOR, do not set FAKEPNOR or clear its setting,
#    ie, 'export FAKEPNOR='
# Note: If FAKEPNOR defined, ONLY a FAKEPNOR will be generated, this is due to
#    some limitations of GEN_PNOR_IMAGE_SCRIPT at the time of porting to p9
.if(${FAKEPNOR} != "")
    HOSTBOOT_DEFAULT_SECTIONS = HBI=${HBI_FINAL_IMG},HBEL=${HBEL_FINAL_IMG},MVPD=${MVPD_FINAL_IMG},DJVPD=${DJVPD_FINAL_IMG},CVPD=${VPO_FAKE_DVPD}
    NIMBUS_SECT = HBD=${NIMBUS_HBD_FINAL_IMG},HCODE=${NIMBUS_HCODE_FINAL_IMG}
    PNOR_IMG_INFO = \
        ${FAKEPNOR}:${DEFAULT_PNOR_LAYOUT}:${NIMBUS_SECT},${HOSTBOOT_DEFAULT_SECTIONS} \
        ${FIPS_PNOR_INFO}
.endif

# Added a different dependency chain for hostboot standalone so we can compile
# faster and call one rule in dist.targets.mk 'update_images_for_sandbox'
.if( ${HB_STANDALONE} == 1)
__IMAGE_BUILD/% : .SPECTARG .PMAKE gen_default_images gen_system_specific_images
# In FSP this is run at install_all phase, so it's assumed all build_all rules
# have completed. If build_all rule dependencies are added they will run again.
.else
__IMAGE_BUILD/% : .SPECTARG .PMAKE gen_system_specific_images
.endif
	@${MAKE:T:R} BUILD_SPECIFIC_IMAGE \
	    "IMAGE_PARAMS=${.TARGET:s/__IMAGE_BUILD\///:s/:/ /g}"

.ifdef IMAGE_PARAMS

IMAGE_TARGET = ${IMAGE_PARAMS:xs/ .*//}
IMAGE_LAYOUT = ${IMAGE_PARAMS:xs/[^ ]* //:xs/ .*//:p}
IMAGE_SECTIONS = ${IMAGE_PARAMS:xs/[^ ]* //:xs/[^ ]* //:xs/ .*//}
IMAGE_BINS = ${IMAGE_SECTIONS:s/,/ /g:xS/[^=]*=//g}
IMAGE_BIN_OPTION = ${IMAGE_SECTIONS:s/,/ /g:S/^/--binFile_/g:s/=/ /g:p}

BUILD_SPECIFIC_IMAGE: .SPECTARG ${IMAGE_TARGET}
	#@echo TARGET ${IMAGE_TARGET}
	#@echo LAYOUT ${IMAGE_LAYOUT}
	#@echo BINARIES ${IMAGE_BINS}
	#@echo BIN_OPTION ${IMAGE_BIN_OPTION}

${IMAGE_TARGET}: ${IMAGE_LAYOUT} ${IMAGE_BINS} ${PNOR_BUILD_SCRIPT}
	.if(${PNOR} == ${IMAGE_TARGET} || ${PNOR} == "" || \
			${FAKEPNOR} == ${IMAGE_TARGET})
		${PNOR_BUILD_SCRIPT} --pnorOutBin ${IMAGE_TARGET} \
			${TARGET_TEST:b--test} --pnorLayout ${IMAGE_LAYOUT} \
			${IMAGE_BIN_OPTION} --fpartCmd "fpart" --fcpCmd "fcp"
	.endif
.endif

build_pnor_images:.SPECTARG ${PNOR_IMG_INFO:S/^/__IMAGE_BUILD\//g}

clobber_build_pnor_images:
	${PNOR_IMG_INFO:@image@${pnorimg:!rm -f ${image:s/:/ /g:xs/ .*//};!e}@}

#################################################
#Special target to copy images to flash dir for simics
#################################################
FLASH_DEST = $(MAKETOP)$(OBJECTDIRTOP)../images/$(CONTEXT)/lab/flash
FLASH_IMG = ${HBB_IMG} ${HBB_ECC_IMG} ${HBBL_IMG}
FAKE_PNOR = fake6m.pnor

update_images_for_sandbox:  gen_default_images gen_system_specific_images build_sbe_partitions build_pnor_images
	mkdir -p ${FLASH_DEST}
	#Copy hostboot base image and bootloader image to flash dir
	${FLASH_IMG:@image@${baseimg:!cd ${FLASH_DEST}; cp -f ${.PATH:F${image}} ${image};!e}@}
	#Truncate fake pnor image down to 6MB and copy to flash dir
	.if(${FAKEPNOR} != "")
		${FAKE_PNOR_IMG_INFO:@image@${pnorimg:!dd if=${image:s/:/ /g:xs/ .*//} of=${FAKE_PNOR} bs=6M count=1; \
			cp -f ${FAKE_PNOR} ${FLASH_DEST}/${FAKE_PNOR};!e}@}
	.endif
	#Copy pnor images to flash dir
	${PNOR_IMG_INFO:@image@${pnorimg:!cd ${FLASH_DEST}; \
		if [ "${PNOR}" == "${image:s/:/ /g:xs/ .*//}" ] || \
			[ "${PNOR}" == "" ] || \
			[ "${FAKEPNOR}" == "${image:s/:/ /g:xs/ .*//}" ]; \
		then cp -f ${.PATH:F${image:s/:/ /g:xs/ .*//}} ${image:s/:/ /g:xs/ .*//}; fi !e}@}

.endif

