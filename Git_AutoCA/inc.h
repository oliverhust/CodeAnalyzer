#ifndef __inc_h__
#define __inc_h__

//�û�ÿ��push�ϴ��ķ�֧��(Ĭ��)
#define BRANCH_PUSH_DFT				" work "
//"�ٷ�"��֧,��ʽ�汾��Ĭ�ϣ�
#define BRANCH_OFFCIAL_DFT			" master "

//������ĵ�һ����������
#define ARG_ME1_START				"start"
#define ARG_ME1_MERGE				"merge"

//·����Ҫ�ͱ�Ľű�Լ����(���ߴ������ļ���ȡ),Ĭ���ļ���
#define FILE_CONFIG_DFT             "Git_AutoCA_Config.txt"
#define FILE_STATUS_DFT				"sys_status.txt"
#define FILE_CA_RESULT_DFT			"CA_Lint.txt"

//PATH_CA_RUN��PATH_SENDMAIL�ͱ�����ҪͬһĿ¼
#define FILE_CA_RUN_DFT				"Run_CA.bat"
#define FILE_SENDMAIL_DFT			"Fini.py"
#define FILE_WGET_TMP_DFT			"__None__"

//Ĭ��socket�������˿�
#define SERVER_PORT_DFT				7777	

//�����ļ��еĹؼ�������
#define KEYWORD_BRANCH_OFFCIAL		"branch_offcial"
#define KEYWORD_BRANCH_PUSH			"branch_push"
#define KEYWORD_ERROR				"error"
#define KEYWORD_WARNING				"warning"
#define KEYWORD_INFO				"info"
#define KEYWORD_NOTE				"note"
#define KEYWORD_TXT_STATUS			"status_txt"
#define KEYWORD_TXT_LINT			"ca_lint_txt"
#define KEYWORD_CA_RUN				"ca_run"
#define KEYWORD_SENDMAIL			"sendmail"
#define KEYWORD_WGET_TMP			"wget_tmp"
#define KEYWORD_CA_PORT				"ca_port"

#define PATH_WRITE_LOG				"git_log_print.txt"

#define LINE_MAXSIZE				512
#define ERROR_FAILED				1		//�������󣬽���������
#define ERROR_BUSYING				2		//æµ�У��������������ڱ���...
//#define ERROR_NOT_FOUND			2		//�Ҳ�������
#define GRADE_LINT					20		//Line���ж���error�����ֵҪ����������
#define BUILD_ID_UNKONWN			"0"


#endif