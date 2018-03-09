/*****************************************************************************
 ** 文 件 名：interaction.cpp
 ** 主 要 类：Interaction
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：lizhl
 ** 创建时间：2014-01-08
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   人机交互，负责语音提示、界面提示等
 ** 主要模块说明: 人机交互类
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "interaction.h"
#include "../XML/xml/IKXmlApi.h"
#include "ikevent.h"
#include <math.h>   //为了在distanceCB中使用pow
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

#include <QDir>

#include <QDateTime>
//#include <QDebug>

#define DISPLAY_2_RECTANGLE_NO
//显示一个框时的框尺寸
//30,140,580,320  //30,30,580,420
const int g_1_rec_leftx = 30;
const int g_1_rec_lefty = 30;//30 //140
const int g_1_rec_width = 580;
const int g_1_rec_height = 420;//420 //320

//全局变量，红外测距值
//int g_infraredDistanceValue = 0;
int Interaction::s_infraredDistanceValue = 0; //红外测距值

std::mutex Interaction::s_interactionInstanceMutex;
Interaction* Interaction::s_pSingletonInteraction = NULL;
std::map<std::string, InteractionCallBack> Interaction::s_interactionCallBacks;//回调函数
//20150113
IKEventIns ikEventIns;
IKEvent *ikEvent;

//显示的是小图时的相关尺寸
const int g_constSmallRectWidth     =   160;    //矩形框宽度
const int g_constSmallRectHeight    =   120;    //矩形框高度
const int g_constSmallCrossSize     =   80;     //叉的尺寸
const int g_constSmallCircleDiameter =  80;     //圆的直径
const int g_constSmallStart_L_Y =  200;    //左框的左上角纵坐标 150
const int g_constSmallStart_R_Y = 200;    //右框的左上角纵坐标 150
const int g_constSmallStart_L_X = 100;    //左框的左上角横坐标
const int g_constSmallStart_R_X = 380;   //右框的左上角横坐标

const int num = 42;//扫描框在整幅图像中移动的次数//30
const int step = 420/num;//扫描框移动的步长

const QLineF linef[12];

const int lineLen = 50;
const int X0 = 30;
const int X1 = X0 + lineLen;
const int X2 = g_constIrisImgWidth/2 - lineLen/2;
const int X3 = g_constIrisImgWidth/2 + lineLen/2;
const int X5 = g_constIrisImgWidth - 30;
const int X4 = X5 - lineLen;

const int Y0 = 30;
const int Y1 = Y0 + lineLen;
const int Y2 = g_constIrisImgHeight/2 - lineLen/2;
const int Y3 = g_constIrisImgHeight/2 + lineLen/2;
const int Y5 = g_constIrisImgHeight - 30;
const int Y4 = Y5 - lineLen;

QLineF g_leftLineF[7];
QLineF g_rightLineF[7];
const int g_lineLen = 50;

const int g_lineX0 = 30;
const int g_lineX1 = g_lineX0 + g_lineLen;
const int g_lineX2 = g_constIrisImgWidth/2 - g_lineLen/2;
const int g_lineX3 = g_constIrisImgWidth/2;
const int g_lineX4 = g_constIrisImgWidth/2 + g_lineLen/2;
const int g_lineX6 = g_constIrisImgWidth - 30;
const int g_lineX5 = g_lineX6 - g_lineLen;

const int g_lineY0 = 30;
const int g_lineY1 = g_lineY0 + lineLen;
const int g_lineY2 = g_constIrisImgHeight/2 - g_lineLen/2;
const int g_lineY3 = g_constIrisImgHeight/2 + g_lineLen/2;
const int g_lineY5 = g_constIrisImgHeight - 30;
const int g_lineY4 = g_lineY5 - g_lineLen;

const int g_irisRadius = 27;//虹膜直径约为180-250，取中间值后除以四
const int g_pupilRadius = 9;
const int g_outerRadius = 55;
const int g_arrowLen = 13;
const int g_arrowSize = 3;
const int g_distanceBtArcAndArr = 5;

//默认的左眼追踪框中心坐标
const int g_defaultLeftCenterX = 180;
const int g_defaultLeftCenterY = 240;

//默认的右眼追踪框中心坐标
const int g_defaultRightCenterX = 460;
const int g_defaultRightCenterY = 240;

extern QString g_IdentName;



#ifdef TTS
static const pa_sample_spec ss =
{
	.format = PA_SAMPLE_S16LE,
	.rate = 16000,
	.channels = 1
};

pa_simple *s = NULL;
SVEnginePool* ep = NULL;
SVEngine* ei = NULL;
EngineOutput* eout = NULL;


void PlayTTS(EngineOutput* eout, void* data, int len)
{
	Exectime etm(__FUNCTION__);
	int error;
	int ret;
	ret = pa_simple_write(s, data, len*sizeof(short), &error);
}

void StartTTS(EngineOutput* eout)
{
	Exectime etm(__FUNCTION__);
	printf("Engine id: %d   frame:%d  sample:%d\n",eout->engine_id,eout->total_frame,eout->total_nsample); // 通过engine_id, 您可以知道是哪一路引擎正在回调该函数
}

void FinishTTS(EngineOutput* eout)
{
	Exectime etm(__FUNCTION__);
	printf("Engine id: %d  Finish.\n",eout->engine_id);
}
#endif

/*****************************************************************************
 *                         播放人机交互语音提示，靠近、靠远等的槽函数
 *  函 数 名：SlotPlaySound
 *  功    能：播放人机交互语音提示，靠近、靠远等
 *  说    明：
 *  参    数：resultType：输入参数，注册、识别的结果类型
 *           irisPositionFlag：输入参数，位置信息标志
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-03-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::SlotPlaySound(InteractionResultType resultType, IrisPositionFlag irisPositionFlag, bool ifUpdatePosition)
{
	Exectime etm(__FUNCTION__);
	//    struct timeval tvSoundEnd;
	//    int durationTime = 0;//声音间隔

	//    if(_updateTvStart)
	//    {
	//        //更新声音播放开始时间
	//        gettimeofday(&_tvSoundStart,NULL);
	//        _updateTvStart = false;
	//    }

	//    //获取当前时间
	//    gettimeofday(&tvSoundEnd,NULL);
	//    durationTime = tvSoundEnd.tv_sec - _tvSoundStart.tv_sec;

	//    if( ((durationTime < g_constSoundInterval) && (ResultUnknown == resultType)) || AlgorithmIdle == resultType )
	//    {
	//        //当前时间与声音播放开始时间间隔太小，并且没有注册或识别结果，则不播放声音
	//        return;
	//    }
	//    else
	//    {
	//        //达到间隔时间，播放声音，将更新声音播放时间标志设为true
	//        _updateTvStart = true;
	//    }

	//    int error;

	//20141218
	if(ResultUnknown == resultType && !ifUpdatePosition)
	{
		//如果没有识别或者注册结果，并且不需要更新位置提示，则返回
		return;
	}


	if(ResultUnknown == resultType)
	{
		//怀疑声音文件set后不会被清除，因此每次播放前清除一下播放队列
		if(_sound)
		{
#ifndef TTS
			_sound->clear();
			delete _sound;
			_sound = Phonon::createPlayer(Phonon::MusicCategory,
					Phonon::MediaSource("./sounds/chs/004thanks.wav"));
#else
			pa_simple_flush(s,&error);
#endif
		}

		switch(irisPositionFlag)
		{
			case Far:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/010closer.wav"));
#else
				ei->TTS_Process("keep closer", eout);
#endif
				break;

			case Near:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/018farther.wav"));
#else
				ei->TTS_Process("keep away", eout);
				//test
				//            int fd;
				//            if ((fd = open("output.wav", O_RDONLY)) < 0)
				//            {
				//                fprintf(stderr, __FILE__": open() failed: %s\n", strerror(errno));
				//            }
				//            if (dup2(fd, STDIN_FILENO) < 0)
				//            {
				//                fprintf(stderr, __FILE__": dup2() failed: %s\n", strerror(errno));
				//            }
				//            ::close(fd);
				//            for (;;)
				//            {
				//                uint8_t buf[BUFSIZE];
				//                ssize_t r;

				//                /* Read some data ... */
				//                if ((r = read(STDIN_FILENO, buf, sizeof(buf))) <= 0)
				//                {
				//                    if (r == 0) /* EOF */
				//                        break;
				//                    fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));

				//                }
				//                /* ... and play it */
				//                if (pa_simple_write(s, buf, (size_t) r, &error) < 0)
				//                {
				//                    fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));

				//                }
				//            }
#endif
				break;

			case Left:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/104right.wav"));
#else
				ei->TTS_Process("move right", eout);
#endif
				break;

			case Right:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/105left.wav"));
#else
				ei->TTS_Process("move left", eout);
#endif
				break;

			case Up:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/102down.wav"));
#else
				ei->TTS_Process("move down", eout);
#endif
				break;

			case Down:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/103up.wav"));
#else
				ei->TTS_Process("move up", eout);
#endif
				break;

			case OK:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/100keep.wav"));
#else
				ei->TTS_Process("keep", eout);
#endif
				break;

			default:
				break;
		}
	}
	else
	{
		//        if((_sound) && (false == _resultFlag))
		if((_sound) && (IdenKeepLastStatus != resultType))
		{
#ifndef TTS
			_sound->clear();
			//            delete _sound;
			//            _sound = Phonon::createPlayer(Phonon::MusicCategory,
			//                                          Phonon::MediaSource("./sounds/chs/004thanks.wav"));
#else
			pa_simple_flush(s,&error);
#endif
		}

		switch(resultType)
		{
			case EnrollSuccess:
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/003enrollsucc.wav"));
				break;
			case EnrollFailed:
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/006enrollstop.wav"));
				break;
			case IdenSuccess:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/002idensucc.wav"));
#else
				ei->TTS_Process("iden success", eout);
#endif
				break;
			case IdenFailed:
#ifndef TTS
				_sound->setCurrentSource(Phonon::MediaSource("./sounds/chs/005retry.wav"));
#else
				ei->TTS_Process("try again", eout);
#endif
				break;
			default:
				break;
		}

	}

	if((_sound) && (false == _resultFlag) && (IdenKeepLastStatus != resultType) && (Unknown != irisPositionFlag))
	{
#ifndef TTS
        _sound->play();
//        if(resultType == IdenSuccess)
//        {
//            QString cmd = "ekho -v \"Mandarin\" -s 0  $"+(g_IdentName+"识别通过&");
//            system(cmd.toStdString().c_str());
//        }
#else
		pa_simple_drain(s, &error);
#endif
	}

}


const unsigned char g_adCMD[4] =            {0x0C, 0x00, 0x00, 0x00};   //发送给串口的获取红外测距命令
const unsigned char g_redAllOnCMD[4] =      {0x07, 0x03, 0x00, 0x00};   //控制红外灯全亮
const unsigned char g_redLeftOnCMD[4] =     {0x07, 0x01, 0x00, 0x00};   //控制左红外灯亮
const unsigned char g_redRightOnCMD[4] =    {0x07, 0x02, 0x00, 0x00};   //控制右红外灯亮
const unsigned char g_redAllOffCMD[4] =     {0x07, 0x00, 0x00, 0x00};   //控制红外灯全灭
const unsigned char g_irisCamOffCMD[4] =    {0x05, 0x00, 0x00, 0x00};   //控制关闭虹膜摄像头
const unsigned char g_irisCamOnCMD[4] =     {0x05, 0x01, 0x00, 0x00};   //控制打开虹膜摄像头
const unsigned char g_shutDownCMD[4] =     {0xff, 0x00, 0x00, 0x00};   //控制关闭USB模块
const unsigned char g_startUpCMD[4] =     {0xfe, 0x00, 0x00, 0x00};   //控制打开USB模块

/*****************************************************************************
 *                         串口获取红外测距值的回调函数
 *  函 数 名：distanceCB
 *  功    能：串口获取红外测距值的回调函数
 *  说    明：
 *  参    数：data：输入参数，串口数据
 *			size：输入参数，串口长度
 *
 *  返 回 值：NULL
 *
 *  创 建 人：lizhl
 *  创建时间：2014-01-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void distanceCB(const unsigned char* data, const int size)
{
	Exectime etm(__FUNCTION__);
	//    qWarning("distance callback");
	int distanceValue = 0;
	int factor = 16;

	//liu-做修改  距离只有data[0] 有效
	if(NULL != data)
	{
		// for(int num = 0; num < size; num++)
		// {
		//TO DO
		//            distanceValue += data[0] * pow(factor, 0);
		//}
		//        g_infraredDistanceValue = distanceValue;

		//liu -改
		distanceValue += data[0];

		Interaction::s_infraredDistanceValue = distanceValue;
		//        std::cout<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<"  ";
		//        std::cout << "distanceValue = " << distanceValue << std::endl;
	}
}



/*****************************************************************************
 *                         获取Interaction实例
 *  函 数 名：GetInteractionInstance
 *  功    能：获取CAPIBase实例
 *  说    明：Interaction采用单件模式
 *  参    数：NULL
 *
 *  返 回 值：!NULL: Interaction实例指针
 *			NULL：获取失败
 *  创 建 人：lizhl
 *  创建时间：2014-02-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
Interaction* Interaction::GetInteractionInstance()
{
	Exectime etm(__FUNCTION__);
	// 双重检查，提高性能
	if (NULL == s_pSingletonInteraction)
	{
		std::lock_guard<std::mutex> lck (s_interactionInstanceMutex);

		if (NULL == s_pSingletonInteraction)
		{
			s_pSingletonInteraction = new Interaction();
		}
	}

	return s_pSingletonInteraction;
}


Interaction::Interaction()
{
	Exectime etm(__FUNCTION__);
	_resultFlag = false;//初始化时没有结果

#ifndef TTS
	_sound = Phonon::createPlayer(Phonon::MusicCategory,
			Phonon::MediaSource("./sounds/chs/004thanks.wav"));
#else
	int error;
	//创建一个pa_simple
	s = pa_simple_new(NULL, "test", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error);

	ep =  CreateEnginePool(2);
	ei = ep->getEngine();
	ei->debug = 0;

	eout = new EngineOutput();
	eout->tts_outputfile = NULL;//"output.wav";
	eout->tts_start_callback = NULL;//(void*)&StartTTS;
	eout->tts_process_callback = (void*)&PlayTTS;
	eout->tts_finish_callback = NULL;//(void*)&FinishTTS;
	eout->writebuffer_in_rawdata = 1;
	eout->userdata = NULL;
	ei->TTS_SetSamplingRate(16000);

	_playSoundThread = std::thread(PlaySoundThread,(void*)this);
#endif

	//获取串口测距
	_serialDistance = SerialMuxDemux::GetInstance();

	//设置回调，在distanceCB中取距离值
	if( false == _serialDistance->AddCallback(0x0c, distanceCB))
	{
		qWarning("serial add callback failed");
		LOG_ERROR("serial add callback failed");
	}
	else
	{
		qWarning("serial add callback success");
	}


	int funResult = _serialDistance->Init(GetSerialPortName());//ttys1//ttyUSB0
	if(0 == funResult)
	{
		qWarning("serial init success");
	}
	else
	{
		qWarning("serial init failed");
		LOG_ERROR("serial init failed");
		// TODO  打开串口失败就不要继续了
		return;
	}

	// 启动后先关闭一次,在开启,保证正常运转
	CloseUsb();
	StartUsb();


	struct timespec ts100ms;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//虹膜摄像头通电后，李言需要1s钟时间加载虹膜摄像头程序，在此时间内不要打开摄像头采图
	//设备上电后虹膜摄像头应该是通电的，此处应该可以不加
	_serialDistance->Send(g_irisCamOnCMD, 4);

	ts100ms.tv_sec = 1;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms
	nanosleep(&ts100ms, NULL);//防止串口丢失命令
	_irisCameraOnFlag = true;//初始化时虹膜摄像头处于开启状态
	//20140718
	//    _serialDistance->Send(g_irisCamOnCMD, 4);
	//    //struct timespec ts100ms;
	//    ts100ms.tv_sec = 0;
	//    ts100ms.tv_nsec = 100*1000*1000;      //100ms
	//    nanosleep(&ts100ms, NULL);//防止串口丢失命令
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	//20150113
	ifClose = false;
	ikEvent = ikEventIns.GetIKEvent();
	connect(ikEvent, SIGNAL(sigCloseInfrared()),
			this, SLOT(CloseInfrared()));

	_getDistanceThread = std::thread(GetDistanceThread, (void*)this);

	_serialDistance->Send(g_redAllOnCMD, 4);
	_infraredLEDOnFlag = true;//初始化时红外灯处于开启状态

	//    _serialDistance->Send(g_redAllOffCMD, 4);
	//    _infraredLEDOnFlag = false;//初始化时红外灯处于关闭状态

	gettimeofday(&_tvHasPerson,nullptr);
	gettimeofday(&_tvNoPerson,nullptr);

	_timerForInfraredLED = new QTimer();

	qRegisterMetaType<InteractionResultType>("InteractionResultType");
	//关联定时器超时信号槽函数
	connect(_timerForInfraredLED,SIGNAL(timeout()),this,SLOT(TimerForInfraredLEDTimeOut()));

    //@for test wake
    _timerTestWake = new QTimer();
    connect(_timerTestWake,SIGNAL(timeout()),this,SLOT(TimerForTest()));
    _timerTestWake->start(60*1000);


	s_interactionCallBacks.clear();//清除回调函数
	_hasPerson = HasPerson;//初始化时认为有人
	_screenPressed = true;
	_updateTvStart = true;

	_filterImage.load(":/image/1.png");

	_linePos = 0;
	_variableRadius = g_outerRadius;


	g_leftLineF[0].setLine(g_lineX0, g_lineY0, g_lineX1, g_lineY0);
	g_leftLineF[1].setLine(g_lineX2, g_lineY0, g_lineX3, g_lineY0);
	g_leftLineF[2].setLine(g_lineX0, g_lineY0, g_lineX0, g_lineY1);
	g_leftLineF[3].setLine(g_lineX0, g_lineY2, g_lineX0, g_lineY3);
	g_leftLineF[4].setLine(g_lineX0, g_lineY4, g_lineX0, g_lineY5);
	g_leftLineF[5].setLine(g_lineX0, g_lineY5, g_lineX1, g_lineY5);
	g_leftLineF[6].setLine(g_lineX2, g_lineY5, g_lineX3, g_lineY5);

	g_rightLineF[0].setLine(g_lineX3, g_lineY0, g_lineX4, g_lineY0);
	g_rightLineF[1].setLine(g_lineX5, g_lineY0, g_lineX6, g_lineY0);
	g_rightLineF[2].setLine(g_lineX6, g_lineY0, g_lineX6, g_lineY1);
	g_rightLineF[3].setLine(g_lineX6, g_lineY2, g_lineX6, g_lineY3);
	g_rightLineF[4].setLine(g_lineX6, g_lineY4, g_lineX6, g_lineY5);
	g_rightLineF[5].setLine(g_lineX3, g_lineY5, g_lineX4, g_lineY5);
	g_rightLineF[6].setLine(g_lineX5, g_lineY5, g_lineX6, g_lineY5);

	//20141218
	noticeCount = 1;


}

Interaction::~Interaction()
{
	Exectime etm(__FUNCTION__);
	_serialDistance->Send(g_redAllOffCMD, 4);
	_infraredLEDOnFlag = false;

	struct timespec ts100ms;
	ts100ms.tv_sec = 0;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms
	nanosleep(&ts100ms, NULL);//防止串口丢失命令

	_serialDistance->Send(g_irisCamOffCMD, 4);
	_irisCameraOnFlag = false;

	_getDistanceThread.join();

#ifdef TTS
	if (s)
	{
		pa_simple_free(s);
	}

	ep->releaseEngine(ei);
	DeleteEnginePool(ep);
#endif

}

// 关闭USB模块
void Interaction::CloseUsb()
{
	Exectime etm(__FUNCTION__);
	struct timespec ts100ms;
	_serialDistance->Send(g_shutDownCMD, 4);
	ts100ms.tv_sec = 1;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms
	nanosleep(&ts100ms, NULL);//防止串口丢失命令
}

// 打开USB模块
void Interaction::StartUsb()
{
	Exectime etm(__FUNCTION__);
	struct timespec ts100ms;
	_serialDistance->Send(g_startUpCMD, 4);
	ts100ms.tv_sec = 2;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms
	nanosleep(&ts100ms, NULL);//防止串口丢失命令
}

//20150113
void Interaction::CloseInfrared()
{
	Exectime etm(__FUNCTION__);
	ifClose = true;
}

/*****************************************************************************
 *                         执行人机交互，包括语音及界面绘图
 *  函 数 名：PlayInteraction
 *  功    能：执行人机交互，包括语音及界面绘图
 *  说    明：如果还没有结果，一定要将传入的resultType设置为ResultUnknown
 *  参    数：resultType：输入参数，注册、识别的结果类型
 *           lrIrisClearFlag：输入参数，左右眼是否清晰标志
 *           irisPositionFlag：输入参数，位置信息标志
 *           paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
 *           如果不是，使用converToFormat函数进行转换
 *           建议使用QImage::Format_ARGB32_Premultiplied
 *
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-01-27
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::PlayInteraction(IN InteractionResultType resultType, IN LRIrisClearFlag lrIrisClearFlag, IN IrisPositionFlag irisPositionFlag, IN bool ifUpdatePosition, OUT QImage &paintImg,
		IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo)
{
	Exectime etm(__FUNCTION__);
	//    PlayInteractionSound(resultType, irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
	//    playSoundEvent.RaiseSoundEvent(resultType,irisPositionFlag);
	//emit SigPlaySound(resultType,irisPositionFlag);

	_resultType = resultType;
	_lrIrisClearFlag = lrIrisClearFlag;
	_irisPositionFlag = irisPositionFlag;
	_leftTraceInfo = leftTraceInfo;
	_rightTraceInfo = rightTraceInfo;

	//    SlotPlaySound(resultType,irisPositionFlag);
	//20141218
	SlotPlaySound(resultType,irisPositionFlag,ifUpdatePosition);

	//20141208
	return;

	if(paintImg.isNull())
	{
		return;
	}
	//    _paintImg = &paintImg;

	//    if(_paintImg->isNull())
	//    {
	//        return;
	//    }

	switch(resultType)
	{
		case AlgorithmIdle:
		case ResultUnknown:
		case EnrollPerformig:
		case IdenPerforming:
			if(LAndRImgBlur == _lrIrisClearFlag)
			{
				_leftColor = Qt::red;
				_rightColor = Qt::red;
			}
			else if(LImgClear == _lrIrisClearFlag)
			{
				_leftColor = Qt::green;
				_rightColor = Qt::red;
			}
			else if(RImgClear == _lrIrisClearFlag)
			{
				_rightColor = Qt::green;
				_leftColor = Qt::red;
			}
			else if(LAndRImgClear == _lrIrisClearFlag)
			{
				_leftColor = Qt::green;
				_rightColor = Qt::green;
			}

			AddFilterImg(paintImg);
			//        DisplayClearBlur();   //显示图像是否清晰提示框
			DrawBox(paintImg);
			DrawScanLine(paintImg);
			DrawTrace(paintImg);
			break;
		case EnrollSuccess:
			break;
		case EnrollFailed:
			break;
		case IdenSuccess:
			_leftColor = Qt::green;
			_rightColor = Qt::green;
			ResetDisplay();
			DrawBox(paintImg);
			AddFilterImg(paintImg);
			AddResultImg(paintImg);
			break;
		case IdenFailed:
			_leftColor = Qt::red;
			_rightColor = Qt::red;
			ResetDisplay();
			DrawBox(paintImg);
			AddFilterImg(paintImg);
			AddResultImg(paintImg);
			break;
		case IdenKeepLastStatus:
			//        ProcessResult(resultType, paintImg);
			ResetDisplay();
			DrawBox(paintImg);
			AddFilterImg(paintImg);
			AddResultImg(paintImg);
			break;
		default:
			break;
	}




	return;
}

/*****************************************************************************
 *                         清除结果
 *  函 数 名：ClearResult
 *  功    能：清除结果
 *  说    明：
 *  参    数：NULL
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-01-27
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::ClearResult()
{
	Exectime etm(__FUNCTION__);
	_resultFlag = false;
}

/*****************************************************************************
 *                         显示图像是否清晰提示框
 *  函 数 名：DisplayClearBlur
 *  功    能：显示图像是否清晰提示框
 *  说    明：
 *  参    数：paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
 *           如果不是，使用converToFormat函数进行转换
 *           建议使用QImage::Format_ARGB32_Premultiplied
 *           lrIrisClearFlag：输入参数，左右眼是否清晰标志
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-01-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::DisplayClearBlur(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);

	//坐标申明，x轴对应图像宽度所在，y轴对应图像高度所在
	int xLeft, yLeft, xRight, yRight;
	if( (g_constIrisImgWidth == 2048) && (g_constIrisImgHeight == 704) )
	{
		//左眼图像起始坐标
		xLeft = 24;
		yLeft = 112;

		//右眼图像起始坐标
		xRight = 1384;
		yRight = 112;
	}
	else
	{
		//左眼图像起始坐标
		xLeft = g_constSmallStart_L_X;
		yLeft = g_constSmallStart_L_Y;

		//右眼图像起始坐标
		xRight = g_constSmallStart_R_X;
		yRight = g_constSmallStart_R_Y;
	}

	QPainter painterRectangle(&paintImg);

	painterRectangle.setRenderHint(QPainter::Antialiasing,true);
	if(LAndRImgBlur == _lrIrisClearFlag)
	{
		painterRectangle.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
	}
	else
	{
		painterRectangle.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));
	}

	if( (g_constIrisImgWidth == 2048) && (g_constIrisImgHeight == 704) )
	{
		painterRectangle.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
		painterRectangle.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112
	}
	else
	{
#ifdef DISPLAY_2_RECTANGLE
		painterRectangle.drawRect(xLeft,yLeft,g_constSmallRectWidth,g_constSmallRectHeight);//24,112
		painterRectangle.drawRect(xRight,yRight,g_constSmallRectWidth,g_constSmallRectHeight);//1384,112
#else
		//        painterRectangle.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);//30,140,580,320  //30,30,580,420
#endif
	}

	painterRectangle.drawLines(linef,12);
}

/*****************************************************************************
 *                         在图像上增加一层图像，将四周模糊化
 *  函 数 名：AddFilterImg
 *  功    能：在图像上增加一层图像，将四周模糊化
 *  说    明：
 *  参    数：paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
 *           如果不是，使用converToFormat函数进行转换
 *           建议使用QImage::Format_ARGB32_Premultiplied
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-08-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::AddFilterImg(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterRectangle(&paintImg);
	//20140819测试
	painterRectangle.setOpacity(0.5);

	painterRectangle.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painterRectangle.fillRect(paintImg.rect(), Qt::transparent);

	painterRectangle.drawImage(0, 0, _filterImage);

	//    bool endValue = painterRectangle.end();
	//    std::cout << "Painter end value is " << endValue << std::endl;
}

void Interaction::ResetDisplay()
{
	Exectime etm(__FUNCTION__);
	_linePos = 0;
	_variableRadius = g_outerRadius;
}

void Interaction::DrawBox(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterBox(&paintImg);
	painterBox.setPen(QPen(_leftColor, 5, Qt::SolidLine, Qt::RoundCap));
	painterBox.drawLines(g_leftLineF, 7);
	painterBox.setPen(QPen(_rightColor, 5, Qt::SolidLine, Qt::RoundCap));
	painterBox.drawLines(g_rightLineF, 7);
}

void Interaction::DrawScanLine(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterLine(&paintImg);
	painterLine.setOpacity(0.15);

	if(_linePos > num)
	{
		_linePos = 0;
	}

	int xLeft = 30;
	int yLeft = _linePos*step + 30;
	int xRight = 610;
	int yRight = _linePos*step + 30;

	QColor col(75,229,0);
	//    QColor col(Qt::red);
	QPen pen(col);
	pen.setWidth(2);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);

	painterLine.setRenderHint(QPainter::Antialiasing);
	//线性渐变
	QLinearGradient linearGradient(xLeft,yLeft,xRight,yRight);
	QRadialGradient radialGradient(320,yRight,800);
	//    linearGradient.setInterpolationMode();

	linearGradient.setColorAt(0.2, Qt::white);
	linearGradient.setColorAt(0.6, Qt::green);
	linearGradient.setColorAt(1.0, Qt::black);
	linearGradient.setSpread(QGradient::RepeatSpread);//渐变的铺展效果        PadSpread,    ReflectSpread,    RepeatSpread

	radialGradient.setColorAt(0.1, Qt::green);
	radialGradient.setColorAt(0.6, Qt::white);
	radialGradient.setColorAt(1.0, Qt::black);
	radialGradient.setSpread(QGradient::RepeatSpread);//渐变的铺展效果        PadSpread,    ReflectSpread,    RepeatSpread

	//    radialGradient.setColorAt(0.1, Qt::green);
	//    radialGradient.setColorAt(0.2, Qt::white);
	//    radialGradient.setColorAt(1.0, Qt::black);
	//    radialGradient.setSpread(QGradient::RepeatSpread);//渐变的铺展效果        PadSpread,    ReflectSpread,    RepeatSpread


	if(LAndRImgBlur == _lrIrisClearFlag)
	{
		//        painterLine.setPen(QPen(Qt::/*red*//*cyan*/yellow, 2, Qt::DashLine, Qt::RoundCap));
		painterLine.setPen(pen);
	}
	else
	{
		painterLine.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::RoundCap));
	}

	//    painterLine.setPen(QPen(QBrush(linearGradient), 5));

	//    painterLine.setPen(QPen(Qt::cyan, 2, Qt::DashDotDotLine, Qt::RoundCap));

	//    painterLine.setBrush(QBrush(linearGradient));
	painterLine.setBrush(QBrush(radialGradient));

	//    painterLine.setBrush(QBrush(Qt::CrossPattern));


	//    painterLine.drawLine(xLeft,yLeft,xRight,yRight);

	//    for(int n = _linePos; n >= 0; n--)
	//    {
	//        painterLine.drawLine(xLeft,n*step + 30,xRight,n*step + 30);
	//    }

	painterLine.drawRect(30,30,580,_linePos*step);

	//    painterLine.drawRect(30,30,580,420);

	_linePos++;
}

void Interaction::DrawTrace(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	int stepRadius = 1;
	ArrowDirection direction;

	if(Unknown == _irisPositionFlag)
	{
		_leftTraceInfo.centC = g_defaultLeftCenterX;
		_leftTraceInfo.centR = g_defaultLeftCenterY;
		_leftTraceInfo.irisRadius = g_irisRadius;
		_leftTraceInfo.pupilRadius = g_pupilRadius;

		_rightTraceInfo.centC = g_defaultRightCenterX;
		_rightTraceInfo.centR = g_defaultRightCenterY;
		_rightTraceInfo.irisRadius = g_irisRadius;
		_rightTraceInfo.pupilRadius = g_pupilRadius;

		_variableRadius -= stepRadius;
		direction = Point2Center;
	}
	else if(Far == _irisPositionFlag)
	{
		_variableRadius -= stepRadius;
		direction = Point2Center;
	}
	else if(OK == _irisPositionFlag)
	{
		_variableRadius = g_outerRadius;
		direction = NOArrow;
	}
	else if(Near == _irisPositionFlag)
	{
		_variableRadius += stepRadius;
		direction = Point2Outer;
	}

	if(0 == _leftTraceInfo.irisRadius + _leftTraceInfo.pupilRadius)
	{
		_leftTraceInfo.irisRadius = g_irisRadius;
		_leftTraceInfo.pupilRadius = g_pupilRadius;
	}

	if(0 == _rightTraceInfo.irisRadius + _rightTraceInfo.pupilRadius)
	{
		_rightTraceInfo.irisRadius = g_irisRadius;
		_rightTraceInfo.pupilRadius = g_pupilRadius;
	}

	if(_leftTraceInfo.centC + _leftTraceInfo.centR > 0)
	{
		DrawCircle(_leftTraceInfo.centC, _leftTraceInfo.centR, _leftTraceInfo.pupilRadius, _leftColor, paintImg);
		DrawCircle(_leftTraceInfo.centC, _leftTraceInfo.centR, _leftTraceInfo.irisRadius, _leftColor, paintImg);
		DrawArrowsAndArcs(_leftTraceInfo.centC, _leftTraceInfo.centR, _variableRadius, direction, _leftColor, paintImg);
	}

	if(_rightTraceInfo.centC + _rightTraceInfo.centR > 0)
	{
		DrawCircle(_rightTraceInfo.centC, _rightTraceInfo.centR, _rightTraceInfo.pupilRadius, _rightColor, paintImg);
		DrawCircle(_rightTraceInfo.centC, _rightTraceInfo.centR, _rightTraceInfo.irisRadius, _rightColor, paintImg);
		DrawArrowsAndArcs(_rightTraceInfo.centC, _rightTraceInfo.centR, _variableRadius, direction, _rightColor, paintImg);
	}

	if(_variableRadius > g_outerRadius + 10)
	{
		_variableRadius = g_outerRadius - 10;
	}
	else if(_variableRadius < g_outerRadius - 10)
	{
		_variableRadius = g_outerRadius + 10;
	}

}

void Interaction::DrawCircle(int centerX, int centerY, int radius, Qt::GlobalColor color, QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterCircle(&paintImg);
	painterCircle.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap));
	painterCircle.drawEllipse(QPoint(centerX, centerY), radius, radius);
}

void Interaction::DrawArrowsAndArcs(int centerX, int centerY, int radius, ArrowDirection direction, Qt::GlobalColor color, QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	DrawArcs(centerX, centerY, radius, -15*16, 30*16, 90*16, color, paintImg);
	if(Point2Center == direction)
	{
		DrawArrow(centerX - radius - g_arrowLen - g_distanceBtArcAndArr, centerY, centerX - radius - g_distanceBtArcAndArr, centerY, color, paintImg);

		DrawArrow(centerX + radius + g_arrowLen + g_distanceBtArcAndArr, centerY, centerX + radius + g_distanceBtArcAndArr, centerY, color, paintImg);

		DrawArrow(centerX, centerY - radius - g_arrowLen - g_distanceBtArcAndArr, centerX, centerY - radius - g_distanceBtArcAndArr, color, paintImg);

		DrawArrow(centerX, centerY + radius + g_arrowLen + g_distanceBtArcAndArr, centerX, centerY + radius + g_distanceBtArcAndArr, color, paintImg);
	}
	else if(Point2Outer == direction)
	{
		DrawArrow(centerX - radius + g_arrowLen + g_distanceBtArcAndArr, centerY, centerX - radius + g_distanceBtArcAndArr, centerY, color, paintImg);

		DrawArrow(centerX + radius - g_arrowLen - g_distanceBtArcAndArr, centerY, centerX + radius - g_distanceBtArcAndArr, centerY, color, paintImg);

		DrawArrow(centerX, centerY - radius + g_arrowLen + g_distanceBtArcAndArr, centerX, centerY - radius + g_distanceBtArcAndArr, color, paintImg);

		DrawArrow(centerX, centerY + radius - g_arrowLen - g_distanceBtArcAndArr, centerX, centerY + radius - g_distanceBtArcAndArr, color, paintImg);
	}

}

void Interaction::DrawArcs(int centerX, int centerY, int radius,  int startAngle, int spanAngle, int stepAngle, Qt::GlobalColor color, QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterArc(&paintImg);

	//    painterArc.translate(centerX, centerY);

	painterArc.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap));

	QRectF rectangle(centerX - radius, centerY - radius, radius*2, radius*2);
	//    int startAngle = 30 * 16;
	//    int spanAngle = 120 * 16;
	//    painterArc.drawArc(rectangle, startAngle, spanAngle);

	//    static int startAngle = 0;
	//    int spanAngle = 12 * 16;
	for(int beginAngle = startAngle; beginAngle < 360*16 + startAngle; beginAngle += stepAngle)
	{
		painterArc.drawArc(rectangle, beginAngle, spanAngle);
	}

	//    startAngle += 30*16;
	//    if(startAngle >= 360*16)
	//    {
	//        startAngle = 0;
	//    }

	//    painterArc.drawEllipse(centerX,centerY,radius,radius);
	//    static qreal ang = 0;
	//    ang += 6.0;
	//    painterArc.save();
	//    painterArc.rotate(ang);


	//    painterArc.drawLine(centerX,centerY,centerX+radius,centerY+radius);

	//    painterArc.restore();
}

void Interaction::DrawArrow(int beginPointX, int beginPointY, int endPointX, int endPointY, Qt::GlobalColor color, QImage &paintImg)
{    
	Exectime etm(__FUNCTION__);
	QPainter painterArrow(&paintImg);

	painterArrow.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap));

	painterArrow.drawLine(beginPointX, beginPointY, endPointX, endPointY);

	int arrowX1, arrowY1, arrowX2, arrowY2;

	if(endPointX > beginPointX)
	{
		arrowX1 = arrowX2 = endPointX - g_arrowSize;
		arrowY1 = endPointY - g_arrowSize;
		arrowY2 = endPointY + g_arrowSize;
	}
	else if(beginPointX > endPointX)
	{
		arrowX1 = arrowX2 = endPointX + g_arrowSize;
		arrowY1 = endPointY - g_arrowSize;
		arrowY2 = endPointY + g_arrowSize;
	}
	else if(endPointY > beginPointY)
	{
		arrowY1 = arrowY2 = endPointY - g_arrowSize;
		arrowX1 = endPointX - g_arrowSize;
		arrowX2 = endPointX + g_arrowSize;
	}
	else if(beginPointY > endPointY)
	{
		arrowY1 = arrowY2 = endPointY + g_arrowSize;
		arrowX1 = endPointX - g_arrowSize;
		arrowX2 = endPointX + g_arrowSize;
	}

	painterArrow.drawLine(endPointX, endPointY, arrowX1, arrowY1);
	painterArrow.drawLine(endPointX, endPointY, arrowX2, arrowY2);
	painterArrow.drawLine(arrowX2, arrowY2, arrowX1, arrowY1);

}

/*****************************************************************************
 *                         处理注册、识别结果的输出
 *  函 数 名：ProcessResult
 *  功    能：处理注册、识别结果的输出
 *  说    明：
 *  参    数：resultType：输入参数，注册、识别的结果类型
 *           paintImg：输出参数，待绘制的图像，其格式为彩色，否则报错；
 *           如果不是，使用converToFormat函数进行转换
 *           建议使用QImage::Format_ARGB32_Premultiplied
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-01-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::ProcessResult(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	static InteractionResultType s_lastInteractionResultType = ResultUnknown;

	if(IdenKeepLastStatus == _resultType)
	{
		_resultType = s_lastInteractionResultType;
	}
	s_lastInteractionResultType = _resultType;
	//    QImage imgCover(g_constIrisImgWidth, g_constIrisImgHeight, QImage::Format_ARGB32_Premultiplied);
	//坐标申明，x轴对应图像宽度所在，y轴对应图像高度所在
	int xLeft, yLeft, xRight, yRight, crossSize, diameter;
	int crossXLeftOrigin, crossXLeftEnd, crossYLeftOrigin, crossYLeftEnd;
	int crossXRightOrigin, crossXRightEnd, crossYRightOrigin, crossYRightEnd;
	int roundXLeft, roundYLeft, roundXRight, roundYRight;

	if( (g_constIrisImgWidth == 2048) && (g_constIrisImgHeight == 704) )
	{
		//左眼图像起始坐标
		xLeft = 24;
		yLeft = 112;

		//右眼图像起始坐标
		xRight = 1384;
		yRight = 112;

		//叉的坐标
		crossSize = 200;//画叉的尺寸

		//圆的相关信息
		//圆的直径
		diameter = 200;

		//左叉的起始坐标
		crossXLeftOrigin = xLeft + (g_constIKImgWidth-crossSize)/2;
		crossXLeftEnd = xLeft + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
		crossYLeftOrigin = yLeft + (g_constIKImgHeight-crossSize)/2;
		crossYLeftEnd = yLeft + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;

		//右叉的起止坐标
		crossXRightOrigin = xRight + (g_constIKImgWidth-crossSize)/2;
		crossXRightEnd = xRight + g_constIKImgWidth - (g_constIKImgWidth-crossSize)/2;
		crossYRightOrigin = yRight + (g_constIKImgHeight-crossSize)/2;
		crossYRightEnd = yRight + g_constIKImgHeight - (g_constIKImgHeight-crossSize)/2;

		//左圆的起始坐标
		roundXLeft = xLeft + (g_constIKImgWidth-diameter)/2;
		roundYLeft = yLeft + (g_constIKImgHeight-diameter)/2;

		//右圆的起始坐标
		roundXRight = xRight + (g_constIKImgWidth-diameter)/2;
		roundYRight = yRight + (g_constIKImgHeight-diameter)/2;
	}
	else
	{
		//框的大小160*120

		//左眼图像起始坐标
		xLeft = g_constSmallStart_L_X;
		yLeft = g_constSmallStart_L_Y;

		//右眼图像起始坐标
		xRight = g_constSmallStart_R_X;
		yRight = g_constSmallStart_R_Y;

		//叉的坐标
		crossSize = g_constSmallCrossSize;//画叉的尺寸

		//圆的相关信息
		//圆的直径
		diameter = g_constSmallCircleDiameter;

		//左叉的起始坐标
		crossXLeftOrigin = xLeft + (g_constSmallRectWidth-crossSize)/2;
		crossXLeftEnd = xLeft + g_constSmallRectWidth - (g_constSmallRectWidth-crossSize)/2;
		crossYLeftOrigin = yLeft + (g_constSmallRectHeight-crossSize)/2;
		crossYLeftEnd = yLeft + g_constSmallRectHeight - (g_constSmallRectHeight-crossSize)/2;

		//右叉的起止坐标
		crossXRightOrigin = xRight + (g_constSmallRectWidth-crossSize)/2;
		crossXRightEnd = xRight + g_constSmallRectWidth - (g_constSmallRectWidth-crossSize)/2;
		crossYRightOrigin = yRight + (g_constSmallRectHeight-crossSize)/2;
		crossYRightEnd = yRight + g_constSmallRectHeight - (g_constSmallRectHeight-crossSize)/2;

		//左圆的起始坐标
		roundXLeft = xLeft + (g_constSmallRectWidth-diameter)/2;
		roundYLeft = yLeft + (g_constSmallRectHeight-diameter)/2;

		//右圆的起始坐标
		roundXRight = xRight + (g_constSmallRectWidth-diameter)/2;
		roundYRight = yRight + (g_constSmallRectHeight-diameter)/2;
	}

	QPainter painterResult(&paintImg);

	painterResult.setRenderHint(QPainter::Antialiasing,true);

	switch(_resultType)
	{
		case EnrollSuccess:
			break;

		case EnrollFailed:
			break;

		case IdenSuccess:
			painterResult.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));


			if( (g_constIrisImgWidth == 2048) && (g_constIrisImgHeight == 704) )
			{
				painterResult.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
				painterResult.drawEllipse(roundXLeft,roundYLeft,diameter,diameter);

				painterResult.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112
				painterResult.drawEllipse(roundXRight,roundYRight,diameter,diameter);
			}
			else
			{
#ifdef  DISPLAY_2_RECTANGLE
				painterResult.drawRect(xLeft,yLeft,g_constSmallRectWidth,g_constSmallRectHeight);//24,112
#else
				//            painterResult.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);
#endif
				painterResult.drawEllipse(roundXLeft,roundYLeft,diameter,diameter);

#ifdef  DISPLAY_2_RECTANGLE
				painterResult.drawRect(xRight,yRight,g_constSmallRectWidth,g_constSmallRectHeight);//1384,112
#else
				//            painterResult.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);
#endif
				painterResult.drawEllipse(roundXRight,roundYRight,diameter,diameter);
			}

			break;

		case IdenFailed:
			painterResult.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));

			if( (g_constIrisImgWidth == 2048) && (g_constIrisImgHeight == 704) )
			{
				painterResult.drawRect(xLeft,yLeft,g_constIKImgWidth,g_constIKImgHeight);//24,112
				painterResult.drawLine(crossXLeftOrigin,crossYLeftOrigin,crossXLeftEnd,crossYLeftEnd);
				painterResult.drawLine(crossXLeftOrigin,crossYLeftEnd,crossXLeftEnd,crossYLeftOrigin);

				painterResult.drawRect(xRight,yRight,g_constIKImgWidth,g_constIKImgHeight);//1384,112
				painterResult.drawLine(crossXRightOrigin,crossYRightOrigin,crossXRightEnd,crossYRightEnd);
				painterResult.drawLine(crossXRightOrigin,crossYRightEnd,crossXRightEnd,crossYRightOrigin);
			}
			else
			{
#ifdef  DISPLAY_2_RECTANGLE
				painterResult.drawRect(xLeft,yLeft,g_constSmallRectWidth,g_constSmallRectHeight);//24,112
#else
				//            painterResult.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);
#endif
				painterResult.drawLine(crossXLeftOrigin,crossYLeftOrigin,crossXLeftEnd,crossYLeftEnd);
				painterResult.drawLine(crossXLeftOrigin,crossYLeftEnd,crossXLeftEnd,crossYLeftOrigin);

#ifdef  DISPLAY_2_RECTANGLE
				painterResult.drawRect(xRight,yRight,g_constSmallRectWidth,g_constSmallRectHeight);//1384,112
#else
				//            painterResult.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);
#endif
				painterResult.drawLine(crossXRightOrigin,crossYRightOrigin,crossXRightEnd,crossYRightEnd);
				painterResult.drawLine(crossXRightOrigin,crossYRightEnd,crossXRightEnd,crossYRightOrigin);
			}

			break;

		default:
			break;
	}
	painterResult.drawLines(linef,12);
}

void Interaction::DisplayRectangle(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	QPainter painterRec(&paintImg);
	switch(_resultType)
	{
		case EnrollSuccess:
			break;

		case EnrollFailed:
			break;

		case IdenSuccess:
			painterRec.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap));
			break;

		case IdenFailed:
			painterRec.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
			break;

		default:
			break;
	}
	//    painterRec.drawRect(g_1_rec_leftx,g_1_rec_lefty,g_1_rec_width,g_1_rec_height);//30,140,580,320  //30,30,580,420
	painterRec.drawLines(linef,12);
}

void Interaction::AddResultImg(QImage &paintImg)
{
	Exectime etm(__FUNCTION__);
	static InteractionResultType s_lastInteractionResultType = ResultUnknown;

	if(IdenKeepLastStatus == _resultType)
	{
		_resultType = s_lastInteractionResultType;
	}
	s_lastInteractionResultType = _resultType;
	//    DisplayRectangle();

	QPainter painterResult(&paintImg);
	painterResult.setOpacity(0.95);

	painterResult.setRenderHint(QPainter::Antialiasing,true);
	QImage sourceImage;
	switch(_resultType)
	{
		case EnrollSuccess:
			break;

		case EnrollFailed:
			break;

		case IdenSuccess:
			sourceImage.load(":/image/yes_.png");

			painterResult.setCompositionMode(QPainter::CompositionMode_SourceOver);
			painterResult.fillRect(paintImg.rect(), Qt::transparent);

			painterResult.drawImage(256, 176, sourceImage);
			painterResult.end();
			break;

		case IdenFailed:
			sourceImage.load(":/image/error_.png");

			painterResult.setCompositionMode(QPainter::CompositionMode_SourceOver);
			painterResult.fillRect(paintImg.rect(), Qt::transparent);

			painterResult.drawImage(256, 176, sourceImage);
			painterResult.end();


			break;

		default:
			break;
	}

}

/*****************************************************************************
 *                         获取红外测距值线程
 *  函 数 名：GetDistanceThread
 *  功    能：获取红外测距值
 *  说    明：
 *  参    数：pParam：输入参数
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* Interaction::GetDistanceThread(void* pParam)
{
	Exectime etm(__FUNCTION__);
	Interaction *pInteractionParam = static_cast <Interaction*> (pParam);
	struct timespec ts100ms;
	ts100ms.tv_sec = 0;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms

	int infraredTime;
	int cameraTime;

	int sleepTime = 30;//30;//单位min
	IKXml::IKXmlDocument doc;
	if(doc.LoadFromFile("ikembx00.xml"))
	{
		//读取休眠时间
		if(!doc.GetElementValue("/IKEMBX00/configures/identify/belltime").empty())
		{
			sleepTime = QString(doc.GetElementValue("/IKEMBX00/configures/systemsleep").c_str()).toInt();
			if(sleepTime < 5)
			{
				sleepTime = 5;
			}
			else if(sleepTime > 120)
			{
				sleepTime = 120;
			}
		}

	}
	else
	{
		std::cout<<"配置文件 ikembx00.xml 未打开！"<<std::endl;
	}

	std::cout << "省电模式时间:" << sleepTime << "min" << std::endl;
    //@ for test Wake
    sleepTime = 1;

	int error;
	while(!pInteractionParam->ifClose)
	{
		//发送命令给串口，在回调中取距离值
		//        qWarning("send ad CMD");
		//pInteractionParam->_serialDistance->Send(g_adCMD, 4);
		//20140704不发送红外，供中昌测韦根
		//        s_infraredDistanceValue = 30;//60

		//        usleep(100000);//每100ms发送一次
		nanosleep(&ts100ms, NULL);//每100ms发送一次

		//        std::cout << "Distance is " << s_infraredDistanceValue << " cm" << std::endl;

		//判断距离值
		//        if(NoPersonDistanceValue > s_infraredDistanceValue)

		{
			std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
			if(LEDOnDistanceValue > s_infraredDistanceValue || pInteractionParam->_screenPressed)
			{
				pInteractionParam->_hasPerson = HasPerson;
				pInteractionParam->_screenPressed = false;
			}
			else if(s_infraredDistanceValue >= LEDOnDistanceValue)
			{
				pInteractionParam->_hasPerson = WaitPerson;
			}
			else
			{
				pInteractionParam->_hasPerson = NoPerson;
			}
		}

		if(pInteractionParam->_hasPerson == HasPerson)
		{
			//有人
			if(false == pInteractionParam->_infraredLEDOnFlag)
			{
				//红外灯处于关闭状态，则打开红外灯
				pInteractionParam->_serialDistance->Send(g_redAllOnCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
				pInteractionParam->_infraredLEDOnFlag = true;

				std::cout << "Has person, send cmd to power on infrared LED" << std::endl;
				nanosleep(&ts100ms, NULL);//防止串口丢失命令

				//为了防止串口丢命令，再发一次
				pInteractionParam->_serialDistance->Send(g_redAllOnCMD, 4);
				nanosleep(&ts100ms, NULL);//防止串口丢失命令

                if(false == pInteractionParam->_irisCameraOnFlag)
				{
					//打开虹膜摄像头
					//虹膜摄像头通电后，李言需要1s钟时间加载虹膜摄像头程序，在此时间内不要打开摄像头采图
					pInteractionParam->_serialDistance->Send(g_irisCamOnCMD, 4);
					pInteractionParam->_irisCameraOnFlag = true;

					std::cout << "Has person, send cmd to power on iris camera" << std::endl;
					nanosleep(&ts100ms, NULL);//防止串口丢失命令

					//为了防止串口丢命令，再发一次
					pInteractionParam->_serialDistance->Send(g_irisCamOnCMD, 4);
					nanosleep(&ts100ms, NULL);//防止串口丢失命令

				}

				for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
						iter != s_interactionCallBacks.end(); ++ iter )
				{
					(iter->second)(HasPerson);
				}
			}

			{
				std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
				gettimeofday(&pInteractionParam->_tvHasPerson, nullptr);
			}

			//关闭定时器，并重新启动定时器计时
			//            pInteractionParam->_timerForInfraredLED->stop();
			//20140320为调试程序崩溃，把本cpp中的timer去掉
			//pInteractionParam->_timerForInfraredLED->start(30*1000);//每次开启红外灯后，维持30秒
		}
		else
		{
			gettimeofday(&pInteractionParam->_tvNoPerson, nullptr);
			{
				std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
				infraredTime = pInteractionParam->_tvNoPerson.tv_sec - pInteractionParam->_tvHasPerson.tv_sec;
			}
            if(infraredTime > 15)//10//30
			{
				//如果当前无人时间距离上次有人时间超过30秒，则关闭红外灯
				if(true == pInteractionParam->_infraredLEDOnFlag)
				{
					{
						std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
						pInteractionParam->_hasPerson = WaitPerson;//wait 20141204
					}

					pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
					pInteractionParam->_infraredLEDOnFlag = false;

					std::cout << "No person, send cmd to power off infrared LED" << std::endl;
					nanosleep(&ts100ms, NULL);//防止串口丢失命令

					//为了防止串口丢命令，再发一次
					pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);
					nanosleep(&ts100ms, NULL);//防止串口丢失命令

					//回调，通知应用程序，准备关闭红外灯 20141204 lizhl
					for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
							iter != s_interactionCallBacks.end(); ++ iter )
					{
						std::cout << "No person, 进入回调通知识别模块" << std::endl;
						(iter->second)(WaitPerson);
					}

				}

				//如果无人时间超过30min，关闭虹膜摄像头
				{
					std::lock_guard<std::mutex> lck(pInteractionParam->_setHasPersonMutex);
					cameraTime = pInteractionParam->_tvNoPerson.tv_sec - pInteractionParam->_tvHasPerson.tv_sec;
				}
                if(cameraTime > sleepTime*30)//60
				{
					if(true == pInteractionParam->_irisCameraOnFlag)
					{
						//回调，通知应用程序，准备关闭摄像头
						for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
								iter != s_interactionCallBacks.end(); ++ iter )
						{
							(iter->second)(NoPerson);
						}

						//关闭虹膜摄像头
						pInteractionParam->_serialDistance->Send(g_irisCamOffCMD, 4);
						pInteractionParam->_irisCameraOnFlag = false;

						std::cout << "No person, send cmd to power off iris camera" << std::endl;
						nanosleep(&ts100ms, NULL);//防止串口丢失命令

						//为了防止串口丢命令，再发一次
						pInteractionParam->_serialDistance->Send(g_irisCamOffCMD, 4);
						nanosleep(&ts100ms, NULL);//防止串口丢失命令
					}

				}

			}

		}
	}

	pInteractionParam->_serialDistance->Send(g_redAllOffCMD, 4);
	nanosleep(&ts100ms, NULL);
	ikEvent->ReplyInfraredClosed();
	return (void*)0;
}


/*****************************************************************************
 *                         处理红外灯点亮状态定时器事件
 *  函 数 名：TimerForInfraredLEDTimeOut
 *  功    能：处理红外灯点亮状态定时器事件
 *  说    明：
 *  参    数：NULL
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-12
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::TimerForInfraredLEDTimeOut()
{
	Exectime etm(__FUNCTION__);
	//定时到，关闭红外灯
	if(true == _infraredLEDOnFlag)
	{
		//_serialDistance->Send(g_redAllOffCMD, 4);//20140320为调试程序崩溃，把本cpp中的timer去掉
		_infraredLEDOnFlag = false;

		for( std::map<std::string, InteractionCallBack>::iterator iter = s_interactionCallBacks.begin();
				iter != s_interactionCallBacks.end(); ++ iter )
		{
			(iter->second)(NoPerson);
		}

	}

}

//20140529测试
void Interaction::TimerForTest()
{
	Exectime etm(__FUNCTION__);
    LOG_INFO("in TimerForTest for wake SDK");
    static int num = 0;
    if(num % 2 == 0)
    {
        ScreenPressResponse();
        LOG_INFO("有人");
    }
    if(num>=100)
    {
        num = 0;
    }
    num ++;
}

void Interaction::ScreenPressResponse()
{
	Exectime etm(__FUNCTION__);
	std::lock_guard<std::mutex> lck(_setHasPersonMutex);
	gettimeofday(&_tvHasPerson, nullptr);
	_screenPressed = true;
}

/*****************************************************************************
 *                         注册回调函数
 *  函 数 名：RegisterCallBack
 *  功    能：注册回调函数
 *  说    明：
 *  参    数：NULL
 *
 *  返 回 值：true：成功；
 *           false：失败
 *  创 建 人：lizhl
 *  创建时间：2014-02-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool Interaction::RegisterCallBack(std::string strName, InteractionCallBack callBack)
{
	Exectime etm(__FUNCTION__);
	s_interactionCallBacks[strName] = callBack;
	return s_interactionCallBacks.count(strName) > 0;
}

/*****************************************************************************
 *                         删除回调函数
 *  函 数 名：DeleteCallBack
 *  功    能：删除回调函数
 *  说    明：
 *  参    数：NULL
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Interaction::DeleteCallBack(std::string strName)
{
	Exectime etm(__FUNCTION__);
	if(s_interactionCallBacks.count(strName) > 0)
	{
		s_interactionCallBacks.erase(strName);
	}
}

#ifdef TTS
//20140605测试TTS增加线程
void* Interaction::PlaySoundThread(void* pParam)
{
	Exectime etm(__FUNCTION__);
	Interaction *pInteractionParam = static_cast <Interaction*> (pParam);
	struct timespec ts100ms;
	ts100ms.tv_sec = 0;
	ts100ms.tv_nsec = 100*1000*1000;      //100ms

	int error;
	while(1)
	{
		//        usleep(100000);//每100ms发送一次
		nanosleep(&ts100ms, NULL);//每100ms发送一次

		//        pa_simple_flush(s,&error);
		ei->TTS_Process("keep away", eout);
		pa_simple_drain(s, &error);
		ei->TTS_Refresh();
	}
	return (void*)0;
}

#endif

