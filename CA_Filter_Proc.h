#ifndef __ca_filter_proc_h__
#define __ca_filter_proc_h__

/* Note��Ϣ��������ԭ����ͳ����Ϣ�޸� */
void CA_Filter_Proc_Note(char *pcSeg, FILE *stream);

/* ��ͨSeg�δ�������������MD5,�ļ�������ѯ���ݿ������� ͳ�Ƹ澯 */
void CA_Filter_Proc_Seg(char *pcSeg, FILE *stream);

/* ����һ���Σ�ѧϰ����Σ��޷�ʶ���򷵻�ERROR_FAILED */
errno_t CA_Filter_Proc_Study(char *pcSeg);

/* ��ȡһ���ļ����ļ�ÿ�о�ΪҪ����CA�澯�Ĵ����ļ����������ݿ� */
void CA_Filter_Proc_IgnoreFile(FILE *fil);

/* ���浱ǰ�����е��������õ����� */
errno_t CA_Filter_Proc_SaveCfgFile(FILE *fil);

#endif