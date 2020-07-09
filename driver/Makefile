# Makefile . makefile of "driver"
#  
# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.

# name of the Driver
DRIVER_KMOD=fsdriver

ifneq (${KERNELRELEASE},)
# We were called by kbuild
	obj-m := $(DRIVER_KMOD).o
$(DRIVER_KMOD)-y := fdriver.o \
		fdriver_debug.o \
		fdriver_procfs.o \
		fdriver_sysfs.o
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
   EXTRA_CFLAGS += -DFS_DEBUG
else

COMPILEFOR = "OCTEONTX_SDK"
#COMPILEFOR = "X86"
#COMPILEFOR = "ARMV8"
#COMPILEFOR = "WEB"


#TMP = ${findstring OCTEONTX_SDK,${COMPILEFOR}}
#ifeq  ($(TMP),OCTEONTX_SDK)
##----------------------Start Cross Compile -------------------------
##     this is for arm - ebb8100    OCTEON SDK 6.0  4.4.6 
#
#ARCH := arm
##KDIR := path/to/your/target/kernel
#KDIR := /usr/local/Cavium_Networks/OCTEONTX-SDK/linux/kernel/linux
#PWD  := $(shell pwd)
#CROSS := aarch64-thunderx-linux-gnu-
#
#default:
#	@echo '    Building fsdriver  for thunderx'
#	@echo '    PLEASE IGNORE THE "Overriding SUBDIRS" WARNING'
#	$(MAKE) -C $(KDIR) M=$(PWD)  SUBDIRS=$(PWD)  ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules
#endif


TMP = ${findstring ARMV8,${COMPILEFOR}}
ifeq  ($(TMP),ARMV8)
#----------------------Start Native ArmV8 Compile -------------------------
#     this is for arm - fluffy  ubuntu 16.04 arm  4.4.0 
	KERNEL_SOURCE := /usr/src/linux-headers-$(shell uname -r)
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
endif


TMP = ${findstring X86,${COMPILEFOR}}
ifeq  ($(TMP),X86)
#----------------------Start Native X86 Compile -------------------------
#    this is for x86 - oldwoof centos 7.1 -> 3.10 kernel
	KERNEL_SOURCE := /usr/src/kernels/$(shell uname -r)
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
endif

TMP = ${findstring WEB,${COMPILEFOR}}
ifeq  ($(TMP),WEB)
#----------------------Start Original Native Compile -------------------------
#    From Web
	KERNEL_SOURCE := /usr/src/linux
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
endif

             
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif

