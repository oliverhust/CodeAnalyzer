#ifndef __CA_Result_h__
#define __CA_Result_h__

typedef enum
{
	CA_RESULT_TYPE_ERROR = 0,
	CA_RESULT_TYPE_WARNING,
	CA_RESULT_TYPE_INFO,
	CA_RESULT_TYPE_NOTE,
	CA_RESULT_TYPE_MAX

}CA_RESULT_TYPE_E;

/* ����CA����æµ��� */
void CA_SetBusy();

/* �ȴ�CA���� */
errno_t CA_WaitFinish();

/* ͨ��CA�ļ��ж�CA�Ƿ�����Ҫ�󣬱�����(�ɹ�/ʧ��) */
BOOL CA_JudgeAndSetReulst();

/* ����CA_JudgeAndSetReulst()����Ľ���ж�CA�Ƿ�ͨ��(�����ٴζ�ȡCA_Lint�ļ�) */
errno_t CA_GetCAResult();

/* ��ȡ��������ĵ�ַ  */
void* CA_GetVariAddress(CA_RESULT_TYPE_E enType);

/* ��ȡ�����Ĵ�С */
int CA_GetVariSize(CA_RESULT_TYPE_E enType);


#endif
