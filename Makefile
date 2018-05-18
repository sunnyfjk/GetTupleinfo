# @Author: fjk
# @Date:   2018-05-18T10:10:37+08:00
# @Last modified by:   fjk
# @Last modified time: 2018-05-18T15:30:45+08:00
all:
	make -C kernelModule module
	make -C App
kernel:
	make -C kernelModule module
kernel-clean:
	make -C kernelModule clean
app:
	make -C App
app-clean:
	make -C App clean
clean:
	make -C kernelModule clean
	make -C App clean
