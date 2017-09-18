#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "Config.h"

#define ISCOMMENT(x)         ('#' == (x))
#define ISUNDERLINE(x)       ('_' == (x))
#define ISEQUAL(x)           ('=' == (x))
#define ISSPILTE(x)          (',' == (x))

#define ERROR_SUCCESS 0
#define ERROR_FAILED 1
#define ERROR_NOT_FOUND 2

const char *g_pcConfigPATH = "";

CONFIG_CFG_S g_CONFIG_Item[CONFIG_ITEMS_MAX];

//��һ����Ŀ��ϢҪ�洢��λ��
int g_CONFIG_Item_Count = 0;

typedef errno_t(*CONFIG_DATA_PROCESS_PF)(char *line, void *piArray, int size);

static errno_t _config_GetNum(char *line, void *piArray, int size);
static errno_t _config_GetNumArray(char *line, void *piArray, int size);
static errno_t _config_GetChar(char *line, void *piArray, int size);
static errno_t _config_GetString(char *line, void *piArray, int size);
static errno_t _config_GetLine(char *line, void *piArray, int size);

/* ��������˳��Ҫ��CONFIG_ITEM_TYPE_Eһ�¶�Ӧ */
CONFIG_DATA_PROCESS_PF g_apfProcFuncArray[CONFIG_ITEM_TYPE_MAX] =
{
	_config_GetNum,
	_config_GetNumArray,
	_config_GetChar,
	_config_GetString,
	_config_GetLine,
};

/* ��ȡĳ�е�Itemname��������������ע�ͷ�����ERROR_NOT_FOUND */
static errno_t _config_GetItemName(char *line, char *pcItemName, int size, int *piNext)
{
	int i = 0, j = 0, len;

	len = strlen(line);

	while (i < len && isspace(line[i]))
	{
		i++;
	}

	if (i == len || ISCOMMENT(line[i]))
	{
		return ERROR_NOT_FOUND;
	}

	if (!isalpha(line[i]) && ISUNDERLINE(line[i]))
	{
		return ERROR_FAILED;
	}

	while (i < len && (isalpha(line[i]) || isdigit(line[i]) || ISUNDERLINE(line[i])))
	{
		pcItemName[j] = line[i];
		i++;
		j++;
	}

	pcItemName[j] = '\0';
	*piNext += i;

	return ERROR_SUCCESS;

}

//�ȴ���=������
static errno_t _config_WaitEqual(char *line, int *piNext)
{
	int i = 0, len;

	len = strlen(line);

	while (i < len && isspace(line[i]))
	{
		i++;
	}

	if (i == len || !ISEQUAL(line[i]))
	{
		*piNext += i;
		return ERROR_NOT_FOUND;
	}

	*piNext += i + 1;
	return ERROR_SUCCESS;

}

//��ȡ��������
static errno_t _config_GetNumArray(char *line, void *piArray, int size)
{
	int i = 0, n = 0, len, t;


	len = strlen(line);
	size = size / sizeof(int);

	while (n < size)
	{
		/* Խ���ո� */
		while (isspace(line[i]))
		{
			i++;
		}

		/* ��ȡ���֣�û�ж�������ERROR_NOT_FOUND  */
		t = sscanf_s(line + i, "%d", (int *)piArray + n, sizeof(int));
		if (1 == t)
		{
			n ++;
		}

		if (0 == n)
		{
			memset((int *)piArray, 0, size * sizeof(int));
			return ERROR_NOT_FOUND;
		}
		else if (1 != t)
		{
			memset((int *)piArray + n, 0, (size - n) * sizeof(int));
			return ERROR_SUCCESS;
		}

		/* Խ������ */
		while (isdigit(line[i]))
		{
			i++;
		}

		/* Խ���ո� */
		while (isspace(line[i]))
		{
			i++;
		}

		/* �ж��ǲ��Ƿָ��� */
		if (ISSPILTE(line[i]))
		{
			i++;
		}

	}

	return ERROR_SUCCESS;

}

//��ȡ��������
static errno_t _config_GetNum(char *line, void *piArray, int size)
{
	if (1 == sscanf_s(line, "%d", (int *)piArray, size))
	{
		return ERROR_SUCCESS;
	}

	return ERROR_NOT_FOUND;

}

//��ȡ�����ո���ַ���
static errno_t _config_GetString(char *line, void *piArray, int size)
{
	if (1 == sscanf_s(line, "%s", (char *)piArray, size))
	{
		return ERROR_SUCCESS;
	}

	return ERROR_NOT_FOUND;

}

//��ȡ�����ַ�
static errno_t _config_GetChar(char *line, void *piArray, int size)
{
	char buf[2];

	if (1 == sscanf_s(line, "%s", buf, sizeof(buf)))
	{
		*(char *)piArray = buf[0];
		return ERROR_SUCCESS;
	}

	return ERROR_NOT_FOUND;

}

//��ȡ�Ⱥź���һ�е�����
static errno_t _config_GetLine(char *line, void *piArray, int size)
{
	int i;

	for (i = 0; '\n' != line[i] && '\0' != line[i]; i++)
	{
		*((char *)piArray + i) = line[i];
	}

	*((char *)piArray + i) = '\0';

	return EXIT_SUCCESS;

}


/*  ������Ŀ���Ƶ��ö�Ӧ�Ĵ�����  */
errno_t Config_ProcessData(char *line, char *pcKeyWord)
{
	int i;
	errno_t err = ERROR_FAILED;

	for (i = g_CONFIG_Item_Count - 1; i >= 0; i--)
	{
		if (0 == strcmp(pcKeyWord, g_CONFIG_Item[i].szKeyWord))
		{
			break;
		}

	}

	if (0 > i)
	{
		return err;
	}

	err = g_apfProcFuncArray[g_CONFIG_Item[i].enType](line, g_CONFIG_Item[i].pData, g_CONFIG_Item[i].size);

	return err;

}

//������ȡ��ÿһ��
errno_t  Config_PhaseEachLine(char *line)
{
	char szItemName[CONFIG_KEYWORD_LEN_MAX];
	int iNext = 0;

	/* ��ȡ��Ŀ���� */
	if (ERROR_SUCCESS != _config_GetItemName(line, szItemName, sizeof(szItemName), &iNext))
	{
		return ERROR_NOT_FOUND;
	}

	/*  �ҵȺ�  */
	if (ERROR_SUCCESS != _config_WaitEqual(line + iNext, &iNext))
	{
		return ERROR_NOT_FOUND;
	}

	/*  ������Ŀ���Ƶ��ö�Ӧ�Ĵ�����  */
	if (ERROR_SUCCESS != Config_ProcessData(line + iNext, szItemName))
	{
		return ERROR_NOT_FOUND;
	}

	return ERROR_SUCCESS;

}

errno_t Config_SetPath(char *pcPath)
{
	FILE *FL;
	errno_t err = ERROR_SUCCESS;

	err = fopen_s(&FL, pcPath, "a");
	if (ERROR_SUCCESS != err || NULL == FL)
	{
		printf("Unable to open when set path\r\n");
		return err;
	}

	fclose(FL);

	g_pcConfigPATH = pcPath;

	return err;
}

errno_t Config_Item_Register(CONFIG_CFG_S *pstCfg)
{
	if (CONFIG_ITEMS_MAX <= g_CONFIG_Item_Count)
	{
		return ERROR_FAILED;
	}
	
	memcpy(g_CONFIG_Item + g_CONFIG_Item_Count, pstCfg, sizeof(CONFIG_CFG_S));
	g_CONFIG_Item_Count++;

	return ERROR_SUCCESS;
}

/* ȥע��һ������ */
errno_t Config_Item_UnRegister(char *pcKeyWord)
{
	int i;

	for (i = g_CONFIG_Item_Count - 1; i >= 0; i--)
	{
		if (0 == strcmp(pcKeyWord, g_CONFIG_Item[i].szKeyWord))
		{
			break;
		}

	}

	if (0 > i)
	{
		return ERROR_FAILED;
	}

	/* ɾ����i�������ǰ�� */
	while (i + 1 < g_CONFIG_Item_Count)
	{
		memcpy(g_CONFIG_Item + i, g_CONFIG_Item + i + 1, sizeof(CONFIG_CFG_S));
		i++;
	}

	g_CONFIG_Item_Count--;

	return ERROR_SUCCESS;
}

/* ��������Ŀȥע�� */
errno_t Config_Item_UnRegisterAll()
{
	g_CONFIG_Item_Count = 0;
	memset(g_CONFIG_Item, 0, sizeof(g_CONFIG_Item));

	return ERROR_SUCCESS;
}


//��ȡ�����ļ� �浽ȫ��������
errno_t Config_Read()
{
	char buf[CONFIG_LINE_MAXSIZE];
	FILE *FL;
	errno_t err = 0;

	err = fopen_s(&FL, g_pcConfigPATH, "r");
	if (ERROR_SUCCESS != err || NULL == FL)
	{
		printf("Unable to open when get config\r\n");
		return err;
	}

	while (fgets(buf, CONFIG_LINE_MAXSIZE, FL))
	{
		Config_PhaseEachLine(buf);
	}

	fclose(FL);

	return err;
}
/*
void main()
{
	int i,age = 20, buf[16];
	char line[512], str[512];
	CONFIG_CFG_S stCfg;

	Config_SetPath("D:\\Hello.txt");

	memset(buf, 0, sizeof(buf));
	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), "error");
	stCfg.enType = CONFIG_ITEM_TYPE_NUMARRAY;
	stCfg.pData = buf;
	stCfg.size = sizeof(buf);
	Config_Item_Register(&stCfg);

	memset(str, 0, sizeof(str));
	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), "name");
	stCfg.enType = CONFIG_ITEM_TYPE_LINE;
	stCfg.pData = str;
	stCfg.size = sizeof(str);
	Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), "age");
	stCfg.enType = CONFIG_ITEM_TYPE_NUM;
	stCfg.pData = &age;
	stCfg.size = sizeof(age);
	Config_Item_Register(&stCfg);

	Config_Read();

	//while (fgets(line, sizeof(line), stdin))
	{
		//Config_PhaseEachLine(line);
		
		puts("\n\n===================================================");
		for (i = 0; i < sizeof(buf) / sizeof(int); i++)
		{
			printf("%d ", buf[i]);
		}
		printf("\n");
		printf("%s\n", str);
		printf("%d\n", age);
		//putchar(*buf);
		puts("===================================================\n\n");

		Config_Item_UnRegister("age");


	}

	(void)getchar();

}
*/

