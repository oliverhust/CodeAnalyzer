#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "Str.h"

#include "CA_Filter.h"
#include "CA_Filter_inc.h"
#include "md5.h"
#include "CA_Run.h"
#include "CA_Filter_Pattern.h"
#include "CA_Filter_Proc.h"
#include "CA_Filter_Data.h"

/* ��¼�����˵��ĸ���澯���� */
static int g_aiMessageHasMatch[CA_FILTER_MESSAGE_MAX] = { 0 };

static void _CA_Filter_Puts(char *pcSeg, FILE *stream)
{
	fprintf_s(stream, "%s\n", pcSeg);
	return;
}

/* ����һ���Σ�����öε�`MD5`���ļ����ʹ�������,�����������򷵻ش��� */
static errno_t _CA_Filter_Proc_GetSegFeature(IN char *pcSeg, OUT CA_FILTER_SEG_GET_S *pstCfg)
{
	CA_FILTER_SEG_GET_S stCfg, stOut;
	int iOffset = 0, iOffTmp, iOutOff = 0;
	errno_t err = ERROR_SUCCESS, err_t;
	BOOL bHasMatch = BOOL_FALSE, bHasMD5 = BOOL_FALSE;
	char szLine[LINE_MAXSIZE] = { 0 }, szCalMD5[SEGMENT_MAXSIZE] = { 0 };
	
	while (0 != (iOffTmp = Str_Get_FirstLine(pcSeg + iOffset, szLine, sizeof(szLine))))
	{
		iOffset += iOffTmp;
		/* �ָ��һ���н��д��� */
		/*iLineLen = pcLine + 1 - pcSeg;
		strncpy_s(szLine, sizeof(szLine), pcSeg + iOffset, iLineLen);
		iOffset += iLineLen;*/

#ifndef CA_FILTER_PROC_SHORTLINE
		/* ������ж�����������������ȡ */
		if (BOOL_TRUE == CA_Filter_Pattern_IsShortLine(szLine))
		{
			err = ERROR_FAILED;
			break;
		}
#endif
		memset(&stCfg, 0, sizeof(stCfg));
		err_t = CA_Filter_Pattern_LineGetAndModify(szLine, &stCfg, szCalMD5 + iOutOff, sizeof(szCalMD5) - iOutOff);
		iOutOff = strlen(szCalMD5);
		/* ������һ��δƥ����� */
		if (ERROR_SUCCESS != err_t)
		{
			continue;
		}
		/* ����һ��ƥ���� */
		bHasMatch = BOOL_TRUE;
		if (stCfg.szMD5[0] != '\0')
		{
			/* ����������б������MD5��ֱ���Ե�ǰCfg���أ�����Ҫ�ٶ��� */
			bHasMD5 = BOOL_TRUE;
			memcpy(pstCfg, &stCfg, sizeof(stCfg));
			break;
		}
		memcpy(&stOut, &stCfg, sizeof(stCfg));

	}

	if (BOOL_FALSE == bHasMatch || ERROR_SUCCESS != err)
	{
		/* ȫ������һ����������,��Ϊδʶ��Ķ� */
		return ERROR_FAILED;
	}
	else if (BOOL_TRUE != bHasMD5)
	{
		/* ���ֳ�MD5,�Լ����� */
		MD5_String(szCalMD5, strlen(szCalMD5), stOut.szMD5);
		memcpy(pstCfg, &stOut, sizeof(stOut));
	}

	return ERROR_SUCCESS;
}

/* ��ȥ��ĩβ���еĶ�  ��ͨSeg�δ�������������MD5,�ļ�������ѯ���ݿ������� ͳ�Ƹ澯 */
void CA_Filter_Proc_Seg(char *pcSeg, FILE *stream)
{
	CA_FILTER_SEG_GET_S stCfg;
	CA_FILTER_MD5_S stMD5Data;

	memset(&stCfg, 0, sizeof(stCfg));
	memset(&stMD5Data, 0, sizeof(stMD5Data));

	//����öε�`MD5`���ļ����ʹ�������  
	if (ERROR_SUCCESS != _CA_Filter_Proc_GetSegFeature(pcSeg, &stCfg))
	{
		_CA_Filter_Puts(pcSeg, stream);
		return;
	}
	
	//��ѯ���ݿ�����Ƿ���� ��û����ֱ��Puts�˳�
	strcpy_s(stMD5Data.szMD5, CA_FILTER_MD5_STR_MAX, stCfg.szMD5);
	if (ERROR_SUCCESS != CA_Filter_Data_IsExist_MD5(&stMD5Data) && 
		ERROR_SUCCESS != CA_Filter_Data_IsExist_FileName(stCfg.szFileName))
	{
		_CA_Filter_Puts(pcSeg, stream);
		return;
	}

	//ͳ�ƺ��Եĸ澯���ͼ�����
	g_aiMessageHasMatch[stCfg.enType]++;
}

void _CA_Filter_Proc_Modify_Note(char(*ppcSubStrings)[STR_PCRE_MAXLINE], int iStringNum)
{
	int iErr, iWarn, iInfo, iNote;

	iErr = atoi(ppcSubStrings[2]) - g_aiMessageHasMatch[CA_FILTER_MESSAGE_ERROR];
	iWarn = atoi(ppcSubStrings[3]) - g_aiMessageHasMatch[CA_FILTER_MESSAGE_WARNING];
	iInfo = atoi(ppcSubStrings[4]) - g_aiMessageHasMatch[CA_FILTER_MESSAGE_INFO];
	iNote = atoi(ppcSubStrings[5]) - g_aiMessageHasMatch[CA_FILTER_MESSAGE_NOTE];

	sprintf_s(ppcSubStrings[1], STR_PCRE_MAXLINE, "%d", iErr + iWarn + iInfo + iNote);
	sprintf_s(ppcSubStrings[2], STR_PCRE_MAXLINE, "%d", iErr);
	sprintf_s(ppcSubStrings[3], STR_PCRE_MAXLINE, "%d", iWarn);
	sprintf_s(ppcSubStrings[4], STR_PCRE_MAXLINE, "%d", iInfo);
	sprintf_s(ppcSubStrings[5], STR_PCRE_MAXLINE, "%d", iNote);	

	return;
}

/* Note��Ϣ��������ԭ����ͳ����Ϣ�޸� ����Ϊ�����Ρ��ϳ�һ�εĶ� */
void CA_Filter_Proc_Note(char *pcSeg, FILE *stream)
{
	int aiOrigin[CA_FILTER_MESSAGE_MAX] = { 0 }, iOffset = 0;
	char szLine[LINE_MAXSIZE] = { 0 }, szNew[LINE_MAXSIZE] = { 0 };
	char *pcXuXian = "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
	
	//��ӡ��һ��,ȡ���ڶ���
	iOffset += Str_Get_FirstLine(pcSeg + iOffset, szLine, sizeof(szLine));
	fprintf(stream, "%s", szLine);
	iOffset += Str_Get_FirstLine(pcSeg + iOffset, szLine, sizeof(szLine));

	//��ȡ���޸�ԭ���ĸ澯���ͼ�����
	CA_Filter_Pattern_ModifyNote(_CA_Filter_Proc_Modify_Note, szLine, szNew, sizeof(szNew));
	fprintf(stream, "%s", szNew);

	//����ʾ���ָ澯��ͳ����Ϣ�����������ߴ���
	fprintf(stream, "\n%s\n", pcXuXian);

	//���ԭͳ����Ϣ
	memset(g_aiMessageHasMatch, 0, sizeof(g_aiMessageHasMatch));

	return;
}

/* ����һ���Σ�ѧϰ����Σ��޷�ʶ���򷵻�ERROR_FAILED */
errno_t CA_Filter_Proc_Study(char *pcSeg)
{
	CA_FILTER_SEG_GET_S stCfg;
	CA_FILTER_MD5_S stMD5;
	errno_t err;

	memset(&stCfg, 0, sizeof(stCfg));
	memset(&stMD5, 0, sizeof(stMD5));

	if (ERROR_SUCCESS != _CA_Filter_Proc_GetSegFeature(pcSeg, &stCfg))
	{
		return ERROR_FAILED;
	}

	strcpy_s(stMD5.szMD5, CA_FILTER_MD5_STR_MAX, stCfg.szMD5);
	err = CA_Filter_Data_Add_MD5(&stMD5);

	return err;
}

/* ���������ļ����浽�ڴ����ݿ��� */
errno_t CA_Filter_Proc_PraseCfgFile(IN char *pcLine)
{
	errno_t err = ERROR_FAILED;
	CA_FILTER_MD5_S stMD5;

	if (NULL != Str_BeginWith(pcLine, CA_FILTER_CFG_COMMENT))
	{
		return ERROR_SUCCESS;
	}
	else if (NULL != Str_BeginWith(pcLine, CA_FILTER_CFG_FILENAME))
	{
		/* ��ӵ����ݿ� Ҫ���Ե��ļ� */
		err = CA_Filter_Data_Add_FileName(pcLine + strlen(CA_FILTER_CFG_FILENAME));
	}
	else if (CA_FILTER_MD5_STR_MAX == strlen(pcLine) + 1)
	{
		/* ��ӵ����ݿ� MD5 */
		memset(&stMD5, 0, sizeof(stMD5));
		strcpy_s(stMD5.szMD5, sizeof(stMD5.szMD5), pcLine);
		err = CA_Filter_Data_Add_MD5(&stMD5);
	}
	return err;
}

/* ��ȡһ���ļ����ļ�ÿ�о�ΪҪ����CA�澯�Ĵ����ļ����������ݿ� */
void CA_Filter_Proc_IgnoreFile(FILE *fil)
{
	int iLen;
	char szLine[LINE_MAXSIZE] = { 0 };

	while (0 != fgets(szLine, sizeof(szLine), fil))
	{
		/* ȥ������ */
		if (BOOL_TRUE == CA_Filter_Pattern_IsAllSpace(szLine))
		{
			continue;
		}
		
		/* ȥ��ĩβ�Ļ��з� */
		iLen = strlen(szLine);
		if ('\n' == szLine[iLen - 1])
		{
			szLine[iLen - 1] = '\0';
			if (2 <= iLen && '\r' == szLine[iLen - 2])
			{
				szLine[iLen - 2] = '\0';
			}
		}		

		/* ���������ļ����浽�ڴ����ݿ��� */
		(void)CA_Filter_Proc_PraseCfgFile(szLine);
		
	}

	return;
}

/* ���浱ǰ�����е��������õ����� */
errno_t CA_Filter_Proc_SaveCfgFile(FILE *fil)
{
	int i, iCount;
	CA_FILTER_MD5_S stMD5;
	char szFileName[CA_FILTER_FILENAME_MAX] = { 0 };	

	memset(&stMD5, 0, sizeof(stMD5));

	/* ��ӡ���Ե��ļ��� */
	fprintf_s(fil, "%s\n", CA_FILTER_CFG_COMMENT_FILE_HEAD);
	iCount = CA_Filter_Data_Count_FileName();
	for (i = 0; i < iCount; i++)
	{
		CA_Filter_Data_Get_FileName(i, szFileName);
		fprintf_s(fil, "%s%s\n", CA_FILTER_CFG_FILENAME, szFileName);
	}
	fprintf_s(fil, "\n");
	
	/* ��ӡMD5 */
	fprintf_s(fil, "%s\n", CA_FILTER_CFG_COMMENT_MD5_HEAD);
	iCount = CA_Filter_Data_Count_MD5();
	for (i = 0; i < iCount; i++)
	{
		CA_Filter_Data_Get_MD5(i, &stMD5);
		fprintf_s(fil, "%s\n", stMD5.szMD5);
	}
	fprintf_s(fil, "\n");	

	return ERROR_SUCCESS;
}