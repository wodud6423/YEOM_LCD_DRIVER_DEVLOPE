obj-m := gpio-sysfs.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR =/home/yeom/workspace/ldd/source/linux_bbb_5.4/
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/
DTB_PATH=/home/yeom/workspace/ldd/source/linux_bbb_5.4/arch/arm/boot/dts/

SOURCE_PATH=$(PWD)/source
OUTPUT_PATH=$(PWD)/output

TARGET_HOST=debian
TARGET_IP=192.168.45.2
TARGET_PATH=/home/debian/lcd_test

CFLAGS+=-I./include

$(shell mkdir -p output)
all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(SOURCE_PATH) modules
	mv $(SOURCE_PATH)/*.ko $(OUTPUT_PATH) 
	mv $(SOURCE_PATH)/*.mod $(OUTPUT_PATH)
	mv $(SOURCE_PATH)/*.o $(OUTPUT_PATH)
clean:
	make -C $(KERN_DIR) M=$(SOURCE_PATH) clean
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(SOURCE_PATH) clean
	rm -rf $(OUTPUT_PATH)/*
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(SOURCE_PATH) help
host:
	make -C $(HOST_KERN_DIR) M=$(SOURCE_PATH) modules
	mv $(SOURCE_PATH)/*.ko $(OUTPUT_PATH) 
	mv $(SOURCE_PATH)/*.mod $(OUTPUT_PATH) 
copy-dtb: 
	scp $(DTB_PATH)am335x-boneblack.dtb $(TARGET_HOST)@$(TARGET_IP):$(TARGET_PATH)

copy-drv: 
	scp *.ko *.elf $(TARGET_HOST)@$(TARGET_IP):$(TARGET_PATH)
app: lcd_app.o lcd.o gpio.o lcd_app.elf

lcd_app.o:
	$(CROSS_COMPILE)gcc -I./include -c -o $@ source/lcd_app_copy.c 
lcd.o: 
	$(CROSS_COMPILE)gcc -I./include -c -o $@ source/lcd_copy.c 
gpio.o: 
	$(CROSS_COMPILE)gcc -I./include -c -o $@ source/gpio_copy.c 
lcd_app.elf: lcd_app.o lcd.o gpio.o 
	$(CROSS_COMPILE)gcc -o $@ $^ -lpthread
	mv *.o *.elf $(OUTPUT_PATH)






