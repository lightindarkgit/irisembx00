#pragma once
// 天气预报信息
typedef struct _tagWeatherInfo
{
    // 城市名字
    std::string CityName;
    // 温度
    std::string TotalTemp;
    // 天气状况(晴、多云、小雨...)
    std::string WeatherInfo;
    // 风级
    std::string Winp;
    // weather ico
    std::string WeatherICO;

}WEATHERINFO, *PWEATHERINFO;

typedef void(*pfCallBack)(WEATHERINFO &wInfo);
