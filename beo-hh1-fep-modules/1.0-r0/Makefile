obj-m := beo-hh1-input.o beo-hh1-leds.o beo-hh1-fwupdate.o beo-hh1-power.o beo-hh1-tunnel.o beo-hh1-misc.o beo-hh1-rtc.o

obj-m += beo-ase-fep.o beo-ase-dsp.o beo-ase-fephw.o beo-ase-s47-display.o beo-ase-s47-misc.o beo-ase-lsl.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

