// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

//#pragma comment(linker, "/subsystem:windows /entry:tmainCRTStartup")

//�����ã���������Ҫע�͵�
//#define system(x) puts(x)

//�Ƿ������printf��¼��Log�ļ�
#define WriteLogToFile 1

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#if WriteLogToFile
#define printf WriteLog
#include "Write_Log.h"
#endif
