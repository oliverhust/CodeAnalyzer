#ifndef __mystr_h__
#define __mystr_h__

/* �ҵ��ַ������  */

/* ·���ָ�� */
#define MYSTR_PATH_SPILT '\\'

#define MYSTR_CACHE_SIZE 512

/* wchar*���ַ���תchar*���ַ��� */
char *Mystr_Wchar2Char(wchar_t *WStr, char *CStr, int StrSize);

/* char*���ַ���תwchar*���ַ��� */
wchar_t *Mystr_Char2Wchar(char *CStr, wchar_t *WStr, int WStrSize);

/* ��ȡ��·�����ļ���������·�� ĩβ����\ szInPath��szOutBase������ͬ */
char *Mystr_GetBasename_Char(char *szInPath, char* szOutBase, int OutSize);

/* �Դ��ڴ���ַ������Ӻ���  ��֧�ֶ��߳�/���� */
char *Mystr_Cat(char *str1, char *str2);

/* �Դ��ڴ�Ķ��ַ������Ӻ��� ���һ������������0��NULL ��֧�ֶ��߳�/���� */
char *Mystr_NCat(char *pcStr1, ...);

/* ��ӡ�ļ����� */
errno_t Str_PrintFile(char *pcPath);

#endif