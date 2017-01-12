##########################################################################
#             .:::::.
#            -+  .| :/
#            o ALAN  s
#            .+.`.| +-                __ _   _ __    ___    ___   _ __
#              .:::y+ .....          / _` | | '__|  / _ \  / _ \ | '_ \
#                  /yo++++o/-     . | (_| | | |    |  __/ |  __/ | | | |
#  .::::.        -o/.       /s/+oo/  \__, | |_|     \___|  \___| |_| |_|
# o  @@ +-      .s-     .-.  -s:      ___ /      _ 
#:: @@ @ dyyyyyyso _-:::  `--.++     |___/      | |
#`o E@I o-      :s-          .s/    __   ___   _| | ___ __ _ _ __   ___
#  .:::-         +o.        `oo`    \ \ / / | | | |/ __/ _` | '_ \ / _ \
#                .ss+:-..-:+o-       \ V /| |_| | | (_| (_| | | | | (_) |
#          .:::./ho``.::::-`          \_/  \__,_|_|\___\__,_|_| |_|\___/
#        .+ --- +.
#        o  GVT  o                          T e c h n o l o g i e s
#        .+ --- +. 
#          -:::-'
#
#    This file: Makefile - rules to build the current project
##########################################################################
#
# GreenVulcano Technologies IoT Platform
#    Copyright (C) 2016 by GreenVulcano S.r.l.
#    All Rights reserved.
#    
#    This data is proprietary to GreenVulcano Technologies and shall be
#    treated as confidential. Disclosure, reproduction or use of this data
#    for any purpose, other than its original issue, is strictly forbidden
#    without the prior written consent of GreenVulcano Technologies
#
##########################################################################

#
# BEGIN - BUILDING CONFIGURATION PART
#         Modify the variables
#

# VERBOSE
# -------
# Set to `1` if you want verbose output from the build script
VERBOSE=1

# ROOT
# -------
# Set this to the directory where the CC-3200 SDK is installed
TI_SIMPLELINK_ROOT=/opt/cc3200-sdk/simplelink
TI_SIMPLELINK_NETAPPS=/opt/cc3200-sdk/netapps

LIBNAME=gvlib-simplelink.a

#
# END - BUILD CONFIGURATION PART
#         Do not modify lines below this disclaimer unless you know
#         exactly what you are doing


PROJBASE=.

ifeq (arm, $(findstring arm, ${MAKECMDGOALS}))
	PREFIX=arm-none-eabi-
else
	PREFIX=
endif


#
# Include the common make definitions.
#
include ./makedefs


TARGET=${BINDIR}/${LIBNAME}
TEST_NOOP=${BINDIR}/test_noop_transport
TEST_STRING_UTILS=${BINDIR}/test_string_utils
TEST_RETURN_VALUE=${BINDIR}/test_return_value


#
# Where to find source files- that do not live in this directory.
#
VPATH+=${PROJBASE}/src
VPATH+=${PROJBASE}/src/common
VPATH+=${PROJBASE}/src/arm/ti-simplelink/src
VPATH+=${PROJBASE}/src/test

#
# Where to find header files that do not live in the source directory.
#

IPATH=${PROJBASE}/src
IPATH+=${PROJBASE}/include
IPATH+=${PROJBASE}/src/arm/ti-simplelink/include

IPATH+=${TI_SIMPLELINK_ROOT}/include
IPATH+=${TI_SIMPLELINK_ROOT}/../simplelink_extlib/provisioninglib
IPATH+=${TI_SIMPLELINK_ROOT}/../driverlib
IPATH+=${TI_SIMPLELINK_ROOT}/../inc


IPATH+=${TI_SIMPLELINK_NETAPPS}/mqtt/include

#
# The default rule, which causes the driver library to be built.
#
.PHONY: arm clean test
arm: ${OBJDIR} ${BINDIR}
arm: ${TARGET}

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${OBJDIR} ${wildcard *~}
	@rm -rf ${BINDIR} ${wildcard *~}


test: ${OBJDIR} ${BINDIR}
test: ${TEST_NOOP}
test: ${TEST_STRING_UTILS}
test: ${TEST_RETURN_VALUE}


#
# The rule to create the target directories.
#
${OBJDIR}:
	@mkdir -p ${OBJDIR}

${BINDIR}:
	@mkdir -p ${BINDIR}


#
# Files to be built and linked to build the final solution.
#
${TARGET}: ${OBJDIR}/gv.o
${TARGET}: ${OBJDIR}/protocol_v1.o
${TARGET}: ${OBJDIR}/mqtt_transport.o
${TARGET}: ${OBJDIR}/mqtt_transport_impl.o


${TEST_NOOP}: ${OBJDIR}/gv.o
${TEST_NOOP}: ${OBJDIR}/protocol_v1.o
${TEST_NOOP}: ${OBJDIR}/test_noop_transport.o

${TEST_STRING_UTILS}: ${OBJDIR}/gv.o
${TEST_STRING_UTILS}: ${OBJDIR}/protocol_v1.o
${TEST_STRING_UTILS}: ${OBJDIR}/test_string_utils.o

${TEST_RETURN_VALUE}: ${OBJDIR}/gv.o
${TEST_RETURN_VALUE}: ${OBJDIR}/protocol_v1.o
${TEST_RETURN_VALUE}: ${OBJDIR}/test_returnvalue.o

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OBJDIR}/*.d} __dummy__
endif
