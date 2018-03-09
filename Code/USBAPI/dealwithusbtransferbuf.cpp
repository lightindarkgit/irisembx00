/******************************************************************************************
 ** 文件名:   DEALWITHUSBTRANSFERBUF_CPP
 ×× 主要类:   DealWithUSBTransferBuf
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   刘中昌
 ** 日  期:   2013-11-22
 ** 修改人:
 ** 日  期:
 ** 描  述:  数据处理函数--根据USB 定义的传输协议 重组所需要的数据实现
 *×× 主要模块说明:
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************************/
#include "dealwithusbtransferbuf.h"
//#include "simplelock.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "simplelock.h"
#include "../Common/Exectime.h"

//测试时间
#include<stdlib.h>
#include<iostream>
using namespace std;

//static pthread_mutex_t* s_mutex = nullptr;
//static int text =0;
static int countPPM = 1;

//extern int countPer;
int oldPer =0;
void IKSaveGrayFile(bool isgray,unsigned char *pImgBuffer, int nWidth, int nHeight)
{
	Exectime etm(__FUNCTION__);
	//    if (oldPer != countPer)
	//    {
	//        oldPer = countPer;
	//        countPPM = 1;
	//    }
	struct tm *ptmNowTime;
	struct timeval tv_NowTime;
	time_t lt;
	char cTimeName[200];
	static int n=0;

	lt =time(NULL);
	ptmNowTime = localtime(&lt);
	gettimeofday(&tv_NowTime,NULL);
	countPPM %=100;
	////    sprintf(cTimeName,"./liuTestPPM/%s_%02d.ppm",&filename,countPPM);
	//    if(isgray)
	//    {
	//        sprintf(cTimeName,"./%02dgray_%02d.ppm",oldPer,countPPM);
	//    }
	//    else
	//    {
	//        sprintf(cTimeName,"./bayer_%02d.ppm",countPPM);
	//    }

	FILE* ff = NULL;
	ff=fopen(cTimeName,"wb");
	if(ff)
	{
		fprintf(ff, "P5\n" "%d %d\n255\n", nWidth, nHeight);
		fwrite(pImgBuffer, 1, nWidth * nHeight , ff);
		fclose(ff);
	}
}

//#define GraySize TransBufferSize;
/**********************************************************************************************************************************
 *                       构造函数
 *  函 数 名： DealWithUSBTransferBuf
 *  功    能：初始化变量 创建线程
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-24
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
DealWithUSBTransferBuf::DealWithUSBTransferBuf():_queuePreview(2),_queuePictureTaken(2)
{
	Exectime etm(__FUNCTION__);
	//初始化变量
	_mediBuf  = new unsigned char[TransBufferSize];
	_messBuf  = new unsigned char[TransBufferSize];
	_grayImageBuf = new unsigned char[PreviewImageSize];
	_previewImageBuf = new unsigned char[PreviewImageSize];

	_grayLeftImageBuf = new unsigned char[PreviewImageSize];
	_previewLeftImageBuf = new unsigned char[PreviewImageSize];
	_grayRightImageBuf = new unsigned char[PreviewImageSize];
	_previewRightImageBuf = new unsigned char[PreviewImageSize];

	//    memset(_mediBuf,0,TransBufferSize);
	//    memset(_messBuf,0,TransBufferSize);
	_index  =0;
	_offset =0;
	_mediBufOffset = 0;
	_transBufType = TransBufType::UnKnow;

	pthread_mutex_init(&_mutex,nullptr);
	pthread_mutex_init(&_copyMutex,nullptr);
	//启动异步处理数据
	pthread_create(&_pollThread, NULL, DealBufThread, (void *)this);
}

/**********************************************************************************************************************************
 *                       析构函数
 *  函 数 名： ~DealWithUSBTransferBuf
 *  功    能：析构函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-24
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
DealWithUSBTransferBuf::~DealWithUSBTransferBuf()
{
	Exectime etm(__FUNCTION__);
	pthread_mutex_destroy(&_mutex);
	pthread_mutex_destroy(&_copyMutex);

	if(_mediBuf)
	{
		delete[] _mediBuf;
	}

	if(_messBuf)
	{
		delete[] _messBuf;
	}
	if(_grayImageBuf)
	{
		delete[] _grayImageBuf;
	}
	if(_previewImageBuf)
	{
		delete[] _previewImageBuf;
	}
	pthread_cancel(_pollThread);
	pthread_join(_pollThread, nullptr);
}

/**********************************************************************************************************************************
 *                       交换内存块函数
 *  函 数 名： DealBuf
 *  功    能：交换内存块
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-12-16
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
unsigned char* DealWithUSBTransferBuf::DealBuf(unsigned char* buf)
{
	Exectime etm(__FUNCTION__);
	//    SimpleLockForMutex lock(_mutex);
	//直接交换单内存块处理 交换内存块
	unsigned char * temp = _mediBuf;
	_mediBuf = buf;
	//将偏移量置零
	_mediBufOffset =0;
	//std::cout<<"----dealbuf------"<<std::endl;
	//cout<<"交换USB 传递内存指针，发送信号-》处理"<<text<<endl;
	_threadWaiter.signal();

	//    if(!_offset)
	//    {
	//        //找头
	//        FindHead();
	//    }
	//    else
	//    {
	//        CopyBuf();
	//    }

	//返回交换内存
	return temp;
	//return buf;
}

/**********************************************************************************************************************************
 *                        获取图像函数
 *  函 数 名：GetImage
 *  功    能：获取图像
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool  DealWithUSBTransferBuf::GetPreview(unsigned char * buf, unsigned int &len)
{
	Exectime etm(__FUNCTION__);
	try
	{
		EyeImage eye;
		if(_queuePreview.Take(eye,1))
		{
			return eye.GetImage(buf, len);
		}
		else
		{
			return false;
		}
	}
	catch(string str)
	{
		std::cout<<str<<std::endl;
		return false;
	}
}

/**********************************************************************************************************************************
 *                        获取图像函数
 *  函 数 名：GetImage
 *  功    能：获取图像
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool  DealWithUSBTransferBuf::GetPictureTaken(unsigned char * buf1, unsigned int &len1,unsigned char *buf2, unsigned int &len2)
{
	Exectime etm(__FUNCTION__);
	//return false;
	//    struct timeval tv_NowTime;
	//    gettimeofday(&tv_NowTime,NULL);
	//    std::cout<<"GetPictureTaken开始 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
	PictureImage pic;
	if(!_queuePictureTaken.Take(pic,1))
	{
		return false;
	}
	bool error = pic.GetImage(buf1, len1,buf2,len2);//_imageManager.GetImage(buf,len);

	//   gettimeofday(&tv_NowTime,NULL);
	//   std::cout<<"GetPictureTaken结束 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;

	return error;
}

/**********************************************************************************************************************************
 *                       交换内存块函数
 *  函 数 名： CopyBuf
 *  功    能：交换内存块
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-12-16
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
//template<typename T>
void DealWithUSBTransferBuf::CopyBuf()
{
	Exectime etm(__FUNCTION__);
	//std::cout<<"----copybuf------"<<std::endl;
	if(_index>=0 && _offset ==0)
	{
		// _timeBegin = clock();
		int size = TransBufferSize - _index;
		//pthread_mutex_lock(&_copyMutex);
		memcpy(_messBuf,_mediBuf + _index,size);
		//pthread_mutex_unlock(&_copyMutex);
		_offset = size;
		_index =0;
	}
	else if(_offset >0 && _index==0 )
	{
		int size  = TransBufferSize - _offset;

		//pthread_mutex_lock(&_copyMutex);
		memcpy(_messBuf+_offset,_mediBuf,size);
		//pthread_mutex_unlock(&_copyMutex);
		_offset =0;

		//因为上一帧多使用8字节，因此下一次查询帧头应向前移动8字节以保证不丢帧
		if(size>10)
		{
			_index = size;
			//std::cout<<"_index = size  值："<<_index<<endl;
		}
		else
		{
			_index = 0;
		}

		//增加帧尾检测，lizhl，20140604
		if(0x00 !=_messBuf[TransBufferSize-2] || 0xFF != _messBuf[TransBufferSize-1] )
		{
			_index = 0;
			FindHead();
			return;
		}

		if(_transBufType == TransBufType::Preview)
		{
			EyeImage image;
			//将采样
			DecodeReduceSampledImage(_messBuf,ImageWidth,ImageHeight,_previewImageBuf);
			//灰度处理
			Bayer2Gray(_previewImageBuf, _grayImageBuf,ImageHeight,ImageWidth / 2 );

			image.SetImageCopy(_grayImageBuf,PreviewImageSize);

			_queuePreview.Put(image);

		}
		else if(_transBufType == TransBufType::Picture)
		{
			PictureImage pic;
			//            pic.SetImage()

			//            struct timeval tv_NowTime;
			//            gettimeofday(&tv_NowTime,NULL);
			//            std::cout<<"小图开始 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
			ReconstructTwoImage(_messBuf,ImageWidth,ImageHeight,_previewLeftImageBuf,_previewRightImageBuf);

			//            gettimeofday(&tv_NowTime,NULL);
			//            std::cout<<"小图1 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;

			Bayer2Gray(_previewLeftImageBuf, _grayLeftImageBuf,ImageHeight,ImageWidth / 2 );

			//            gettimeofday(&tv_NowTime,NULL);
			//            std::cout<<"小图2 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
			Bayer2Gray(_previewRightImageBuf, _grayRightImageBuf,ImageHeight,ImageWidth / 2 );


			//            gettimeofday(&tv_NowTime,NULL);
			//            std::cout<<"小图3 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
			//            ReconstructTwoImage(_messBuf,ImageWidth,ImageHeight,_grayLeftImageBuf,_grayRightImageBuf);
			pic.SetImage(_grayLeftImageBuf,PreviewImageSize,_grayRightImageBuf,PreviewImageSize);

			//            gettimeofday(&tv_NowTime,NULL);
			//            std::cout<<"小图4 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
			//正常不允许丢图 （最多允许保留7张图片）
			//            if(_queuePictureTaken.Size()>7)
			//            {
			//                _queuePictureTaken.Take();
			//                _queuePictureTaken.Put(pic);
			//            }
			//            else
			//            {
			_queuePictureTaken.Put(pic);

			//                gettimeofday(&tv_NowTime,NULL);
			//                std::cout<<"小图5 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
			//            }
		}
		//        交换
		//_grayImageBuf = _imageManager.SetImage(_grayImageBuf);


		//std::cout << "存储生成的图片完成！： " << endl;

		//std::cout << "处理时间：" << double(clock()-_timeBegin)/CLOCKS_PER_SEC << std::endl;
		//_imageBuf = _imageManager.SetImage(_imageBuf);
		FindHead();
	}
	else
	{
		std::cout << "数据出来错误！" << std::endl;
		_index=0;
		_offset =0;
	}
}

/**********************************************************************************************************************************
 *                       降采样函数
 *  函 数 名： DecodeReduceSampledImage
 *  功    能：降采样（图像质量降一倍）
 *  说    明：buf 大小为 image 的一倍
 *  参    数： width \height为buf的宽高  image 宽度不变高的减小一倍
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-04-22
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void DealWithUSBTransferBuf::DecodeReduceSampledImage(const unsigned char *buf,int width,int height, unsigned char *image)
{
	Exectime etm(__FUNCTION__);
	try
	{
		SimpleLockForMutex lock(_mutex);
		int offset = 0;
		int endIndex = width * height;

		const unsigned char * pBuf = buf;
		for(int index=0;index<endIndex;index +=4)
		{
			image[offset++] = pBuf[index + 1];
			image[offset++] = pBuf[index];
		}

	}catch(exception e)
	{
		std::cout<<"处理数据错误！"<<std::endl;
	}
}


/**********************************************************************************************************************************
 *                       组帧函数
 *  函 数 名： ReconstructImage
 *  功    能：组帧
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-12-16
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void DealWithUSBTransferBuf::ReconstructImage(const unsigned char *messBuf,int imageW,int imageH,unsigned char* imageBuf )
{
	Exectime etm(__FUNCTION__);
	SimpleLockForMutex lock(_mutex);
	int cnstOffset = imageH * imageW / 2;
	int hrOffset = 0;
	int halfWidth = imageW /2;
	unsigned char* pBuf = imageBuf;
	int increas,frontIndex,lastIndex;
	//组帧
	for(int heightCount = imageH- 1 ; heightCount > 0 ; heightCount-- )
	{
		for( frontIndex = heightCount * halfWidth+8,
				lastIndex = cnstOffset + heightCount * halfWidth ,
				increas = 0; increas < imageW;)
		{
			pBuf[hrOffset++] = messBuf[lastIndex+1];
			pBuf[hrOffset++] = messBuf[lastIndex];
			pBuf[hrOffset++] = messBuf[frontIndex+1];
			pBuf[hrOffset++] = messBuf[frontIndex];
			frontIndex +=2;
			lastIndex  +=2;
			increas +=4;
		}
	}
}

///**********************************************************************************************************************************
//*                       组帧函数
//*  函 数 名： ReconstructImage
//*  功    能：组帧
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2013-12-16
//*  修 改 人：
//*  修改时间：
//*********************************************************************************************************************************/
void DealWithUSBTransferBuf::ReconstructTwoImage(const unsigned char *messBuf,int imageH,int imageW,unsigned char* leftImageBuf ,unsigned char* rightImageBuf)
{
	Exectime etm(__FUNCTION__);
	SimpleLockForMutex lock(_mutex);

	int cnstOffset = imageH * imageW / 4;
	int hrOffset = imageH * imageW / 2;
	unsigned char* pLeftBuf = leftImageBuf;
	unsigned char* pRightBuf = rightImageBuf;

	//组帧
	for(int index = 0 , leftFrontIndex =0,leftLastIndex =cnstOffset ,
			rightFrontIndex = 2*cnstOffset,rightLastIndex = rightFrontIndex + cnstOffset;
			index < hrOffset ;  )
	{
		pLeftBuf[index]    = messBuf[leftLastIndex];
		pRightBuf[index++] = messBuf[rightLastIndex];
		pLeftBuf[index]    = messBuf[leftLastIndex+1];
		pRightBuf[index++] = messBuf[rightLastIndex+1];
		pLeftBuf[index]    = messBuf[leftFrontIndex];
		pRightBuf[index++] = messBuf[rightFrontIndex];
		pLeftBuf[index]    = messBuf[leftFrontIndex+1];
		pRightBuf[index++] = messBuf[rightFrontIndex+1]; //deanji

		leftFrontIndex  +=2;
		leftLastIndex   +=2;
		rightFrontIndex +=2;
		rightLastIndex  +=2;

		//        for( leftFrontIndex  = heightCount * imageW ,
		//             leftLastIndex   = cnstOffset + leftFrontIndex,
		//             rightFrontIndex = cnstOffset + leftLastIndex ,
		//             rightLastIndex  = cnstOffset + rightFrontIndex ,
		//             increas = 0; increas < imageW;)
		//        {

		//            pLeftBuf[hrOffset]    = messBuf[leftLastIndex+1];
		//            pRightBuf[hrOffset++] = messBuf[rightFrontIndex+1];
		//            pLeftBuf[hrOffset]    = messBuf[leftLastIndex];
		//            pRightBuf[hrOffset++] = messBuf[rightFrontIndex];
		//            pLeftBuf[hrOffset]    = messBuf[leftFrontIndex+1];
		//            pRightBuf[hrOffset++] = messBuf[rightLastIndex+1];
		//            pLeftBuf[hrOffset]    = messBuf[leftFrontIndex];
		//            pRightBuf[hrOffset++] = messBuf[rightLastIndex];


		//            increas +=4;
		//        }
	}
}

/**********************************************************************************************************************************
 *                       查找数据头函数
 *  函 数 名： FindHead
 *  功    能： 查找数据头
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-12-16
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void DealWithUSBTransferBuf::FindHead()
{
	Exectime etm(__FUNCTION__);
	//std::cout<<"----FindHead------"<<std::endl;
	unsigned char * originalBuf;
	//查询数据头
	for(; _index <  TransBufferSize - 7 ; _index ++)
	{
		originalBuf =  _mediBuf+_index;
		if(      originalBuf[0] == 0x5A
				&&  originalBuf[1] == 0x5A
				&&  originalBuf[2] == 0xFF
				&&  originalBuf[3] == 0xFF
				&&  originalBuf[4] == 0x00
				&&  originalBuf[5] == 0x00
		  )
		{
			if(originalBuf[6] == 0x02
					&&  originalBuf[7] == 0x00 )
			{
				_transBufType = TransBufType::Preview;
				CopyBuf();
				return;
			}else if(originalBuf[6] == 0x01
					&&  originalBuf[7] == 0x00)
			{
				_transBufType = TransBufType::Picture;
				//std::cout<<"两幅小图！"<<endl;
				CopyBuf();
				return ;
			}
		}
	}

	//如果该帧无头则丢弃
	std::cout<<"没有匹配的图像头:当前位置： "<<_index<<std::endl;
	_index =0;
	_offset =0;
	_transBufType = TransBufType::UnKnow;
	return;
}

/**********************************************************************************************************************************
 *                       处理数据进程
 *  函 数 名： DealBufThread
 *  功    能： 处理数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-12-16
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void *DealWithUSBTransferBuf::DealBufThread(void *arg)
{
	Exectime etm(__FUNCTION__);
	//yichangchuli
	//    try
	//    {
	DealWithUSBTransferBuf* dealBuf = static_cast<DealWithUSBTransferBuf*>(arg);

	while(true)
	{
		dealBuf->_threadWaiter.wait();
		//cout<<"接受到发送信号进行处理 ："<<text<<endl;
		//SimpleLockForMutex lock(dealBuf->_mutex);
		if(!dealBuf->_offset)
		{
			//找头
			dealBuf->FindHead();
		}
		else
		{
			//            PictureImage per;
			dealBuf->CopyBuf();
		}
		//cout<<"处理完成进入等待》》》》》 ："<<text++<<endl;
	}
	//    }catch()
	//    {
	//    }
}


unsigned short static s_rLut[]={
	0,	   77,	  153,	  230,	  306,	  383,	  459,	  536,	  612,	  689,	  765,	  842,	  919,	  995,	 1072,	 1148,	 1225,	 1301,	 1378,	 1454,	 1531,	 1607,	 1684,	 1761,	 1837,	 1914,	 1990,	 2067,	 2143,	 2220,	 2296,	 2373,
	2449,	 2526,	 2602,	 2679,	 2756,	 2832,	 2909,	 2985,	 3062,	 3138,	 3215,	 3291,	 3368,	 3444,	 3521,	 3598,	 3674,	 3751,	 3827,	 3904,	 3980,	 4057,	 4133,	 4210,	 4286,	 4363,	 4440,	 4516,	 4593,	 4669,	 4746,	 4822,
	4899,	 4975,	 5052,	 5128,	 5205,	 5282,	 5358,	 5435,	 5511,	 5588,	 5664,	 5741,	 5817,	 5894,	 5970,	 6047,	 6124,	 6200,	 6277,	 6353,	 6430,	 6506,	 6583,	 6659,	 6736,	 6812,	 6889,	 6966,	 7042,	 7119,	 7195,	 7272,
	7348,	 7425,	 7501,	 7578,	 7654,	 7731,	 7807,	 7884,	 7961,	 8037,	 8114,	 8190,	 8267,	 8343,	 8420,	 8496,	 8573,	 8649,	 8726,	 8803,	 8879,	 8956,	 9032,	 9109,	 9185,	 9262,	 9338,	 9415,	 9491,	 9568,	 9645,	 9721,
	9798,	 9874,	 9951,	10027,	10104,	10180,	10257,	10333,	10410,	10487,	10563,	10640,	10716,	10793,	10869,	10946,	11022,	11099,	11175,	11252,	11329,	11405,	11482,	11558,	11635,	11711,	11788,	11864,	11941,	12017,	12094,	12170,
	12247,	12324,	12400,	12477,	12553,	12630,	12706,	12783,	12859,	12936,	13012,	13089,	13166,	13242,	13319,	13395,	13472,	13548,	13625,	13701,	13778,	13854,	13931,	14008,	14084,	14161,	14237,	14314,	14390,	14467,	14543,	14620,
	14696,	14773,	14850,	14926,	15003,	15079,	15156,	15232,	15309,	15385,	15462,	15538,	15615,	15692,	15768,	15845,	15921,	15998,	16074,	16151,	16227,	16304,	16380,	16457,	16534,	16610,	16687,	16763,	16840,	16916,	16993,	17069,
	17146,	17222,	17299,	17375,	17452,	17529,	17605,	17682,	17758,	17835,	17911,	17988,	18064,	18141,	18217,	18294,	18371,	18447,	18524,	18600,	18677,	18753,	18830,	18906,	18983,	19059,	19136,	19213,	19289,	19366,	19442,	19519
};

unsigned short static s_gLut[]={
	0,	   75,	  150,	  225,	  301,	  376,	  451,	  526,	  601,	  676,	  751,	  826,	  902,	  977,	 1052,	 1127,	 1202,	 1277,	 1352,	 1428,	 1503,	 1578,	 1653,	 1728,	 1803,	 1878,	 1954,	 2029,	 2104,	 2179,	 2254,	 2329,
	2404,	 2479,	 2555,	 2630,	 2705,	 2780,	 2855,	 2930,	 3005,	 3081,	 3156,	 3231,	 3306,	 3381,	 3456,	 3531,	 3607,	 3682,	 3757,	 3832,	 3907,	 3982,	 4057,	 4132,	 4208,	 4283,	 4358,	 4433,	 4508,	 4583,	 4658,	 4734,
	4809,	 4884,	 4959,	 5034,	 5109,	 5184,	 5260,	 5335,	 5410,	 5485,	 5560,	 5635,	 5710,	 5785,	 5861,	 5936,	 6011,	 6086,	 6161,	 6236,	 6311,	 6387,	 6462,	 6537,	 6612,	 6687,	 6762,	 6837,	 6913,	 6988,	 7063,	 7138,
	7213,	 7288,	 7363,	 7438,	 7514,	 7589,	 7664,	 7739,	 7814,	 7889,	 7964,	 8040,	 8115,	 8190,	 8265,	 8340,	 8415,	 8490,	 8566,	 8641,	 8716,	 8791,	 8866,	 8941,	 9016,	 9091,	 9167,	 9242,	 9317,	 9392,	 9467,	 9542,
	9617,	 9693,	 9768,	 9843,	 9918,	 9993,	10068,	10143,	10218,	10294,	10369,	10444,	10519,	10594,	10669,	10744,	10820,	10895,	10970,	11045,	11120,	11195,	11270,	11346,	11421,	11496,	11571,	11646,	11721,	11796,	11871,	11947,
	12022,	12097,	12172,	12247,	12322,	12397,	12473,	12548,	12623,	12698,	12773,	12848,	12923,	12999,	13074,	13149,	13224,	13299,	13374,	13449,	13524,	13600,	13675,	13750,	13825,	13900,	13975,	14050,	14126,	14201,	14276,	14351,
	14426,	14501,	14576,	14652,	14727,	14802,	14877,	14952,	15027,	15102,	15177,	15253,	15328,	15403,	15478,	15553,	15628,	15703,	15779,	15854,	15929,	16004,	16079,	16154,	16229,	16305,	16380,	16455,	16530,	16605,	16680,	16755,
	16830,	16906,	16981,	17056,	17131,	17206,	17281,	17356,	17432,	17507,	17582,	17657,	17732,	17807,	17882,	17958,	18033,	18108,	18183,	18258,	18333,	18408,	18483,	18559,	18634,	18709,	18784,	18859,	18934,	19009,	19085,	19160
};

unsigned short static s_bLut[]={
	0,	   29,	   58,	   88,	  117,	  146,	  175,	  204,	  233,	  263,	  292,	  321,	  350,	  379,	  409,	  438,	  467,	  496,	  525,	  554,	  584,	  613,	  642,	  671,	  700,	  730,	  759,	  788,	  817,	  846,	  876,	  905,
	934,	  963,	  992,	 1021,	 1051,	 1080,	 1109,	 1138,	 1167,	 1197,	 1226,	 1255,	 1284,	 1313,	 1342,	 1372,	 1401,	 1430,	 1459,	 1488,	 1518,	 1547,	 1576,	 1605,	 1634,	 1663,	 1693,	 1722,	 1751,	 1780,	 1809,	 1839,
	1868,	 1897,	 1926,	 1955,	 1985,	 2014,	 2043,	 2072,	 2101,	 2130,	 2160,	 2189,	 2218,	 2247,	 2276,	 2306,	 2335,	 2364,	 2393,	 2422,	 2451,	 2481,	 2510,	 2539,	 2568,	 2597,	 2627,	 2656,	 2685,	 2714,	 2743,	 2772,
	2802,	 2831,	 2860,	 2889,	 2918,	 2948,	 2977,	 3006,	 3035,	 3064,	 3094,	 3123,	 3152,	 3181,	 3210,	 3239,	 3269,	 3298,	 3327,	 3356,	 3385,	 3415,	 3444,	 3473,	 3502,	 3531,	 3560,	 3590,	 3619,	 3648,	 3677,	 3706,
	3736,	 3765,	 3794,	 3823,	 3852,	 3881,	 3911,	 3940,	 3969,	 3998,	 4027,	 4057,	 4086,	 4115,	 4144,	 4173,	 4202,	 4232,	 4261,	 4290,	 4319,	 4348,	 4378,	 4407,	 4436,	 4465,	 4494,	 4524,	 4553,	 4582,	 4611,	 4640,
	4669,	 4699,	 4728,	 4757,	 4786,	 4815,	 4845,	 4874,	 4903,	 4932,	 4961,	 4990,	 5020,	 5049,	 5078,	 5107,	 5136,	 5166,	 5195,	 5224,	 5253,	 5282,	 5311,	 5341,	 5370,	 5399,	 5428,	 5457,	 5487,	 5516,	 5545,	 5574,
	5603,	 5633,	 5662,	 5691,	 5720,	 5749,	 5778,	 5808,	 5837,	 5866,	 5895,	 5924,	 5954,	 5983,	 6012,	 6041,	 6070,	 6099,	 6129,	 6158,	 6187,	 6216,	 6245,	 6275,	 6304,	 6333,	 6362,	 6391,	 6420,	 6450,	 6479,	 6508,
	6537,	 6566,	 6596,	 6625,	 6654,	 6683,	 6712,	 6742,	 6771,	 6800,	 6829,	 6858,	 6887,	 6917,	 6946,	 6975,	 7004,	 7033,	 7063,	 7092,	 7121,	 7150,	 7179,	 7208,	 7238,	 7267,	 7296,	 7325,	 7354,	 7384,	 7413,	 7442
};

///**********************************************************************************************************************************
//*                       将imH*imW大小的bayer数据(raw数据)以线性差值的方法转化为RGB图像，同时转化为灰度图像
//*  函 数 名： DealBufThread
//*  功    能： 处理数据
//*  说    明：注意：原始raw数据位imH*imW，则获得的灰度图像大小为(imH)*(imW)
//*  参    数：
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2013-12-16
//*  修 改 人：
//*  修改时间：
//*********************************************************************************************************************************/
void DealWithUSBTransferBuf::Bayer2Gray(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW)
{
	Exectime etm(__FUNCTION__);
	unsigned char*  gr11 = bayerFrame;
	unsigned char*  r12  = bayerFrame+1;
	unsigned char*  gr13 = bayerFrame+2;
	unsigned char*  b21 = bayerFrame+imW;
	unsigned char*  fb22 = bayerFrame+imW+1;
	unsigned char*  b23 = bayerFrame+imW+2;
	unsigned char*  gr31 = bayerFrame+imW+imW;
	unsigned char*  r32 = bayerFrame+imW+imW+1;
	unsigned char*  gr33 = bayerFrame+imW+imW+2;

	int row, column;
	int grayPos1=0;
	int grayPos2=imW;
	unsigned short sumR12Gr11=0;
	unsigned short sumR32Gb22=0;
	for(row=0;row<imH-2;row+=2)
	{
		for(column=0;column<imW-2;column+=2)
		{
			sumR12Gr11 = s_rLut[* r12 ]+s_gLut[* fb22 ];
			grayIm[grayPos1++]= (sumR12Gr11+s_gLut[*  gr11 ]+s_bLut[* b21 ])>>8;
			grayIm[grayPos1++]= (sumR12Gr11+s_gLut[* gr13 ]+s_bLut[* b23 ])>>8;
			sumR32Gb22 = s_rLut[* r32 ]+s_gLut[* fb22 ];
			grayIm[grayPos2++]= (sumR32Gb22+s_gLut[* gr31 ]+s_bLut[* b21 ])>>8;
			grayIm[grayPos2++]= (sumR32Gb22+s_gLut[* gr33 ]+s_bLut[* b23 ])>>8;
			gr11 +=2;
			r12  +=2;
			gr13 +=2;
			b21  +=2;
			fb22 +=2;
			b23  +=2;
			gr31 +=2;
			r32  +=2;
			gr33 +=2;
		}

		grayIm[grayPos1++]= (s_rLut[*  r12 ]+s_gLut[*  gr11 ]+s_gLut[* fb22 ]+s_bLut[* b21 ])>>8;//单独处理最后一列
		grayIm[grayPos2++]= (s_rLut[* r32 ]+s_gLut[* fb22 ]+s_gLut[* gr31 ]+s_bLut[* b21 ])>>8;//单独处理最后一列
		grayIm[grayPos1++]= grayIm[grayPos1-1];//增加一列
		grayIm[grayPos2++]= grayIm[grayPos2-1];//增加一列
		grayIm[grayPos1++]= grayIm[grayPos1-1];//增加一列
		grayIm[grayPos2++]= grayIm[grayPos2-1];//增加一列

		gr11 +=(2+imW);
		r12  +=(2+imW);
		gr13 +=(2+imW);
		b21  +=(2+imW);
		fb22 +=(2+imW);
		b23  +=(2+imW);
		gr31 +=(2+imW);
		r32  +=(2+imW);
		gr33 +=(2+imW);
		grayPos1+=imW-1;
		grayPos2+=imW-1;
	}
	for(column=0;column<imW-2;column+=2)
	{
		sumR12Gr11 = s_rLut[*  r12 ]+s_gLut[* fb22 ];
		grayIm[grayPos1++]= (sumR12Gr11+s_gLut[*  gr11 ]+s_bLut[* b21 ])>>8;
		grayIm[grayPos1++]= (sumR12Gr11+s_gLut[* gr13 ]+s_bLut[* b23 ])>>8;
		gr11 +=2;
		r12  +=2;
		gr13 +=2;
		b21  +=2;
		fb22 +=2;
		b23  +=2;
	}
	grayIm[grayPos1++]= (s_rLut[*  r12 ]+s_gLut[*  gr11 ]+s_gLut[* fb22 ]+s_bLut[* b21 ])>>8;//单独处理最后一行的最后一列
	grayIm[grayPos1++]= grayIm[grayPos1-1];//单独处理最后一行的最后一列
	pthread_mutex_lock(&_copyMutex);
	memcpy(grayIm+grayPos1,grayIm+grayPos1-imW,imW);
	pthread_mutex_unlock(&_copyMutex);
	return;

}




/**********************************************************************************************************************************
 *                       灏唅mH*imW澶у皬鐨刡ayer鏁版嵁(raw鏁版嵁)浠ョ嚎鎬у樊鍊肩殑鏂规硶杞寲涓篟GB鍥惧儚锛屽悓鏃惰浆鍖栦负鐏板害鍥惧儚
 *  鍑?鏁?鍚嶏細 DealBufThread
 *  鍔?   鑳斤細 澶勭悊鏁版嵁
 *  璇?   鏄庯細娉ㄦ剰锛氬師濮媟aw鏁版嵁浣峣mH*imW锛屽垯鑾峰緱鐨勭伆搴﹀浘鍍忓ぇ灏忎负(imH)*(imW)
 *  鍙?   鏁帮細
 *  杩?鍥?鍊硷細
 *  鍒?寤?浜猴細鍒樹腑鏄?*  鍒涘缓鏃堕棿锛?013-12-16
 *  淇?鏀?浜猴細
 *  淇敼鏃堕棿锛?*********************************************************************************************************************************/
//void DealWithUSBTransferBuf::Bayer2Gray(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW)
//{

//    int row, column;
//    int grayPos1=0;
//    int grayPos2=imW-1;
////    unsigned char bayerFrame[2048*704];
////    for(row=0;row<imH;row++)
////    {
////        for(column=0;column<imW;column++)
////        {
////            //bayerFrame[grayPos1+column]=bayerFrame1[grayPos2-column];
////            bayerFrame[grayPos1+column]=bayerFrame1[grayPos1+column];
////        }
////        grayPos1+=imW;
////        grayPos2+=imW;
////    }

//    //
//    unsigned char*  gr11 = bayerFrame;
//    unsigned char*  r12  = bayerFrame+1;
//    unsigned char*  gr13 = bayerFrame+2;
//    unsigned char*  b21 = bayerFrame+imW;
//    unsigned char*  fb22 = bayerFrame+imW+1;
//    unsigned char*  b23 = bayerFrame+imW+2;
//    unsigned char*  gr31 = bayerFrame+imW+imW;
//    unsigned char*  r32 = bayerFrame+imW+imW+1;
//    unsigned char*  gr33 = bayerFrame+imW+imW+2;

//    //int row, column;
//    grayPos1=0;
//    grayPos2=imW;
//    unsigned short sumR12Gr11=0;
//    unsigned short sumR32Gb22=0;
//    for(row=0;row<imH-2;row+=2)
//    {
//        for(column=0;column<imW-2;column+=2)
//        {
//            sumR12Gr11 = s_rLut[* r12 ]+s_gLut[* fb22 ];
//            grayIm[grayPos1++]= (sumR12Gr11+s_gLut[*  gr11 ]+s_bLut[* b21 ])>>8;
//            grayIm[grayPos1++]= (sumR12Gr11+s_gLut[* gr13 ]+s_bLut[* b23 ])>>8;
//            sumR32Gb22 = s_rLut[* r32 ]+s_gLut[* fb22 ];
//            grayIm[grayPos2++]= (sumR32Gb22+s_gLut[* gr31 ]+s_bLut[* b21 ])>>8;
//            grayIm[grayPos2++]= (sumR32Gb22+s_gLut[* gr33 ]+s_bLut[* b23 ])>>8;
//            gr11 +=2;
//            r12  +=2;
//            gr13 +=2;
//            b21  +=2;
//            fb22 +=2;
//            b23  +=2;
//            gr31 +=2;
//            r32  +=2;
//            gr33 +=2;
//        }

//        grayIm[grayPos1++]= (s_rLut[*  r12 ]+s_gLut[*  gr11 ]+s_gLut[* fb22 ]+s_bLut[* b21 ])>>8;//鍗曠嫭澶勭悊鏈€鍚庝竴鍒?        grayIm[grayPos2++]= (s_rLut[* r32 ]+s_gLut[* fb22 ]+s_gLut[* gr31 ]+s_bLut[* b21 ])>>8;//鍗曠嫭澶勭悊鏈€鍚庝竴鍒?        grayIm[grayPos1++]= grayIm[grayPos1-1];//澧炲姞涓€鍒?        grayIm[grayPos2++]= grayIm[grayPos2-1];//澧炲姞涓€鍒?        grayIm[grayPos1++]= grayIm[grayPos1-1];//澧炲姞涓€鍒?        grayIm[grayPos2++]= grayIm[grayPos2-1];//澧炲姞涓€鍒?        //        {
//        //            //lzc 澧炲姞鏈€鍚庝竴鍒?        //            grayIm[grayPos1++]=  grayIm[grayPos1];
//        //            grayIm[grayPos2++]=  grayIm[grayPos2];
//        //            grayIm[grayPos1++]=  grayIm[grayPos1];
//        //            grayIm[grayPos2++]=  grayIm[grayPos2];
//        //        }
//        gr11 +=(2+imW);
//        r12  +=(2+imW);
//        gr13 +=(2+imW);
//        b21  +=(2+imW);
//        fb22 +=(2+imW);
//        b23  +=(2+imW);
//        gr31 +=(2+imW);
//        r32  +=(2+imW);
//        gr33 +=(2+imW);
//        grayPos1+=imW-1;
//        grayPos2+=imW-1;
//    }
//    for(column=0;column<imW-2;column+=2)
//    {
//        sumR12Gr11 = s_rLut[*  r12 ]+s_gLut[* fb22 ];
//        grayIm[grayPos1++]= (sumR12Gr11+s_gLut[*  gr11 ]+s_bLut[* b21 ])>>8;
//        grayIm[grayPos1++]= (sumR12Gr11+s_gLut[* gr13 ]+s_bLut[* b23 ])>>8;
//        gr11 +=2;
//        r12  +=2;
//        gr13 +=2;
//        b21  +=2;
//        fb22 +=2;
//        b23  +=2;
//    }
//    grayIm[grayPos1++]= (s_rLut[*  r12 ]+s_gLut[*  gr11 ]+s_gLut[* fb22 ]+s_bLut[* b21 ])>>8;//鍗曠嫭澶勭悊鏈€鍚庝竴琛岀殑鏈€鍚庝竴鍒?    grayIm[grayPos1++]= grayIm[grayPos1-1];//鍗曠嫭澶勭悊鏈€鍚庝竴琛岀殑鏈€鍚庝竴鍒?    memcpy(grayIm+grayPos1,grayIm+grayPos1-imW,imW);
//    return;

//}
