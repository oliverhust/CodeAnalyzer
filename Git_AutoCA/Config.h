#ifndef __config_h__
#define __config_h__

/* һ�е�����볤�� ��'\0' */
#define CONFIG_LINE_MAXSIZE 512

#define CONFIG_KEYWORD_LEN_MAX 63

/* ���֧�ֶ��ٵ���Ŀ  */
#define CONFIG_ITEMS_MAX     256

/* ��Ŀ������ */
enum CONFIG_ITEM_TYPE_E
{
	CONFIG_ITEM_TYPE_NUM = 0,   /* ������ֵ */
	CONFIG_ITEM_TYPE_NUMARRAY,  /* ���� */
	CONFIG_ITEM_TYPE_CHAR,      /* �����ַ� */
	CONFIG_ITEM_TYPE_STRING,   /* �����ַ����������пո�����������ַ���'_' */
	CONFIG_ITEM_TYPE_LINE,     /* '=' �ź���������һ�У������ո� ���Զ�ȥ��'\n'�� */

	CONFIG_ITEM_TYPE_MAX
};

typedef struct tag_CONFIG_CFG_S
{
	char szKeyWord[CONFIG_KEYWORD_LEN_MAX + 1];
	enum CONFIG_ITEM_TYPE_E enType;
	void *pData;    /*  Ҫ�洢���ı���ָ��  */
	int size;       /* ����/������������ֽ��� */

}CONFIG_CFG_S;


errno_t Config_SetPath(char *pcPath);
errno_t Config_Item_Register(CONFIG_CFG_S *pstCfg);
errno_t Config_Item_UnRegister(char *pcKeyWord);
errno_t Config_Item_UnRegisterAll();
errno_t Config_PhaseEachLine(char *line);
errno_t Config_Read();

/* ���ע���˶��Keyһ����������Ƕ���ע�ᣬ��ʹ������ע����
ȥע��ʱ���ж�����ͬ��Key������ȥע�����µ�һ��  */


#endif

/*
��ȡ�����ļ�,��
name = INFO
error = 12  19  5  6
*/
