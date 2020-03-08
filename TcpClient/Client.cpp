#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<WinSock2.h>
#include<string.h>
#include<iostream>



using namespace std;
#pragma comment(lib,"ws2_32.lib")



int main()
{

	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		cout << "WSAStartup error \n" << endl;
		system("pause");
		return 1;
	}

	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(12345);
	addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.154.1");
	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketClient == INVALID_SOCKET)
	{
		std::cout << "socket() create error";
		system("pause");
		return 1;
	}
	SOCKET connectResult = connect(socketClient, (sockaddr*)&addrServer, sizeof(addrServer));
	if (connectResult == SOCKET_ERROR)
	{
		std::cout << "connect() error\n";
		system("pause");
		return 1;
	}

	std::cout << "connect success,wait data......" << std::endl;;

	//while (1)
	//{
	char sendData[] = "hello i am client\n";
	send(socketClient, sendData, sizeof(sendData), 0);

	char recvBuff[1024];
	memset(recvBuff, 0, sizeof(recvBuff));

	if (recv(socketClient, recvBuff, sizeof(recvBuff), 0) > 0)
		std::cout << "recv from server:" << recvBuff << std::endl;


	//}

	closesocket(socketClient);

	WSACleanup();
	getchar();

	return 0;


}