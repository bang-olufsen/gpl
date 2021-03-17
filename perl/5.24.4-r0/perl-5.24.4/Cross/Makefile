## Toplevel Makefile for cross-compilation of perl

override TOPDIR=${shell pwd}
export TOPDIR
include $(TOPDIR)/config
export CFLAGS
export SYS=$(ARCH)-$(OS)
export CROSS=$(ARCH)-$(OS)-
export FULL_OPTIMIZATION = -fexpensive-optimizations -fomit-frame-pointer -O2
export OPTIMIZATION = -O2

export CC = $(CROSS)gcc
export CXX = $(CROSS)g++
export LD = $(CC)
export STRIP = $(CROSS)strip
export AR = $(CROSS)ar
export RANLIB = $(CROSS)ranlib


## Optimisation work
ifeq ($(ARCH),arm)
 ifdef CONFIG_TARGET_ARM_SA11X0
   ifndef Architecture
     Architecture = armv4l-strongarm
   endif
   FULL_OPTIMIZATION += -march=armv4 -mtune=strongarm1100 -mapcs-32
   OPTIMIZATION += -march=armv4 -mtune=strongarm1100 -mapcs-32
 endif
endif

CFLAGS+=$(FULL_OPTIMIZATION)

all:
	@echo Please read the README file before doing anything else.

perl:
	@echo Perl cross-build directory is $(TOPDIR)
	@echo Target arch is $(SYS)
	@echo toolchain: $(CC), $(CXX), $(LD), $(STRIP), $(AR), $(RANLIB)
	@echo Optimizations: $(FULL_OPTIMIZATION)

	$(TOPDIR)/generate_config_sh config.sh-$(SYS) > $(TOPDIR)/../config.sh
	cd $(TOPDIR)/.. ; ./Configure -S ; make depend ; make ; make more
	cd $(TOPDIR)/.. ; mkdir -p fake_config_library ; cp lib/Config.pm lib/Config_heavy.pl fake_config_library
	cd $(TOPDIR)/.. ; $(MAKE) more2 "PERLRUN=hostperl -I$(TOPDIR)/../fake_config_library -MConfig"
	cd $(TOPDIR)/.. ; $(MAKE) more3 "PERLRUN=hostperl -I$(TOPDIR)/../fake_config_library -MConfig"
	cd $(TOPDIR)/.. ; $(MAKE) more4 "PERLRUN=hostperl -I$(TOPDIR)/../fake_config_library -MConfig"
	cd $(TOPDIR)/.. ; rm -rf install_me_here
#	cd $(TOPDIR)/.. ; make install-strip
	cd $(TOPDIR)/.. ; sh -x Cross/warp


