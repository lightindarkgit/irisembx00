/******************************************************************************************
** 文件名:   Encryption.cpp
×× 主要类:   Encryption
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-04-08
** 修改人:
** 日  期:
** 描  述: AES加密算法的封装类实现文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
//
#include "encryption.h"
#include "classFactory.h"
#include "encryptBase.h"
#include "../Common/Exectime.h"

Encryption::Encryption()
{
	Exectime etm(__FUNCTION__);
	// TODO Auto-generated constructor stub

}

Encryption::~Encryption()
{
	Exectime etm(__FUNCTION__);
	// TODO Auto-generated destructor stub
}

bool Encryption::CreateInstance(std::string name)
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	this->pEncrypt = SIMPLY_CREATE_OBJECT(EncryptBase,name);//EncryptBase
	if (nullptr == this->pEncrypt)
	{
		bRet = false;
	}

	return bRet;
}

int Encryption::Encrypt(const char *plaintext ,char *ciphertext,int len)
{
	Exectime etm(__FUNCTION__);
	int iRet = 1;
	this->pEncrypt->encrypt(plaintext,ciphertext,len);
	return iRet;
}

int Encryption::Derypt(const char *ciphertext,char *plaintext ,int len)
{
	Exectime etm(__FUNCTION__);
	int iRet = 1;
	this->pEncrypt->decrypt(ciphertext,plaintext,len);
	return iRet;
}
