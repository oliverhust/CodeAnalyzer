#include "stdafx.h"
#include <string.h>
#include <windows.h>

#include "CA_Filter.h"
#include "CA_Filter_inc.h"
#include "CA_Filter_Data.h"


static CA_FILTER_MD5_S g_astMD5[CA_FILTER_DATA_MD5_MAX] = { 0 };
static int g_iMD5Num = 0;		/* ��ǰ�洢������ */
static char g_aszFileName[CA_FILTER_DATA_FILENAME_MAX][CA_FILTER_FILENAME_MAX] = { 0 };
static int g_iFileNameNum = 0;	/* ��ǰ�洢������ */

/* ��ӵ���CA�澯��Ŀ MD5 */
errno_t CA_Filter_Data_Add_MD5(CA_FILTER_MD5_S *pstMD5)
{
	int i, iPos, iCmp = -1;

	/* �ҵ�Ҫ�����λ�� */
	for (iPos = 0; iPos < g_iMD5Num; iPos++)
	{
		iCmp = _stricmp(pstMD5->szMD5, g_astMD5[iPos].szMD5);
		if (iCmp <= 0)
		{
			break;
		}

	}

	/* ��ֹ�ظ���� */
	if (0 == iCmp)
	{
		return ERROR_SUCCESS;
	}

	/* ��λ�ú���������� */
	for (i = g_iMD5Num - 1; i >= iPos; i--)
	{
		memcpy(&g_astMD5[i + 1], &g_astMD5[i], sizeof(CA_FILTER_MD5_S));
	}

	/* ���� */
	memcpy(&g_astMD5[iPos], pstMD5, sizeof(CA_FILTER_MD5_S));
	g_iMD5Num++;

	return ERROR_SUCCESS;
}

/* ɾ������CA�澯��Ŀ MD5 */
errno_t CA_Filter_Data_Del_MD5(CA_FILTER_MD5_S *pstMD5)
{
	int i, iPos;

	for (iPos = 0; iPos < g_iMD5Num; iPos++)
	{
		if (0 == _stricmp(pstMD5->szMD5, g_astMD5[iPos].szMD5))
		{
			/* ��ǰ�ƶ�һλ */
			for (i = iPos; i <= g_iMD5Num - 2; i++)
			{
				memcpy(&g_astMD5[i], &g_astMD5[i + 1], sizeof(CA_FILTER_MD5_S));
			}
			g_iMD5Num--;
			memset(&g_astMD5[g_iMD5Num], 0, sizeof(CA_FILTER_MD5_S));

			break;
		}
	}

	return ERROR_SUCCESS;
}

/* ɾ������CA�澯��Ŀ MD5 */
void CA_Filter_Data_Del_MD5_All()
{
	memset(g_astMD5, 0, g_iMD5Num * sizeof(CA_FILTER_MD5_S));
	g_iMD5Num = 0;

}

/* ��ѯ����CA�澯��Ŀ ��MD5���Ƿ���� */
errno_t CA_Filter_Data_IsExist_MD5(CA_FILTER_MD5_S *pstMD5)
{
	int low = 0, high = g_iMD5Num - 1, mid;
	errno_t err = ERROR_FAILED;
	BOOL bEq;

	while (low <= high)
	{
		mid = (low + high) / 2;
		bEq = _stricmp(g_astMD5[mid].szMD5, pstMD5->szMD5);
		if (0 == bEq)
		{
			err = ERROR_SUCCESS;
			break;
		}
		else if (bEq > 0)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}

	return err;
}

/* ���㵱ǰ�ڴ��б���� MD5��Ŀ ���� */
int CA_Filter_Data_Count_MD5()
{
	return g_iMD5Num;
}

/* ��ȡ�ڴ��е�index��MD5��Ŀ������ */
errno_t CA_Filter_Data_Get_MD5(int index, CA_FILTER_MD5_S *pstCfg)
{
	if (0 <= index && index < g_iMD5Num)
	{
		memcpy(pstCfg, &g_astMD5[index], sizeof(CA_FILTER_MD5_S));
		return ERROR_SUCCESS;
	}

	return ERROR_FAILED;
}
/*=======================================================================================*/
/* ��ӵ���CA�澯��Ŀ �ļ��� */
errno_t CA_Filter_Data_Add_FileName(char *pcFileName)
{
	int i, iPos;

	if (NULL == pcFileName || strlen(pcFileName) >= CA_FILTER_FILENAME_MAX)
	{
		return ERROR_FAILED;
	}

	/* �ҵ�Ҫ�����λ�� */
	for (iPos = 0; iPos < g_iFileNameNum; iPos++)
	{
		if (_stricmp(pcFileName, g_aszFileName[iPos]) < 0)
		{
			break;
		}

	}

	/* ��λ�ú���������� */
	for (i = g_iFileNameNum - 1; i >= iPos; i--)
	{
		strcpy_s(g_aszFileName[i + 1], CA_FILTER_FILENAME_MAX, g_aszFileName[i]);
	}

	/* ���� */
	strcpy_s(g_aszFileName[iPos], CA_FILTER_FILENAME_MAX, pcFileName);
	g_iFileNameNum++;

	return ERROR_SUCCESS;
}

/* ɾ������CA�澯��Ŀ �ļ��� */
errno_t CA_Filter_Data_Del_FileName(char *pcFileName)
{
	int i, iPos;

	if (NULL == pcFileName)
	{
		return ERROR_FAILED;
	}

	for (iPos = 0; iPos < g_iFileNameNum; iPos++)
	{
		if (0 == _stricmp(pcFileName, g_aszFileName[iPos]))
		{
			/* ��ǰ�ƶ�һλ */
			for (i = iPos; i <= g_iFileNameNum - 2; i++)
			{
				strcpy_s(g_aszFileName[i], CA_FILTER_FILENAME_MAX, g_aszFileName[i + 1]);
			}
			g_iFileNameNum--;
			memset(g_aszFileName[g_iFileNameNum], 0, CA_FILTER_FILENAME_MAX);

			break;
		}
	}

	return ERROR_SUCCESS;
}

/* ɾ������CA�澯��Ŀ �ļ��� */
void CA_Filter_Data_Del_FileName_All()
{
	memset(g_aszFileName, 0, g_iFileNameNum * CA_FILTER_FILENAME_MAX);
	g_iFileNameNum = 0;

}

/* ��ѯ����CA�澯��Ŀ ���ļ������Ƿ���� */
errno_t CA_Filter_Data_IsExist_FileName(char *pcFileName)
{
	int low = 0, high = g_iFileNameNum - 1, mid;
	errno_t err = ERROR_FAILED;
	BOOL bEq;

	if (NULL == pcFileName)
	{
		return ERROR_FAILED;
	}

	while (low <= high)
	{
		mid = (low + high) / 2;
		bEq = _stricmp(g_aszFileName[mid], pcFileName);
		if (0 == bEq)
		{
			err = ERROR_SUCCESS;
			break;
		}
		else if (bEq > 0)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}

	return err;
	
}

/* ���㵱ǰ�ڴ��б���� �ļ�����Ŀ ���� */
int CA_Filter_Data_Count_FileName()
{
	return g_iFileNameNum;
}

/* ��ȡ�ڴ��е�index�� �ļ�����Ŀ ������ */
errno_t CA_Filter_Data_Get_FileName(int index, char pcFileName[CA_FILTER_FILENAME_MAX])
{
	if (0 <= index && index < g_iFileNameNum)
	{
		strcpy_s(pcFileName, CA_FILTER_FILENAME_MAX, g_aszFileName[index]);
		return ERROR_SUCCESS;
	}

	return ERROR_FAILED;
}


/*=======================================================================================*/
/*
int _tmain()
{
	int i;
	CA_FILTER_MD5_S stMD5;
	char szBuf[CA_FILTER_FILENAME_MAX];


#define ADD(x) strcpy_s(stMD5.szMD5, sizeof(stMD5.szMD5), x); CA_Filter_Data_Add_MD5(&stMD5);
#define DEL(x) strcpy_s(stMD5.szMD5, sizeof(stMD5.szMD5), x); CA_Filter_Data_Del_MD5(&stMD5);
#define DELALL() CA_Filter_Data_Del_MD5_All()
#define GET(x) strcpy_s(stMD5.szMD5, sizeof(stMD5.szMD5), x); \
	if (ERROR_SUCCESS == CA_Filter_Data_IsExist_MD5(&stMD5)) \
	{ \
	printf("Get %s Success\r\n", stMD5.szMD5); \
	}\
	else \
	{ \
	printf("Get %s Failed\r\n", stMD5.szMD5); \
	}
#define PRINTALL() \
	printf("\r\nPrint All Current Data:\r\n");\
	for (i = 0; i < CA_Filter_Data_Count_MD5(); i++)\
	{\
		CA_Filter_Data_Get_MD5(i, &stMD5);\
		printf("%s\r\n", stMD5.szMD5);\
	}\
	printf("\r\n");
	
	
#define ADD(x) CA_Filter_Data_Add_FileName(x)
#define DEL(x) CA_Filter_Data_Del_FileName(x)
#define DELALL() CA_Filter_Data_Del_FileName_All()
#define GET(x) if(ERROR_SUCCESS == CA_Filter_Data_IsExist_FileName(x))\
	{ \
		printf("Get %s Success\r\n", x); \
	}\
	else \
	{\
		printf("Get %s Failed\r\n", x); \
	}
#define PRINTALL() \
	printf("\r\nPrint All Current Data:\r\n"); \
	for (i = 0; i < CA_Filter_Data_Count_FileName(); i++)\
	{\
		CA_Filter_Data_Get_FileName(i, szBuf); \
		printf("%s\r\n", szBuf); \
	}\
	printf("\r\n");


	PRINTALL();
	ADD("CA_Filter_Data_IsExist_FileName_");
	PRINTALL();
	ADD("2A_Filter_Data_Get_FileName_cha2");
	PRINTALL();
	ADD("3A_Filter_Data_Get_FileName_cha3");
	PRINTALL();

	DEL("CA_Filter_Data_IsExist_FileName_");
	PRINTALL();
	DEL("2A_Filter_Data_Get_FileName_char");
	PRINTALL();
	DEL("3A_Filter_Data_Get_FileName_cha3");
	PRINTALL();
		//DELALL();
	printf("---------------------------------------\r\n");
	GET("1A_Filter_Data_Get_FileName_cha1");
	GET("2A_Filter_Data_Get_FileName_chaB");
	GET("2A_Filter_Data_Get_FileName_cha2");
	GET("3A_Filter_Data_Get_FileName_cha3");
	printf("---------------------------------------\r\n");

	DELALL();
	PRINTALL();

	(void)getchar();

}
*/
