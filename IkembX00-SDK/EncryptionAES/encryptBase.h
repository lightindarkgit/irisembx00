/*
 * encryptBase.h
 *
 *  Created on: 2014年4月10日
 *      Author: fjf
 */

#pragma once
#include <string.h>
#define UCHAR_MAX     0xff
class EncryptBase
{
public:
	EncryptBase();
	virtual ~EncryptBase();
public:
	virtual void encrypt(const char *in, char *result, size_t num, int mode = CBC) = 0;
	virtual void decrypt(const char *in, char *result, size_t num, int mode = CBC) = 0;
public:
    /**
     * AES's working-mode (also all other block ciphers' working-mode):
     *   ECB: Electronic Codebook mode
     *   CBC: Cipher Block Chaining mode
     *   CFB: Cipher Feedback mode
     *   OFB: Output Feedback mode
     *   CTR: Counter mode
     */
    enum { ECB = 0, CBC = 1, CFB = 2, OFB = 3, CTR = 4 };
    enum { DEFAULT_KEY_LENGTH = 16 };      // default key length
    enum { BLOCK_SIZE = 16 };              // block size
};


