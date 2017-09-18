// Git_AutoCA.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>
#include <time.h>

#include "inc.h"
#include "Str.h"
#include "status.h"
#include "Config.h"
#include "CA_Result.h"
#include "Git_MemConf.h"
#include "Write_Log.h"
#include "Server_Socket.h"
#include "Server_Proc.h"

static char *g_pcSuccessFailed[] = { "Success", "Failed", "Unknown" };

//��ӡ��ǰʱ��
void print_time(void)
{
	time_t tim;
	char buf[LINE_MAXSIZE];
	time(&tim);
	ctime_s(buf, sizeof(buf), &tim);

	printf("%s", buf);

	return;
}

/* �ȴ�������� */
void _otherjobs_WaitFinish()
{
	printf("Wait for OtherJobs finish...\r\n");
	status_set("WaitOtherJobsFinish");
	while(ERROR_BUSYING == ServerProc_GetOtherJobsResult())
	{
		Sleep(1);
	}
	status_set("OtherJobs_finish");
	printf("Get signal of OtherJobs finish.\r\n");
	return;
}

//ֱ�Ӱѷ�֧�ϲ���������CA��Ҳ�������ʼ�
errno_t merge()
{
	//�ȶ�ȡ�����ļ�
	Config_Read();
	
	printf("==================================================\r\n");
	printf("Some Documents will be merge directly...\r\n");

	//CAͨ�����л���BRANCH_OFFCIAL��֧,����BRANCH_PUSH��֧�ϲ�
	system(Str_Cat("cd .. && git checkout ", GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL)));
	system(Str_Cat("cd .. && git merge ", GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH)));

	//���ý���״̬
	if (ERROR_SUCCESS != status_set("stop"))
	{
		return ERROR_FAILED;
	}

	//system(PATH_SENDMAIL" success");
	printf("Congurations! Merge Successfully! ^_^\r\n");

	return ERROR_SUCCESS;

}

//ÿ�ι���ǰ�ĳ�ʼ��
void _work_init()
{
	char *pcName = NULL;
	
	//��ʼ��CA����ͱ�������ȫ�ֱ���
	CA_SetBusy();
	ServerProc_ResetOtherJobsResult();

	//��ȡ�����ļ�
	Config_Read();

	//ÿ�ι�����ӡ��ͷ
	printf("==================================================\r\n");
	printf("Auto CA Start!\r\nPlease do not close the window..\r\n");
	print_time();

	//��ʱ�ȴ�Զ�̴������BRANCH_PUSH
	Sleep(1000);

	//ɾ��hooks��"wget"��������ʱ�ļ�
	pcName = GitAutoCA_GetFileName(FILE_TYPE_WGET_TMP);
	if (0 != strcmp(pcName, FILE_WGET_TMP_DFT))
	{
		system(Str_NCat("del /F ", GitAutoCA_GetFileName(FILE_TYPE_WGET_TMP), NULL));
	}

	return;
}

/***********************************��ʼ����******************************************
���յ�start�źź�Ĺ���
*************************************************************************************/
//���յ�start�źź�Ĺ�������Ҫ��
errno_t work()
{
	char buf[LINE_MAXSIZE] = { 0 };

	//ÿ�ι���ǰ�ĳ�ʼ��
	_work_init();
	
	//����״̬ΪCAing
	if (ERROR_SUCCESS != status_set("CAing"))
	{
		return ERROR_FAILED;
	}

	//ɾ��ԭ����Lint�ļ�
	sprintf_s(buf, sizeof(buf), "del /F %s", GitAutoCA_GetFileName(FILE_TYPE_CA_RESULT));
	system(buf);

	//�л���BRANCH_PUSH��֧
	system(Str_Cat("cd .. && git checkout ", GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH)));
	Sleep(500);  //������֧��ͬ��Ԥ��ʱ���л�
	
	//��ʼCA...
	system(Str_Cat(GitAutoCA_GetFileName(FILE_TYPE_CA_RUN), " >> Run_CA_Log.txt"));

	//�ȴ�CA������
	(void)CA_WaitFinish();

	//�л���OFFICAL��֧�������ʼ��Ľű�Ҫ�л����÷�֧�£��ҷ�֧δ����OFFICAL��
	system(Str_Cat("cd .. && git checkout ", GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL)));

	//����CA���
	(void)CA_JudgeAndSetReulst();
	(void)status_set("CA_finish");

	//�ȴ���ȡ����������
	_otherjobs_WaitFinish();

	//���ý���״̬
	if (ERROR_SUCCESS != status_set("stop"))
	{
		return ERROR_FAILED;
	}	

	printf("Congurations! CA Finished! ^_^\r\n");

	return ERROR_SUCCESS;
}

errno_t _init_get_config()
{
	char buf[LINE_MAXSIZE] = { 0 };
	CONFIG_CFG_S stCfg;
	errno_t err;

	//���������ļ�·��
	err = Config_SetPath(GitAutoCA_GetFileName(FILE_TYPE_CONFIG));
	if (ERROR_SUCCESS != err)
	{
		return err;
	}

	//�����ļ��Ĺؼ���ע��

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_TXT_STATUS);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetFileName(FILE_TYPE_STATUS);
	stCfg.size = PATH_FILENAME_MAX;
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_TXT_LINT);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetFileName(FILE_TYPE_CA_RESULT);
	stCfg.size = PATH_FILENAME_MAX;
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_CA_RUN);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetFileName(FILE_TYPE_CA_RUN);
	stCfg.size = PATH_FILENAME_MAX;
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_SENDMAIL);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetFileName(FILE_TYPE_FINI_PY);
	stCfg.size = PATH_FILENAME_MAX;
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_WGET_TMP);
	stCfg.enType = CONFIG_ITEM_TYPE_LINE;
	stCfg.pData = GitAutoCA_GetFileName(FILE_TYPE_WGET_TMP);
	stCfg.size = PATH_FILENAME_MAX;
	err |= Config_Item_Register(&stCfg);

	//----------------------------------------------------------------------

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_CA_PORT);
	stCfg.enType = CONFIG_ITEM_TYPE_NUM;
	stCfg.pData = GitAutoCA_GetCAServerPort_Addr();
	stCfg.size = sizeof(unsigned int);
	err |= Config_Item_Register(&stCfg);

	//----------------------------------------------------------------------

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_BRANCH_OFFCIAL);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL);
	stCfg.size = PATH_BRANCHNAME_MAX;
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_BRANCH_PUSH);
	stCfg.enType = CONFIG_ITEM_TYPE_STRING;
	stCfg.pData = GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH);
	stCfg.size = PATH_BRANCHNAME_MAX;
	err |= Config_Item_Register(&stCfg);

	//----------------------------------------------------------------------
	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_ERROR);
	stCfg.enType = CONFIG_ITEM_TYPE_NUMARRAY;
	stCfg.pData = CA_GetVariAddress(CA_RESULT_TYPE_ERROR);
	stCfg.size = CA_GetVariSize(CA_RESULT_TYPE_ERROR);
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_WARNING);
	stCfg.enType = CONFIG_ITEM_TYPE_NUMARRAY;
	stCfg.pData = CA_GetVariAddress(CA_RESULT_TYPE_WARNING);
	stCfg.size = CA_GetVariSize(CA_RESULT_TYPE_WARNING);
	err |= Config_Item_Register(&stCfg);
	
	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_INFO);
	stCfg.enType = CONFIG_ITEM_TYPE_NUMARRAY;
	stCfg.pData = CA_GetVariAddress(CA_RESULT_TYPE_INFO);
	stCfg.size = CA_GetVariSize(CA_RESULT_TYPE_INFO);
	err |= Config_Item_Register(&stCfg);

	strcpy_s(stCfg.szKeyWord, sizeof(stCfg.szKeyWord), KEYWORD_NOTE);
	stCfg.enType = CONFIG_ITEM_TYPE_NUMARRAY;
	stCfg.pData = CA_GetVariAddress(CA_RESULT_TYPE_NOTE);
	stCfg.size = CA_GetVariSize(CA_RESULT_TYPE_NOTE);
	err |= Config_Item_Register(&stCfg);


	if (ERROR_SUCCESS != err)
	{
		return err;
	}

	//��ȡ�����ļ�
	err |= Config_Read();

	return err;

}

errno_t Init(int argc, _TCHAR* argv[])
{
	HANDLE handle;
	char szTmp[LINE_MAXSIZE] = { 0 };
	errno_t err = ERROR_SUCCESS;
	
	//��ȡ��ǰ·��,������Ϊ��ǰ����Ŀ¼
	Str_Wchar2Char(argv[0], szTmp, sizeof(szTmp));
	Str_GetBasename_Char(szTmp, szTmp, sizeof(szTmp));
	if (szTmp[0] != '\0' && ERROR_SUCCESS != _chdir(szTmp))
	{
		return ERROR_FAILED;
	}

	//��ǰ����Ŀ¼����ȫ�ֱ���
	memcpy(GitAutoCA_GetPath(), szTmp, strlen(szTmp) + 1);
	_getcwd(szTmp, sizeof(szTmp));
	memcpy(GitAutoCA_GetPathAbso(), szTmp, strlen(szTmp) + 1);
	
	//��ʼ��Log�ļ� ������������printf֮ǰ��
#if WriteLogToFile
	if (ERROR_SUCCESS != WriteLog_Init(PATH_WRITE_LOG))
	{
		return ERROR_FAILED;
	}
#endif	

	//��ȡ�����ļ�
	err |= _init_get_config();
	
	//��������ӡ��Ϣ
	system("cls");
	printf("Git CA Auto Check Tool\r\n");
	printf("MyPATH = %s\r\n", GitAutoCA_GetPath());
	printf("MyPATH_Absolute = %s\r\n", GitAutoCA_GetPathAbso());
	print_time();

	//����״̬Ϊstop
	err |= status_set("stop");

	//�л���BRANCH_OFFCIAL��֧
	system(Str_Cat("cd .. && git checkout ", GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL)));
	
	//����ԭ�е��ϴ���֧(����֮ǰ������������ϵ�����)
	system(Str_Cat("cd .. && git branch -D ", GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH)));

	//�½��ϴ���֧
	system(Str_Cat("cd .. && git checkout -b ", GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH)));

	//�л���BRANCH_OFFCIAL��֧
	system(Str_Cat("cd .. && git checkout ", GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL)));
	

	//����socket�����߳�
	handle = CreateThread(NULL, 0, Socket_Start, NULL, 0, NULL);

	return err;
}

void Fini()
{
#if WriteLogToFile
	WriteLog_Fini();
#endif
}

/***********************************������******************************************
ѭ����ȡ״̬����ȡ��start��ʼwork()
***********************************************************************************/
//��������ÿ���ȡһ�Σ��ȴ�start�������work();����hello���Ӧfine
int _tmain(int argc, _TCHAR* argv[])
{

	//============���¿�ʼд��=============
	char status[LINE_MAXSIZE];
	errno_t err = 0;

	//��ʼ��
	err = Init(argc, argv);
	if (ERROR_SUCCESS != err)
	{
		Fini();
		return err;
	}

	while (1)
	{
		memset(status, 0, sizeof(status));

		//��ȡ״̬
		if (ERROR_SUCCESS != status_get(status))
		{
			err = ERROR_FAILED;
			break;
		}

		//���Ϊhello���Ӧfine
		if (0 == strncmp(status, "hello", strlen("hello")))
		{
			printf("I`m fine,thank you\r\n");
			if (ERROR_SUCCESS != status_set("fine"))
			{
				err = ERROR_FAILED;
				break;
			}
		}
		//��״̬Ϊstart������
		else if (0 == strncmp(status, "start", strlen("start")))
		{
			//��ʼ��ҵ
			if (ERROR_SUCCESS != work())
			{
				err = ERROR_FAILED;
				break;
			}
		}
		//��״̬Ϊmerge��ֱ�Ӻ���BARNCH_OFFCIAL
		else if (0 == strncmp(status, "merge", strlen("merge")))
		{
			merge();
		}

		//��ʱ1��
		Sleep(1000);

	}

	//������������˳����������з�֧
	if (ERROR_SUCCESS != err)
	{
		status_set("lock_all");
	}

	Fini();
	return err;
}

/*
typedef errno_t(*GIT_WORK_PF)(void);
#define GIT_HOOK_NAME_MAX			64
typedef struct tagWorks
{
char szName[GIT_HOOK_NAME_MAX];
GIT_WORK_PF pfWork;
}GIT_WORKS_S;

static GIT_WORKS_S g_stWorks[] =
{
{ "hello", IamFine },
{ "start", work },
{ "merge", merge },
};
*/