#ifndef __str_h__
#define __str_h__

/* �ҵ��ַ������  */

/* �����Ҫ�õ�������صĺ��������ڹ����ж���STR_USE_PCRE */
#define STR_USE_PCRE

/* ·���ָ�� */
#define MYSTR_PATH_SPILT			'\\'

/* �ָ��еı�ʶ�� */
#define STR_CHARACTER_NEWLINE		'\n'

/* �Դ����建��Ĵ�С */
#define MYSTR_CACHE_SIZE			512

/* �����붨�� ÿ��ģ�����65535�� */
#define MODULE_ERROR_STR			0x00100000
#define STR_ERROR_TYPE_ERROR		(1UL << 12)
#define STR_ERROR_TYPE_WARNING		(2UL << 12)
#define STR_ERROR_TYPE_INFO			(3UL << 12)
#define ERROR_STR_NO_MATCH			(MODULE_ERROR_STR | STR_ERROR_TYPE_INFO | 0x001)

/* wchar*���ַ���תchar*���ַ��� */
char *Str_Wchar2Char(wchar_t *WStr, char *CStr, int StrSize);

/* char*���ַ���תwchar*���ַ��� */
wchar_t *Str_Char2Wchar(char *CStr, wchar_t *WStr, int WStrSize);

/* ��ȡ��·�����ļ���������·�� ĩβ����\ szInPath��szOutBase������ͬ */
char *Str_GetBasename_Char(char *szInPath, char* szOutBase, int OutSize);

/* ��ȡ��·�����е��ļ��� (��Str_GetBasename_Char�����෴)*/
char *Str_GetFilename_Char(char *szInPath, char* szOutFileName, int OutSize);

/* �Դ��ڴ���ַ������Ӻ��� */
char *Str_Cat(char *str1, char *str2);

/* �Դ��ڴ��������ַ������Ӻ��� ���һ������������0 */
char *Str_NCat(char *pcStr1, ...);

/* ��ӡ�ļ����� */
errno_t Str_PrintFile(char *pcPath);

/* ���pcPatt��pcHead��ͷ�򷵻�ͷ��������ַ��������򷵻�NULL */
char *Str_BeginWith(char *pcPatt, char *pcHead);

/* ��ȡ�ַ����ĵ�һ�е�pcFirstLine,���ظ����ַ����ĳ��� */
int Str_Get_FirstLine(char *pcContent, char *pcFirstLine, int iFirstLineSize);

/* �ַ������Ƿ�˳�����ĳЩ�ַ���,���һ������������(char *)0,*/
/* ƥ�䷵�����һ��ƥ���ַ���ĩβ֮����ַ�������ƥ�䷵��NULL */
/* ����������п��ַ���""������ */
char *Str_Contains_N(char *pcInput, ...);

//===============================����ʽ��صĺ���==============================
#ifdef STR_USE_PCRE
#include "pcre.h"

/* ����ÿ���������ַ�����С */
#define STR_PCRE_MAXLINE					512

/* ���������Ӵ��� ��\0 */
#define STR_PCRE_MAX_SUB					10	

/* Str_PcreModify�Ļص����������Ϊ�Ӵ���\0,\1,\2..��ɵ��ַ������飬��\0��������ֱ���޸ģ�,�ַ����ĸ��� */
typedef void(*STR_PCREMODIFY_PROC_PF)(char (*ppcSubStrings)[STR_PCRE_MAXLINE], int iStringNum);

/* ע��һ��pcre���滻 ����pcre�ṹ��ָ��(ʧ�ܷ���NULL) ��Σ�ƥ������ʽ, pcre�ı���option (��PCRE_UTF8..)*/
pcre *Str_PcreModify_Register(const char *pcPattern, int iOption);

/* ע��һ��pcre���滻 ��Σ�pcre�ṹ��ָ��ĵ�ַ */
void Str_PcreModify_UnRegister(pcre *pstPcre);

/* pcre�޸ģ��޸�ƥ����Ӵ�\1,\2,\3.. */
/* ��Σ�pcreָ��,Modify�Ļص�����, �����ı�������ı�������ı����ַ�����С */
/* ƥ�����޸ĳɹ�����ERROR_SUCCESS��δƥ�䷵��ERROR_STR_NO_MATCH ��������ԭ�ĵ�pcOut */
errno_t Str_PcreModify(pcre *pstPcre, STR_PCREMODIFY_PROC_PF pfModify, char *pcContent, char *pcOut, int size);


#endif

#endif