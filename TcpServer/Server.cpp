#include<WinSock2.h>
#include<string.h>
#include<iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

using namespace std;
/*
��ʾ����ws2_32.lib����⡣���ڹ���������д������ws2_32.lib��Ч��һ�������ַ�ʽ�ȼۣ���˵һ����ʽһ����ʽ���ã���
*/
#pragma comment(lib,"ws2_32.lib")

//�������˿ں�
#define INT_SERVER_PORT 12345



int main() {

	SOCKET serverSocket = INVALID_SOCKET;
	WSADATA data;
	//��ʼ�������̵��õ�Winsock��ص�dll
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		cout << "WSAStartup() error \n" << endl;
		//�ͷŶ�Winsock���ӿ�ĵ���
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}
	//��ʼ��socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);//ipv4,��,tcp����
	//��ʼ��sockaddr_in
	sockaddr_in addrserver;
	memset(&addrserver, 0, sizeof(sockaddr_in));
	addrserver.sin_family = AF_INET;
	addrserver.sin_port = htons(INT_SERVER_PORT);
	addrserver.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//����˽�socket���ַ����
	if (bind(serverSocket, (LPSOCKADDR)&addrserver, sizeof(SOCKADDR_IN)) == INVALID_SOCKET) {
		cout << "bind() error \n" << endl;
		//�ͷŶ�Winsock���ӿ�ĵ���
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}
	//������������
	if (listen(serverSocket, 5) == INVALID_SOCKET) {
		cout << "listen() error \n" << endl;
		//�ͷŶ�Winsock���ӿ�ĵ���
		if (WSACleanup())
			cout << "WSACleanup() error \n" << endl;
		else
			cout << "WSACleanup() error \n" << endl;
		system("pause()");
		return 1;
	}

	//ѭ����������  
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	//while (true)
	//{
	printf("�ȴ�����...\n");
	sClient = accept(serverSocket, (SOCKADDR*)&remoteAddr, &nAddrlen);
	if (sClient == INVALID_SOCKET)
	{
		//printf("accept error !");
		//Sleep(10);
		//continue;
	}
	printf("���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));

	//��������  
	int ret = recv(sClient, revData, 255, 0);
	printf(revData);
	memset(revData, 0, 255);
	/*	if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}*/

		//��������  
	const char* sendData = "��ã�TCP�ͻ��ˣ�\n";
	send(sClient, sendData, strlen(sendData), 0);
	closesocket(sClient);
	//}

	closesocket(serverSocket);
	WSACleanup();
	getchar();
	return 0;

}

