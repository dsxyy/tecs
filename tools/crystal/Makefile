KERNELNAME=/lib/modules/$(shell uname -r)/build
SUBDIR := $(shell pwd)
DIR_INSTALL=$(INSTALLROOT)/

#ifneq ($(KERNELRELEASE),)
	obj-m := BladeCrystal.o
#else

default:
	@echo "Begin to make...."
	$(MAKE) -C $(KERNELNAME) M=$(SUBDIR) ARCH=x86_64 modules

#endif

install:
	mkdir -p $(DIR_INSTALL)
	cp -f ./start.sh $(DIR_INSTALL)
	cp -f ./BladeCrystal.ko $(DIR_INSTALL)

clean:
	$(RM) *.o *.mod.c *.mod.o *.ko 




