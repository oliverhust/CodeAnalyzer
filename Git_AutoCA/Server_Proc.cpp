#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "inc.h"
#include "CA_Result.h"
#include "Git_MemConf.h"
#include "Server_Socket.h"
#include "Server_Proc.h"

static char *g_pcSuccessFailed[] = { MSG_SUCCESS, MSG_FAILED, MSG_BUSYING };

/* ������������������ ERROR_SUCCESS��ʾ��� ERROR_BUSY��ʾ���ڽ��� */
static errno_t g_OtherJobsResult = ERROR_BUSYING;

/* ��״̬g_OtherJobsResult��ΪĬ��ֵ */
void ServerProc_ResetOtherJobsResult()
{
	g_OtherJobsResult = ERROR_BUSYING;
	return;
}
/* ��ȡ������ */
errno_t ServerProc_GetOtherJobsResult()
{
	return g_OtherJobsResult;
}

/* ˵���ѱ�����ɣ�����¼������ */
errno_t ServerProc_RecvOtherJobsResult(char *pcInMsg, char *pcSend, int iSendSize)
{
	printf("Receive the message of OtherJobs Finished\r\n");
	
	g_OtherJobsResult = ERROR_SUCCESS;
	
	//֪ͨ�ͻ����ѳɹ���ñ�����
	strcpy_s(pcSend, iSendSize, SEND_HEAD_SUCCESS);

	return ERROR_SUCCESS;
}

/* ��Ӧ�ͻ��˲�ѯCA������ļ�·�� */
errno_t ServerProc_GetCAResultPath(char *pcInMsg, char *pcSend, int iSendSize)
{
	//��ȡCA���
	if (ERROR_SUCCESS == CA_GetCAResult())
	{
		//CA·��
		sprintf_s(pcSend, iSendSize, "%s%s\\%s",
			SEND_HEAD_SUCCESS,
			GitAutoCA_GetPathAbso(),
			GitAutoCA_GetFileName(FILE_TYPE_CA_RESULT));
	}
	else if (ERROR_FAILED == CA_GetCAResult())
	{
		//CA·��
		sprintf_s(pcSend, iSendSize, "%s%s\\%s",
			SEND_HEAD_FAILED,
			GitAutoCA_GetPathAbso(),
			GitAutoCA_GetFileName(FILE_TYPE_CA_RESULT));
	}
	else
	{
		strcpy_s(pcSend, iSendSize, SEND_HEAD_BUSY);
	}

	return ERROR_SUCCESS;
}

/* ��Ӧ�ͻ��˲�ѯFini.py��·�� */
errno_t ServerProc_GetFiniPyPath(char *pcInMsg, char *pcSend, int iSendSize)
{
	sprintf_s(pcSend, iSendSize, "%s\\%s %s %s %s", 
		GitAutoCA_GetPathAbso(),  GitAutoCA_GetFileName(FILE_TYPE_FINI_PY),
		GitAutoCA_GetBranchName(BRANCH_TYPE_OFFCIAL),
		GitAutoCA_GetBranchName(BRANCH_TYPE_PUSH),
		g_pcSuccessFailed[CA_GetCAResult()]
		);

	return ERROR_SUCCESS;
}

