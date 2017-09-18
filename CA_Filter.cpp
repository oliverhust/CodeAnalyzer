// CA_Filter.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>
#include <time.h>
#include "Str.h"
#include "md5.h"

#include "CA_Filter.h"
#include "CA_Filter_inc.h"
#include "CA_Run.h"
#include "CA_Filter_Pattern.h"
#include "CA_Filter_Proc.h"
#include "CA_Filter_Data.h"

/* CA�ű������� */
static char g_szCAnalyzerBat[CA_FILTER_FILENAME_MAX] = { CA_FILTER_BAT_FILE };

/* ��ȡһ�ε����� ֱ������һ�����в����� ���������в������� ����ֵΪд���ַ��� */
int _CA_Filter_ReadSegment(FILE *fil, char *pcSeg, int size)
{
	int iOffset = 0;
	char szLine[LINE_MAXSIZE] = { 0 };

	/* ��ȡһ��Ȼ���ж��Ƿ���� ���ǿ�����ѭ����ȡ */
	while (0 != fgets(szLine, sizeof(szLine), fil) &&
		   !CA_Filter_Pattern_IsAllSpace(szLine))
	{
		strcpy_s(pcSeg + iOffset, size - iOffset, szLine);
		iOffset += strlen(szLine);
	}

	/* BUG�޸�:���һ�п��ܲ��������з�����ӻ��з����ε�ĩβ(��ֹ���һ���μ���MD5ʱȱ��\n���¼������) */
	if (iOffset >= 1 && pcSeg[iOffset - 1] != '\n')
	{
		strcpy_s(pcSeg + iOffset, size - iOffset, "\n");
		iOffset += strlen("\n");
	}

	return iOffset;

}

/* CAѧϰ�������� */
errno_t _CA_Filter_Study(FILE *fil)
{
	int iOffset = 0 ;
	char szLine[LINE_MAXSIZE] = { 0 }, szSeg[SEGMENT_MAXSIZE] = { 0 };

	/* ������һ��(��һ�п��ܰ���Windows���±��Դ�������ͷ) */
	if (0 == fgets(szLine, sizeof(szLine), fil))
	{
		return ERROR_SUCCESS;
	}
	
	/* �ֶη���ѧϰ�������� */
	while (0 != fgets(szLine, sizeof(szLine), fil))
	{
		/* ����ǿ���ֱ�Ӵ�ӡ���У��򲻽���ѧϰ */
		if (BOOL_TRUE == CA_Filter_Pattern_IsPutsDirect(szLine))
		{
			continue;
		}

		/* ��ȡһ�� */
		iOffset = sprintf_s(szSeg, sizeof(szSeg), "%s", szLine);
		iOffset += _CA_Filter_ReadSegment(fil, szSeg + iOffset, sizeof(szSeg) - iOffset);

		/* ��ͨSeg�δ�������������MD5,�ļ�������ѯ���ݿ������� ͳ�Ƹ澯 */
		/* �޷�ʶ��Ķβ�ѧϰ */
		(void)CA_Filter_Proc_Study(szSeg);

	}

	return ERROR_SUCCESS;

}


/* CA���˴������� */
errno_t _CA_Filter_Proc(FILE *fil, FILE *stream)
{
	int iOffset = 0;
	BOOL bHasReadNextLine = BOOL_FALSE;
	char szLine[LINE_MAXSIZE] = { 0 }, szSeg[SEGMENT_MAXSIZE] = { 0 };

	/* ������һ��(��һ�п��ܰ���Windows���±��Դ�������ͷ) */
	if (0 == fgets(szLine, sizeof(szLine), fil))
	{
		return ERROR_FAILED;
	}
	fprintf(stream, "%s", szLine);
	
	/* �ֶη�����˴������� */
	/* �����Ҫ��ʱԤ��һ�У�������ڴ��������ж�ȡ��szLine��Ȼ����iOffsetΪԤ��ȡ�ĳ��� */
	while (BOOL_TRUE == bHasReadNextLine || 0 != fgets(szLine, sizeof(szLine), fil))
	{
		bHasReadNextLine = BOOL_FALSE;
		if (BOOL_TRUE == CA_Filter_Pattern_IsPutsDirect(szLine))
		{
			fprintf(stream, "%s", szLine);
			continue;
		}

		/* ��ȡһ�Σ������Note900���ٶ�ȡһ�Σ�������Note�������̴��� */
		iOffset = sprintf_s(szSeg, sizeof(szSeg), "%s", szLine);
		iOffset += _CA_Filter_ReadSegment(fil, szSeg + iOffset, sizeof(szSeg) - iOffset);
		if (CA_Filter_Pattern_IsNote(szSeg))
		{
			/* Note��Ϣ��������ԭ����ͳ����Ϣ�޸� */
			CA_Filter_Proc_Note(szSeg, stream);

			/* ��ȡ��ʶ����һ���Ƿ�Ϊ�����Ϣ, ��������Ԥ������һ�� */
			if (0 == fgets(szLine, sizeof(szLine), fil))
			{
				break;
			}
			
			if (BOOL_TRUE == CA_Filter_Pattern_IsAfterMessage(szLine))
			{
				/* ������Щ��ϢĿǰ�ݲ�����ֱ�Ӻ���Ҳ����ӡ */
				_CA_Filter_ReadSegment(fil, szSeg, sizeof(szSeg));
			}
			else
			{
				/* �������Note900�����Ϣ��ѵ�ǰԤ���˵��н��е���һ��ѭ���� */
				bHasReadNextLine = BOOL_TRUE;
			}
			
			continue;
		}

		/* ��ͨSeg�δ�������������MD5,�ļ�������ѯ���ݿ������� ͳ�Ƹ澯 */
		/* �޷�ʶ��Ķ�ֱ�Ӵ�ӡ */
		CA_Filter_Proc_Seg(szSeg, stream);

	}

	return ERROR_SUCCESS;

}

/* ��ʼ�� */
errno_t CA_Filter_Init()
{
	if (ERROR_SUCCESS != CA_Filter_Pattern_Init())
	{
		return ERROR_FAILED;
	}

	return ERROR_SUCCESS;
}

/* ȥ��ʼ��*/
void CA_Filter_Fini()
{
	CA_Filter_Pattern_Fini();

	return;
}

/* ���ݱ�׼��CA Lint�ļ�����ѧϰ */
errno_t CA_Filter_Study(char *pcStandardFile)
{
	FILE *fil;
	errno_t err;

	if (ERROR_SUCCESS != fopen_s(&fil, pcStandardFile, "r") || NULL == fil)
	{
		printf("Open Study File %s Error.\r\n", pcStandardFile);
		return ERROR_FAILED;
	}

	err = _CA_Filter_Study(fil);
	fclose(fil);

	return err;
}

/* ��ȡ�����ļ����������ݿ� */
errno_t CA_Filter_ReadCfgFiles(char *pcFileNameFile)
{
	FILE *fil;

	if (ERROR_SUCCESS != fopen_s(&fil, pcFileNameFile, "r") || NULL == fil)
	{
		//printf("Read Cfg File %s Error.\r\n", pcFileNameFile);
		return ERROR_FAILED;
	}

	CA_Filter_Proc_IgnoreFile(fil);

	fclose(fil);

	return ERROR_SUCCESS;
}

/* �����ݿ��е����ñ��浽�ļ��� */
errno_t _CA_Filter_SaveCfgFile(FILE *fil)
{
	return CA_Filter_Proc_SaveCfgFile(fil);
}

/* �����ݿ��е����ñ��浽�����ļ� */
errno_t CA_Filter_SaveCfgFiles(char *pcFileNameFile)
{
	FILE *fil;
	errno_t err;

	if (ERROR_SUCCESS != fopen_s(&fil, pcFileNameFile, "w") || NULL == fil)
	{
		printf("Write Cfg File %s Error.\r\n", pcFileNameFile);
		return ERROR_FAILED;
	}

	err = _CA_Filter_SaveCfgFile(fil);
	fclose(fil);

	return err;
}

/* ��CA_Lint�ļ����ݽ��й��ˣ�����浽STREAM�� */
errno_t _CA_Filter(char *pcSrcFile, FILE *stream)
{
	FILE *fil;
	errno_t err;
	int i;

	/* ��γ��Դ�ԭCA�澯�ļ� */
	for (i = 0; i < CA_FILTER_OPENORIGIN_TIMES; ++i)
	{
		if (ERROR_SUCCESS == fopen_s(&fil, pcSrcFile, "r") && NULL != fil)
		{
			break;
		}
		Sleep(CA_FILTER_OPENORIGIN_INTERVAL);
	}

	if (CA_FILTER_OPENORIGIN_TIMES == i)
	{
		printf("Open Origin File %s Error.\r\n", pcSrcFile);
		return ERROR_FAILED;
	}

	err = _CA_Filter_Proc(fil, stream);
	fclose(fil);

	return err;
}

/* ��CA_Lint�ļ����ݽ��й��ˣ�����浽��һ���ļ� */
errno_t CA_Filter(char *pcSrcFile, char *pcDstFile)
{
	FILE *fil;
	errno_t err;

	if (ERROR_SUCCESS != fopen_s(&fil, pcDstFile, "w") || NULL == fil)
	{
		printf("Open CA Filter Dst file %s error.\r\n", pcDstFile);
		return ERROR_FAILED;
	}

	err = _CA_Filter(pcSrcFile, fil);

	fclose(fil);

	return err;
}

/* ���ڴ���ɾ������ѧϰ��� */
errno_t CA_Filter_ClearAll()
{
	CA_Filter_Data_Del_MD5_All();
	CA_Filter_Data_Del_FileName_All();
	return ERROR_SUCCESS;
}

/* ====================����CA_Filter.exe��Ҫ�ĺ���================= */
#ifdef CA_FILTER_MAKE_EXE
static errno_t _Init(int argc, _TCHAR* argv[])
{
	errno_t err = ERROR_SUCCESS;
	char szArg0[CA_FILTER_FILENAME_MAX] = { 0 },
		 szTmp[CA_FILTER_FILENAME_MAX] = { 0 };
	

	//��ȡ��ǰ·��,������Ϊ��ǰ����Ŀ¼
	Str_Wchar2Char(argv[0], szArg0, sizeof(szArg0));
	Str_GetBasename_Char(szArg0, szTmp, sizeof(szTmp));
	if (szTmp[0] != '\0' && ERROR_SUCCESS != _chdir(szTmp))
	{
		return ERROR_FAILED;
	}

	//�����ǰ�����������MCA�ű�������һ�������˺�׺�����������MCA
	Str_GetFilename_Char(szArg0, szTmp, sizeof(szTmp));
	if (0 == strcmp(szTmp, CA_FILTER_MCA_EXE_NAME))
	{
		strcpy_s(g_szCAnalyzerBat, sizeof(g_szCAnalyzerBat), CA_FILTER_M_BAT_FILE);
	}
	else
	{
		strcpy_s(g_szCAnalyzerBat, sizeof(g_szCAnalyzerBat), CA_FILTER_BAT_FILE);
	}

	err = CA_Filter_Init();

	return err;
}

static void _Fini()
{
	CA_Filter_Fini();
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (ERROR_SUCCESS != _Init(argc, argv))
	{
		printf("CA Filter Init Failed.\r\n");
		return ERROR_FAILED;
	}
	
	/* ��ȡ�����ļ� */
	(void)CA_Filter_ReadCfgFiles(CA_FILTER_READ_CFG_FILE);

	/* ѧϰ */
	(void)CA_Filter_Study(CA_FILTER_STUDY_FILE);

	/* ִ��CA��������������ļ�CA_FILTER_ORIGIN_RESULT */
	CA_Run(argc, argv, g_szCAnalyzerBat, CA_FILTER_ORIGIN_RESULT);

		
	/* CA���ˣ�������浽CA_FILTER_NEW_RESULT */
	if (ERROR_SUCCESS != CA_Filter(CA_FILTER_ORIGIN_RESULT, CA_FILTER_NEW_RESULT))
	{
		printf("CA Filter Error.\r\n");
		_Fini();
		return ERROR_FAILED;
	}

	/* ��ӡ��� */
	(void)Str_PrintFile(CA_FILTER_NEW_RESULT);
	/* ���ڴ��е��������Ϊ�����ļ� */
	CA_Filter_SaveCfgFiles(CA_FILTER_AUTOSAVE_CFGFILE);

	_Fini();

	return ERROR_SUCCESS;
}

#endif

