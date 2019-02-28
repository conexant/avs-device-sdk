#snd-soc-cx2072x-objs := cx2072x.o
#snd-soc-cx2092x-objs := cx2092x.o
snd-soc-cx9000-objs := cx9000.o
snd-soc-simple-card-plus-objs := simple-card-plus.o
#obj-m += snd-soc-cx2072x.o
#obj-m += snd-soc-cx2092x.o
obj-m += snd-soc-cx9000.o
obj-m += snd-soc-simple-card-plus.o

dtbo-y += rpi-cxsmartspk2-i2s-plus.dtbo
#dtbo-y += rpi-cxsmartspk-i2s-plus.dtbo
#dtbo-y += rpi-cxsmartspk-usb2-plus.dtbo
#dtbo-y += rpi-cxsmartspk-usb.dtbo
target += $(dtbo-y)
always := $(dtbo-y)
clean-files := *.dtbo
UNAME := $(shell uname -r)

all:
	make -C /lib/modules/$(BUILD_ARG)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(BUILD_ARG)/build M=$(PWD) clean

