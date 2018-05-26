# GeTupleInfo
获取tcp，udp 五元组信息。
#程序说明
应用程序保存的五元祖信息只包括udp协议tcp协议的。五元组信息在文件中保存的格式为二进制格式，如果想要查看具体信息，请编译的时候奖Makefile中的__DEBUG__选项取消注释。
使用乒乓的功能提高程序的运行效率。
#编译步骤
make
#加载kernel lkm
sudo insmod GetTuple.ko
#执行应用程序
sudo ./GetTuple
#注意事项
kernel lkm所在目录为kernelModule中。
app program 所在目录为App
该kernel lkm 在ubuntu16.04的版本下开发，使用的内核版本为4.13.0-41-generic。在其他环境下不确保可以使用。
应用程序必须是用root权限执行。
应用程序使用的编辑器版本为：gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.9)。
