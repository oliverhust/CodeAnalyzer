#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "Str.h"
#include "pcre.h"

#include "CA_Filter.h"
#include "CA_Filter_inc.h"
#include "CA_Filter_Pattern.h"

//#pragma comment(lib, "pcre.lib") 

#define CA_FILTER_PCRE_OVECT_NUM				30

static pcre *g_pstReBasic, *g_pstReMD5, *g_pstModifyNote;
static pcre_extra* g_pcExBasic, *g_pcExMD5;

static char *g_apcCA_Filter_MessageType[CA_FILTER_MESSAGE_MAX] = { "Error", "Warning", "Info", "Note" };

/* ʶ��ǰ���Ƿ��л�����Ϣ(�ļ������澯����) �ĳ�ʼ�� */
static errno_t _CA_Filter_Pattern_BasicInfo_Init()
{
	int iErroffset;
	const char* pcError;
	char *pcPattern = "\\s([C-Zc-z_]:.*) [lL](\\d+) (\\w+) (\\d+)";

	/* ��������ʽ */
	g_pstReBasic = pcre_compile(pcPattern, PCRE_UTF8, &pcError, &iErroffset, NULL);
	if (NULL == g_pstReBasic)
	{
		printf("compile regex expression \'%s\' error.\r\n", pcPattern);
		return ERROR_FAILED;
	}

	/* �Ա���������ѧϰ */
	g_pcExBasic = pcre_study(g_pstReBasic, 0, &pcError);

	return ERROR_SUCCESS;
}

/* ʶ��ǰ���Ƿ���MD5��Ϣ �ĳ�ʼ�� */
static errno_t _CA_Filter_Pattern_MD5_Init()
{
	int iErroffset;
	const char* pcError;
	char *pcPattern = "\\s[C-Zc-z_]:.* [lL]\\d+ \\w+ \\d+.*\\[([0-9a-fA-F]{32})\\]";

	/* ��������ʽ */
	g_pstReMD5 = pcre_compile(pcPattern, PCRE_UTF8, &pcError, &iErroffset, NULL);
	if (NULL == g_pstReMD5)
	{
		printf("compile regex expression \'%s\' error.\r\n", pcPattern);
		return ERROR_FAILED;
	}

	/* �Ա���������ѧϰ */
	g_pcExMD5 = pcre_study(g_pstReMD5, 0, &pcError);

	return ERROR_SUCCESS;
}

errno_t _CA_Filter_Modify_Note_Init()
{
	char *pcPattern = "Total (\\d+) message.* (\\d+) Error, (\\d+) Warning, (\\d+) Info, (\\d+) Note";

	g_pstModifyNote = Str_PcreModify_Register(pcPattern, PCRE_UTF8);
	if (NULL != g_pstModifyNote)
	{
		return ERROR_SUCCESS;
	}

	return ERROR_FAILED;
}

/* ƥ���׼��:Ԥ��������ʽ */
errno_t CA_Filter_Pattern_Init()
{
	errno_t err = ERROR_SUCCESS;

	err |= _CA_Filter_Pattern_BasicInfo_Init();
	err |= _CA_Filter_Pattern_MD5_Init();
	err |= _CA_Filter_Modify_Note_Init();

	return err;
}

/* ƥ��������ͷ��ڴ� */
void CA_Filter_Pattern_Fini()
{
	pcre_free(g_pstReBasic);
	pcre_free_study(g_pcExBasic);

	pcre_free(g_pstReMD5);
	pcre_free_study(g_pcExMD5);

	Str_PcreModify_UnRegister(g_pstModifyNote);

	return;
}

/* δƥ���еĴ��� �������֡��ո� */
static void _ca_Proc_NotMatchLine(char *pcINLine, char *pcOut, int iOutSize)
{
	int iIN = 0, iOut = 0;
	unsigned char c;

	if (NULL == pcINLine || NULL == pcOut)
	{
		return;
	}

	while (NULL != (c = (unsigned char)pcINLine[iIN]))
	{
		/* ���������Ҳ��ǿո�ͼӽ�ȥ */
		if (!isdigit(c) && !isspace(c))
		{
			if (iOut >= iOutSize - 1)
			{
				break;
			}
			pcOut[iOut] = c;
			iOut++;
		}
		iIN++;
	}
	pcOut[iOut] = '\0';

	return;
}


/* ��ʶ���ȡ��Ϣ:�ļ��� MD5(����Ϊ"") �澯���ͣ������ȥ���кŵ��ַ��� */
errno_t CA_Filter_Pattern_LineGetAndModify(char *pcContent, CA_FILTER_SEG_GET_S *pstCfg, char *pcOut, int size)
{
	int aiOvector[CA_FILTER_PCRE_OVECT_NUM] = { 0 }, iRc, iLen, i, iLineNumBegin, iLineNumFinNext;
	char szSub[LINE_MAXSIZE] = { 0 }, szID[CA_FILTER_MESS_ID_MAX] = { 0 };

	iLen = strlen(pcContent);

	//��ȡ������Ϣ,�����к�λ��
	iRc = pcre_exec(g_pstReBasic, g_pcExBasic, pcContent, iLen, 0, 0, aiOvector, sizeof(aiOvector) / sizeof(int));
	if (iRc < 0)
	{
		/* δƥ������ */
		_ca_Proc_NotMatchLine(pcContent, pcOut, size);
		return ERROR_FAILED;
	}

	//����Ҫ�Ĳ����鿽���������д���
	//�ļ���
	pcre_copy_substring(pcContent, aiOvector, iRc, 1, szSub, sizeof(szSub));
	Str_GetFilename_Char(szSub, szSub, sizeof(szSub));
	strcpy_s(pstCfg->szFileName, CA_FILTER_FILENAME_MAX, szSub);
	//�к���Ϣ
	iLineNumBegin = aiOvector[2 * 2 + 0];
	iLineNumFinNext = aiOvector[2 * 2 + 1];
	//�澯����
	pcre_copy_substring(pcContent, aiOvector, iRc, 3, szSub, sizeof(szSub));
	for (i = 0; i < CA_FILTER_MESSAGE_MAX; i++)
	{
		if (0 == strcmp(g_apcCA_Filter_MessageType[i], szSub))
		{
			pstCfg->enType = (CA_FILTER_MESSAGE_E)i;
			break;
		}
	}
	/* �Ҳ������ָ澯���� Error,Info,... */
	if (CA_FILTER_MESSAGE_MAX == i)
	{
		_ca_Proc_NotMatchLine(pcContent, pcOut, size);
		return ERROR_FAILED;
	}
	//�澯ID
	pcre_copy_substring(pcContent, aiOvector, iRc, 4, szID, sizeof(szID));

	//���Ի�ȡMD5
	memset(aiOvector, 0, sizeof(aiOvector));
	iRc = pcre_exec(g_pstReMD5, g_pcExMD5, pcContent, iLen, 0, 0, aiOvector, sizeof(aiOvector) / sizeof(int));
	if (0 <= iRc)
	{
		pcre_copy_substring(pcContent, aiOvector, iRc, 1, pstCfg->szMD5, CA_FILTER_MD5_STR_MAX);
	}

	//ֻ����ļ��� �澯���� �澯ID
	sprintf_s(pcOut, size, "%s %s %s\n", pstCfg->szFileName, g_apcCA_Filter_MessageType[pstCfg->enType], szID);

	return ERROR_SUCCESS;
}

/* �޸�����ͳ����Ϣ */
errno_t CA_Filter_Pattern_ModifyNote(STR_PCREMODIFY_PROC_PF pfModify, char *pcContent, char *pcOut, int size)
{
	return Str_PcreModify(g_pstModifyNote, pfModify, pcContent, pcOut, size);
}

/* ����Ч���ַ���2~6����֮����Ϊ�����С� */
BOOL CA_Filter_Pattern_IsShortLine(char *pcPatt)
{
	int i, iLen, iCount = 0;

	iLen = strlen(pcPatt);

	for (i = 0; i < iLen; i++)
	{
		if (!isspace((unsigned char)pcPatt[i]))
		{
			iCount++;
		}
	}

	if (CA_FILTER_PATTERN_SHORT_MIN <= iCount && iCount <= CA_FILTER_PATTERN_SHORT_MAX)
	{
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

/* �ж��Ƿ�Note900��һ�εĸ��ָ澯���� */
BOOL CA_Filter_Pattern_IsAfterMessage(char *pcPatt)
{
	char szLine[LINE_MAXSIZE] = { 0 };

	Str_Get_FirstLine(pcPatt, szLine, sizeof(szLine));

	if (NULL != Str_Contains_N(szLine, "Count", "Num", "Type", (char *)0) || 
		NULL != Str_Contains_N(szLine, "Count", "No", "Type", (char *)0))
	{
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

/* �Ƿ�ȫ��Ϊ�ո��ַ� */
BOOL CA_Filter_Pattern_IsAllSpace(char *pcPatt)
{
	int i, iLen;
	BOOL bRet = BOOL_TRUE;

	iLen = strlen(pcPatt);

	for (i = 0; i < iLen; i++)
	{
		if (!isspace((unsigned char)pcPatt[i]))
		{
			bRet = BOOL_FALSE;
			break;
		}
	}

	return bRet;
}


/* �Ƿ����ֱ�Ӵ�ӡ�����账�� CA�澯�߽磬�س� */
BOOL CA_Filter_Pattern_IsPutsDirect(char *pcPatt)
{
	/* ���ȫ���ǿո�������� */
	if (BOOL_TRUE == CA_Filter_Pattern_IsAllSpace(pcPatt))
	{
		return BOOL_TRUE;
	}
	
	if (NULL != Str_BeginWith(pcPatt, "--- END") ||
		NULL != Str_BeginWith(pcPatt, "--- Lintware") ||
		NULL != Str_BeginWith(pcPatt, "--- Codeware") ||
		NULL != Str_BeginWith(pcPatt, "--- Checkware"))
	{
		return BOOL_TRUE;
	}

	if (NULL != strstr(pcPatt, "CAnalyzer") && 
	   (NULL != Str_BeginWith(pcPatt, "---") || NULL != Str_BeginWith(pcPatt, "--- BEG")))
	{
		return BOOL_TRUE;
	}

	return BOOL_FALSE;
}

/* �ж��Ƿ�ΪNote��Ϣ */
BOOL CA_Filter_Pattern_IsNote(char *pcPatt)
{
	if (NULL != Str_BeginWith(pcPatt, "Note 900") &&
		NULL != Str_Contains_N(pcPatt, "\n", "Total", "message", "Error", "Warning", "Info", "Note", (char *)0))
	{
		return BOOL_TRUE;
	}
	return BOOL_FALSE;
}


/*
static char *pcContent = \
"*** E:\\SVTI\\Source\\NETFWD\\src\\sbin\\tunnel\\xmluiplugin\\tunnel_xml_tunnels.c L753 Warning 426 _xml_TunnelProcTlvMsg(?, ?, !=0, !=0) \r\n"
"    E:\\SVTI\\Source\\NETFWD\\src\\sbin\\tunnel\\xmluiplugin\\tunnel_xml_tunnels.c L676 Warning 426[_xml_TunnelProcTlvMsg][6FE139CB319B099894F74038C800CB1A]\r\n"
"Call to function 'TLV_CreateReceiveMsg(unsigned int, void *, unsigned short)' violates semantic '2p' \r\n";
static char *pcContent1 = "Hello, world!\r\n";

int _tmain(int argc, char* argv[])
{
	pcre *pstRe;
	pcre_extra* pcEx;
	int aiOvector[30], iErroffset, iRc;
	const char* pcError;
	char *pcPattern = "\\s([C-Zc-z_]:.*) [lL]([0-9][0-9]*) (\\w+) (\\d+)";
	char szSub[LINE_MAXSIZE] = { 0 };

	int i, iOffset = 0;

	// ��������ʽ 
	pstRe = pcre_compile(pcPattern, PCRE_MULTILINE | PCRE_UTF8, &pcError, &iErroffset, NULL);
	if (NULL == pstRe)
	{
		printf("compile regex expression \'%s\' error.\r\n", pcPattern);
		return ERROR_FAILED;
	}

	// �Ա���������ѧϰ 
	pcEx = pcre_study(pstRe, PCRE_STUDY_JIT_COMPILE, &pcError);

	// ��ȡ���ƥ���� 
	// ����ʽƥ��,����ƥ���ƫ��λ�� 
	while (0 <= (iRc = pcre_exec(pstRe, pcEx, pcContent, strlen(pcContent), iOffset, 0, aiOvector, sizeof(aiOvector) / sizeof(int))))
	{
		printf("Match Regex Expression:\r\n");
		// ��ȡ��������Ӵ� 
		for (i = 0; i <= 4; ++i)
		{
			pcre_copy_substring(pcContent, aiOvector, iRc, i, szSub, sizeof(szSub));
			printf("Sub%d=%s\r\n", i, szSub);
			//��ӡƥ���Ӵ�����ʼ�ֽ�λ��
			printf("aiOvector[%d]=%d\r\n", 2 * i, aiOvector[2 * i]);
			//��ӡƥ���Ӵ�ĩβ�ٺ���һ���ֽڵ�λ��
			printf("aiOvector[%d]=%d\r\n", 2 * i + 1, aiOvector[2 * i + 1]);

		}

		// ����ƫ�ƣ�׼����һ��ƥ�� 
		iOffset = aiOvector[1];

		printf("\r\n");
	}

	pcre_free(pstRe);
	system("pause");
	return 0;
}
*/

/* //ԭ��Ĵ���
int _tmain(int argc, char* argv[])
{
	char pattern[] = "\\d+";
	char content[] = "owaefj ifea 894 aewoai 43";
	const char* error;
	int erroffset;
	int cur = 0;
	int ovector[30];
	pcre *re;

	re = pcre_compile(pattern, PCRE_MULTILINE | PCRE_NO_AUTO_CAPTURE | PCRE_UTF8, &error, &erroffset, NULL);

	pcre_extra* pcEx = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &error);
	int rc = pcre_exec(re, pcEx, content, strlen(content), cur, PCRE_NOTEMPTY, ovector, sizeof(ovector)/sizeof(int));

	if (rc >= 0)
	{
		char* ptr;
		ptr = content + ovector[1];
		*ptr = '\0';
		printf("%s\n", content + ovector[0]);
	}

	pcre_free(re);
	system("pause");
	return 0;
}
*/
