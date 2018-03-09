#ifndef IKCONFIG_H
#define IKCONFIG_H
#include <string>


class IkConfig
{
private:
    IkConfig();
    ~IkConfig();
public:
    static int GetDevLogoCfg(); //获得设备logo配置  0 irisking 1 istek
    static std::string GetDevComCfg(); //vantron 万创系统   dx 顶星设备
    static int GetMouseCfg(); //1 显示鼠标 0隐藏鼠标
    static int GetRebootTime();  //获取设备自动重启时间  hour
};

#endif // IKCONFIG_H
