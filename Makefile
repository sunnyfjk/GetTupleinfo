# @Author: fjk
# @Date:   2018-05-18T10:10:37+08:00
# @Last modified by:   fjk
# @Last modified time: 2018-05-18T15:10:23+08:00
kernel:
	make -C kernelModule module
kernel-clean:
	make -C kernelModule clean
App:
	make -C App GetTuple
App-clean:
	make -C App clean
