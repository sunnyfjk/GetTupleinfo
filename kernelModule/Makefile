# @Author: fjk
# @Date:   2018-05-18T10:11:06+08:00
# @Last modified by:   fjk
# @Last modified time: 2018-05-20T16:08:38+08:00
obj-m	=GetTuple.o
#CFLAGS_GetTuple.o +=-D__DEBUG__
KERNEL=/lib/modules/$(shell uname -r )/build
PWD=$(shell pwd)
module:
	make -C $(KERNEL) M=$(PWD) modules
clean:
	make -C $(KERNEL) M=$(PWD) clean
