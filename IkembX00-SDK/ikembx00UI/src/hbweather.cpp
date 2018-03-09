

/************************************************************
 *
 * weather source
 *
 * http://www.weather.com.cn/data/sk/101010100.html
 * http://www.weather.com.cn/data/cityinfo/101010100.html
 *
 *
 *
 ***********************************************************/

#include "hbweather.h"
#include <QDebug>
#include <QTextCodec>
#include <QTextStream>
#include "../Common/Logger.h"
#include "ikxmlhelper.h"
#include "commononqt.h"
#include "../Common/Exectime.h"

QString str1;

HBWeather::HBWeather(QString str,QObject *parent) :
	QObject(parent)
{
	
	manager = new QNetworkAccessManager(parent);
	connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slot_reDirect(QNetworkReply*)));
	m_isNameFromNet = true;
	m_cityName = getCityName();
	m_isCityUrl = 1;
}

HBWeather::~HBWeather()
{
	

}
/*****************************************************************************
 *                        解析气象信息函数
 *  函 数 getweather
 *  功    能：将获取到的气象信息（js格式）数据解析出来供本地使用。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-2
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void HBWeather::getweather(QNetworkReply *replay)
{
	
	QTextCodec *codec = QTextCodec::codecForName("utf8");
	QString str= codec->toUnicode( replay->readAll());
	str.replace(QString("["),QString("+"));
	str.replace(QString("]"),QString("+"));
	QStringList list= str.split('+');
	if(list.count()>=2)
		str=list.at(list.count()-2);

	QStringList list2= str.split('}');

	QString a;

	str = list2.at(0);
	str.replace(QString("{"),QString(""));
	str.replace(QString("\""),QString(""));
	list=str.split(',');
	for(int i=0;i<list.count();i++)
	{

		QString st;
		QStringList li;
		st=list.at(i);
		li=st.split(':');

		if(li.first()=="low")
			m_data.temp1=li.at(1).mid(2);
		else if(li.first()=="high")
			m_data.temp2=li.at(1).mid(2);
		else if(li.first()=="type")
			m_data.weather1=li.at(1);
		else if(li.first()=="fengli")
			m_data.wind1=li.at(1);
		else if(li.first()=="fengxiang")
			m_data.wind2=li.at(1);
		else if(li.first()=="date")
			m_data.date_y=li.at(1);
		else if(li.first()=="week")
			m_data.week=li.at(1);
	}
	emit finish(true);
}

/*****************************************************************************
 *                        获取气象信息
 *  函 数 slot_reDirect
 *  功    能：根据输入链接，获取气象信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-2
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void HBWeather::slot_reDirect(QNetworkReply *reply)
{
/*
	int http_status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	reply->close();
	reply->deleteLater();

   // LOG_DEBUG("http status %d",http_status);

	// 判断是否是重定向
	if (http_status >= 300 && http_status <= 309 )
	{
		QVariant possibleRedirectUrl =  reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
		QUrl redirect = possibleRedirectUrl.toUrl();
		if(!redirect.isEmpty())
		{
			manager->get(QNetworkRequest(redirect));
		}
	}
	else
	{
		if(m_isCityUrl == 1)
		{
			//  程序运行时只获取一次
			parseCityName(reply);
		}
		else
		{
			getweather(reply);
		}

	}
    */
}
/*****************************************************************************
 *                        打开气象数据链接
 *  函 数 openWeatherUrl
 *  功    能：根据城市名称，获取城市代码，形成URL链接。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-2
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void HBWeather::openWeatherUrl(QString cityID)
{
	
	QString url = "http://wthrcdn.etouch.cn/weather_mini?citykey=" +cityID;
	manager->get(QNetworkRequest(QUrl(url)));
}
/*****************************************************************************
 *                        通過网络获取设备ip所在城市
 *  函 数 GetCityFromIP()
 *  功    能：根据城市名称，获取城市代码，形成URL链接。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-21
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void HBWeather::getCityNameFromIP()
{
	
	//QString ipApiUrl = "http://int.dpool.sina.com.cn/iplookup/iplookup.php";
	// QString ipApiUrl = "http://ip.taobao.com/service/getIpInfo.php?ip=123.123.123.123";
	QString ipApiUrl = "http://pv.sohu.com/cityjson?ie=utf-8";
	manager->get(QNetworkRequest(QUrl(ipApiUrl)));
}

/*****************************************************************************
 *                        获取城市气象编号
 *  函 数 getCityID
 *  功    能：获取城市气象编号。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-2
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString HBWeather::getCityID(QString cityName)
{
	
	QString cityCode;
	QFile cityInf("./citycodeutf8.txt");

	if (!cityInf.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<< cityInf.errorString();
	}

	while (!cityInf.atEnd())
	{
		QString strc = cityInf.readLine().trimmed();
		QString name  = strc.mid(10).toLocal8Bit();

		if (name == cityName)
		{
			cityCode = strc.left(9);
			break;
		}

	}
	cityInf.close();

	return cityCode;
}
/*****************************************************************************
 *                        获取城市名称
 *  函 数 getCityNameFromCityId
 *  功    能：通过城市ID获取城市名称。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-08-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString HBWeather::getCityNameFromCityId(QString cityName)
{
	
	QString cityCode;
	QFile cityInf("./citycodeutf8.txt");

	if (!cityInf.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<< cityInf.errorString();
	}

	while (!cityInf.atEnd())
	{
		QString strc = cityInf.readLine().trimmed();
		QString name  = strc.mid(10).toLocal8Bit();

		if (name == cityName)
		{
			cityCode = strc.left(9);
			break;
		}

	}
	cityInf.close();

	return cityCode;
}
/*****************************************************************************
 *                        获取城市气象图片
 *  函 数 getWeatherPic
 *  功    能：显示相应的气象图片。
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：guodj
 *  创建时间：2016-06-2
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
QString HBWeather::getWeatherPic(QString weather)
{
	QString picID="";
	if(weather == QString::fromLocal8Bit("晴"))
	{
		picID = ":/image/ui_weather_0.png";
	}

	else if(weather == QString::fromLocal8Bit("多云"))
	{
		picID = ":/image/ui_weather_1.png" ;
	}

	else if(weather == QString::fromLocal8Bit("阴"))
	{
		picID = ":/image/ui_weather_2.png" ;
	}

	else if(weather == QString::fromLocal8Bit("阵雨"))
	{
		picID = ":/image/ui_weather_3.png" ;
	}

	else if(weather == QString::fromLocal8Bit("雷阵雨"))
	{
		picID = ":/image/ui_weather_4.png" ;
	}
	else if(weather == QString::fromLocal8Bit("阵雨伴有冰雹"))
	{
		picID = ":/image/ui_weather_5.png" ;
	}
	else if(weather == QString::fromLocal8Bit("雨加雪"))
	{
		picID = ":/image/ui_weather_6.png" ;
	}
	else if(weather == QString::fromLocal8Bit("小雨"))
	{
		// 小雨
		picID = ":/image/ui_weather_7.png" ;
	}
	else if(weather == QString::fromLocal8Bit("中雨") || weather == QString::fromLocal8Bit("小到中雨"))
	{
		// 中雨
		picID = ":/image/ui_weather_8.png" ;
	}
	else if(weather == QString::fromLocal8Bit("大雨") || weather == QString::fromLocal8Bit("大雨到暴雨") || weather == QString::fromLocal8Bit("暴雨") || weather == QString::fromLocal8Bit("暴雨到大暴雨") || weather == QString::fromLocal8Bit("大暴雨") || weather == QString::fromLocal8Bit("大暴雨到特大暴雨") || weather == QString::fromLocal8Bit("特大暴雨") || weather == QString::fromLocal8Bit("中雨到大雨"))
	{
		// 大雨 暴雨 大暴雨 特大暴雨
		picID = ":/image/ui_weather_9-10-11-12.png" ;
	}
	else if(weather == QString::fromLocal8Bit("阵雪"))
	{
		// 阵雪
		picID = ":/image/ui_weather_13.png" ;
	}
	else if(weather == QString::fromLocal8Bit("小雪"))
	{
		// 小雪
		picID = ":/image/ui_weather_14.png" ;
	}

	else if(weather == QString::fromLocal8Bit("中雪") || weather == QString::fromLocal8Bit("中到大雪"))
	{
		// 中雪
		picID = ":/image/ui_weather_15.png" ;
	}
	else if(weather == QString::fromLocal8Bit("大雪") || weather == QString::fromLocal8Bit("大到暴雪"))
	{
		// 大雪
		picID = ":/image/ui_weather_16.png" ;
	}
	else if(weather == QString::fromLocal8Bit("暴雪") || weather == QString::fromLocal8Bit("大暴雪"))
	{
		// 暴雪
		picID = ":/image/ui_weather_17.png" ;
	}
	else if(weather == QString::fromLocal8Bit("雾"))
	{
		// 雾
		picID = ":/image/ui_weather_18.png" ;
	}
	else if(weather == QString::fromLocal8Bit("冻雨"))
	{
		// 冻雨
		picID = ":/image/ui_weather_19.png" ;
	}
	else if(weather == QString::fromLocal8Bit("沙尘暴"))
	{
		// 沙尘暴
		picID = ":/image/ui_weather_20.png" ;
	}
	else if(weather == QString::fromLocal8Bit("浮尘"))
	{
		// 浮尘
		picID = ":/image/ui_weather_29.png" ;
	}
	else if(weather == QString::fromLocal8Bit("扬沙"))
	{
		// 扬沙
		picID = ":/image/ui_weather_30.png" ;
	}
	else if(weather == QString::fromLocal8Bit("强沙尘暴"))
	{
		// 强沙尘暴
		picID = ":/image/ui_weather_31.png" ;
	}
	else if(weather == QString::fromLocal8Bit("霾"))
	{
		// 霾
		picID = ":/image/ui_weather_53.png" ;
	}
	else
	{
		// 晴
		picID = ":/image/ui_weather_0.png" ;
	}
	return picID;
}

QString HBWeather::getCityName()
{
	
	IKXmlHelper sysCfg(GetXmlFilePath());
	std::string cityname = sysCfg.GetCityName();

	return QString::fromStdString(cityname);
}

void HBWeather::parseCityName(QNetworkReply *replay)
{
	
	QByteArray bytes =  replay->readAll();
	QString str1(bytes);
	QString temt = "cname";
	int cityIdPos = str1.indexOf(temt,0);

	// 获取城市信息成功
	if(cityIdPos > 0 )
	{
		QString cityName = str1.mid(cityIdPos+9);
		cityName.replace(QString::fromLocal8Bit("市\"};"),"");

		if(isCityExist(cityName))
		{
			m_isCityUrl = 0;
			m_cityNameFromNet = cityName;
		}

	}
	else
	{
		m_isCityUrl = 1;

	}

	emit finish(true);
}

weatherdata HBWeather::GetWeatherData()
{
    Exectime etm(__FUNCTION__);
	
	QString city ;

	if(m_isNameFromNet)
	{
		getCityNameFromIP();
		city = m_cityNameFromNet;
	}
	else
	{
		city = m_cityName;
	}

	QString cityID = getCityID(city);

	//打开链接，获取天气信息到成员 m_data中
	openWeatherUrl(cityID);
	m_data.city = city;
	m_data.pic = getWeatherPic(m_data.weather1);

	return m_data;
}
void HBWeather::SetCityName(bool isFromNet,QString cityName)
{
	
	m_data.weather1 = "";
	m_isNameFromNet = isFromNet;
	m_cityName = cityName;      ///自定义城市
}

bool HBWeather::isCityExist(QString &cityName)
{
	
	QString cityCode;

	QFile cityInf("./citycodeutf8.txt");

	if (!cityInf.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<< cityInf.errorString();
	}

	while (!cityInf.atEnd())
	{
		QString strc = cityInf.readLine().trimmed();
		QString name  = strc.mid(10).toLocal8Bit();
		QString name2 = name+QString::fromLocal8Bit("市");

		if (name == cityName || name2 == cityName )
		{
			cityName = name;
			cityCode = strc.left(9);
			cityInf.close();
			return true;
		}

	}

	cityInf.close();
	return false;
}

