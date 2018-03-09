2014-02-10
 如何使用restoreDB.local

 这是个SHELL程序，用于在ubuntu系统下恢复备份的数据库文件。

 使用方法如下：
 ./restoreDB.local irisApp irisApp.bak

 其中，irisApp就是要操作的数据库名称，
 irisApp.bak是之前备份的数据库文件。

 irisApp和irisApp.bak是缺省参数，就是说，如果就用这两个参数，则直接运行
 ./restoreDB.local
 也是可以的。如果有变化，就在运行时，输入参数。
