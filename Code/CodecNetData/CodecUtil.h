/*
 * CodecUtil.h
 * 网络编解码应用
 * 从CodecNetData.h中拆分出来，
 * 目的是想使使用者include CodecNetData.h一个头文件即可
 * 之前需要包含多个头文件
 *
 *  Created on: 2014年3月20日
 *      Author: yqhe
 */

#pragma once

#include "RequestCode.h"
#include "NetDataStructDef.h"
#include "../Common/Logger.h"

// 类：编码头数据
class EncodeNetHeadData
{
private:
	EncodeNetHeadData()
    {
    };

	~EncodeNetHeadData()
    {
    };

public:
    // 填充头数据
    static void Set(unsigned short reqCode, int len, char* buf)
    {
    	PHeadDataHasLen pHead = (PHeadDataHasLen)buf;
    	pHead->Head0          = g_head0;
    	pHead->Head1          = g_head1;
    	pHead->RequestCode    = reqCode;
    	pHead->RequestSubCode = g_defaultSubCode;
    	pHead->Len            = len;
    };

};

// 类：计算校验值
class CheckData
{
private:
    CheckData()
    {
    };

    ~CheckData()
    {
    };

public:

    /*******************************************************************
    *           计算一段数据的校验值
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：DatabaseAccess
    * 功    能：计算一段数据的校验值
    * 说    明：网络双方校验都直接+buf[index]即可
    * 	       由于char 与 unsigned short之间的数据转换问题
    * 	       可能与手工计算得到的结果有差异
    * 参    数：
    *           输入参数
    *           buf —— 需要校验的数据
    *           len —— 数据长度
    * 返 回 值：校验结果
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 13:57
    * 修 改 人：yqhe
    * 创建日期：2015/4/13 14:77
    * 修改内容：buf[index]强制类型转换为unsigned char，原来没有强制类型转换
    *         因测试中发现，最终累加出来的结果最大值是0xff（预想中的最大值是0xffff）
    *         Win下测试，如果等号两边singed和unsigned匹配，就可以得到预期结果
    *         Linux下测试，必须都为unsigned类型（可做进一步测试）
    *******************************************************************/
    static unsigned short Calc(const char *buf, int len)
    {
        unsigned short checkValue = 0;
        for (int index = 0; index < len; index++)
        {
            checkValue += (unsigned char)buf[index];
        }

//        仅用于输出测试
//        std::stringstream oss;
//        oss << std::hex;

//        for (size_t index=0; index<0x10; index++)
//        {
//            unsigned short c = 0;
//            memmove(&c, &buf[index], 1);
//            oss << "0x"  << std::setw(2) << std::setfill('0') << c << " ";
//        }

//        oss << "checkValue = " << checkValue;
//        oss << std::dec;
//        std::cout << oss.str() << std::endl;

        return checkValue;
    };

    /*******************************************************************
    *         判断校验值是否正确
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：IsCorrect
    * 功    能：判断校验值是否正确
    * 说    明：网络双方校验都直接+buf[index]即可
    * 	       由于char 与 unsigned short之间的数据转换问题
    * 	       可能与手工计算得到的结果有差异
    * 参    数：
    *           输入参数
    *           buf —— 需要校验的数据
    *           len —— 数据长度
    *           checkValue —— 待比对的校验值
    * 返 回 值：true —— 校验值正确；false —— 校验值错误
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 13：57
    *******************************************************************/
    static bool IsCorrect(const char*buf, int len, unsigned short checkValue)
    {
    	unsigned short calcValue = Calc(buf, len);
    	if (checkValue!=calcValue)
    	{
    		return false;
    	}
    	else
    	{
    		return true;
    	}
    }
}; // end of CheckData


// 编码网络数据
class EncodeData
{
private:
	EncodeData()
    {
    }

	~EncodeData()
    {
    }

private:

    /*******************************************************************
    *           填充有数据指令的头信息
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：FillHasDataHead
    * 功    能：填充有数据指令的头信息
    * 说    明：
    * 参    数：
    *           输入参数
    *           reqCode —— 请求码
    *           输出参数
    *           buf     —— 已填充的头信息
    * 返 回 值：无
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 14：02
    *******************************************************************/
    static void FillHasDataHead(int reqCode, int dataSize, char *buf)
    {
		EncodeNetHeadData::Set(reqCode, dataSize, buf);
    }

    /*******************************************************************
    *           填充校验数据
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：FillCheck
    * 功    能：填充校验数据
    * 说    明：
    * 参    数：
    *           输入参数
    *           dataSize —— 实际数据长度（不包括头）
    *           buf      —— 已填充的头+数据的内存，最后还没有校验
    *           输出参数
    *           buf      —— 已填充了校验的内存
    * 返 回 值：无
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 17：12
    *******************************************************************/
    static void FillCheck(char *buf)
    {
    	PHeadDataHasLen pHead = (PHeadDataHasLen)buf;
		int calcCheckSize = g_headOccupy + pHead->Len;

		unsigned short checkValue = CheckData::Calc(buf, calcCheckSize);

		// 记录校验值
		memmove(buf + calcCheckSize, &checkValue, g_checkSumOccupy);
    }

public:
    /*******************************************************************
    *           编码无数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：WithoutDataCmd
    * 功    能：编码无数据指令
    * 说    明：
    * 参    数：
    *           输入参数
    *           reqCode —— 请求码
    *           输出参数
    *           buf     —— 已编码的数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 14：02
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    static int WithoutDataCmd(int reqCode, char *buf)
    {
		PControlCmd pCtrlCmd;

		// 设置编码数据中的头数据
		// 数据长度为0，使用缺省值
		EncodeNetHeadData::Set(reqCode, 0, buf);

		// 计算校验
		pCtrlCmd = (PControlCmd)buf;

		//校验：去除校验两个字节后的整个帧
        int checkLen = g_ctrlCmdCheckLen;
		pCtrlCmd->Check = CheckData::Calc(buf, checkLen);

		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,checkLen,buf);

        return (checkLen + g_checkSumOccupy);
    }

    /*******************************************************************
    *           编码数组格式数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：ArrayDataCmd
    * 功    能：编码数组格式数据指令
    * 说    明：以变长数组形式存在的数据
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           vod        —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           pNetData —— 已填充的网络信息
    *           		    注意，调用这个函数，确保NetT这个模版满足
    *           		    有ArraySize和ArrayData元素
    *           		    ArraySize用于描述有多少条ArrayData
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/17 17：02
    *******************************************************************/
    template<typename DataT, typename NetT>
	static int ArrayDataCmd(int reqCode,
			const std::vector<DataT> &vod,
			NetT *pNetData,
			bool needCheck=true)
	{
		int dataSize = g_arraySizeOccupy + vod.size()*sizeof(DataT);

		char *buf = (char*)pNetData;
        FillHasDataHead(reqCode, dataSize, buf);

        // 设置数组长度
        pNetData->ArraySize = vod.size();

        // 依次拷贝
        size_t beginPos = 0; 
        for (auto v : vod)
        {
            // modified at 20140527 17:12 by yqhe
            // 或者用pNetData->ArrayData + beginPos效果也是一样的
            // 或者用&pNetData->ArrayData[beginPos]效果也是一样的
            // 注意，这里的步长是一个DataT结构，而不是取char的步长，之前在这里出错了
            memmove(&pNetData->ArrayData[0] + beginPos, &v, sizeof(DataT));
            beginPos ++;
		}

		if (needCheck)
		{
			FillCheck(buf);
		}

        int totalSize = g_headOccupy + dataSize + g_checkSumOccupy;

		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,totalSize,buf);

        return totalSize;
	}

    /*******************************************************************
    *          编码数据结构格式数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：BlockDataCmd
    * 功    能：编码数据结构格式数据指令
    * 说    明：此数据结构中无变长数组类数据
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           writeData  —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           buf     —— 编码好了的网络传输数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 22：02
    *******************************************************************/
    template<typename DataT>
    static int BlockDataCmd(int reqCode, const DataT& writeData, char* buf, bool needCheck=true)
    {
		int dataSize = sizeof(DataT);

		// 填充请求头部
        FillHasDataHead(reqCode, dataSize, buf);

        // 填充请求实际数据
        memmove(buf + g_headOccupy, &writeData, dataSize);

		if (needCheck)
		{
			FillCheck(buf);
		}

        int totalLen = g_headOccupy + dataSize + g_checkSumOccupy;

		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,totalLen,buf);

        return totalLen;
    }

    /*******************************************************************
    *          编码人员照片数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：PhotoDataCmd
    * 功    能：编码人员照片数据指令
    * 说    明：由于照片数据结构较为特殊，所以单独编码
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           writeData  —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           buf     —— 编码好了的网络传输数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 22：02
    *******************************************************************/
    static int PhotoDataCmd(int reqCode, const PersonPhoto& writeData, char* buf, bool needCheck = true)
    {
        int dataSize = sizeof(PersonPhoto) + writeData.PhotoSize;

        // 填充请求头部
        FillHasDataHead(reqCode, dataSize, buf);

        // 填充请求实际数据
        // 分两部分填充
        memmove(buf + g_headOccupy, &writeData, sizeof(PersonPhoto));
        memmove(buf + g_headOccupy + sizeof(PersonPhoto), &writeData.Photo, writeData.PhotoSize);

        if (needCheck)
        {
            FillCheck(buf);
        }

        int totalLen = g_headOccupy + dataSize + g_checkSumOccupy;

		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,totalLen,buf);

        return totalLen;
    }

    /*******************************************************************
    *           编码数组格式数据回应
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：ArrayDataResponse
    * 功    能：编码数组格式数据回应
    * 说    明：以变长数组形式存在的数据
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           vod        —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           pNetData —— 已填充的网络信息
    *           		    注意，调用这个函数，确保NetT这个模版满足
    *           		    有ArraySize和ArrayData元素
    *           		    ArraySize用于描述有多少条ArrayData
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/20 13：05
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    template<typename DataT, typename NetT>
	static int ArrayDataResponse(int reqCode,
			const std::vector<DataT> &vod,
			NetT *pNetData,
			bool needCheck=true)
	{
		int dataSize = g_arraySizeOccupy + vod.size()*sizeof(DataT);
		int dataWithErrCodeSize = dataSize + 4;

		char *buf = (char*)pNetData;
        FillHasDataHead(reqCode, dataWithErrCodeSize, buf);

        // 设置回应错误码
        pNetData->Hd.ErrorCode = g_success;

        // 设置数组长度
        pNetData->ArraySize = vod.size();

        // 依次拷贝
        size_t beginPos = 0;
        for (auto v : vod)
        {
            memmove(&pNetData->ArrayData[0] + beginPos, &v, sizeof(DataT));
            beginPos ++;
        }

		if (needCheck)
		{
			FillCheck(buf);
		}

        int totalLen = g_headOccupy + pNetData->Hd.Len + g_checkSumOccupy;
		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,totalLen,buf);

        return totalLen;
	}

    /*******************************************************************
    *          编码数据结构格式数据回应
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：BlockDataResponse
    * 功    能：编码数据结构格式数据回应
    * 说    明：此数据结构中无变长数组类数据，而是一个定长的数据结构
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           writeData  —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           buf     —— 编码好了的网络传输数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/4/30 07：58
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    template<typename DataT>
    static int BlockDataResponse(int reqCode,
    		const DataT& writeData,
    		char* buf,
    		bool needCheck=true)
    {
		int dataSize = sizeof(DataT);

		int dataWithErrCodeSize = dataSize + 4;

		// 填充请求头部
        FillHasDataHead(reqCode, dataWithErrCodeSize, buf);

		PBlockSuccessAck pAck = (PBlockSuccessAck)buf;
        pAck->ErrorCode = g_success;

        // 填充请求实际数据
        memmove(buf + g_respDataStart, &writeData, dataSize);

		if (needCheck)
		{
			FillCheck(buf);
		}

        int totalLen = g_headOccupy + dataWithErrCodeSize + g_checkSumOccupy;
		LOG_DEBUG("CMD: [%08X],Len: %d, Data: [%032X]",reqCode,totalLen,buf);

        return totalLen;
    }

};

// 解码网络数据
class DecodeData
{
private:
	DecodeData()
    {
    }

	~DecodeData()
    {
    }

public:
    /*******************************************************************
    *          解码指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：Command
    * 功    能：解码指令
    * 说    明：只解码头数据，便于确定是什么请求，
    * 		   因为还不知道是什么指令，因此不计算校验
    * 参    数：
    *           输入参数
    *           buf     —— 网络上获取的数据
    * 			输出参数
    * 			pCtrlCmd —— 解码后的头数据，校验值总置为0
    * 返 回 值：无
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 11：17
    *******************************************************************/
	static void Command(const char *buf, PControlCmd ctrlCmd)
	{
    	OnlyCommand(buf, ctrlCmd);

    	return;
	}

    ///////////////////////////////////////////////
    // 解码请求数据：REQ_XXX
    ///////////////////////////////////////////////

    /*******************************************************************
    *          解码无数据的网络数据请求（解码REQ_xxx）
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： WithoutDataCmd
    * 功    能：解码无数据的网络数据请求
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf—— 需要解码的网络数据
    * 			输出参数
    *           ctrlCmd —— 解码结果
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：23
    *******************************************************************/
	static bool WithoutDataCmd(const char *buf, ControlCmd ctrlCmd)
	{
		// 检测校验码是否正确
		if (!IsCheckCorrect(buf))
		{
			// 如果校验码不正确，则返回false
			return false;
		}

		memmove(&ctrlCmd, buf, sizeof(ControlCmd));

		return true;
	}

    /*******************************************************************
    *          解码有数据的数组型网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： ArrayDataCmd
    * 功    能：解码有数据的数组型网络数据
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           vod     —— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：05
    *******************************************************************/
    template<typename DataT, typename NetT>
	static bool ArrayDataCmd(
			const NetT *pNetData,
			std::vector<DataT> &vod,
			bool needCheck=true)
	{
		int len = pNetData->ArraySize;

		char* buf = (char*)pNetData;

		// 检测校验码是否正确
		if (needCheck && (!IsCheckCorrect(buf)))
		{
			// 如果校验码不正确，则返回false
			return false;
		}

		for (int num = 0; num < len; num++)
		{
			DataT od;
			memmove(&od, &pNetData->ArrayData[num], sizeof(DataT));
			vod.push_back(od);
		}

		return true;
	}

    /*******************************************************************
    *          解码有数据的结构型网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： BlockDataCmd
    * 功    能：解码有数据的结构型网络数据
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：05
    *******************************************************************/
    template<typename DataT>
	static bool BlockDataCmd(const char *buf, DataT &readData, bool needCheck=true)
	{
		// 检测校验码是否正确
		if (needCheck && (!IsCheckCorrect(buf)))
		{
			// 如果校验码不正确，则返回false
			return false;
		}

		int dataSize = sizeof(DataT);
		memmove((char*)&readData, buf + g_headOccupy, dataSize);

		return true;
	}


    /*******************************************************************
    *          解码照片网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： PhotoDataCmd
    * 功    能：解码照片网络数据
    * 说    明：由于方式较为特殊，所以另外写一个函数
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/30 11：41
    *******************************************************************/
    static bool PhotoDataCmd(const char* buf, PersonPhoto &readData, bool needCheck = true)
    {
        // 检测校验码是否正确
        if (needCheck && (!IsCheckCorrect(buf)))
        {
            // 如果校验码不正确，则返回false
            return false;
        }

        PHeadDataHasLen pHead = (PHeadDataHasLen)buf;

        int photoSize = pHead->Len - sizeof(PersonPhoto);

        int firstCopySize = sizeof(PersonPhoto);

        // 分成两部分拷贝，因为photo实际数据部分不一定和前面的是连续内存
        char* _tempData = (char*)&readData;
        memmove(_tempData, buf + g_headOccupy, firstCopySize);
        memmove(_tempData+firstCopySize, buf + g_headOccupy + firstCopySize, photoSize);
 
        return true;
    }
    
    /*******************************************************************
    *          获得照片网络数据中照片的长度
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： GetPhotoSize
    * 功    能：获得照片网络数据中照片的长度
    * 说    明：便于在解码之前，知道要分配多长的内存
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：-1  —— 校验失败，后面就不要解码了
    *          >0  —— 照片长度
    * 创 建 人：yqhe
    * 创建日期：2014/5/30 11：46
    *******************************************************************/
    static int GetPhotoSize(const char* buf, bool needCheck = true)
    {
        // 检测校验码是否正确
        if (needCheck && (!IsCheckCorrect(buf)))
        {
            // 如果校验码不正确，则返回false
            return -1;
        }

        PHeadDataHasLen pHead = (PHeadDataHasLen)buf;

        int photoSize = pHead->Len - sizeof(PersonPhoto);

        return photoSize;
    }

    ///////////////////////////////////////////////
    // 解码回应数据：ACK_XXX
    ///////////////////////////////////////////////

    /*******************************************************************
    *          解码无数据的网络数据回应（解码ACK_xxx）
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： WithoutDataResponse
    * 功    能：解码无数据的网络数据回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf—— 需要解码的网络数据
    * 			输出参数
    * 			ack —— 解码后的回应一般格式
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：23
    *******************************************************************/
	static bool WithoutDataResponse(const char *buf, NormalAck &ack)
	{
		return DecodeResponse(buf, ack);
	} // end of WithoutDataResponse

    /*******************************************************************
    *          解码数据为数组型的网络数据回应(ACK_XXX)
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： WithArrayDataResponse
    * 功    能：解码数据为数组型的网络数据回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    * 			ack     —— 解码后的回应一般格式
    *           vod     —— 已解码的数组型数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/03 10：45
    *******************************************************************/
    template<typename DataT, typename NetT>
	static bool ArrayDataResponse(
			const NetT *pNetData,
			NormalAck &ack,
			std::vector<DataT> &vod,
			bool needCheck=true)
	{
		char* buf = (char*)pNetData;
		bool decodeRet = DecodeResponse(buf, ack, needCheck);

		// 如果解码失败，或返回值为错误，则返回
		if (g_success != ack.ErrorCode)
		{
			return decodeRet;
		}

	    // 回应为成功时，解码数组型数据
		int len = pNetData->ArraySize;
		for (int num = 0; num < len; num++)
		{
			DataT od;
			memmove(&od, &pNetData->ArrayData[num], sizeof(DataT));
			vod.push_back(od);
		}

		return true;
	}

    /*******************************************************************
    *          解码有数据的结构型网络数据回应(ACK_XXX)
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： BlockDataResponse
    * 功    能：解码有数据的结构型网络数据回应(ACK_XXX)
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/3 11：30
    *******************************************************************/
    template<typename DataT>
	static bool BlockDataResponse(const char *buf,
			NormalAck &ack,
			DataT &readData,
			bool needCheck=true)
	{
		bool decodeRet = DecodeResponse(buf, ack, needCheck);

		// 如果解码失败，或返回值为错误，则返回
		if (g_success != ack.ErrorCode)
		{
			return decodeRet;
		}

		int dataSize = sizeof(DataT);
		memmove(&readData, buf + g_respDataStart, dataSize);

		return true;
	}

private:
    /*******************************************************************
    *          判断网络传来的数据校验码是否正确
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： IsCheckCorrect
    * 功    能：判断网络传来的数据校验码是否正确
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf		  —— 需要校验的数据
    * 			checkSize —— 需要校验的数据长度
    * 返 回 值：无
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：10
    *******************************************************************/
	static bool IsCheckCorrect(const char* buf)
	{
		PHeadDataHasLen pHead = (PHeadDataHasLen)buf;

		int checkSize = g_headOccupy + pHead->Len;

		// 获得校验值
		unsigned short transCheckValue = 0;
		memmove(&transCheckValue, buf+checkSize, g_checkSumOccupy);

		return (CheckData::IsCorrect(buf, checkSize, transCheckValue));
	}

    /*******************************************************************
    *          尝试解码网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： OnlyCommand
    * 功    能：尝试解码网络数据
    * 说    明：在还未知道发来的网络数据是什么的情况下
    * 		   尝试获得该请求的头部数据，以察看请求码
    * 参    数：
    * 			输入参数
    * 			buf —— 需要解码的网络数据
    * 			输出参数
    * 			pCtrlCmd —— 解码后的头数据，校验值总置为0
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 11：23
    *******************************************************************/
	static void OnlyCommand(const char *buf, PControlCmd pCtrlCmd)
	{
		memmove(pCtrlCmd, buf, sizeof(ControlCmd));

		// 校验强制设置为0
		pCtrlCmd->Check = 0;

	}

    /*******************************************************************
    *          解码网络无数据成功回应
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： SuccessResponse
    * 功    能：解码网络无数据成功回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf —— 需要解码的网络数据
    * 			输出参数
    * 			pAck —— 解码后的回应数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 13：39
    *******************************************************************/
	static bool Success(const char *buf, NormalAck &ack)
	{
		PNormalSuccessAck pAck = (PNormalSuccessAck)buf;

		ack.Set(*pAck);
		return true;
	}

    /*******************************************************************
    *          解码网络无数据失败回应
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： ErrorResponse
    * 功    能：解码网络无数据失败回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf —— 需要解码的网络数据
    * 			输出参数
    * 			pAck —— 解码后的回应数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 13：39
    *******************************************************************/
	static bool Error(const char *buf, NormalAck &ack)
	{
		// 返回失败及提示信息
		PNormalErrorAck pAck = (PNormalErrorAck)buf;
		ack.Set(*pAck);
		return true;
	}

	static bool DecodeResponse(const char *buf,
			NormalAck &ack,
			bool needCheck = true)
	{
		// 检测校验码是否正确
		if (needCheck && (!IsCheckCorrect(buf)))
		{
			// 如果校验码不正确，则返回false
			return false;
		}

		// 检查返回数据
	    PHeadDataWithErrorCode pHead = (PHeadDataWithErrorCode)buf;

	    if (g_success == pHead->ErrorCode)
		{
	    	// 回应错误码表示成功，设置一般性回应数据
			return Success(buf, ack);
		}
		else
		{
			// 回应错误码表示失败，设置一般性回应数据
			return (Error(buf, ack));
		}

	}

};
