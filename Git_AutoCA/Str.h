#ifndef __str_h__
#define __str_h__

/* �ҵ��ַ������  */

/* ·���ָ�� */
#define MYSTR_PATH_SPILT '\\'

#define MYSTR_CACHE_SIZE 512

/* wchar*���ַ���תchar*���ַ��� */
char *Str_Wchar2Char(wchar_t *WStr, char *CStr, int StrSize);

/* char*���ַ���תwchar*���ַ��� */
wchar_t *Str_Char2Wchar(char *CStr, wchar_t *WStr, int WStrSize);

/* ��ȡ��·�����ļ���������·�� ĩβ����\ szInPath��szOutBase������ͬ */
char *Str_GetBasename_Char(char *szInPath, char* szOutBase, int OutSize);

/* �Դ��ڴ���ַ������Ӻ��� */
char *Str_Cat(char *str1, char *str2);

/* �Դ��ڴ��������ַ������Ӻ��� ���һ������������0 */
char *Str_NCat(char *pcStr1, ...);

/* ��ӡ�ļ����� */
errno_t Str_PrintFile(char *pcPath);

#endif