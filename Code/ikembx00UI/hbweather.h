#ifndef HBWEATHER_H
#define HBWEATHER_H


#include <QObject>
#include <QtNetwork>
#include <QTimer>

#include "hb_weather_struct.h"

class HBWeather : public QObject
{
    Q_OBJECT
public:
    explicit HBWeather(QString str,QObject *parent = 0);
    ~HBWeather();



signals:
    void finish(bool);
public slots:
    void getweather(QNetworkReply *replay);
    void slot_reDirect(QNetworkReply *reply);
private:
    QNetworkAccessManager   *manager;
    QNetworkReply           *replay;
    QString                 m_cityNameFromNet;           /// city name
    weatherdata             m_data;
    bool                    m_isNameFromNet;
    QString                 m_cityName;
    int                     m_isCityUrl;
public:
    /*
     * 返回气象信息和气象图片信息
     */
       weatherdata GetWeatherData();
      void SetCityName(bool isFromNet,QString cityName);
private:
    /*
     * open weather information url
     */
    void openWeatherUrl(QString cityID);

    /*
     * get city weather ID
     */
    QString getCityID(QString cityName);

    /*
     *获取城市名称
     */
    QString getCityName();

    bool isCityExist(QString &cityName);

    /*
     * 通过网络ip获得所在城市名称
     */
    void getCityNameFromIP();

    void parseCityName(QNetworkReply *replay);

    QString HBWeather::getCityNameFromCityId(QString cityName);

    /*
     *  获取气象图片ID
     *  输入：气象信息
     *  返回：天气对应图片ID
     */
    QString getWeatherPic(QString);



};


#endif // HBEATHER_H

