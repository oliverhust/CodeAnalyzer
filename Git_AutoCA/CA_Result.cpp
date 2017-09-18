#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>

#include "inc.h"
#include "CA_Result.h"
#include "Git_MemConf.h"

//CA�Ƿ����
static int g_CAResult = ERROR_FAILED;

//����CA Lint�е�Error,Warning,Info���ƣ���������ֵ������merge
static int g_limit_error[GRADE_LINT] = { 0 };
static int g_limit_warning[GRADE_LINT] = { 0 };
static int g_limit_info[GRADE_LINT] = { 0 };
static int g_limit_note[GRADE_LINT] = { 0 };

/* ����CA����æµ��� */
void CA_SetBusy()
{
	g_CAResult = ERROR_BUSY;
	return;
}

/* ��ȡCA�Ƿ���ɱ�� ע�⣺�ͻ��������ڱ�hook����1�����ܲ�ѯ */
errno_t CA_GetCAResult()
{
	return g_CAResult;
}

/* ��ȡ��������ĵ�ַ  */
void* CA_GetVariAddress(CA_RESULT_TYPE_E enType)
{
	void *pv;
	
	if (CA_RESULT_TYPE_MAX <= enType)
	{
		return NULL;
	}

	switch (enType)
	{
	case CA_RESULT_TYPE_ERROR:
		pv = g_limit_error;
		break;
	case CA_RESULT_TYPE_WARNING:
		pv = g_limit_warning;
		break;
	case CA_RESULT_TYPE_INFO:
		pv = g_limit_info;
		break;
	case CA_RESULT_TYPE_NOTE:
		pv = g_limit_note;
		break;
	default:
		pv = NULL;
		break;

	}

	return pv;
}

/* ��ȡ�����Ĵ�С */
int CA_GetVariSize(CA_RESULT_TYPE_E enType)
{
	int size = 0;

	if (CA_RESULT_TYPE_MAX <= enType)
	{
		return NULL;
	}

	switch (enType)
	{
	case CA_RESULT_TYPE_ERROR:
		size = sizeof(g_limit_error);
		break;
	case CA_RESULT_TYPE_WARNING:
		size = sizeof(g_limit_warning);
		break;
	case CA_RESULT_TYPE_INFO:
		size = sizeof(g_limit_info);
		break;
	case CA_RESULT_TYPE_NOTE:
		size = sizeof(g_limit_note);
		break;
	default:
		size = 0;
		break;

	}

	return size;
}


//�����ַ���bufѰ��pattern��λ�ò�����,buf���ΪMAXLINE
int str_find_pos(char *buf, char *pattern)
{
	int i, len;

	len = strlen(pattern);
	for (i = 0; '\0' != buf[i]; i++)
	{
		if (0 == strncmp(buf + i, pattern, len))
		{
			return i;
		}
	}

	return LINE_MAXSIZE;
}

//��buf�л�ȡerror/warning/info�ĸ���
int err_get_num(char *buf, int i)
{
	int j = i - 1;
	int n = 0;
	char c = buf[i];

	//��buf[i-1]���ض�,ֱ����������������ͣ.j��i�ֱ�ָ���������˵Ŀո�
	while (isdigit(buf[j]))
	{
		j--;
	}
	if (i - 1 - j > 4 || j == i - 1)
	{
		//̫������� ��ǧ��,���߶�ȡ�����������
		return 9999;
	}

	buf[i] = '\0';
	n = atoi(buf + j + 1);
	buf[i] = c;

	return n;
}

//�Ƚ��������飬ǰ�ߵ�ÿ��ֵ�����ڵ��ں���ʱ������
BOOL err_array_cmp(int great[], int smaller[], int size)
{
	int i;
	BOOL bIsTrue = TRUE;
	for (i = 0; i < size; i++)
	{
		if (great[i] < smaller[i])
		{
			bIsTrue = FALSE;
			break;
		}
	}

	return bIsTrue;

}



/***********************************��ȡCA���******************************************
��CA������ɵ��ļ��ж��Ƿ�ͨ��CA �ɿ�ͷ��ʶ"Note 900"�ж�
***************************************************************************************/
BOOL _CA_JudgeAndSetReulst()
{
	char buf[LINE_MAXSIZE];
	FILE *FL;
	errno_t err = 0;
	int num_err[GRADE_LINT] = { 0 }, num_warn[GRADE_LINT] = { 0 };
	int num_info[GRADE_LINT] = { 0 }, num_note[GRADE_LINT] = { 0 };
	int j, len;
	char *p_pattern = "Note 900";//�����ַ�
	BOOL b_result = FALSE;

	err = fopen_s(&FL, GitAutoCA_GetFileName(FILE_TYPE_CA_RESULT), "r");
	if (ERROR_SUCCESS != err || NULL == FL)
	{
		printf("Unable to open when read CA Result.\r\n");
		return FALSE;
	}

	j = 0;
	len = strlen(p_pattern);
	printf("Get CA Result:\r\n");
	//��ȡError����
	while (0 != fgets(buf, LINE_MAXSIZE, FL))
	{
		//������p_pattern��ͷ����һ��
		if (0 == strncmp(buf, p_pattern, len))
		{
			//��ȡ��һ�е�buf
			(void)fgets(buf, LINE_MAXSIZE, FL);
			//�ҵ��˺�"Error,"��ʼ��ȡ���������	
			num_err[j] = err_get_num(buf, str_find_pos(buf, " Error,"));
			num_warn[j] = err_get_num(buf, str_find_pos(buf, " Warning,"));
			num_info[j] = err_get_num(buf, str_find_pos(buf, " Info,"));
			num_note[j] = err_get_num(buf, str_find_pos(buf, " Note."));
			buf[LINE_MAXSIZE - 1] = '\0';
			printf("%s [%d,%d,%d,%d]\r\n", buf, num_err[j], num_warn[j], num_info[j], num_note[j]);
			j++;
		}

	}

	if (0 == j || GRADE_LINT < j)
	{
		return FALSE;
	}

	//���������������Ƿ�ͨ��
	if (TRUE == err_array_cmp(g_limit_error, num_err, j) &&
		TRUE == err_array_cmp(g_limit_warning, num_warn, j) &&
		TRUE == err_array_cmp(g_limit_info, num_info, j) &&
		TRUE == err_array_cmp(g_limit_note, num_note, j))
	{
		b_result = TRUE;
	}

	fclose(FL);

	return b_result;
}

BOOL CA_JudgeAndSetReulst()
{
	if (TRUE == _CA_JudgeAndSetReulst())
	{
		g_CAResult = ERROR_SUCCESS;
		printf("Judge CA Result:Success\r\n");
		return TRUE;
	}

	g_CAResult = ERROR_FAILED;
	printf("Judge CA Result:Failed\r\n");
	return FALSE;
}

errno_t CA_WaitFinish()
{	
	return ERROR_SUCCESS;
}
