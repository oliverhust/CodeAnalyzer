#ifndef __write_log_h__
#define __write_log_h__

#define WriteLog_PrintScreen 1

/* ����·�� */
errno_t WriteLog_Init(const char *cPath);

/* �ر��ļ� */
errno_t WriteLog_Fini();

/* ���������������ӡLog���ļ��� */
int WriteLog(const char *fmt, ...);

/* �պ��� ��ִ�ж��� */
int WriteLog_Null(const char *fmt, ...);

/*

#if WriteLogToFile
if (ERROR_SUCCESS != WriteLog_Init(PATH_WRITE_LOG))
{
return ERROR_FAILED;
}
#endif


#if WriteLogToFile
WriteLog_Fini();
#endif


//ת��printf()��Log���
#define WriteLogToFile 1

#if WriteLogToFile
#define printf WriteLog
#include "Write_Log.h"
#endif

*/


#endif