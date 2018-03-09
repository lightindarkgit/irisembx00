/******************************************************************************************
** 文件名:   Encryption.h
×× 主要类:   Encryption
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-04-08
** 修改人:
** 日  期:
** 描  述: AES加密算法的封装类头文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#pragma once
#include <memory>
#include <string>
class EncryptBase;
class Encryption
{
public:
	Encryption();
	virtual ~Encryption();
public:
	bool CreateInstance(std::string name);
public:
	//设置密钥
	int MakeCipherCode(const char*key,int len = 16);
	int Encrypt(const char *plaintext ,char *ciphertext,int len);//加密
	int Derypt(const char *ciphertext,char *plaintext ,int len);//解密
private:
	std::shared_ptr<EncryptBase> pEncrypt;
};


