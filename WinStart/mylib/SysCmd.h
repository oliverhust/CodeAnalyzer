#ifndef __syscmd_h__
#define __syscmd_h__

/* һ�������ʾ�ַ��� */
#define SYSCMD_STD_LINE_MAX				256

/* �·���� */
#define SYSCMD_PATH_MAX					256

/* ����ϵͳ����ִ�н������ʱ�ļ� */
errno_t SysCmd_SetResultFile(char *pcPath);

/* ��ȡϵͳ�����ִ�н�� */
errno_t SysCmd_Result(const char *pcCmd, char *pcOut, int size);

#endif