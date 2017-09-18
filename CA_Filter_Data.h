#ifndef __ca_filter_data__h__
#define __ca_filter_data__h__


/* �����Դ洢��MD5��Ŀ���� */
#define CA_FILTER_DATA_MD5_MAX					8192
/* �����Դ�ſɺ����ļ��������� */
#define CA_FILTER_DATA_FILENAME_MAX				128

typedef struct tagCA_FILTER_MD5
{
	char szMD5[CA_FILTER_MD5_STR_MAX];
}CA_FILTER_MD5_S;

//================================================================================

/* ��ӵ���CA�澯��Ŀ MD5 */
errno_t CA_Filter_Data_Add_MD5(CA_FILTER_MD5_S *pstMD5);

/* ɾ������CA�澯��Ŀ MD5 */
errno_t CA_Filter_Data_Del_MD5(CA_FILTER_MD5_S *pstMD5);

/* ɾ������CA�澯��Ŀ MD5 */
void CA_Filter_Data_Del_MD5_All();

/* ��ѯ����CA�澯��Ŀ ��MD5���Ƿ���� */
errno_t CA_Filter_Data_IsExist_MD5(CA_FILTER_MD5_S *pstMD5);

/* ���㵱ǰ�ڴ��б���� MD5��Ŀ ���� */
int CA_Filter_Data_Count_MD5();

/* ��ȡ�ڴ��е�index��MD5��Ŀ������ */
errno_t CA_Filter_Data_Get_MD5(int index, CA_FILTER_MD5_S *pstCfg);

//================================================================================

/* ��ӵ���CA�澯��Ŀ �ļ��� */
errno_t CA_Filter_Data_Add_FileName(char *pcFileName);

/* ɾ������CA�澯��Ŀ �ļ��� */
errno_t CA_Filter_Data_Del_FileName(char *pcFileName);

/* ɾ������CA�澯��Ŀ �ļ��� */
void CA_Filter_Data_Del_FileName_All();

/* ��ѯ����CA�澯��Ŀ ���ļ������Ƿ���� */
errno_t CA_Filter_Data_IsExist_FileName(char *pcFileName);

/* ���㵱ǰ�ڴ��б���� �ļ�����Ŀ ���� */
int CA_Filter_Data_Count_FileName();

/* ��ȡ�ڴ��е�index�� �ļ�����Ŀ ������ */
errno_t CA_Filter_Data_Get_FileName(int index, char pcFileName[CA_FILTER_FILENAME_MAX]);

//================================================================================

#endif