# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/usr/diag/mdia/makefile $
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
ROOTPATH = ../../../..

EXTRAINCDIR += ${ROOTPATH}/src/include/usr/diag
##########################################################
# Following are needed to include hwpf/hwp/fapi2.H
##########################################################
EXTRAINCDIR += ${ROOTPATH}/src/import/hwpf/fapi2/include
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/ecmddatabuffer
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/utils
##########################################################
# Following is needed to include memdiags.H
##########################################################
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/common/include
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/memory
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/ffdc
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/memory/lib/mcbist
EXTRAINCDIR += ${ROOTPATH}/src/import/generic/memory/
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/fapi2
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/targeting/common
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/utils/imageProcs

MODULE = mdia

OBJS += mdiamonitor.o
OBJS += mdiatrace.o
OBJS += mdiaworkitem.o
OBJS += mdiamba.o
OBJS += mdia.o
OBJS += mdiasm.o

SUBDIRS += test.d

include ${ROOTPATH}/config.mk
