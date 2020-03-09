#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#include <thread>

#pragma comment(lib, "ws2_32.lib")

// 要考虑字节对齐问题（32位和64位，平台和系统）
enum CMDTYPE
{
	CMD_LOGIN,			// 登录
	CMD_LOGIN_RESULT,	// 登录返回结果
	CMD_LOGOUT,			// 登出
	CMD_LOGOUT_RESULT,	// 登出返回结果
	CMD_NEW_USER_JOIN,	// 新用户加入
	CMD_ERROR			// 错误
};

// 消息头
struct DataHeader
{
	int	cmd;			// 命令类型
	int dataLength;		// 消息体的数据长度
};

// 消息体
// DataPackage
// 登录
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

// 登录结果
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

// 登出
struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

// 登出结果
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

// 新用户加入
// 登出
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
	// 缓冲区(4096字节)
	char szRecv[4096] = {};
	// 5、接收客户端的请求
	// 先接收消息头
	int recvLen = recv(sock, szRecv, sizeof(DataHeader), 0);
	DataHeader* pHeader = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("与服务器断开连接，任务结束...\n");
		return -1;
	}

	// 6、处理请求
	switch (pHeader->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		LoginResult* loginRes = (LoginResult*)szRecv;

		printf("收到服务器消息：CMD_LOGIN_RESULT, 数据长度：%d, result：%d\n",
			loginRes->dataLength, loginRes->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		LogoutResult* logoutRes = (LogoutResult*)szRecv;

		printf("收到服务器消息：CMD_LOGOUT_RESULT, 数据长度：%d, result：%d\n",
			logoutRes->dataLength, logoutRes->result);
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		NewUserJoin* userJoin = (NewUserJoin*)szRecv;

		printf("收到服务器消息：CMD_NEW_USER_JOIN, 数据长度：%d\n",
			userJoin->dataLength);
	}
	break;
	}

	return 0;
}

bool g_bRun = true;		// 是否退出程序

// 命令输入 线程入口函数
void cmdThread(SOCKET sock)
{
	while (true)
	{
		// 输入请求命令
		char cmdBuf[128] = { 0 };
		printf("请输入命令：[exit | login | logout | other]\n");
		scanf("%s", &cmdBuf);

		// 处理请求
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程...\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			// 5.向服务器发送命令请求
			Login login;
			strcpy(login.userName, "ccf");
			strcpy(login.passWord, "ccfPwd");

			send(sock, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			// 5.向服务器发送命令请求
			Logout logout;
			strcpy(logout.userName, "ccf");
			send(sock, (const char*)&logout, sizeof(logout), 0);
		}
		else
		{
			printf("不支持的命令，请重新输入.\n");
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
		printf("WSAStartup() 错误，创建套接字库失败!\n");
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("创建套接字失败: %ld\n", WSAGetLastError());
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
		printf("连接服务器失败.\n");
		WSACleanup();
		return -1;
	}

	printf("客户端成功连接到服务器.\n");

	// 循环输入命令
	// 启动线程
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
			printf("select任务结束1...\n");
			break;
		}

		if (FD_ISSET(ConnectSocket, &fdReads))
		{
			//FD_CLR(ConnectSocket, &fdReads);

			// 有数据可读，可以处理了
			if (-1 == processor(ConnectSocket))
			{
				printf("select任务结束2\n");
				break;
			}
		}

		//Login login;
		//strcpy(login.userName, "ccf");
		//strcpy(login.passWord, "ccfPwd");

		//send(ConnectSocket, (const char*)&login, sizeof(Login), 0);
		//Sleep(1000);	// 发送登录数据后延时1s

		//printf("空闲时间处理其他业务...\n");
	}

	WSACleanup();

	printf("客户端已退出...\n");

	getchar();

	return 0;
}