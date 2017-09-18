#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "inc.h"
#include "Git_MemConf.h"

/* �����������ô涼�浽����ļ���ȫ�ֱ����У�ͨ�������ӿڲ�ѯ */

/* ��ǰ����·������main������argv[0]��ã����������·��Ҳ�����Ǿ���·����*/
static char MyPath[PATH_MAXSIZE] = { 0 };
/* ��ǰ��������·�� */
static char MyPathAbso[PATH_MAXSIZE] = { 0 };
/* �����˳��Ҫ��FILE_TYPE_E����һ�� */
static char MyFileName[FILE_TYPE_MAX][PATH_FILENAME_MAX] =
{
	FILE_CONFIG_DFT,
	FILE_STATUS_DFT,
	FILE_CA_RESULT_DFT,
	FILE_CA_RUN_DFT,
	FILE_SENDMAIL_DFT,
	FILE_WGET_TMP_DFT,


};
/* ��֧�����˳��Ҫ��BRANCH_TYPE_Eһ�� */
static char MyBranchName[BRANCH_TYPE_MAX][PATH_BRANCHNAME_MAX] =
{
	" master ",
	" work "
};

/* ������ȫ�ֶ˿ں� */
static unsigned int g_CAServerPort = SERVER_PORT_DFT;

char *GitAutoCA_GetPath()
{
	char *pcRet = MyPath;

	if (NULL == pcRet)
	{
		printf("Get My Path error.\r\n");
	}

	return MyPath;
}

char *GitAutoCA_GetPathAbso()
{
	char *pcRet = MyPathAbso;

	if (NULL == pcRet)
	{
		printf("Get My PathAbsolute error.\r\n");
	}

	return MyPathAbso;
}

char *GitAutoCA_GetFileName(enum FILE_TYPE_E FileType)
{
	char *pcRet = MyFileName[FileType];

	if (NULL == pcRet)
	{
		printf("Get FileName error, file type = %d.\r\n", FileType);
	}

	return MyFileName[FileType];
}

char *GitAutoCA_GetBranchName(enum BRANCH_TYPE_E BranchType)
{
	char *pcRet = MyBranchName[BranchType];

	if (NULL == pcRet)
	{
		printf("Get Branch name error,branch type = %d.\r\n", BranchType);
	}

	return MyBranchName[BranchType];
}

unsigned int GitAutoCA_GetCAServerPort()
{
	return g_CAServerPort;
}

unsigned int *GitAutoCA_GetCAServerPort_Addr()
{
	return &g_CAServerPort;
}