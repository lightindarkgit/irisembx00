/******************************************************************************************
** 文件名:   AES.h
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-04-08
** 修改人:
** 日  期:
** 描  述: AES加密算法的头文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:这个AES算法加密是一个比较标准的算法，学习很清晰，但是加密较慢；
**       所以提供另外一个速度快的加密AES算法及复杂的加密方式
**
*****************************************************************************************/
#pragma once
class AES
{
public:
	AES(unsigned char* key);
	virtual ~AES();
	unsigned char* Cipher(unsigned char* input);
	unsigned char* InvCipher(unsigned char* input);
	void* Cipher(void* input, int length=0);
	void* InvCipher(void* input, int length);

private:
	unsigned char Sbox[256];
	unsigned char InvSbox[256];
	unsigned char w[11][4][4];

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
};
