#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "Str.h"

#define ERROR_SUCCESS							0
#define ERROR_FAILED							1
#define STR_READ_FILE_EACH_SIZE					4096

char StrCache[MYSTR_CACHE_SIZE] = { 0 };

char *Str_Wchar2Char(wchar_t *WStr, char *CStr, int StrSize)
{
	size_t len = wcslen(WStr) + 1;
	size_t converted = 0;

	if ((unsigned int)StrSize < len * sizeof(char))
	{
		return NULL;
	}

	wcstombs_s(&converted, CStr, len, WStr, _TRUNCATE);

	CStr[StrSize - 1] = '\0';

	return CStr;
}

wchar_t *Str_Char2Wchar(char *CStr, wchar_t *WStr, int WStrSize)
{
	size_t len = strlen(CStr) + 1;
	size_t converted = 0;

	if ((unsigned int)WStrSize < len * sizeof(wchar_t))
	{
		return NULL;
	}

	mbstowcs_s(&converted, WStr, len, CStr, _TRUNCATE);

	WStr[WStrSize / sizeof(wchar_t)-1] = 0;

	return WStr;
}

char *Str_GetBasename_Char(char *szInPath, char* szOutBase, int OutSize)
{
	int i, len;

	len = strlen(szInPath);
	if (OutSize < len + 1)
	{
		return NULL;
	}

	for (i = len - 1; i >= 0; i--)
	{
		if (MYSTR_PATH_SPILT == szInPath[i])
		{
			break;
		}
	}

	if (0 < i)
	{
		memcpy(szOutBase, szInPath, i);
		szOutBase[i] = '\0';
	}
	else
	{
		szOutBase[0] = '\0';
	}


	return szOutBase;
}

char *Str_GetFilename_Char(char *szInPath, char* szOutFileName, int OutSize)
{
	int i, len;

	len = strlen(szInPath);
	if (OutSize < len + 1)
	{
		return NULL;
	}

	for (i = len - 1; i >= 0; i--)
	{
		if (MYSTR_PATH_SPILT == szInPath[i])
		{
			break;
		}
	}
	i++;
	memcpy(szOutFileName, szInPath + i, len - i);
	szOutFileName[len - i] = '\0';	

	return szOutFileName;
}


char *Str_Cat(char *str1, char *str2)
{
	int i = 0;

	i += sprintf_s(StrCache + i, sizeof(StrCache)-i, "%s", str1);
	i += sprintf_s(StrCache + i, sizeof(StrCache)-i, "%s", str2);

	return StrCache;

}

char *Str_NCat(char *pcStr1, ...)
{
	va_list stAp;
	int off = 0;
	char *pcArgs = pcStr1;

	memset(StrCache, 0, sizeof(StrCache));

	/* ��ʼ������Ҫ��ȡ...ǰ�ĵ�һ�������� */
	va_start(stAp, pcStr1);

	while (NULL != pcArgs)
	{
		/* ������������ӡ��ȫ�ֱ��� */
		off += sprintf_s(StrCache + off, sizeof(StrCache)-off, "%s", pcArgs);

		/* ��ȡ��һ������ */
		pcArgs = va_arg(stAp, char *);
	}

	va_end(stAp);

	return StrCache;
}

/* ��ӡ�ļ����� */
errno_t Str_PrintFile(char *pcPath)
{
	FILE *fil;
	int iRead;
	char szBuf[STR_READ_FILE_EACH_SIZE + 1] = { 0 };

	if (0 != fopen_s(&fil, pcPath, "r") || NULL == fil)
	{
		printf("Open file %s error.\r\n", pcPath);
		return ERROR_FAILED;
	}

	/* ÿ�ζ�ȡ4096�ֽ� */
	while (0 != (iRead = fread(szBuf, sizeof(char), sizeof(szBuf) - 1, fil)))
	{
		szBuf[iRead] = '\0';
		printf("%s", szBuf);
	}
	printf("\r\n");

	fclose(fil);

	return ERROR_SUCCESS;
}

/* ���pcPatt��pcHead��ͷ�򷵻�ͷ��������ַ��������򷵻�NULL */
char *Str_BeginWith(char *pcPatt, char *pcHead)
{
	unsigned int uiLen;

	uiLen = strlen(pcHead);
	if (0 == strncmp(pcPatt, pcHead, uiLen))
	{
		return (pcPatt + uiLen);
	}

	return NULL;
}

/* �ַ�����ȫ�����������������򲻿�����������ҷ���ERROR_FAILED */
errno_t Str_SaveCopy(char *pcDst, int iDstSize, char *pcSrc)
{
	int iLen;

	iLen = strlen(pcSrc);
	if (iLen < iDstSize)
	{
		memcpy(pcDst, pcSrc, iLen + 1);
		pcDst[iLen] = '\0';
		return ERROR_SUCCESS;
	}
	
	memcpy(pcDst, pcSrc, iDstSize - 1);
	pcDst[iDstSize - 1] = '\0';
	return ERROR_FAILED;

}

/* ��ȡ�ַ����ĵ�һ�е�pcFirstLine,���ظ����ַ����ĳ��� */
int Str_Get_FirstLine(char *pcContent, char *pcFirstLine, int iFirstLineSize)
{
	int iLen;
	char *pcLine;

	pcLine = strchr(pcContent, STR_CHARACTER_NEWLINE);
	if (NULL == pcLine)
	{
		Str_SaveCopy(pcFirstLine, iFirstLineSize, pcContent);
		return strlen(pcContent);
	}

	iLen = pcLine + 1 - pcContent;
	if (iLen >= iFirstLineSize)
	{
		strncpy_s(pcFirstLine, iFirstLineSize, pcContent, iFirstLineSize - 1);
		pcFirstLine[iFirstLineSize - 1] = '\0';
	}
	else
	{
		strncpy_s(pcFirstLine, iFirstLineSize, pcContent, iLen);
		pcFirstLine[iLen] = '\0';
	}	

	return iLen;
}

/* �ַ������Ƿ�˳�����ĳЩ�ַ���,���һ������������(char *)0,*/
/* ƥ�䷵�����һ��ƥ���ַ���ĩβ֮����ַ�������ƥ�䷵��NULL */
/* ����������п��ַ���""������ */
char *Str_Contains_N(char *pcInput, ...)
{
	va_list stAp;
	int iOffset = 0;
	errno_t err = ERROR_SUCCESS;
	char *pcArg, *pcMatch = pcInput;

	/* ��ʼ������Ҫ��ȡ...ǰ�ĵ�һ�������� */
	va_start(stAp, pcInput);

	while (NULL != (pcArg = va_arg(stAp, char *)))
	{
		/* �Ƿ�ƥ�� */
		pcMatch = strstr(pcMatch, pcArg);
		if (NULL == pcMatch)
		{
			break;
		}
		pcMatch += strlen(pcArg);
	}

	va_end(stAp);

	return pcMatch;
}

//===============================����ʽ��صĺ���==============================
#ifdef STR_USE_PCRE

/* ע��һ��pcre���滻 ����pcre�ṹ��ָ��(ʧ�ܷ���NULL) ��Σ�ƥ������ʽ, pcre�ı���option (��PCRE_UTF8..)*/
pcre *Str_PcreModify_Register(const char *pcPattern, int iOption)
{
	pcre *pstPcre;
	int iErroffset;
	const char* pcError;

	/* ��������ʽ */
	pstPcre = pcre_compile(pcPattern, iOption, &pcError, &iErroffset, NULL);
	if (NULL == pstPcre)
	{
		printf("compile regex expression \'%s\' error.\r\n", pcPattern);
		return NULL;
	}

	return pstPcre;
}

/* pcre�޸ģ��޸�ƥ����Ӵ�\1,\2,\3.. */
/* ��Σ�pcreָ��,Modify�Ļص�����, �����ı�������ı�������ı����ַ�����С */
/* ƥ�����޸ĳɹ�����ERROR_SUCCESS��δƥ�䷵��ERROR_STR_NO_MATCH ��������ԭ�ĵ�pcOut */
errno_t Str_PcreModify(pcre *pstPcre, STR_PCREMODIFY_PROC_PF pfModify, char *pcContent, char *pcOut, int size)
{
	int aiOvector[3 * STR_PCRE_MAX_SUB] = { 0 }, iLen, i, iRc, iOutOff = 0, iTmp;
	errno_t err = ERROR_SUCCESS;
	char (*ppcSubs)[STR_PCRE_MAXLINE];

	iLen = strlen(pcContent);
	if (iLen >= STR_PCRE_MAXLINE || NULL == pstPcre)
	{
		return ERROR_FAILED;
	}

	iRc = pcre_exec(pstPcre, NULL, pcContent, iLen, 0, 0, aiOvector, sizeof(aiOvector) / sizeof(int));
	if (iRc < 0)
	{
		/* δƥ�䣬����ԭ�ĵ�pcOut */
		strcpy_s(pcOut, size, pcContent);
		return ERROR_STR_NO_MATCH;
	}

	/* ������\0,\1,\2... */
	ppcSubs = (char (*)[512])malloc(iRc * STR_PCRE_MAXLINE);
	if (NULL == ppcSubs)
	{
		printf("No enough resource to malloc.\r\n");
		return ERROR_FAILED;
	}

	for (i = 0; i < iRc; i++)
	{
		pcre_copy_substring(pcContent, aiOvector, iRc, i, ppcSubs[i], STR_PCRE_MAXLINE);
	}
	
	/* ���ûص����� */
	pfModify(ppcSubs, iRc);

	/* ������װ�ַ��� */
	//��û�в�����ʱ������ص������޸�ƥ���ַ���
	if (iRc <= 1)
	{
		iOutOff = aiOvector[0];
		memcpy(pcOut, pcContent, iOutOff);

		iTmp = strlen(ppcSubs[0]);
		if (iOutOff + iTmp >= size)
		{
			return ERROR_FAILED;
		}
		memcpy(pcOut + iOutOff, ppcSubs[0], iTmp);
		iOutOff += iTmp;

		if (iOutOff + iLen - aiOvector[1] >= size)
		{
			return ERROR_FAILED;
		}
		strcpy_s(pcOut + iOutOff, size - iOutOff, pcContent + aiOvector[1]);
		return ERROR_SUCCESS;
	}

	//������ʽ���в�����ʱ����ʹ�ûص������е�ppcSubs[0]�����Ǹ��ݲ�����\1,\2..����ƴ��
	iOutOff = aiOvector[2 * 1 + 0];
	memcpy(pcOut, pcContent, iOutOff);	

	for (i = 1; i < iRc - 1; i++)
	{
		iTmp = strlen(ppcSubs[i]);
		if (iOutOff + iTmp >= size)
		{
			err = ERROR_FAILED;
			break;
		}
		memcpy(pcOut + iOutOff, ppcSubs[i], iTmp);
		iOutOff += iTmp;

		iTmp = aiOvector[2 * (i + 1) + 0] - aiOvector[2 * i + 1];
		if (iTmp <= 0 || iTmp > iLen || iOutOff + iTmp >= size)
		{
			err = ERROR_FAILED;
			break;
		}
		memcpy(pcOut + iOutOff, pcContent + aiOvector[2 * i + 1], iTmp);
		iOutOff += iTmp;		
	}

	if (ERROR_SUCCESS != err)
	{
		return err;
	}

	iTmp = strlen(ppcSubs[iRc - 1]);
	if (iOutOff + iTmp >= size)
	{
		return ERROR_FAILED;
	}
	memcpy(pcOut + iOutOff, ppcSubs[iRc - 1], iTmp);
	iOutOff += iTmp;

	if (iOutOff + iLen - aiOvector[2 * (iRc - 1) + 1] >= size)
	{
		return ERROR_FAILED;
	}
	strcpy_s(pcOut + iOutOff, size - iOutOff, pcContent + aiOvector[2 * (iRc - 1) + 1]);

	free(ppcSubs);
	return ERROR_SUCCESS;
}

/* ע��һ��pcre���滻 ��Σ�pcre�ṹ��ָ��ĵ�ַ */
void Str_PcreModify_UnRegister(pcre *pstPcre)
{
	pcre_free(pstPcre);
	return;
}


#endif



/* ���� */
/*
int main(int argc, char *argv[])
{

	char c;

	puts(Str_NCat("You ", "are ", "a ", "pig ", "!!!", NULL));
	puts(Str_NCat("Hello", NULL));
	puts(Str_NCat(NULL));
	puts(Str_NCat("1234567"));
	c = 'Q';
	putchar(c);

	printf("============Str_FilePrint Test================\r\n");
	Str_PrintFile("CA_Result_Origin.txt");
	printf("============Str_FilePrint Test Finish================\r\n");

	char *patt = "--- 19:06:29.44 ALL CAnalyzer E:\\SVTI\\Source\\VPN\\src\\sbin\\ipsec\r\n"
		"        Total 253 messages ( 5 Files), 240 Error, 12 Warning, 1 Info, 0 Note. [0/56] \r\n";

	
	if (Str_Contains_N(patt, "---", "\\S", "\\S", "\\V", "\\", "\\", "\\", "\r\n", 0))
		{printf("Contains.\r\n");}else{printf("No Match.\r\n");}

	if (Str_Contains_N(patt, "\\", "\\", "\\", "\\", "\\", "\\", "\\", "\r\n", 0))
		{printf("Contains.\r\n");}else{printf("No Match.\r\n");}

	if (Str_Contains_N(patt, "[0/56] \r\n", "", 0))
		{printf("Contains.\r\n");}else{printf("No Match.\r\n");}

	if (Str_Contains_N(patt, "", "---", "messages", 0))
		{printf("Contains.\r\n");}else{printf("No Match.\r\n");}

	(void)getchar();

	return 0;
}
*/
/*
void  _str_modify(char(*ppcSubStrings)[STR_PCRE_MAXLINE], int iStringNum)
{
	int i;
	
	//printf("StringNum = %d\r\n", iStringNum);
	//printf("Match \\0 = %s\r\n", ppcSubStrings[0]);

	for (i = 1; i < iStringNum; i++)
	{
		//printf("Sub[%d] = %s\r\n", i, ppcSubStrings[i]);
		sprintf_s(ppcSubStrings[i], STR_PCRE_MAXLINE, "%d", 9 * atoi(ppcSubStrings[i]));
	}
	//printf("\r\n");
	

	//ɾ������
	//for (i = 1; i < iStringNum; i++)
	//{
	//	ppcSubStrings[i][0] = '\0';
	//}

	return;
}
//�޲�������滻����ֻ��\0��
void  _str_modify2(char(*ppcSubStrings)[STR_PCRE_MAXLINE], int iStringNum)
{
	//printf("StringNum = %d\r\n", iStringNum);
	//printf("Match \\0 = %s\r\n", ppcSubStrings[0]);

	strcpy_s(ppcSubStrings[0], STR_PCRE_MAXLINE, "Hello,world!");
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	pcre *pstPcre;
	int iOff = 0,  iTmp;
	char szBuf[STR_PCRE_MAXLINE], szOut[STR_PCRE_MAXLINE];
	char *pcContent = "\
  TUNNEL_KDBG(TUNNEL_DEBUG_ERROR, \"\\r\\n Failed to allocate memory to save drv context.\");\r\n\
    E:\\SVTI\\Source\\NETFWD\\src\\kernel\\tunnel\\tunnel_kdrv.c L2944 Info 4710 \r\n\
        Use 'ERROR_NO_ENOUGH_RESOURCE' etc. Ref. WeekofBugs 20140704 05\r\n\
\r\n\
Note 900 \r\n\
        Total 2 messages produced, 0 Error, 0 Warning, 2 Info, 0 Note.\r\n\
\r\n\
    Count  No    Type\r\n\
    2      4710  Info\r\n\
\r\n\
--- BEG 20:43:24.14  CAnalyzer:ALL  SrcVer: B66000\r\n\
--- END 20:46:35.10  2015-07-28\r\n\
--- Lintware    Rev  2014-09-25 11:10:04\r\n\
--- Codeware    Rev  2014-09-24 11:36:23\r\n\
--- Checkware   Rev  2014-09-18 15:12:23\
";

	char *pcPattern = "(\\d+) messages produced, (\\d+) Error, (\\d+) Warning, (\\d+) Info, (\\d+)";
	char *pcPattern2 = "\\d+ messages produced, \\d+ Error,";

	pstPcre = Str_PcreModify_Register(pcPattern, PCRE_UTF8);

	while (0 < (iTmp = Str_Get_FirstLine(pcContent + iOff, szBuf, sizeof(szBuf))))
	{
		iOff += iTmp;
		if (ERROR_FAILED == Str_PcreModify(pstPcre, _str_modify, szBuf, szOut, sizeof(szOut)))
		{
			printf("\r\n\r\n====================Error!=======================\r\n\r\n");
			continue;
		}
		printf("%s", szOut);
	}

	Str_PcreModify_UnRegister(pstPcre);

	(void)getchar();

	return 0;
}
*/
