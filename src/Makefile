obj-m += bmp280_driver.o

KERNEL_DIR ?= /home/lucas/IMD/buildroot/output/build/linux-custom

all:

	make -C $(KERNEL_DIR) \
	ARCH=arm64 CROSS_COMPILE=aarch64-buildroot-linux-gnu- \
	M=$(PWD) modules

