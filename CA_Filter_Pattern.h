#ifndef __ca_filter_pattern_h__
#define __ca_filter_pattern_h__

/* "���е���С����" */
#define CA_FILTER_PATTERN_SHORT_MIN		2

/* "���е���С���" */
#define CA_FILTER_PATTERN_SHORT_MAX		6

/* �澯ID�ַ����Ĺ�� */
#define CA_FILTER_MESS_ID_MAX			8

/* ƥ���׼��:Ԥ��������ʽ */
errno_t CA_Filter_Pattern_Init();

/* ƥ��������ͷ��ڴ� */
void CA_Filter_Pattern_Fini();

/* ����Ч���ַ���2~6����֮����Ϊ�����С� */
BOOL CA_Filter_Pattern_IsShortLine(char *pcPatt);

/* �Ƿ�ȫ��Ϊ�ո��ַ� */
BOOL CA_Filter_Pattern_IsAllSpace(char *pcPatt);

/* �Ƿ����ֱ�Ӵ�ӡ�����账�� CA�澯�߽磬�س� */
BOOL CA_Filter_Pattern_IsPutsDirect(char *pcPatt);

/* �ж��Ƿ�ΪNote��Ϣ */
BOOL CA_Filter_Pattern_IsNote(char *pcPatt);

/* �ж��Ƿ�Note900��һ�εĸ��ָ澯���� */
BOOL CA_Filter_Pattern_IsAfterMessage(char *pcPatt);

/* ��ʶ���ȡ��Ϣ:�ļ��� MD5(����Ϊ"") �澯���ͣ������ȥ���кŵ��ַ��� */
errno_t CA_Filter_Pattern_LineGetAndModify(char *pcContent, CA_FILTER_SEG_GET_S *pstCfg, char *pcOut, int size);

/* �޸�����ͳ����Ϣ */
errno_t CA_Filter_Pattern_ModifyNote(STR_PCREMODIFY_PROC_PF pfModify, char *pcContent, char *pcOut, int size);

#endif