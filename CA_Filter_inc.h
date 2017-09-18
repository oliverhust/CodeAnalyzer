#ifndef __ca_filter_inc_h__
#define __ca_filter_inc_h__

/* �����ļ���MD5ע��ͷ */
#define CA_FILTER_CFG_COMMENT_MD5_HEAD				"# The MD5 of CA error:"

/* �����ļ��ĺ����ļ���ע��ͷ */
#define CA_FILTER_CFG_COMMENT_FILE_HEAD				"# The Files with CA error to be ignore:"

/* ÿһ������ֽ�����ͨ�ã� */
#define LINE_MAXSIZE					1024

/* ���Ķ��ֽ��� */
#define SEGMENT_MAXSIZE					8192

/* ���Դ�CA���ԭ�ļ��Ĵ��� */
#define CA_FILTER_OPENORIGIN_TIMES		20

/* ���Դ�CA���ԭ�ļ���ʱ���� ���� */
#define CA_FILTER_OPENORIGIN_INTERVAL	200

#define IN
#define OUT
#define ERROR_FAILED					1
#define BOOL_TRUE						1
#define BOOL_FALSE						0

#define CA_FILTER_MD5_STR_MAX			33
#define CA_FILTER_MESSAGE_TYPE_LEN_MAX	16

typedef enum
{
	CA_FILTER_MESSAGE_ERROR = 0,
	CA_FILTER_MESSAGE_WARNING,
	CA_FILTER_MESSAGE_INFO,
	CA_FILTER_MESSAGE_NOTE,
	CA_FILTER_MESSAGE_MAX
}CA_FILTER_MESSAGE_E;

typedef struct tagMessageCount
{
	int iError;
	int iWarning;
	int iInfo;
	int iNote;
}CA_FILTER_MESSAGE_S;

typedef struct tagCA_FILTER_SEG_GET
{
	CA_FILTER_MESSAGE_E enType;
	char szFileName[CA_FILTER_FILENAME_MAX];
	char szMD5[CA_FILTER_MD5_STR_MAX];
}CA_FILTER_SEG_GET_S;

#ifdef CA_FILTER_MAKE_EXE

/* CAԭ�����ʱ�ļ��� */
#define CA_FILTER_ORIGIN_RESULT						"CA_Filter_Result_Origin.txt"

/* CA���˺�Ľ���ļ��� */
#define CA_FILTER_NEW_RESULT						"CA_Filter_Result_New.txt"

/* ÿ�ζ�ȡ�������ļ��� */
#define CA_FILTER_READ_CFG_FILE						"CA_Filter.cfg"

/* ѧϰ���ļ��� */
#define CA_FILTER_STUDY_FILE						"CA_Filter_Study.txt"

/* �Զ��������õ��ļ��� */
#define CA_FILTER_AUTOSAVE_CFGFILE					"CA_Filter_AutoSave.cfg"

#endif

#endif