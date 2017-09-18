#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>

#include "inc.h"
#include "CA_Result.h"
#include "Git_MemConf.h"
#include "Server_Socket.h"
#include "Server_Proc.h"

#pragma comment(lib, "ws2_32.lib")

SERVER_FUNC_S g_stServerFunction[] = 
{
	{ ServerProc_RecvOtherJobsResult, SEND_HEAD_ALL_JOBS },
	{ ServerProc_GetCAResultPath, SEND_HEAD_CA_RESULT_PATH},
	{ ServerProc_GetFiniPyPath, SEND_HEAD_FINI_PY_PATH}
};

/* ���������Ϣͷ�����ö�Ӧ������ */
errno_t _server_ServiceFunctions(char *pcRecv, char *pcSend, int iSendSize)
{
	int i, n, iHeadLen;

	n = sizeof(g_stServerFunction) / sizeof(SERVER_FUNC_S);
	for (i = 0; i < n; i++)
	{
		iHeadLen = strlen(g_stServerFunction[i].szHead);
		if (0 == strncmp(pcRecv, g_stServerFunction[i].szHead, iHeadLen))
		{
			g_stServerFunction[i].pfProc(pcRecv + iHeadLen, pcSend, iSendSize);
			break;
		}
	}

	if (i == n)
	{
		return ERROR_FAILED;
	}

	return ERROR_SUCCESS;
}

/* ������Ϣͷ�б��������Ȼ�󷢻���Ϣ */
errno_t _server_Services(SOCKET ConnFd)
{
	int i;
	errno_t ret = ERROR_SUCCESS;
	char szRecvBuf[SERVER_RECV_BUF_MAX] = { 0 };
	char szSendBuf[SERVER_SEND_BUF_MAX] = { 0 };

	while (1)
	{
		i = recv(ConnFd, szRecvBuf, sizeof(szRecvBuf) - 1, 0);
		if (0 == i)
		{
			//�ɿͻ��������ر�����
			closesocket(ConnFd);
			ret = ERROR_SUCCESS;
			break;
		}
		else if (0 > i)
		{
			printf("A Client`s message receive error.\r\n");
			closesocket(ConnFd);
			ret = ERROR_FAILED;
			break;
		}
		szRecvBuf[i] = '\0';
		printf("Recv:%s\r\n", szRecvBuf);

		if (ERROR_SUCCESS != _server_ServiceFunctions(szRecvBuf, szSendBuf, sizeof(szSendBuf)))
		{
			printf("A Client`s message has error.\r\n");
			closesocket(ConnFd);
			ret = ERROR_FAILED;
			break;
		}

		i = send(ConnFd, szSendBuf, strlen(szSendBuf), 0);
		printf("Send:%s\r\n", szSendBuf);
		if (i != strlen(szSendBuf))
		{
			printf("Server SendError.\r\n");
			closesocket(ConnFd);
		}

	}

	return ret;
}


/* Socket_Server�߳� */
DWORD WINAPI Socket_Start(LPVOID pM)
{
	// ����socket��̬���ӿ�(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // ��ṹ�����ڽ���Wjndows Socket�Ľṹ��Ϣ��  
	int err;

	wVersionRequested = MAKEWORD(1, 1);   // ����1.1�汾��WinSock��  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
		return -1;          // ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup  
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) 
	{
		// ���������ֽ��ǲ���1�����ֽ��ǲ���1��ȷ���Ƿ������������1.1�汾  
		// ����Ļ�������WSACleanup()�����Ϣ����������  
		WSACleanup();
		return -1;
	}

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockSrv  
	// SOCKET socket(int af, int type, int protocol);  
	// ��һ��������ָ����ַ��(TCP/IPֻ����AF_INET��Ҳ��д��PF_INET)  
	// �ڶ�����ѡ���׽��ֵ�����(��ʽ�׽���)�����������ض���ַ�������Э�飨0Ϊ�Զ���  
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockSrv)
	{
		printf("Create socket error\r\n");
		return ERROR_FAILED;
	}

	// �׽���sockSrv�뱾�ص�ַ����  
	// int bind(SOCKET s, const struct sockaddr* name, int namelen);  
	// ��һ��������ָ����Ҫ�󶨵��׽��֣�  
	// �ڶ���������ָ�����׽��ֵı��ص�ַ��Ϣ���õ�ַ�ṹ�������õ�����Э��Ĳ�ͬ����ͬ  
	// ������������ָ��������Э���ַ�ĳ���  
	// PS: struct sockaddr{ u_short sa_family; char sa_data[14];};  
	//                      sa_familyָ���õ�ַ���壬 sa_data��ռλռ��һ���ڴ������������  
	//     ��TCP/IP�У���ʹ��sockaddr_in�ṹ�滻sockaddr���Է�����д��ַ��Ϣ  
	//   
	//     struct sockaddr_in{ short sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8];};  
	//     sin_family��ʾ��ַ�壬����IP��ַ��sin_family��Ա��һֱ��AF_INET��  
	//     sin_portָ����Ҫ������׽��ֵĶ˿ڡ�  
	//     sin_addr�����׽��ֵ�����IP��ַ��  
	//     sin_zero[8]�������������sockaddr_in��sockaddr�ṹ�ĳ���һ����  
	//     ��IP��ַָ��ΪINADDR_ANY�������׽������κη�������ػ�����IP��ַ���ͻ�������ݡ�  
	//     �����ֻ���׽���ʹ�ö��IP�е�һ����ַ����ָ��ʵ�ʵ�ַ����inet_addr()������  
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ��INADDR_ANYת��Ϊ�����ֽ��򣬵��� htonl(long��)��htons(����)  
	//addrSrv.sin_addr.S_un.S_addr = htonl(inet_addr("192.168.56.28"));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(GitAutoCA_GetCAServerPort());

	if (-1 == bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) // �ڶ�����Ҫǿ������ת��  
	{
		printf("Bind port %d Error\r\n", GitAutoCA_GetCAServerPort());
		return ERROR_FAILED;
	}

	// ���׽�������Ϊ����ģʽ���������󣩣� listen()֪ͨTCP������׼���ý�������  
	// int listen(SOCKET s,  int backlog);  
	// ��һ������ָ����Ҫ���õ��׽��֣��ڶ�������Ϊ���ȴ����Ӷ��е���󳤶ȣ�  
	if (-1 == listen(sockSrv, SERVER_LISTEN_MAX))
	{
		printf("Listen %d Nums Error\r\n", SERVER_LISTEN_MAX);
		return ERROR_FAILED;
	}

	// accept()���������ӣ��ȴ��ͻ�������  
	// SOCKET accept(  SOCKET s,  struct sockaddr* addr,  int* addrlen);  
	// ��һ������������һ�����ڼ���״̬�µ��׽���  
	// �ڶ���������sockaddr���ڱ���ͻ��˵�ַ����Ϣ  
	// ����������������ָ�������ַ�ĳ���  
	// ���ص��������������״̬�µ��׽���ͨ�ŵ��׽���  

	// �ͻ������û��˽���ͨ��  

	// send(), ���׽����Ϸ�������  
	// int send( SOCKET s,  const char* buf,  int len,  int flags);  
	// ��һ����������Ҫ������Ϣ���׽��֣�  
	// �ڶ�����������������Ҫ�����͵����ݣ�  
	// ������������buffer�����ݳ��ȣ�  
	// ���ĸ�������һЩ���Ͳ���������  

	// recv(), ���׽����Ͻ�������  
	// int recv(  SOCKET s,  char* buf,  int len,  int flags);  
	// ��һ���������������Ӻ���׽��֣�  
	// �ڶ�����������������  
	// �������������������ݵĳ��ȣ�  
	// ���ĸ�������һЩ���Ͳ���������  

	SOCKADDR_IN  addrClient;
	int len = sizeof(SOCKADDR);

	// ���ϵȴ��ͻ�������ĵ���
	while(1)
	{      
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		if (-1 == sockConn)
		{
			printf("Accept a Client Error.\r\n");
			continue;
		}
		
		//������Ϣͷ��ɶ�Ӧ����
		(void)_server_Services(sockConn);
	}

	return ERROR_SUCCESS;
}