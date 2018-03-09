2014-03-07
数据库接口有较大修改，现在有三种数据类型的封装：
uuid, 特征数据(最大1024字节)，大数据（大于1024字节，例如图像使用）
此目录下提供对这三种数据类型的测试

运行make可编译出测试程序。

测试程序向IKEmbX00项目的postgresql数据库中写person表和irisdatabase表。

测试代码主体是main-linux.cpp。

2014-02-27
提交postgresql数据库中，uuid和bytea数据类型封装后的使用方法。

因为暂时不会写makefile，所以仅提供.h及.cpp文件用于参考。

cpp文件在src目录下，h文件在当前目录的include目录下。

IKDatabaseWrapper.h是对uuid和bytea数据类型的封装，在../IKDatabaseLib/include目录下。
main-linux.cpp及DataAccessType.h中有对这两种封装数据类型的使用方式。

2014-02-21

在linux环境下，

运行 make 可编译出测试程序。

运行测试程序需要有一下条件：
1、本机安装postgresql
2、存在irisApp库
3、库中有person表，表结构与原来矿山考勤的一样


