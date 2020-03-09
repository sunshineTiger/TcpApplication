#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#include <thread>

#pragma comment(lib, "ws2_32.lib")

// Ҫ�����ֽڶ������⣨32λ��64λ��ƽ̨��ϵͳ��
enum CMDTYPE
{
	CMD_LOGIN,			// ��¼
	CMD_LOGIN_RESULT,	// ��¼���ؽ��
	CMD_LOGOUT,			// �ǳ�
	CMD_LOGOUT_RESULT,	// �ǳ����ؽ��
	CMD_NEW_USER_JOIN,	// ���û�����
	CMD_ERROR			// ����
};

// ��Ϣͷ
struct DataHeader
{
	int	cmd;			// ��������
	int dataLength;		// ��Ϣ������ݳ���
};

// ��Ϣ��
// DataPackage
// ��¼
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

// ��¼���
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

// �ǳ�
struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

// �ǳ����
struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

// ���û�����
// �ǳ�
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};


int processor(SOCKET sock)
{
	// ������(4096�ֽ�)
	char szRecv[4096] = {};
	// 5�����տͻ��˵�����
	// �Ƚ�����Ϣͷ
	int recvLen = recv(sock, szRecv, sizeof(DataHeader), 0);
	DataHeader* pHeader = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("��������Ͽ����ӣ��������...\n");
		return -1;
	}

	// 6����������
	switch (pHeader->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		LoginResult* loginRes = (LoginResult*)szRecv;

		printf("�յ���������Ϣ��CMD_LOGIN_RESULT, ���ݳ��ȣ�%d, result��%d\n",
			loginRes->dataLength, loginRes->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		LogoutResult* logoutRes = (LogoutResult*)szRecv;

		printf("�յ���������Ϣ��CMD_LOGOUT_RESULT, ���ݳ��ȣ�%d, result��%d\n",
			logoutRes->dataLength, logoutRes->result);
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		NewUserJoin* userJoin = (NewUserJoin*)szRecv;

		printf("�յ���������Ϣ��CMD_NEW_USER_JOIN, ���ݳ��ȣ�%d\n",
			userJoin->dataLength);
	}
	break;
	}

	return 0;
}

bool g_bRun = true;		// �Ƿ��˳�����

// �������� �߳���ں���
void cmdThread(SOCKET sock)
{
	while (true)
	{
		// ������������
		char cmdBuf[128] = { 0 };
		printf("���������[exit | login | logout | other]\n");
		scanf("%s", &cmdBuf);

		// ��������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳�...\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			// 5.�������������������
			Login login;
			strcpy(login.userName, "ccf");
			strcpy(login.passWord, "ccfPwd");

			send(sock, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			// 5.�������������������
			Logout logout;
			strcpy(logout.userName, "ccf");
			send(sock, (const char*)&logout, sizeof(logout), 0);
		}
		else
		{
			printf("��֧�ֵ��������������.\n");
		}
	}
}

int main(int argc, char* argv[])
{
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("WSAStartup() ���󣬴����׽��ֿ�ʧ��!\n");
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("�����׽���ʧ��: %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(27015);

	//----------------------
	// Connect to server.
	if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("���ӷ�����ʧ��.\n");
		WSACleanup();
		return -1;
	}

	printf("�ͻ��˳ɹ����ӵ�������.\n");

	// ѭ����������
	// �����߳�
	std::thread thread_(cmdThread, ConnectSocket);
	thread_.detach();

	while (g_bRun)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);

		FD_SET(ConnectSocket, &fdReads);

		timeval timeout = { 0, 0 };
		int ret = select(ConnectSocket, &fdReads, NULL, NULL, &timeout);
		if (ret < 0)
		{
			printf("select�������1...\n");
			break;
		}

		if (FD_ISSET(ConnectSocket, &fdReads))
		{
			//FD_CLR(ConnectSocket, &fdReads);

			// �����ݿɶ������Դ�����
			if (-1 == processor(ConnectSocket))
			{
				printf("select�������2\n");
				break;
			}
		}

		//Login login;
		//strcpy(login.userName, "ccf");
		//strcpy(login.passWord, "ccfPwd");

		//send(ConnectSocket, (const char*)&login, sizeof(Login), 0);
		//Sleep(1000);	// ���͵�¼���ݺ���ʱ1s

		//printf("����ʱ�䴦������ҵ��...\n");
	}

	WSACleanup();

	printf("�ͻ������˳�...\n");

	getchar();

	return 0;
}