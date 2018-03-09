所有的动态和静态库都要放到相应的库位置下

20160620 added by yqhe
Logger相关文件及使用方法
1、请将IKEmbX00/Code/lib目录下的
liblog4cpp.a
liblog4cpp.la
liblog4cpp.so
liblog4cpp.so.5
liblog4cpp.so.5.0.6
liblogger.so
拷贝到/usr/local/lib目录下

2、sudo方式运行ldconfig
sudo ldonfig

3、在需要写log的文件中，加上
#include "Logger.h" // 此文件在Code/Common目录下

4、写log的例子:
例子1——
Logger::LOGD("send recog to server");

例子2——
std::stringstream oss;
oss << "[" << __FUNCTION__ << "]: send(add) - ";
oss << vecAddPerson.size();
oss << " person(s) to server";
Logger::LOGD(oss.str().c_str());

5、Logger::LOGD() 写Debug信息
   Logger::LOGW() 写Warning信息
   依次类推，详情请查询Logger.h

6、log4cpp配置文件的样例在IKServerManager/03Code/log4cpp目录下，文件名为log4cpp.conf，可参考


