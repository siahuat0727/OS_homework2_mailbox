MODULE_NAME = mailbox
obj-m += $(MODULE_NAME).o
ccflags-y := -std=gnu99

PWD         := $(shell pwd)
KVERSION    := $(shell uname -r)
KERNEL_DIR   = /lib/modules/$(KVERSION)/build
all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
