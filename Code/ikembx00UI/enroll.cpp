/*****************************************************************************
 ** 文 件 名：enroll.cpp
 ** 主 要 类：Enroll
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：lizhl
 ** 创建时间：2013-10-15
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   注册模块
 ** 主要模块说明: 注册类
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "enroll.h"
#include "ui_enroll.h"
#include "dialogenroll.h"
#include <sys/time.h>
#include "QDateTime"
//#include <QDebug>
#include <QDir>
#include "../XML/xml/IKXmlApi.h"
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//增加数据库操作相关头文件
#include <postgresql/soci-postgresql.h>
#include "IKBackendSession.h"
#include "IKDatabaseLib.h"
#include "IKDatabaseType.h"
#include "dboperationinterface.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

#include "iksocket.h"

#define DIS_FRAMERATE_NO
#define FEATURE_CHECK_EXIST
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


APIIrisInfo *Enroll::s_enrIrisL = NULL;
APIIrisInfo *Enroll::s_enrIrisR = NULL;
LRSucFailFlag Enroll::s_enrLrFlag = EnrRecBothFailed;
Interaction *Enroll::s_enrollInteraction;
LRIrisClearFlag Enroll::s_lrIrisClearFlag;                           //图像是否清晰标志
IrisPositionFlag Enroll::s_irisPositionFlag;                         //图像位置信息标志
IrisTraceInfo Enroll::s_leftTraceInfo;
IrisTraceInfo Enroll::s_rightTraceInfo;

//20140930
int Enroll::s_leftEnrNum;
int Enroll::s_rightEnrNum;

unsigned char Enroll::_grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式
bool Enroll::_signaledFlag;
CAlgApi *Enroll::_enrollAlgApi;                     //注册模块算法API实例


//unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];   //采集图像buffer
//unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式

//--------------------------画图事件响应-----------------------------
ImagePaintEvent imPaintEvent;

/*****************************************************************************
 *                         注册模块发射绘制采集到的图像信号
 *  函 数 名：RaisePaintCapImgEvent
 *  功    能：采图
 *  说    明：
 *  参    数：result：采集图像成功与否的标志
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void ImagePaintEvent::RaisePaintCapImgEvent(int result)
{
	Exectime etm(__FUNCTION__);
	emit SigPaintCapImg(result);
}

/*****************************************************************************
 *                         注册模块发射绘制注册结果信号
 *  函 数 名：RaisePaintEnrollResultEvent
 *  功    能：注册模块发射绘制注册结果信号
 *  说    明：
 *  参    数：result：输入参数，注册函数结果
 *           enrIrisL：输入参数，左眼注册结果虹膜信息结构体
 *           numEnrL：输入参数，左眼注册结果虹膜个数
 *           enrIrisR：输入参数,右眼注册结果虹膜信息结构体
 *           numEnrR：输入参数，右眼注册结果虹膜个数
 *           lrSucFailFlag：输入参数 表明注册结果
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-27
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void ImagePaintEvent::RaisePaintEnrollResultEvent(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrSucFailFlag)
{
	Exectime etm(__FUNCTION__);
	emit SigPaintEnrollResult(result, enrIrisL, numEnrL, enrIrisR, numEnrR, lrSucFailFlag);
}

//--------------------------画图事件响应-----------------------------

/*****************************************************************************
 *                         Enroll构造函数
 *  函 数 名：Enroll
 *  功    能：分配Enroll类资源
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
Enroll::Enroll(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Enroll),
    _enrIrisManager(SingleControl<IrisManager>::CreateInstance())
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	setWindowFlags(Qt::CustomizeWindowHint);
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setBackgroundColor(QColor(242,241,240));

	qRegisterMetaType<LRSucFailFlag>("LRSucFailFlag");
	qRegisterMetaType<IrisPositionFlag>("IrisPositionFlag");

	_dialogEnroll = new DialogEnroll(this);
	_dialogEnroll->hide();

	connect(&imPaintEvent,SIGNAL(SigPaintCapImg(int)),this,SLOT(CapImageUpdate(int)));
	connect(&imPaintEvent,SIGNAL(SigPaintEnrollResult(int, APIIrisInfo* , int , APIIrisInfo* , int, LRSucFailFlag)),
			this, SLOT(EnrollResultImageUpdate(int, APIIrisInfo* , int , APIIrisInfo* , int, LRSucFailFlag)),Qt::AutoConnection);

	//20141011
	connect(ui->rbtnBothEye, SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
	connect(ui->rbtnAnyEye, SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
	connect(ui->rbtnLeftEye, SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
	connect(ui->rbtnRightEye, SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
	//20141011

	s_enrollInteraction = Interaction::GetInteractionInstance();

	_enrollAlgApi = new CAlgApi();
	if(E_ALGAPI_OK != _enrollAlgApi->Init())
	{
		std::cout << "_enrollAlgApi Init failed" << std::endl;
		//        return;
	}

	_signaledFlag = false;//初始化时先不采图，进入注册界面后调用SetPersonInfo再采图
	//启动采图线程，由于_signaledFlag为false，因此先不采图
	_capImgFlag = true;
	//    _capImgThread = std::thread(CapImgThread, (void*)this);


	//^^^^^^^^^^^^^^
#ifdef DIS_FRAMERATE
	_frameNum = 0;
	_showTime = new QTimer(this);
	connect(_showTime, SIGNAL(timeout()), this, SLOT(ShowTime()));
#endif
	//_captureImageTimerId = this->startTimer(1000);
	//vvvvvvvvvvvvvv

	//20140929
	InitLabelStatus();

	//20141202
	InitLabelMask();

	//20141208
	InitTraceGif();
}

/*****************************************************************************
 *                         Enroll析构函数
 *  函 数 名：~Enroll
 *  功    能：释放Enroll类资源
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
Enroll::~Enroll()
{
	Exectime etm(__FUNCTION__);
	_capImgFlag = false;
	if(_capImgThread.joinable())
	{
		_capImgThread.join();
	}
	delete _dialogEnroll;
	delete _enrollAlgApi;
	delete ui;
}

/*****************************************************************************
 *                         设置人员信息，并对界面作相应初始化，由人员管理模块调用
 *  函 数 名：SetPersonInfo
 *  功    能：设置人员信息，并对界面作相应初始化，由人员管理模块调用
 *  说    明：
 *  参    数：personInfomation：人员信息
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-31
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::SetPersonInfo(PersonInfo *personInfomation)
{
	Exectime etm(__FUNCTION__);
	//设置界面
	ui->widgetDisplayIm->show();
	ui->widgetEnrollResultIms->hide();
	ui->btnSaveEnrollResult->setEnabled(false);
	ui->btnEnroll->setText(QObject::tr("开始注册"));
	ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
	ui->btnEnroll->setIconSize(QSize(40,40));    

	ui->btnEnrollReturn->setText(QObject::tr("返回"));
	ui->btnEnrollReturn->setIcon(QIcon(":/image/jtbq_022.ico"));
	ui->btnEnrollReturn->setIconSize(QSize(40,40));


	//使能眼睛模式按钮
	EnableEyeModeRbtn();

	_personInfo = *personInfomation;

	QString qstrUserName = QString::fromLocal8Bit(_personInfo.name.c_str());
	ui->labUserName->setText(qstrUserName);
	QString qstrDepartment = QString::fromLocal8Bit(_personInfo.depart.c_str());

	ui->labDepartment->setText(qstrDepartment);


	_signaledFlag = true;//采图线程开始采图
	_btnEnrollClicked = false;//启动时注册按钮没有被点击

	ui->rbtnBothEye->setChecked(true);//默认为双眼模式注册

	_enrollAlgApi->AsyncCapIrisImg(s_enrollInteraction->s_infraredDistanceValue, CBAlgApiCapIrisImg);

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifdef DIS_FRAMERATE
	_showTime->start(1000);
#endif
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	QDateTime nowTime = QDateTime::currentDateTime();
	//设置系统时间显示格式
	QString strTime = nowTime.toString("hh:mm:ss");
	QString strDate = nowTime.toString("yyyy") + tr("年") + nowTime.toString("MM") + tr("月") + nowTime.toString("dd") + tr("日");
	//界面显示
	DrawStatusLabel(StatusEnrNotStart);

	//设置背景
	QPalette palette;

	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/ui_background.png")));//bkground.jpg
	this->setPalette(palette);

	_labMask->setVisible(true);
}

/*****************************************************************************
 *                         开始注册
 *  函 数 名：on_btnEnroll_clicked
 *  功    能：开始注册
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnEnroll_clicked()
{
	Exectime etm(__FUNCTION__);
	ui->widgetEnrollResultIms->hide();
	ui->widgetDisplayIm->show();

	ui->btnSaveEnrollResult->setEnabled(false);
	_isLeftFeatureExisted = false;//点击注册时，将虹膜特征是否注册过的标志置为false
	_isRightFeatureExisted = false;

	_indexOfLeftExistedFeature = -1;//点击注册时，已存在特征的序号为-1，表示没有重复特征
	_indexOfRightExistedFeature = -1;

	ui->btnEnrollReturn->setText(QObject::tr("返回"));
	ui->btnEnrollReturn->setIcon(QIcon(":/image/jtbq_022.ico"));
	ui->btnEnrollReturn->setIconSize(QSize(40,40));

	bool ifSaveImg = false;

	if(_btnEnrollClicked)
	{
		//如果注册按钮被点击过，则停止注册
		EnableEyeModeRbtn();
		_enrollAlgApi->StopEnroll();
		ui->btnEnroll->setText(QObject::tr("开始注册"));
		ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
		ui->btnEnroll->setIconSize(QSize(40,40));

		_btnEnrollClicked = false;

		DrawStatusLabel(StatusEnrStop);

		//20141210
		SetTraceVisible(false);

	}
	else
	{
		//如果注册按钮没有被点击过，则开始注册
		DisableEyeModeRbtn();
		ui->btnEnroll->setText(QObject::tr("停止注册"));
		ui->btnEnroll->setIcon(QIcon(":/image/stop.ico"));
		ui->btnEnroll->setIconSize(QSize(40,40));

		if( true != CheckAndSetEyeMode() )
		{
			LOG_ERROR("CheckAndSetEyeMode");
			return;
		}

		s_enrIrisL = NULL;
		s_enrIrisR = NULL;

		int funResult = E_ALGAPI_DEFAUL_ERROR;
		_numEnrL = g_constMaxEnrollImNum;
		_numEnrR = g_constMaxEnrollImNum;

		funResult = _enrollAlgApi->AsyncStartEnroll(CBAsyncEnroll, _numEnrL, _numEnrR);
		if(E_ALGAPI_OK != funResult)
		{
			std::cout << "AsyncEnroll failed" << std::endl;
		}
		_btnEnrollClicked = true;

		//20141202
		_labMask->setVisible(true);

		//20141210
		SetTraceVisible(true);

		DrawStatusLabel(StatusEnrStart);
	}

}

/*****************************************************************************
 *                         注册模块返回，不保存结果
 *  函 数 名：on_btnEnrollReturn_clicked
 *  功    能：注册模块返回，不保存结果
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnEnrollReturn_clicked()
{
	Exectime etm(__FUNCTION__);
	//不保存注册虹膜图像并退出
	_enrollAlgApi->StopEnroll();
	_signaledFlag = false;//注册界面关闭，不再采图
#ifdef DIS_FRAMERATE
	_showTime->stop();
#endif
	close();
	emit sigEnrollResult(false);
}


/*****************************************************************************
 *                         退出注册界面
 *  函 数 名：ExitEnrollUI
 *  功    能：退出注册界面，由其上一层调用，用于系统强行退出注册界面，而不是通过界面操作退出
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-06-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::ExitEnrollUI()
{
	Exectime etm(__FUNCTION__);
	if(_signaledFlag)
	{
		//不保存注册虹膜图像并退出
		_enrollAlgApi->StopEnroll();
		_signaledFlag = false;//注册界面关闭，不再采图
#ifdef DIS_FRAMERATE
		_showTime->stop();
#endif
		close();
		emit sigEnrollResult(false);
	}
}

/*****************************************************************************
 *                         保存结果，注册模块返回
 *  函 数 名：on_btnSaveEnrollResult_clicked
 *  功    能：保存结果，注册模块返回
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnSaveEnrollResult_clicked()
{
	Exectime etm(__FUNCTION__);

#ifdef FEATURE_CHECK_EXIST
	DialogFeatureExist dialogFeatureExist(this);

	//如果特征已存在，即重复注册，弹出对话框让用户选择
	PeopleInfo personLeft;
	PeopleInfo personRight;

	if(_isLeftFeatureExisted || _isRightFeatureExisted || _personInfo.hasEnrolledIris)
	{
        LOG_INFO("Exist Iris.Left: %d,Right: %d,HasIris: %d",_isLeftFeatureExisted , _isRightFeatureExisted , _personInfo.hasEnrolledIris)
		//这种方式需要修改，反复load特征反复release特征耗时太长
        LoadExistedFeature();
		if(_isLeftFeatureExisted)
		{
			_enrIrisManager.SelectInfoFromFeature(_indexOfLeftExistedFeature,personLeft,IdentifyType::Left);
		}

		if(_isRightFeatureExisted)
		{
			_enrIrisManager.SelectInfoFromFeature(_indexOfRightExistedFeature,personRight,IdentifyType::Right);
		}

        // common by liushu
		//删除虹膜特征
        //_enrIrisManager.ReleaseFeature();
        //_enrIrisManager.ReleaseSuperData();


		//需要根据index从数据库中找出名字
		//截止20140313，不能在装载特征时获得部门

		if(_isLeftFeatureExisted || _isRightFeatureExisted)
		{
			dialogFeatureExist.XferExistedPersonInfo(personLeft.Depart, personLeft.Name,
					personRight.Depart, personRight.Name);
		}
		else if(_personInfo.hasEnrolledIris)
		{
			dialogFeatureExist.XferExistedPersonInfo(_personInfo.depart, _personInfo.name,
                    _personInfo.depart, _personInfo.name,1);
		}

		if(QDialog::Rejected == dialogFeatureExist.exec())
		{
			return;
		}

	}
#endif

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//保存数据到数据库
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//需要对返回类型_typeOfFeatureExistReturn进行判断，是增加特征还是覆盖特征

#ifdef FEATURE_CHECK_EXIST

	if(CoverFeature == dialogFeatureExist.returnType)
	{
		//如果是覆盖，则先删除原来personID下的特征
		std::vector<IkUuid> vecUuid;
		IkUuid personDelId;

		//        UuidControl uuidDelCtrl;
		uuid_t uuidT;

		if(_isLeftFeatureExisted && _isRightFeatureExisted)
		{
			if(uuid_compare(personLeft.Id, personRight.Id))
			{
				//如果两者不相等
				uuid_copy(uuidT, personLeft.Id);
				personDelId.Set(uuidT);
				vecUuid.push_back(personDelId);

				uuid_copy(uuidT, personRight.Id);
				personDelId.Set(uuidT);
				vecUuid.push_back(personDelId);

			}
			else
			{
				uuid_copy(uuidT, personLeft.Id);
				personDelId.Set(uuidT);
				vecUuid.push_back(personDelId);
			}
		}
		else if(_isLeftFeatureExisted)
		{
			uuid_copy(uuidT, personLeft.Id);
			personDelId.Set(uuidT);
			vecUuid.push_back(personDelId);
		}
		else if(_isRightFeatureExisted)
		{
			uuid_copy(uuidT, personRight.Id);
			personDelId.Set(uuidT);
			vecUuid.push_back(personDelId);
		}
		else if(_personInfo.hasEnrolledIris)
		{
			UuidControl uuidCtrlEnrolled;
			uuid_t personIdEnrolled;
			uuidCtrlEnrolled.UuidParse(_personInfo.id.c_str(),personIdEnrolled);
			personDelId.Set(personIdEnrolled);
			vecUuid.push_back(personDelId);
		}

		DBOperationInterface::DevDelIrisDataBaseAndInsTempIris(vecUuid);
        // Add by liushu  del memfeature
        int nmemret = 0;
        for(int i = 0; i < vecUuid.size(); i ++)
        {
            InfoData    infodata;
            vecUuid[i].Get(infodata.PeopleData.Id);
            nmemret = SingleControl<IrisManager>::CreateInstance().DeletePersonAndFeatureData(infodata);
            char szuid[40] = {0};
            uuid_unparse(infodata.PeopleData.Id,szuid);
            LOG_INFO("Enroll DeletePersonAndFeatureData: pid: [%s] result: %d",szuid,nmemret);
        }
	}

#endif
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	//打开数据库
	//DatabaseAccess dbAccess;
	std::vector<IrisDataBase> vecIrisDataBase;
	IrisDataBase irisDataBase;
	std::vector<TempIrisDataBase> vecTempIrisDataBase;
	TempIrisDataBase tempIrisDataBase;

	/////////////////////////////////////////////////////////////////
	//生成uuid
	//    Person person;
	// 生成人员UUID
	UuidControl uuidCtrl;
	uuid_t irisDataId;
	uuid_t personId;//使用王磊传入的id

	//保存左眼注册信息到数据库
	if( (EnrRecLeftSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag) )
	{
		for (int count=0; count < 2/*_numEnrL*/; count++)
		{
			uuidCtrl.GetUUID(irisDataId);
			irisDataBase.irisDataID.Set(irisDataId);//"sassleft" +count;//待修改
			tempIrisDataBase.irisDataID.Set(irisDataId);

			uuidCtrl.UuidParse(_personInfo.id.c_str(),personId);
			irisDataBase.personID.Set(personId);//"671e1657-cc34-4b0b-a1fd-1ced7ecba225";//"zhangsan";//待修改
			tempIrisDataBase.personID.Set(personId);
			tempIrisDataBase.operation = 0;//增加虹膜数据操作

			irisDataBase.irisCode.Set(s_enrIrisL[count].IrisEnrTemplate, g_constIKEnrFeatureLen);//与matchIrisCode有什么区别
			irisDataBase.eyePic.Set(s_enrIrisL[count].ImgData, g_constBMPHeaderLength + g_constIKImgSize);
			irisDataBase.matchIrisCode.Set(s_enrIrisL[count].IrisRecTemplate, g_constIKRecFeatureLen);

			irisDataBase.devSn = "";//待修改
			irisDataBase.eyeFlag = int(IdentifyType::Left);//左眼标记
			irisDataBase.regTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			//            irisDataBase.regPalce = 0;//待修改
			irisDataBase.createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			irisDataBase.updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			irisDataBase.memo = "";//待修改

			irisDataBase.pupilRow = s_enrIrisL[count].PupilRow;
			irisDataBase.pupilCol = s_enrIrisL[count].PupilCol;
			irisDataBase.pupilRadius = s_enrIrisL[count].PupilRadius;
			//            irisDataBase.pupilVisiblePercent = 0;//没有这一项，需要删除

			irisDataBase.irisRow = s_enrIrisL[count].IrisRow;
			irisDataBase.irisCol = s_enrIrisL[count].IrisCol;
			irisDataBase.irisRadius = s_enrIrisL[count].IrisRadius;

			irisDataBase.focusScore = s_enrIrisL[count].FocusScore;
			irisDataBase.percentVisible = s_enrIrisL[count].PercentVisible;
			irisDataBase.spoofValue = s_enrIrisL[count].SpoofValue;
			irisDataBase.interlaceValue = s_enrIrisL[count].InterlaceValue;
			irisDataBase.qualityLevel = s_enrIrisL[count].QualityLevel;
			irisDataBase.qualityScore = s_enrIrisL[count].QualityScore;

			vecIrisDataBase.push_back(irisDataBase);
			vecTempIrisDataBase.push_back(tempIrisDataBase);
		}
	}


	//保存右眼注册图像
	if( (EnrRecRightSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag))
	{
		for (int count=0; count < 2/*_numEnrR*/; count++)
		{
			uuidCtrl.GetUUID(irisDataId);
			irisDataBase.irisDataID.Set(irisDataId);//"sassright" +count;//待修改
			tempIrisDataBase.irisDataID.Set(irisDataId);

			uuidCtrl.UuidParse(_personInfo.id.c_str(),personId);
			irisDataBase.personID.Set(personId);//"671e1657-cc34-4b0b-a1fd-1ced7ecba225";//"zhangsan";//待修改
			tempIrisDataBase.personID.Set(personId);
			tempIrisDataBase.operation = 0;//增加虹膜数据操作

			irisDataBase.irisCode.Set(s_enrIrisR[count].IrisEnrTemplate, g_constIKEnrFeatureLen);//与matchIrisCode有什么区别
			irisDataBase.eyePic.Set(s_enrIrisR[count].ImgData, g_constBMPHeaderLength + g_constIKImgSize);
			irisDataBase.matchIrisCode.Set(s_enrIrisR[count].IrisRecTemplate, g_constIKRecFeatureLen);

			irisDataBase.devSn = "";//待修改
			irisDataBase.eyeFlag = int(IdentifyType::Right);//右眼标记
			irisDataBase.regTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			//            irisDataBase.regPalce = 0;//待修改
			irisDataBase.createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			irisDataBase.updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
			irisDataBase.memo = "";//待修改

			irisDataBase.pupilRow = s_enrIrisR[count].PupilRow;
			irisDataBase.pupilCol = s_enrIrisR[count].PupilCol;
			irisDataBase.pupilRadius = s_enrIrisR[count].PupilRadius;            

			irisDataBase.irisRow = s_enrIrisR[count].IrisRow;
			irisDataBase.irisCol = s_enrIrisR[count].IrisCol;
			irisDataBase.irisRadius = s_enrIrisR[count].IrisRadius;

			irisDataBase.focusScore = s_enrIrisR[count].FocusScore;
			irisDataBase.percentVisible = s_enrIrisR[count].PercentVisible;
			irisDataBase.spoofValue = s_enrIrisR[count].SpoofValue;
			irisDataBase.interlaceValue = s_enrIrisR[count].InterlaceValue;
			irisDataBase.qualityLevel = s_enrIrisR[count].QualityLevel;
			irisDataBase.qualityScore = s_enrIrisR[count].QualityScore;

			vecIrisDataBase.push_back(irisDataBase);
			vecTempIrisDataBase.push_back(tempIrisDataBase);
		}
	}

	DBOperationInterface::DevAddIrisDataBase(vecIrisDataBase);
	DBOperationInterface::DevAddTempIrisDataBase(vecTempIrisDataBase);
    // Add by liushu  add memfeature
    vector<IkUuid>  vecpersonid;
    for(int i = 0; i < vecIrisDataBase.size(); i ++)
    {
        vecpersonid.clear();
        vecpersonid.push_back(vecIrisDataBase[i].personID);

        // 从数据库获取person信息
        std::vector<Person> persons;
        DBOperationInterface::GetPersons(vecpersonid,persons);

        int nmemret = 0;
        if(persons.size() > 0)      // 只取一条
        {
            InfoData    infodata;
            vecIrisDataBase[i].irisDataID.Get(infodata.ud);   //特征ID
            infodata.TypeControl = (IdentifyType)vecIrisDataBase[i].eyeFlag;
            vecIrisDataBase[i].irisCode.Get(infodata.FeatureData,512);

            persons[0].personID.Get(infodata.PeopleData.Id);
            vecIrisDataBase[i].irisDataID.Get(infodata.PeopleData.FeatureID);
            infodata.PeopleData.CardID = persons[0].cardID;       //卡号  此两个变量是在二期新增 2014-04-15
            infodata.PeopleData.WorkSn = persons[0].workSn;       //工号
            infodata.PeopleData.Name = persons[0].name;
            infodata.PeopleData.Depart = persons[0].departName;
            infodata.PeopleData.Sex = persons[0].sex;

            nmemret = SingleControl<IrisManager>::CreateInstance().AddFeatureData(infodata);
            char sztemp[40] = {0};
            uuid_unparse(infodata.ud,sztemp);
            LOG_INFO("AddFeatureData: pid: [%s], fid: [%s] result: %d",persons[0].personID.Unparse().c_str(),sztemp,nmemret);
        }
    }

	IkSocket::GetIkSocketInstance()->UploadTemp();

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	_enrollAlgApi->StopEnroll();//由于UI界面对保存按钮做了设置，因此如果能够点击“保存”按钮，则注册已经停止了，这里其实可以不用再停止了
	_signaledFlag = false;//注册界面关闭，不再采图

#ifdef DIS_FRAMERATE
	_showTime->stop();
#endif

	close();
	emit sigEnrollResult(true);
}

/*****************************************************************************
 *                         注册模块将采集到的图像显示到界面
 *  函 数 名：CapImageUpdate
 *  功    能：注册模块将采集到的图像显示到界面
 *  说    明：
 *  参    数：result：采集图像成功与否标志位
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::CapImageUpdate(int result)
{
	Exectime etm(__FUNCTION__);
	//    std::lock_guard<std::mutex> lck (_capImgMutex, std::adopt_lock);
	if(E_ALGAPI_OK == result)
	{
		//20141218
		s_enrollInteraction->noticeCount++;
		if(s_enrollInteraction->noticeCount % g_noticeNum == 0)
		{
			ifUpdatePosition = true;
		}
		else
		{
			ifUpdatePosition = false;
		}

		//20140811
		//只有采集图像成功才更新界面
		QImage imageBig;
		imageBig = QImage::fromData(_grayCapImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength);//, "BMP"

		if(!imageBig.isNull())
		{
			QImage paintImg = imageBig.convertToFormat(QImage::Format_ARGB32_Premultiplied);
			//如果处于注册期间则进行人机交互
			if(_btnEnrollClicked)
			{
				s_enrollInteraction->PlayInteraction(ResultUnknown, s_lrIrisClearFlag, s_irisPositionFlag, ifUpdatePosition, paintImg,
						s_leftTraceInfo, s_rightTraceInfo);
				DrawTraceGif(s_irisPositionFlag, s_lrIrisClearFlag, s_leftTraceInfo, s_rightTraceInfo);

				if(ifUpdatePosition)
				{
					DrawStatusLabel(StatusAlgIsPerform);
				}
			}
			//            else
			//            {
			//                s_enrollInteraction->PlayInteraction(AlgorithmIdle, s_lrIrisClearFlag, s_irisPositionFlag, paintImg,
			//                                                     s_leftTraceInfo, s_rightTraceInfo);
			//            }

			paintImg = paintImg.scaled (ui->labRealTimeIm->width(),
					ui->labRealTimeIm->height(),
					Qt::IgnoreAspectRatio);// KeepAspectRatio

            QPainter painter(&paintImg);
             QPen pen(Qt::gray, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);

            QPoint centL(Lx,Ly);
            painter.drawEllipse(centL,Lr,Lr);
            QPoint centR(Rx,Ry);
            painter.drawEllipse(centR,Rr,Rr);
            painter.end();
			ui->labRealTimeIm->setPixmap(QPixmap::fromImage(paintImg));

		}


		//20140930
		if(!_btnEnrollClicked)
		{
			s_leftEnrNum = 0;
			s_rightEnrNum = 0;
		}

		QString leftStr = "左眼已注册\n图像：" + QString::number(s_leftEnrNum,10);
		QString rightStr = "右眼已注册\n图像：" + QString::number(s_rightEnrNum,10);
		ui->labLeftStr->setText(leftStr);
		ui->labRightStr->setText(rightStr);

		QLabel * labLeft[] = {ui->labLeft1,ui->labLeft2,ui->labLeft3};
		QLabel * labRight[] = {ui->labRight1,ui->labRight2,ui->labRight3};

		for(int num = 0; num < 3; num++)
		{
			if(num < s_leftEnrNum)
			{
				labLeft[num]->setPixmap(QPixmap(":/image/greenEye.png"));
			}
			else
			{
				labLeft[num]->setPixmap(QPixmap(":/image/grayEye.png"));//redEye.png
			}

			if(num < s_rightEnrNum)
			{
				labRight[num]->setPixmap(QPixmap(":/image/greenEye.png"));
			}
			else
			{
				labRight[num]->setPixmap(QPixmap(":/image/grayEye.png"));//redEye.png
			}

		}


		//^^^^
#ifdef DIS_FRAMERATE
		_frameNum++;
#endif
		//vvvv
	}

	if(_signaledFlag)
	{
		//如果需要采图，则继续采图
		_enrollAlgApi->AsyncCapIrisImg(s_enrollInteraction->s_infraredDistanceValue, CBAlgApiCapIrisImg);
	}


}


/*****************************************************************************
 *                         从数据库中导入已注册虹膜特征
 *  函 数 名：LoadExistedFeature
 *  功    能：从数据库中导入已注册虹膜特征，供验证虹膜是否重复注册使用
 *  说    明：
 *  参    数：NULL
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::LoadExistedFeature()
{
	Exectime etm(__FUNCTION__);
    // common by liushu
	//初始化
    //_enrIrisManager.Init();

	//加载普通人员特征
	_enrIrisManager.GetIrisFeature(IdentifyType::Left,_enrCodeListL,_enrCodeNumL);
	_enrIrisManager.GetIrisFeature(IdentifyType::Right,_enrCodeListR,_enrCodeNumR);
    LOG_INFO("GetIrisFeature L: %d, R: %d",_enrCodeNumL,_enrCodeNumR);

	//加载管理员特征
	_enrIrisManager.GetSuperFeature(IdentifyType::Left,_enrAdminCodeListL,_enrAdminCodeNumL);
	_enrIrisManager.GetSuperFeature(IdentifyType::Right,_enrAdminCodeListR,_enrAdminCodeNumR);
}


/*****************************************************************************
 *                         注册模块将注册结果图像显示到界面
 *  函 数 名：EnrollResultImageUpdate
 *  功    能：注册模块将注册结果图像显示到界面
 *  说    明：
 *  参    数：result：函数调用结果
 *           enrIrisL：输入参数，左眼注册结果虹膜信息结构体
 *           numEnrL：输入参数，左眼注册结果虹膜个数
 *           enrIrisR：输入参数,右眼注册结果虹膜信息结构体
 *           numEnrR：输入参数，右眼注册结果虹膜个数
 *           lrSucFailFlag：输入参数 表明注册结果
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-27
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::EnrollResultImageUpdate(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrEnrSucFailFlag)
{
	Exectime etm(__FUNCTION__);
	_btnEnrollClicked = false;

	ui->btnEnroll->setText(QObject::tr("重新注册"));
	ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
	ui->btnEnroll->setIconSize(QSize(40,40));
	EnableEyeModeRbtn();

	//    int funResult = E_ALGAPI_ERROR_BASE;
    APIMatchOutput apiMatchOutputL;
    APIMatchOutput apiMatchOutputR;

    IKInitAPIMatchOutput(&apiMatchOutputL); //初始化APIMatchOutput信息
    IKInitAPIMatchOutput(&apiMatchOutputR); //初始化APIMatchOutput信息

	if(E_ALGAPI_OK == result)
	{
		ui->widgetDisplayIm->hide();

		DrawStatusLabel(StatusEnrSuccess);

		//20141202
		_labMask->setVisible(false);

		ui->widgetEnrollResultIms->show();

#ifdef FEATURE_CHECK_EXIST
		//加载虹膜特征
        LOG_INFO("LoadExistedFeature..... begin");
        LoadExistedFeature();
        LOG_INFO("LoadExistedFeature..... end");
#endif

		QImage resultImg;

		QPushButton *btns[] = {ui->btnIrisImgLeft1, ui->btnIrisImgLeft2, ui->btnIrisImgLeft3,
			ui->btnIrisImgRight1, ui->btnIrisImgRight2, ui->btnIrisImgRight3};

		QLineEdit * scores[] = {ui->lineEditScoreLeft1,ui->lineEditScoreLeft2,ui->lineEditScoreLeft3,
			ui->lineEditScoreRight1,ui->lineEditScoreRight2,ui->lineEditScoreRight3};

		QLineEdit * occlusions[] = {ui->lineEditOcclusionLeft1,ui->lineEditOcclusionLeft2,ui->lineEditOcclusionLeft3,
			ui->lineEditOcclusionRight1,ui->lineEditOcclusionRight2,ui->lineEditOcclusionRight3};

		for(int count = 0; count < 6; count++)
		{
			btns[count]->setPixmap(NULL);
			btns[count]->resize(btns[count]->width(),btns[count]->height());

			occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
			scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
			occlusions[count]->setText(QString::number(0,10));
			scores[count]->setText( QString::number(0,10));
		}

		int qualityScore = 0;
		int occlusionsScore = 0;

        ///@ modify  if((EnrRecLeftSuccess == lrEnrSucFailFlag) || (EnrRecBothSuccess == lrEnrSucFailFlag))
        if((EnrRecLeftSuccess == lrEnrSucFailFlag) || (EnrRecBothSuccess == lrEnrSucFailFlag))
        {
            for(int count = 0; count < numEnrL; count++)
            {
                resultImg = QImage::fromData(enrIrisL[count].ImgData, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

				resultImg = resultImg.scaled(btns[count]->width(),btns[count]->height(),Qt::IgnoreAspectRatio);

				btns[count]->setPixmap(QPixmap::fromImage(resultImg));

				btns[count]->resize(btns[count]->width(),btns[count]->height());

				qualityScore = enrIrisL[count].QualityScore;
				occlusionsScore = enrIrisL[count].PercentVisible;

				if(occlusionsScore < MinEnrollOcclusion)
				{
					occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
				}
				else
				{
					occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
				}
				if(qualityScore < MinEnrollQualityScore)
				{
					scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
				}
				else
				{
					scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
				}

				occlusions[count]->setText(QString::number(occlusionsScore,10));
				scores[count]->setText( QString::number(qualityScore,10));

#ifdef FEATURE_CHECK_EXIST
				//注册后检查该虹膜特征是否已经保存
				if( (E_ALGAPI_OK == _enrollAlgApi->Match(enrIrisL[count].IrisRecTemplate, 1,
								_enrCodeListL,_enrCodeNumL,
                                &apiMatchOutputL))
						&& (false == _isLeftFeatureExisted) )
				{
                    LOG_INFO("CheckExist  CodeListL: %d , result: %d",_enrCodeNumL,apiMatchOutputL.MatchIndex[0]);
					_isLeftFeatureExisted = true;
                    _indexOfLeftExistedFeature = apiMatchOutputL.MatchIndex[0];
				}
#endif
			}

		}

		if((EnrRecRightSuccess == lrEnrSucFailFlag) || (EnrRecBothSuccess == lrEnrSucFailFlag))
		{
			for(int count = 0; count < numEnrR; count++)
			{
				resultImg = QImage::fromData(enrIrisR[count].ImgData, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

				//            resultImg = resultImg.scaled(320,240,Qt::IgnoreAspectRatio);
				resultImg = resultImg.scaled(btns[count+3]->width(),btns[count+3]->height(),Qt::IgnoreAspectRatio);

				btns[count+3]->setPixmap(QPixmap::fromImage(resultImg));
				//            btns[count+3]->resize(QSize(320,240));
				btns[count+3]->resize(btns[count+3]->width(),btns[count+3]->height());

				qualityScore = enrIrisR[count].QualityScore;
				occlusionsScore = enrIrisR[count].PercentVisible;

				if(occlusionsScore < MinEnrollOcclusion)
				{
					occlusions[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
				}
				else
				{
					occlusions[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
				}
				if(qualityScore < MinEnrollQualityScore)
				{
					scores[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
				}
				else
				{
					scores[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
				}

				occlusions[count+3]->setText(QString::number(occlusionsScore,10));
				scores[count+3]->setText( QString::number(qualityScore,10));

#ifdef FEATURE_CHECK_EXIST
				//注册后检查该虹膜特征是否已经保存
				if( (E_ALGAPI_OK == _enrollAlgApi->Match(enrIrisR[count].IrisRecTemplate, 1,
								_enrCodeListR, _enrCodeNumR,
                                &apiMatchOutputR))
						&& (false == _isRightFeatureExisted) )
				{
                    LOG_INFO("CheckExist  CodeListR: %d , result: %d",_enrCodeNumR,apiMatchOutputR.MatchIndex[0]);
					_isRightFeatureExisted = true;
                    _indexOfRightExistedFeature = apiMatchOutputR.MatchIndex[0];
				}
#endif
			}
		}



		//        ui->btnIrisImgLeft1->setIconSize(QSize(240,320));
		//        ui->btnIrisImgLeft1->resize(QSize(240,320));
		//        ui->btnIrisImgLeft2->setIconSize(QSize(240,320));
		//        ui->btnIrisImgLeft2->resize(QSize(240,320));
		//        ui->btnIrisImgLeft3->setIconSize(QSize(240,320));
		//        ui->btnIrisImgLeft3->resize(QSize(240,320));
		//        ui->btnIrisImgRight1->setIconSize(QSize(240,320));
		//        ui->btnIrisImgRight1->resize(QSize(240,320));
		//        ui->btnIrisImgRight2->setIconSize(QSize(240,320));
		//        ui->btnIrisImgRight2->resize(QSize(240,320));
		//        ui->btnIrisImgRight3->setIconSize(QSize(240,320));
		//        ui->btnIrisImgRight3->resize(QSize(240,320));

#ifdef FEATURE_CHECK_EXIST
        // common by liushu
		//删除虹膜特征
        //_enrIrisManager.ReleaseFeature();
        //_enrIrisManager.ReleaseSuperData();
#endif

		ui->btnSaveEnrollResult->setEnabled(true);//保存按钮设为可用
		ui->btnEnrollReturn->setText(QObject::tr("放弃保存"));
		ui->btnEnrollReturn->setIcon(QIcon(":/image/jtbq_022.ico"));
		ui->btnEnrollReturn->setIconSize(QSize(40,40));
	}
	else
	{
		DrawStatusLabel(StatusEnrFailed);
	}

	//20141209
	SetTraceVisible(false);


}



/*****************************************************************************
 *                         注册模块采图线程
 *  函 数 名：CapImgThread
 *  功    能：采图
 *  说    明：
 *  参    数：pParam：输入参数
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2013-12-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void* Enroll::CapImgThread(void* pParam)
{
	Exectime etm(__FUNCTION__);
	Enroll *pEnrollParam = static_cast <Enroll*> (pParam);
	int funResult = E_ALGAPI_ERROR_BASE;

	struct timespec ts1us;
	ts1us.tv_sec = 0;
	ts1us.tv_nsec = 1000;//1000;      //1000 ns = 1 us,最小休眠时间为1 us??

	while(pEnrollParam->_capImgFlag)
	{
		if(pEnrollParam->_signaledFlag)
		{
			//            std::lock_guard<std::mutex> lck (pEnrollParam->_capImgMutex, std::adopt_lock);
			//            funResult = pEnrollParam->_enrollAlgApi->SyncCapIrisImg(pEnrollParam->_grayCapImgBuffer + g_constIrisPPMHeaderLength, pEnrollParam->_lrIrisClearFlag, pEnrollParam->_irisPositionFlag);
			//            funResult = pEnrollParam->_enrollAlgApi->SyncCapIrisImg(g_infraredDistanceValue, pEnrollParam->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
			//            funResult = pEnrollParam->_enrollAlgApi->SyncCapIrisImg(pEnrollParam->s_enrollInteraction->s_infraredDistanceValue, pEnrollParam->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
			//            if(E_ALGAPI_OK == funResult)
			//            {
			//                imPaintEvent.RaisePaintCapImgEvent(funResult);

			//                //^^^^^^^^^^^^^^^
			////                pEnrollParam->_frameNum++;
			//                //vvvvvvvvvvvvvvv
			//            }
			//            usleep(0);
			nanosleep(&ts1us,NULL);
		}
		else
		{
			//            usleep(0);
			nanosleep(&ts1us,NULL);
		}
	}
	return (void*)0;
}

/*****************************************************************************
 *                         异步注册回调函数
 *  函 数 名：
 *  功    能：异步注册回调函数
 *  说    明：在回调函数中取异步注册结果
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int Enroll::CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag)
{
	Exectime etm(__FUNCTION__);
	QImage img;//处理注册结果时目前只是为了适应ProcessResult的接口参数，实际没有使用
	img = QImage::fromData(_grayCapImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength);//, "BMP"
	_enrollAlgApi->SaveImg(false,false,NULL,NULL,NULL,NULL);
	if(E_ALGAPI_OK == funResult)
	{
		//注册成功，处理注册结果
		s_enrIrisL = enrIrisL;
		s_enrIrisR = enrIrisR;
		s_enrLrFlag = lrFlag;

		//        s_enrollInteraction->ProcessResult(EnrollSuccess, img);
		s_enrollInteraction->PlayInteraction(EnrollSuccess, s_lrIrisClearFlag, s_irisPositionFlag, false, img,
				s_leftTraceInfo, s_rightTraceInfo);

	}
	else if(E_ALGAPI_ENR_IDEN_FAILED == funResult)
	{
		//注册失败，在界面显示注册失败
		s_enrollInteraction->PlayInteraction(EnrollFailed, s_lrIrisClearFlag, s_irisPositionFlag, false, img,
				s_leftTraceInfo, s_rightTraceInfo);

	}
	else
	{
		//其他情况，如注册过程中注册被人为停止，不再处理，直接返回即可

	}

	s_enrollInteraction->ClearResult();

	imPaintEvent.RaisePaintEnrollResultEvent(funResult, enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag);
	return 0;
}

/*****************************************************************************
 *                         异步采图回调函数
 *  函 数 名：
 *  功    能：异步采图回调函数
 *  说    明：在回调函数中取异步采图结果
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int Enroll::CBAlgApiCapIrisImg(int funResult, unsigned char* pIrisImg, LRIrisClearFlag lrIrisClearFlag, IrisPositionFlag irisPositionFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo,
		int leftEnrNum, int rightEnrNum)
{
	Exectime etm(__FUNCTION__);
	if(E_ALGAPI_OK == funResult)
	{
		//        _grayCapImgBuffer = pIrisImg;
		memcpy(_grayCapImgBuffer, pIrisImg, g_constIrisImgSize + g_constBMPHeaderLength);
		s_lrIrisClearFlag = lrIrisClearFlag;
		s_irisPositionFlag = irisPositionFlag;
		s_leftTraceInfo = leftTraceInfo;
		s_rightTraceInfo = rightTraceInfo;

		s_leftEnrNum = leftEnrNum;
		s_rightEnrNum = rightEnrNum;

		//^^^^^^^^^^^^^^^
		//        _frameNum++;
		//vvvvvvvvvvvvvvv
	}
	//无论采集图像成功与否，都应该继续下一次采图，否则可能导致由于某一次失败而后续不再采集图像
	imPaintEvent.RaisePaintCapImgEvent(funResult);

	return 0;
}

/*****************************************************************************
 *                         检查并设置眼睛模式
 *  函 数 名：CheckAndSetEyeMode
 *  功    能：扫描哪个radiobutton被选中，并设置相应的眼睛模式
 *  说    明：
 *  参    数：
 *  返 回 值：true：成功
 *           false：失败
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool Enroll::CheckAndSetEyeMode()
{
	Exectime etm(__FUNCTION__);
	int funResult = E_ALGAPI_DEFAUL_ERROR;
	funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
	if(E_ALGAPI_OK != funResult)
	{
		return false;
	}

	if(ui->rbtnAnyEye->isChecked())
	{
		_enrollParamStruct.EyeMode = AnyEye;
	}
	else if(ui->rbtnBothEye->isChecked())
	{
		_enrollParamStruct.EyeMode = BothEye;
	}
	else if(ui->rbtnLeftEye->isChecked())
	{
		_enrollParamStruct.EyeMode = LeftEye;
	}
	else if(ui->rbtnRightEye->isChecked())
	{
		_enrollParamStruct.EyeMode = RightEye;
	}

	funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
	if(E_ALGAPI_OK != funResult)
	{
		return false;
	}
	return true;
}

/*****************************************************************************
 *                         去使能眼睛模式选择按钮
 *  函 数 名：DisableEyeModeRbtn
 *  功    能：去使能眼睛模式选择按钮
 *  说    明：
 *  参    数：
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::DisableEyeModeRbtn()
{
	Exectime etm(__FUNCTION__);
	ui->rbtnAnyEye->setEnabled(false);
	ui->rbtnBothEye->setEnabled(false);
	ui->rbtnLeftEye->setEnabled(false);
	ui->rbtnRightEye->setEnabled(false);
}

/*****************************************************************************
 *                         使能眼睛模式选择按钮
 *  函 数 名：EnableEyeModeRbtn
 *  功    能：使能眼睛模式选择按钮
 *  说    明：
 *  参    数：
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::EnableEyeModeRbtn()
{
	Exectime etm(__FUNCTION__);
	ui->rbtnAnyEye->setEnabled(true);
	ui->rbtnBothEye->setEnabled(true);
	ui->rbtnLeftEye->setEnabled(true);
	ui->rbtnRightEye->setEnabled(true);
}

/*****************************************************************************
 *                         点击注册结果左眼第一幅小图浏览大图
 *  函 数 名：on_btnIrisImgLeft1_clicked
 *  功    能：点击注册结果左眼第一幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgLeft1_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisL[0]);
	_dialogEnroll->exec();
}

/*****************************************************************************
 *                         点击注册结果左眼第二幅小图浏览大图
 *  函 数 名：on_btnIrisImgLeft2_clicked
 *  功    能：点击注册结果左眼第二幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgLeft2_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisL[1]);
	_dialogEnroll->exec();
}

/*****************************************************************************
 *                         点击注册结果左眼第三幅小图浏览大图
 *  函 数 名：on_btnIrisImgLeft3_clicked
 *  功    能：点击注册结果左眼第三幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgLeft3_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisL[2]);
	_dialogEnroll->exec();
}

/*****************************************************************************
 *                         点击注册结果右眼第一幅小图浏览大图
 *  函 数 名：on_btnIrisImgRight1_clicked
 *  功    能：点击注册结果右眼第一幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgRight1_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisR[0]);
	_dialogEnroll->exec();
}

/*****************************************************************************
 *                         点击注册结果右眼第二幅小图浏览大图
 *  函 数 名：on_btnIrisImgRight2_clicked
 *  功    能：点击注册结果右眼第二幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgRight2_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisR[1]);
	_dialogEnroll->exec();
}

/*****************************************************************************
 *                         点击注册结果右眼第三幅小图浏览大图
 *  函 数 名：on_btnIrisImgRight3_clicked
 *  功    能：点击注册结果右眼第三幅小图浏览大图
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2013-12-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void Enroll::on_btnIrisImgRight3_clicked()
{
	Exectime etm(__FUNCTION__);
	_dialogEnroll->XferIrisInfo(&s_enrIrisR[2]);
	_dialogEnroll->exec();
}

//^^^^^^^^^^^^^^^^^^^^^^^^
void Enroll::ShowTime()
{
	Exectime etm(__FUNCTION__);
#ifdef DIS_FRAMERATE
	QString strFrameNum;

	strFrameNum.setNum(_frameNum);
	//    ui->lineEditDepartment->setText(strFrameNum);
	ui->labDepartment->setText(strFrameNum);

	_frameNum = 0;
#endif

	QDateTime nowTime = QDateTime::currentDateTime();
	//设置系统时间显示格式
	QString strTime = nowTime.toString("hh:mm:ss");
	QString strDate = nowTime.toString("yyyy") + tr("年") + nowTime.toString("MM") + tr("月") + nowTime.toString("dd") + tr("日");

	//界面显示
	//    ui->labDate->setText(strDate);
	//    ui->labTime->setText(strTime);


}
//vvvvvvvvvvvvvvvvvvvvvvvvvv

void Enroll::on_rbtn_clicked()
{
	Exectime etm(__FUNCTION__);
	QRadioButton *rbtns[] = {ui->rbtnAnyEye, ui->rbtnBothEye, ui->rbtnLeftEye, ui->rbtnRightEye};
	for(int index = 0; index < 4; index++)
	{
		if(rbtns[index]->isChecked())
		{
			rbtns[index]->setStyleSheet("QRadioButton{  font: bold;spacing: 10px;color:rgb(0, 0, 255);    };");
		}
		else
		{
			rbtns[index]->setStyleSheet("QRadioButton{  font: normal;spacing: 10px;color:rgb(0, 0, 0);    };");
		}
	}
}

void Enroll::InitLabelStatus()
{
	Exectime etm(__FUNCTION__);
	_labStatus = new QLabel(this);
	//    _labStatus->setGeometry((1024-668)/2, 600-80, 668, 80);
	_labStatus->setGeometry(g_imgBeginX + g_statusBeginX, g_imgBeginY + g_statusBeginY, g_statusWidth, g_statusHeight);

	QFont serifFont("浪漫雅圆", 26, QFont::Bold);
	_labStatus->setFont(serifFont);
	_labStatus->setAlignment(Qt::AlignCenter);

	DrawStatusLabel(StatusEnrNotStart);
}

void Enroll::InitLabelMask()
{
	Exectime etm(__FUNCTION__);
	_labMask = new QLabel(this,/*Qt::SplashScreen | */Qt::WindowStaysOnTopHint/* | Qt::X11BypassWindowManagerHint*/ );
	_labMask->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498

	QImage image(":/image/ui_mask_green.png");
	image.scaled(g_imgWidth, g_imgHeight);
	QPixmap pic1(QPixmap::fromImage(image));

	//设置图片透明度
	//    QPixmap* alpic1=new QPixmap(pic1.size());
	//    alpic1->fill(QColor(128,128,0,200));
	//    pic1.setAlphaChannel(*alpic1);
	_labMask->setPixmap(pic1);
}

void Enroll::InitTraceGif()
{
	Exectime etm(__FUNCTION__);
    Lx = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;                   //170
    Ly = g_imgHeight - (g_defaultTraceLeftBeginY + g_defaultTraceSize/2)-30; //268
    Lr = 80;
    Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
    Ry = g_imgHeight - (g_defaultTraceRightBeginY + g_defaultTraceSize/2)-30; //424
    Rr = 80;

	_movTraceL = new QMovie(":/image/ui_trace_r.gif");
	_movTraceL->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

	_labTraceL = new QLabel(this);
	_labTraceL->setMovie(_movTraceL);
	_labTraceL->setGeometry(g_defaultTraceLeftBeginX + g_imgBeginX, g_defaultTraceLeftBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);

	_movTraceR = new QMovie(":/image/ui_trace_r.gif");
	_movTraceR->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

	_labTraceR = new QLabel(this);
	_labTraceR->setMovie(_movTraceR);
	_labTraceR->setGeometry(g_defaultTraceRightBeginX + g_imgBeginX, g_defaultTraceRightBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);

	//20141210
	_isClearL = _isClearR = _isLastClearL = _isLastClearR = false;
	SetTraceVisible(false);
}

//20141208
void Enroll::SetTraceVisible(bool isvisible)
{
	Exectime etm(__FUNCTION__);
	if(isvisible)
	{
		_movTraceL->setFileName(":/image/ui_trace_r.gif");
		_movTraceL->restart();
		_labTraceL->setVisible(true);

		_movTraceR->setFileName(":/image/ui_trace_r.gif");
		_movTraceR->restart();
		_labTraceR->setVisible(true);
	}
	else
	{
		_movTraceL->stop();
		_labTraceL->setVisible(false);
		_movTraceR->stop();
		_labTraceR->setVisible(false);
	}
}

void Enroll::DrawStatusLabel(StatusLabelType statusType)
{
	Exectime etm(__FUNCTION__);
	switch(statusType)
	{
		case StatusAlgIsPerform:
			if(Far == s_irisPositionFlag)
			{
				_labStatus->setVisible(true);
				_labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_closer.png)}");
				_labStatus->setText("请靠近一点");
			}
			else if(Near == s_irisPositionFlag)
			{
				_labStatus->setVisible(true);
				_labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_further.png)}");
				_labStatus->setText("请远一点");
			}
			break;

		case StatusEnrStop:
		case StatusEnrNotStart:
			_labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_wait.png)}");
			_labStatus->setText("请点击\"开始注册\"");
			_labStatus->setVisible(true);
			break;

		case StatusEnrFailed:
			_labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_wait.png)}");
			_labStatus->setVisible(true);
			_labStatus->setText("请点击\"重新注册\"");
			break;

		case StatusEnrStart:
		case StatusEnrSuccess:
		case StatusCardAndIri:

		default:
			_labStatus->setVisible(false);
			break;
	}


}

void Enroll::DrawTraceGif(IrisPositionFlag irisPositionFlag, LRIrisClearFlag lrIrisClearFlag, IrisTraceInfo leftTraceInfo, IrisTraceInfo rightTraceInfo)
{
	Exectime etm(__FUNCTION__);
	if(Unknown == irisPositionFlag)
	{
//		leftTraceInfo.centC = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;
//		leftTraceInfo.centR = g_defaultTraceLeftBeginY + g_defaultTraceSize/2;

//		rightTraceInfo.centC = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
//		rightTraceInfo.centR = g_defaultTraceRightBeginY + g_defaultTraceSize/2;
        leftTraceInfo.centC = Lx;
        leftTraceInfo.centR = Ly;

        rightTraceInfo.centC = Rx;
        rightTraceInfo.centR = Ry;
	}

	if(leftTraceInfo.centC + leftTraceInfo.centR > 0)
	{
		if(LImgClear == lrIrisClearFlag || LAndRImgClear == lrIrisClearFlag)
		{
			_isClearL = true;
		}
		else
		{
			_isClearL = false;
		}

		if(_isClearL != _isLastClearL)
		{
			if(_isClearL)
			{
				_movTraceL->setFileName(":/image/ui_trace_g.gif");
			}
			else
			{
				_movTraceL->setFileName(":/image/ui_trace_r.gif");
			}
			_movTraceL->restart();
			_labTraceL->setMovie(_movTraceL);
		}

		_labTraceL->setGeometry((int)(leftTraceInfo.centC * g_scaling) - g_defaultTraceSize/2 + g_imgBeginX,
				(int)(leftTraceInfo.centR * g_scaling) - g_defaultTraceSize/2 + g_imgBeginY,
				g_defaultTraceSize, g_defaultTraceSize);

		if(!_labTraceL->isVisible())
		{
			_labTraceL->setVisible(true);
		}
	}
	else
	{
		//未检测到眼睛则不显示追踪gif
		_labTraceL->setVisible(false);
	}

	if(rightTraceInfo.centC + rightTraceInfo.centR > 0)
	{
		if(RImgClear == lrIrisClearFlag || LAndRImgClear == lrIrisClearFlag)
		{
			_isClearR = true;
		}
		else
		{
			_isClearR = false;
		}


		if(_isClearR != _isLastClearR)
		{
			if(_isClearR)
			{
				_movTraceR->setFileName(":/image/ui_trace_g.gif");
			}
			else
			{
				_movTraceR->setFileName(":/image/ui_trace_r.gif");
			}
			_movTraceR->restart();
			_labTraceR->setMovie(_movTraceR);
		}

		_labTraceR->setGeometry((int)(rightTraceInfo.centC * g_scaling) - g_defaultTraceSize/2 + g_imgBeginX,
				(int)(rightTraceInfo.centR * g_scaling) - g_defaultTraceSize/2 + g_imgBeginY,
				g_defaultTraceSize, g_defaultTraceSize);

		if(!_labTraceR->isVisible())
		{
			_labTraceR->setVisible(true);
		}
	}
	else
	{
		//未检测到眼睛则不显示追踪gif
		_labTraceR->setVisible(false);
	}

	_isLastClearL = _isClearL;
	_isLastClearR = _isClearR;
}

