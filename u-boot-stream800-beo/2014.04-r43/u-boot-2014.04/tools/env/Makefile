#
# (C) Copyright 2002-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

# fw_printenv is supposed to run on the target system, which means it should be
# built with cross tools. Although it may look weird, we only replace "HOSTCC"
# with "CC" here for the maximum code reuse of scripts/Makefile.host.

HOSTSRCS := $(srctree)/tools/env/*.c
HEADERS := fw_env.h $(OBJTREE)/include/config.h

# Compile for a hosted environment on the target
HOSTCFLAGS  = $(patsubst -I%,-idirafter%, $(UBOOTINCLUDE)) \
		-idirafter $(srctree)/tools/env \
                -DUSE_HOSTCC \
                -DTEXT_BASE=$(TEXT_BASE) \

ifeq ($(MTD_VERSION),old)
HOSTCFLAGS += -DMTD_OLD
endif

always := fw_printenv_host fw_printenv

$(obj)/fw_printenv: FORCE
	$(CC) $(HOSTCFLAGS) $(HOSTLDFLAGS) -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8 -o $@ $(HOSTSRCS)

$(obj)/fw_printenv_host: FORCE
	$(HOSTCC) $(HOSTCFLAGS) $(HOSTLDFLAGS) -o $@ $(HOSTSRCS)
