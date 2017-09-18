#ifndef __write_log_h__
#define __write_log_h__

#define WriteLog_PrintScreen 1

#define WRITELOG_FILENAME_MAX 256

/* ����·�� */
errno_t WriteLog_Init(const char *cPath);

/* �ر��ļ� */
errno_t WriteLog_Fini();

/* ���������������ӡLog���ļ��� */
int WriteLog(const char *fmt, ...);

#endif

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
#define printf(x) WriteLog(x)
#include "Write_Log.h"
#endif

*/


