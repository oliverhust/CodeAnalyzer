#pragma once

#include <windows.h>

/* MD5����ֽ���(���ַ���) */
#define MD5_BYTES			16

/* MD5�ַ������� */
#define MD5_STR_LEN			33

/* MD5 context. */
typedef struct {
	UINT32 state[4]; /* state(ABCD) */
	UINT32 count[2]; /* number of bits, modulo 2 ^ 64 (lsb first) */
	unsigned char buffer[64]; /* input buffer */
}MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char[16], MD5_CTX *);

/* ���Ϊ32���ַ��� */
void MD5_String(char *pcInput, int iInputLen, char pcOutStr[MD5_STR_LEN]);
