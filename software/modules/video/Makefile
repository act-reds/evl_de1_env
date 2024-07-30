obj-m += de1_video_module.o
de1_video_module-objs := de1_video.o 

PWD         = `pwd`

BSP_DIR     = $(PWD)/../../../output

TOOLCHAIN   = $(BSP_DIR)/host/bin/arm-buildroot-linux-gnueabihf-
ARCH = arm

KERNEL_SRC  = $(BSP_DIR)/build/linux-ipipe-core-5.4.151-arm-4

XENOMAI_SRC = xenomai-stable_v3.1.x

EXTRA_CFLAGS += -I$(KERNEL_SRC)/include \
-I$(BSP_DIR)/build/$(XENOMAI_SRC)/include/ \
-I$(BSP_DIR)/build/$(XENOMAI_SRC)/include/cobalt \
-I$(BSP_DIR)/build/$(XENOMAI_SRC)/include/cobalt/kernel \
-I$(BSP_DIR)/build/$(XENOMAI_SRC)/kernel/cobalt/include/ \
-I$(BSP_DIR)/build/$(XENOMAI_SRC)/kernel/cobalt/arch/arm/include/ 

all: modules

modules:
	make -C $(KERNEL_SRC) CPPFLAGS=$(CPP_FLAGS) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(TOOLCHAIN) M=$(PWD) modules 

clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean

