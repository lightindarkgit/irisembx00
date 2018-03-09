/*****************************************************************************
** 文 件 名：serialframe.cpp
** 主 要 类：SerialFrame
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   帧提取类的实现，处理串口数据流中的帧及配置帧信息
** 主要模块说明: 帧提取类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "../include/serialframe.h"
#include "../include/serialfifo.h"


/*****************************************************************************
*                        获取帧信息
*  函 数 名：GetMin
*  功    能：获取最小值
*  说    明：
*  参    数：
*           n1, 比较参数
*           n2, 比较参数
*  返 回 值：
*           较小的那个参数
*  创 建 人：王磊
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
template<typename T>
T GetMin(IN const T n1, IN const T n2)
{
    return n1 >= n2 ? n2 : n1;
}



SerialFrame::SerialFrame():
    _rdPos(0),
    _wrPos(0),
    _isEmpty(true),
    _isFull(false),
    HeaderFlag(false)
{
    //TO DO
    pframefifo = new serialfifo();
}

SerialFrame::~SerialFrame()
{
//    delete[] _dataBuf;
	delete pframefifo;
	pframefifo = NULL;
}

///****************************************************************************
//*                        初始化帧提取类
//*  函 数 名：Init
//*  功    能：初始化帧信息，和数据长度以及为数据缓冲区分配内存
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//****************************************************************************/
//bool SerialFrame::Init(IN const FrameInfo& frameInfo)
//{
//    _frameInfo = frameInfo;
//    _dataSize = 0;

//    _pDataBuf = new unsigned char[2 * _frameInfo.FRAME_LEN];
//}

///*****************************************************************************
//*                        获取帧信息
//*  函 数 名：GetFrameInfo
//*  功    能：获取帧信息，如帧头，帧尾和帧长
//*  说    明：
//*  参    数：
//*           frameInfo，帧信息输出参数
//*  返 回 值：
//*           返回E_SERIAL_OK
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SerialFrame::GetFrameInfo(OUT FrameInfo &frameInfo)
//{
//    frameInfo = _frameInfo;

//    return E_SERIAL_OK;
//}

///*****************************************************************************
//*                        设置帧信息
//*  函 数 名：SetFrameInfo
//*  功    能：设置帧信息，如帧头，帧尾和帧长
//*  说    明：
//*  参    数：
//*           frameInfo，帧信息输入参数
//*  返 回 值：
//*           返回E_SERIAL_OK
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SerialFrame::SetFrameInfo(IN const FrameInfo &frameInfo)
//{
//    _frameInfo = frameInfo;

//    return E_SERIAL_OK;
//}

///*****************************************************************************
//*                        获取帧头
//*  函 数 名：FindFrameHeader
//*  功    能：从指定数据流中找到帧头，并返回帧头索引
//*  说    明：
//*  参    数：
//*           pBuf, 串口数据缓冲区
//*           dataSize， 串口数据缓冲区大小
//*  返 回 值：
//*           成功：返回帧头索引， 失败：返回负值表示出错
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SerialFrame::FindFrameHeader(IN unsigned char* pBuf, IN const int dataSize)
//{
//    unsigned char* pBuffer = pBuf;

//    while(pBuffer < pBuf + dataSize)
//    {
//        //判断当前位置是否是帧头
//        if(*pBuffer == 0xdd)
//        {
//            return pBuffer - pBuf;
//        }
//        pBuffer++;
//    }

//    return E_SERIAL_ERROR;
//}

///*****************************************************************************
//*                        获取一帧数据
//*  函 数 名：GetFrame
//*  功    能：从指定数据流中提取一帧数据
//*  说    明：
//*  参    数：
//*           pFrameBuf，帧缓冲区指针
//*           pDataBuf，数据流缓冲区指针
//*           dataSize， 数据流中的数据字节数
//*  返 回 值：
//*           成功且获取到一帧数据则返回E_SERIAL_OK，成功但没有获取到一帧数据则返回E_SERIAL_NO_FRAME，其它返回对应错误码
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SerialFrame::GetFrame(OUT unsigned char *pFrameBuf, IN unsigned char *pDataBuf, IN const int dataSize)
//{
//    if(nullptr == _dataBuf || nullptr == pFrameBuf)
//    {
//        return E_SERIAL_INVALID_PTR;
//    }

//    if(dataSize <= 0)
//    {
//        return E_SERIAL_NO_FRAME;
//    }

//    //合并数据流
//    memmove(_dataBuf + _dataSize, pDataBuf, dataSize);
//    _dataSize += dataSize;

//    //标识是否取到至少一帧完整数据
//    bool gotAFrame = false;

//    for(unsigned char* pBuf = _dataBuf; pBuf < _dataBuf +  _dataSize;)
//    {
//        //判断帧头和帧尾
//        int frameHeaderOffset = FindFrameHeader(pBuf, _dataBuf + _dataSize - pBuf);
//        if(E_SERIAL_ERROR == frameHeaderOffset)
//        {
//            memset(_dataBuf, 0, 2 * frameBufSize);
//            _dataSize = 0;

//            break;
//        }

//        //将指针指向有效帧的帧头
//        pBuf += frameHeaderOffset;

//        //剩余数据不足一帧
//        if((_dataSize - frameHeaderOffset) < frameBufSize)
//        {
//            _dataSize -= frameHeaderOffset;
//            memmove(_dataBuf, pBuf, _dataSize);   //将不足一帧的数据搬移到缓冲区开始位置
//            memset(pBuf + _dataSize, 0, frameHeaderOffset);   //清除缓冲区中的无效数据

//            break;
//        }

//        //解析到一帧数据
//        if(0xed == *(pBuf + frameBufSize - 1))
//        {
//            if(false == CheckCRC16(pBuf, FRAME_CRCED_DATA_LEN + FRAME_CRC_DATA_LEN))
//            {
//                pBuf += FRAME_LEN;
//                continue;
//            }

//            gotAFrame = true;          //置获取到一帧数据标志
//            //清空输出缓冲区
//            memset(pFrameBuf, 0, frameBufSize);

//            memmove(pFrameBuf, pBuf + FRAME_DATA_OFFSET, frameBufSize);     //拷贝帧数据
//            pBuf += FRAME_LEN;        //将数据缓冲区指针指向下一帧的起始字节
//        }
//        else
//        {
//            pBuf++;
//        }
//    }

//    return gotAFrame ? E_SERIAL_OK : E_SERIAL_NO_FRAME;
//}

/*****************************************************************************
*                        获取一帧数据
*  函 数 名：FromFrame
*  功    能：从指定数据流中提取一帧数据
*  说    明：
*  参    数：
*           pFrameBuf，帧缓冲区指针
*           pDataBuf，数据流缓冲区指针
*           dataSize， 数据流中的数据字节数
*  返 回 值：
*           成功且获取到一帧数据则返回E_SERIAL_OK，成功但没有获取到一帧数据则返回E_SERIAL_NO_FRAME，其它返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialFrame::FormFrame(OUT unsigned char *pFrameBuf, IN const unsigned char *pRawDataBuf, IN const int dataSize)
{
    if(nullptr == pRawDataBuf || nullptr == pFrameBuf)
    {
        return E_SERIAL_INVALID_PTR;
    }

    unsigned char* pBuf = pFrameBuf;
    memset(pFrameBuf, 0, FRAME_LEN);

    pBuf[0] = 0xdd;         //封装帧头
    pBuf[1] = 0x09;         //封装帧信息之帧长
    memcpy(pBuf + 2, pRawDataBuf, dataSize);    //第三个参数为用户数据长度

    unsigned short crcBits;
    GenerateCRC16(&crcBits, pFrameBuf, 6);         //带校验数据长度
//    memcpy(pBuf + 6, &crcBits, 2);       //第三个参数为校验位长度
    pBuf[6] = crcBits & 0x00FF;
    pBuf[7] = (crcBits >> 8) & 0x00FF;
    pBuf[8] = 0xed;        //封装帧尾

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        获取一帧数据
*  函 数 名：GetFrame
*  功    能：从指定数据流中提取一帧数据
*  说    明：
*  参    数：
*           crcVal，CRC的计算结果
*           rawData，待校验数据
*           dataLen， 待校验字节长度
*  返 回 值：
*           成功则返回E_SERIAL_OK，其它返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SerialFrame::GenerateCRC16(OUT unsigned short *crcVal, IN const unsigned char *rawData, IN const int dataLen)
{
    if(nullptr == crcVal || nullptr == rawData)
    {
        return E_SERIAL_INVALID_PTR;
    }

    unsigned short crcHi = 0;
    unsigned short crcLo = 0;
    unsigned short crc;
    crc = 0xFFFF;

    for (int i = 0; i < dataLen; i++)
    {
        crc = CaculateCRC16(*rawData, crc);
        rawData++;
    }

    crcHi = crc / 256;
    crcLo = crc % 256;
    crc = (crcHi << 8) | crcLo;

    *crcVal = crc;

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        计算移入一个字节之后的CRC值
*  函 数 名：CaculateCRC16
*  功    能：每移入一个字节，计算出此时对应的CRC值
*  说    明：
*  参    数：
*           dataIn，移入的字节值
*           crcIn，移入该字节之前对应的CRC值
*  返 回 值：
*           成功则返回新的CRC值，其它返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
unsigned short SerialFrame::CaculateCRC16(IN unsigned char dataIn, IN unsigned short crcIn)
{
    //TO DO
    unsigned short wCheck = 0;
    crcIn = crcIn ^ dataIn;

    for(int i = 0; i < 8; i++)
    {
        wCheck = crcIn & 1;
        crcIn = crcIn >> 1;
        crcIn = crcIn & 0x7FFF;

        if(wCheck == 1)
        {
            crcIn = crcIn ^ 0xA001;
        }
        crcIn = crcIn & 0xFFFF;
    }

    return crcIn;
}

/*****************************************************************************
*                        校验接收到的数据的CRC
*  函 数 名：CheckCRC16
*  功    能：通过CRC校验接收到的数据
*  说    明：
*  参    数：
*           rawData，接收到的数据
*           dataLen，除CRC之外的数据长度
*  返 回 值：
*           校验正确返回true，其它返回false
*  创 建 人：王磊
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool SerialFrame::CheckCRC16(IN const unsigned char *rawData, IN const int dataLen)
{
    //TO DO
    if(nullptr == rawData)
    {
        return false;
    }

    int rtnVal = E_SERIAL_ERROR;
    unsigned short crc;

    rtnVal = GenerateCRC16(&crc, rawData, dataLen);
    if(E_SERIAL_OK == rtnVal)
    {
        return (0 == crc);
    }

    return false;
}

/*****************************************************************************
*                        向帧析取器的缓冲区中注入数据
*  函 数 名：WriteData
*  功    能：向帧析取器的缓冲区中注入接收到的串口数据
*  说    明：
*  参    数：
*           pBuf，串口数据流缓冲区指针
*           dataSize， 串口数据流中的数据字节数
*  返 回 值：
*           成功则返回E_SERIAL_OK，其它返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-12-11
*  修 改 人：
*  修改时间：
*****************************************************************************/

int SerialFrame::WriteData(IN const unsigned char* pBuf, IN const int dataSize)
{
    if(dataSize <= 0)
    {
        return E_SERIAL_NO_DATA;
    }

//	for(int i=0;i<dataSize;i++) framefifo.writebytetofifo(pBuf[i]);
	for(int i=0;i<dataSize;i++) pframefifo->writebytetofifo(pBuf[i]);


    return E_SERIAL_OK;
}

//  old deanji
/*
int SerialFrame::WriteData(IN const unsigned char* pBuf, IN const int dataSize)
{
    //TO DO
    if(dataSize <= 0)
    {
        return E_SERIAL_NO_DATA;
    }

    //在写入缓冲区之前对缓冲区锁定
    std::lock_guard<std::mutex> lock(_bufMutex);

    //暂存读写标志
    int tWrPos = _wrPos;
    int tRdPos = _rdPos;

    //写入新数据
    int leftCount = frameDataBufSize - _wrPos;    //从写入起点到缓冲区的结尾的字节长度
    if(leftCount > dataSize)
    {
        memcpy(_frameDataBuf + _wrPos, pBuf, dataSize);
        _wrPos = (_wrPos + dataSize) % frameDataBufSize;   //更新写入起点
    }
    else
    {
        memcpy(_frameDataBuf + _wrPos, pBuf, leftCount);
        _wrPos = dataSize - leftCount;
        memcpy(_frameDataBuf, pBuf + leftCount, _wrPos);
    }

    if(_isFull)     //缓冲区满
    {
        //TO DO
        _rdPos = _wrPos;   //更新读出起点
    }
    else if(tWrPos == tRdPos)    //缓冲区空
    {
        //TO DO
        _isFull = (_wrPos == _rdPos);    //更新缓冲区满标志
        _isEmpty = false;
    }
    else if(tWrPos < tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos =  _isFull ? _wrPos : _rdPos;
        }
        else
        {
            _isFull = true;
            _rdPos = _wrPos;

        }
    }
    else if(tWrPos > tRdPos)
    {
        //TO DO
        if(leftCount > dataSize)
        {
            _isFull = false;
        }
        else
        {
            _isFull = _wrPos >= _rdPos;
            _rdPos = _isFull ? _wrPos : _rdPos;
        }
    }


    return E_SERIAL_OK;
}
*/
/*****************************************************************************
*                        获取一帧数据
*  函 数 名：GetFrame
*  功    能：从指定数据流中提取一帧数据
*  说    明：
*  参    数：
*           pFrameBuf，帧缓冲区指针
*  返 回 值：
*           成功且获取到一帧数据则返回E_SERIAL_OK，其他则返回E_SERIAL_NO_FRAME
*  创 建 人：王磊
*  创建时间：2013-12-11
*  修 改 人：
*  修改时间：
*****************************************************************************/

int SerialFrame::GetFrame(unsigned char* pFrameBuf)
{
    //TO DO
    unsigned char framebuf[9];
	int i;
	int fifonumber;

	if(HeaderFlag == true)  //  header has been finded
	{
		if( pframefifo->fifonumber() < (FRAME_LEN - 1))
		{
			return E_SERIAL_NO_FRAME;
		}
		else
		{
			for(i=0;i<FRAME_LEN - 1;i++)
			{
				framebuf[i]	 = pframefifo->readbytefromfifo();				
			}
			
			HeaderFlag = false;

			if(framebuf[7]==FRAME_TAIL)
			{
				memcpy(pFrameBuf,framebuf+1,6);
				return E_SERIAL_OK;
			}
			else
			{
				return E_SERIAL_NO_FRAME;
			}
		}
	}
	else
	{
		fifonumber = pframefifo->fifonumber();
		if( fifonumber < FRAME_LEN)
		{
			return E_SERIAL_NO_FRAME;
		}
		else
		{			
			for(i=0;i<fifonumber;i++)
			{
				if(FRAME_HEADER == pframefifo->readbytefromfifo()) 
				{
					HeaderFlag = true;
					break;
				}					
			}

			if(HeaderFlag)
			{
				if( pframefifo->fifonumber() < (FRAME_LEN - 1))
				{
					return E_SERIAL_NO_FRAME;
				}
				else
				{
					for(i=0;i<FRAME_LEN - 1;i++)
					{
						framebuf[i]	 = pframefifo->readbytefromfifo();				
					}
			
					HeaderFlag = false;

					if(framebuf[7]==FRAME_TAIL)
					{
						memcpy(pFrameBuf,framebuf+1,6);
						return E_SERIAL_OK;
					}
					else
					{
						return E_SERIAL_NO_FRAME;
					}
				}
			}
			else
			{
				return E_SERIAL_NO_FRAME;
			}

		}  
	}
	
}

/*
int SerialFrame::GetFrame(unsigned char* pFrameBuf)
{
    //TO DO
    unsigned char framebuf[9];
	int i;
	int fifonumber;

	if(HeaderFlag == true)  //  header has been finded
	{
		if( framefifo.fifonumber() < (FRAME_LEN - 1))
		{
			return E_SERIAL_NO_FRAME;
		}
		else
		{
			for(i=0;i<FRAME_LEN - 1;i++)
			{
				framebuf[i]	 = framefifo.readbytefromfifo();				
			}
			
			HeaderFlag = false;

			if(framebuf[7]==FRAME_TAIL)
			{
				memcpy(pFrameBuf,framebuf+1,6);
				return E_SERIAL_OK;
			}
			else
			{
				return E_SERIAL_NO_FRAME;
			}
		}
	}
	else
	{
		fifonumber = framefifo.fifonumber();
		if( fifonumber < FRAME_LEN)
		{
			return E_SERIAL_NO_FRAME;
		}
		else
		{			
			for(i=0;i<fifonumber;i++)
			{
				if(FRAME_HEADER == framefifo.readbytefromfifo()) 
				{
					HeaderFlag = true;
					break;
				}					
			}

			if(HeaderFlag)
			{
				if( framefifo.fifonumber() < (FRAME_LEN - 1))
				{
					return E_SERIAL_NO_FRAME;
				}
				else
				{
					for(i=0;i<FRAME_LEN - 1;i++)
					{
						framebuf[i]	 = framefifo.readbytefromfifo();				
					}
			
					HeaderFlag = false;

					if(framebuf[7]==FRAME_TAIL)
					{
						memcpy(pFrameBuf,framebuf+1,6);
						return E_SERIAL_OK;
					}
					else
					{
						return E_SERIAL_NO_FRAME;
					}
				}
			}
			else
			{
				return E_SERIAL_NO_FRAME;
			}

		}  
	}
	
}
*/



// old
/*
int SerialFrame::GetFrame(unsigned char* pFrameBuf)
{
    //TO DO
    if(_isEmpty)
    {
        return E_SERIAL_NO_FRAME;
    }

    unsigned char tmpFrameBuf[FRAME_LEN];

    std::lock_guard<std::mutex> lock(_bufMutex);
    while(!_isEmpty)
    {
        //TO DO
        while(FRAME_HEADER !=  _frameDataBuf[_rdPos])
        {
            ++_rdPos %= frameDataBufSize;
            if(_rdPos == _wrPos)
            {
                _isEmpty = true;
                break;
            }
        }


        //如果当前缓冲区中不足一帧数据，则直接返回
        if(_isEmpty ||
                ((_wrPos > _rdPos) && (_wrPos - _rdPos) < FRAME_LEN) ||
                ((_wrPos < _rdPos) && (_wrPos + frameDataBufSize - _rdPos) < FRAME_LEN))
        {
            return E_SERIAL_NO_FRAME;
        }

        //此处已无需重复判断_rdPos与 _wrPos相等的情况
        if(_wrPos > _rdPos)
        {
            //如果找到帧尾，则将帧析取出来并结束解析数据然后返回，否则继续遍历
            if(FRAME_TAIL == _frameDataBuf[_rdPos + FRAME_LEN - 1])   //解析到一个完整帧
            {
                memcpy(tmpFrameBuf, _frameDataBuf + _rdPos, FRAME_LEN);

                //根据心协议不进行CRC校验
//                if(CheckCRC16(tmpFrameBuf, FRAME_CRCED_DATA_LEN + FRAME_CRC_DATA_LEN))
//                {
//                    memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE);
//                    _rdPos = (_rdPos + FRAME_LEN) % frameDataBufSize;                 //更新读出起点
//                    _isEmpty = (_rdPos == _wrPos);

//                    break;
//                }
                //liu   替换原代码
                memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE + FRAME_CRC_DATA_LEN);
                _rdPos = (_rdPos + FRAME_LEN) % frameDataBufSize;                 //更新读出起点
                _isEmpty = (_rdPos == _wrPos);

                break;
            }

        }
        else  //此处并不包含_rdPos与 _wrPos相等的情况
        {
            int foreFramePartSize = frameDataBufSize - _rdPos;         //一帧的前一部分
            int backFramePartSize = FRAME_LEN - foreFramePartSize;     //一帧的后一部分

            if(_wrPos < backFramePartSize)
            {
                return E_SERIAL_NO_DATA;  //如果剩余数据不足一帧
            }


            if(foreFramePartSize < FRAME_LEN)
            { 
                //如果找到帧尾，则将帧析取出来并结束解析数据然后返回，否则继续遍历
                if(FRAME_TAIL == _frameDataBuf[backFramePartSize - 1])
                {
                    memcpy(tmpFrameBuf, _frameDataBuf + _rdPos, foreFramePartSize);
                    memcpy(tmpFrameBuf + foreFramePartSize, _frameDataBuf, backFramePartSize);

//                    if(CheckCRC16(tmpFrameBuf, FRAME_CRCED_DATA_LEN + FRAME_CRC_DATA_LEN))
//                    {
//                        memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE);
//                        _rdPos = backFramePartSize;      //更新读出起点
//                        _isEmpty = (_rdPos == _wrPos);

//                        break;
//                    }

                    //liu
                    memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE + FRAME_CRC_DATA_LEN);
                    _rdPos = backFramePartSize;      //更新读出起点
                    _isEmpty = (_rdPos == _wrPos);

                    break;


                } 
            }
            else
            {
                //如果找到帧尾，则将帧析取出来并结束解析数据然后返回，否则继续遍历
                if(FRAME_TAIL ==  _frameDataBuf[_rdPos + FRAME_LEN - 1])
                {
                    memcpy(tmpFrameBuf, _frameDataBuf + _rdPos, FRAME_LEN);

//                    if(CheckCRC16(tmpFrameBuf, FRAME_CRCED_DATA_LEN + FRAME_CRC_DATA_LEN))
//                    {
//                        memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE);
//                        _rdPos = (_rdPos + FRAME_LEN) % frameDataBufSize;                 //更新读出起点
//                        _isEmpty = (_rdPos == _wrPos);

//                        break;
//                    }
                    //liu
                    memcpy(pFrameBuf, tmpFrameBuf + FRAME_DATA_OFFSET, FRAME_DATA_SIZE + FRAME_CRC_DATA_LEN);
                    _rdPos = (_rdPos + FRAME_LEN) % frameDataBufSize;                 //更新读出起点
                    _isEmpty = (_rdPos == _wrPos);

                    break;

                }
            }
        }

        ++_rdPos %= frameDataBufSize;
        _isFull = false;      //此时缓冲区已处于非逻辑满状态，或许该结论放在此处有不妥之处，但出于性能考虑放在此处不影响结果
    }

    return E_SERIAL_OK;
}
*/

///*****************************************************************************
//*                        获取缓冲区是否为空状态
//*  函 数 名：IsEmpty
//*  功    能：获取缓冲区是否为空状态
//*  说    明：
//*  参    数：
//*  返 回 值：
//*           返回缓冲区的空状态标志
//*  创 建 人：王磊
//*  创建时间：2013-12-11
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//bool SerialFrame::IsEmpty()
//{
//    return _isEmpty;
//}

///****************************************************************************
//*                        获取缓冲区是否为满状态
//*  函 数 名：IsEmpty
//*  功    能：获取缓冲区是否为满状态
//*  说    明：
//*  参    数：
//*  返 回 值：
//*           返回缓冲区的满状态标志
//*  创 建 人：王磊
//*  创建时间：2013-12-11
//*  修 改 人：
//*  修改时间：
//****************************************************************************/
//bool SerialFrame::IsFull()
//{
//    return _isFull;
//}
