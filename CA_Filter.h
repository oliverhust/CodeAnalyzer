#ifndef __ca_filter_h__
#define __ca_filter_h__

/* �Ƿ�ֻΪ����CA_Filter.exe��ִ���ļ� */
#define CA_FILTER_MAKE_EXE

/* CA�ű����� */
#define CA_FILTER_BAT_FILE							"CAnalyzer.bat"

/* MCA�ű����� */
#define CA_FILTER_M_BAT_FILE						"MCAnalyzer.bat"

/* ��ΪMCAʱ�ĳ����� */
#define CA_FILTER_MCA_EXE_NAME						"MCA_Filter.exe"

/* �����������Զ���ͬ�ȴ���ѧϰ/���ˣ�Ĭ����#define */
#define CA_FILTER_PROC_SHORTLINE

/* CA�ű�������󳤶� �����ļ�·�� */
#define CA_FILTER_ARG_LEN					512

/* Ҫ���Ե��ļ�������󳤶� */
#define CA_FILTER_FILENAME_MAX				255

/* CA�����ļ��п�ͷע�ͱ�ʶ */
#define CA_FILTER_CFG_COMMENT				"#"

/* CA�����ļ���Ҫ���Ե��ļ����Ŀ�ͷ��ʶ */
#define CA_FILTER_CFG_FILENAME				"--"

/* ��ʼ�� */ 
errno_t CA_Filter_Init();

/* ȥ��ʼ��*/
void CA_Filter_Fini();

/* ���ݱ�׼��CA Lint�ļ�����ѧϰ */
errno_t CA_Filter_Study(char *pcStandardFile);

/* ��ȡ�����ļ����������ݿ� */
errno_t CA_Filter_ReadCfgFiles(char *pcFileNameFile);

/* �����ݿ��е����ñ��浽�����ļ� */
errno_t CA_Filter_SaveCfgFiles(char *pcFileNameFile);

/* ��CA_Lint�ļ����ݽ��й��ˣ�����浽��һ���ļ� */
errno_t CA_Filter(char *pcSrcFile, char *pcDstFile);

/* ���ڴ���ɾ������ѧϰ��� */
errno_t CA_Filter_ClearAll();

#endif