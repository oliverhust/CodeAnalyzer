#ifndef __server_proc_h__
#define __server_proc_h__

/* ��״̬g_OtherJobsResult��ΪĬ��ֵ */
void ServerProc_ResetOtherJobsResult();

/* ��ȡ������ */
errno_t ServerProc_GetOtherJobsResult();

/* ˵���ѱ�����ɣ�����¼������ */
errno_t ServerProc_RecvOtherJobsResult(char *pcInMsg, char *pcSend, int iSendSize);

/* ��Ӧ�ͻ��˲�ѯCA������ļ�·�� */
errno_t ServerProc_GetCAResultPath(char *pcInMsg, char *pcSend, int iSendSize);

/* ��Ӧ�ͻ��˲�ѯFini.py��·�� */
errno_t ServerProc_GetFiniPyPath(char *pcInMsg, char *pcSend, int iSendSize);



#endif