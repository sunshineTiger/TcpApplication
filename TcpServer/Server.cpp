#include<WinSock2.h>
#include<string.h>
#include<iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

using namespace std;
/*
表示链接ws2_32.lib这个库。和在工程设置里写上链入ws2_32.lib的效果一样（两种方式等价，或说一个隐式一个显式调用），
*/
#pragma comment(lib,"ws2_32.lib")

//服务器端口号
#define INT_SERVER_PORT 12345



int main() {

	SOCKET serverSocket = INVALID_SOCKET;
	WSADATA data;
	//初始化供进程调用的Winsock相关的dll
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		cout << "WSAStartup() error \n" << endl;
		//释放对Winsock链接库的调用
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}
	//初始化socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);//ipv4,流,tcp链接
	//初始化sockaddr_in
	sockaddr_in addrserver;
	memset(&addrserver, 0, sizeof(sockaddr_in));
	addrserver.sin_family = AF_INET;
	addrserver.sin_port = htons(INT_SERVER_PORT);
	addrserver.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//服务端将socket与地址关联
	if (bind(serverSocket, (LPSOCKADDR)&addrserver, sizeof(SOCKADDR_IN)) == INVALID_SOCKET) {
		cout << "bind() error \n" << endl;
		//释放对Winsock链接库的调用
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}
	//服务端网络监听
	if (listen(serverSocket, 5) == INVALID_SOCKET) {
		cout << "listen() error \n" << endl;
		//释放对Winsock链接库的调用
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}

	//循环接收数据  
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	//while (true)
	//{
	printf("等待连接...\n");
	sClient = accept(serverSocket, (SOCKADDR*)&remoteAddr, &nAddrlen);
	if (sClient == INVALID_SOCKET)
	{
		//printf("accept error !");
		//Sleep(10);
		//continue;
	}
	printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

	//接收数据  
	int ret = recv(sClient, revData, 255, 0);
	printf(revData);
	memset(revData, 0, 255);
	/*	if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}*/

		//发送数据  
	const char* sendData = "你好，TCP客户端！\n";
	send(sClient, sendData, strlen(sendData), 0);
	closesocket(sClient);
	//}

	closesocket(serverSocket);
	WSACleanup();
	getchar();
	return 0;

}

