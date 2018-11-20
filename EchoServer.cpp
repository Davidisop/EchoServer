#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>	// InetNtop(), inet_ntop()
#include <map>

#pragma comment(lib, "ws2_32")

bool BeginWinSock();
bool EndWinSock();

DWORD WINAPI ClientThread(LPVOID arg)
{
	SOCKET sock = (SOCKET)arg;
	char buf[256] = { 0, };
	while (true)
	{
		int iRecvByte = recv(sock, buf, sizeof(buf), 0);
		if (iRecvByte == 0)
		{
			break;
		}
		int iSendByte = send(sock, buf, iRecvByte, 0);
		if (iSendByte == 0 || iSendByte == SOCKET_ERROR)
		{
			break;
		}
		Sleep(1);
	}
	closesocket(sock);
	return 1;
}

struct User
{
	SOCKET sock;
	SOCKADDR_IN Addr;
};

int main(void)
{
	const u_short port = 10000;

	if (BeginWinSock() == false)
	{
		return -1;
	}

	SOCKET Listensock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int iRet = bind(Listensock, (sockaddr*)&addr, sizeof(addr));
	if (iRet != 0)
	{
		return -1;
	}

	iRet = listen(Listensock, SOMAXCONN);
	if (iRet != 0)
	{
		return -1;
	}

	//u_long on = TRUE;
	//ioctlsocket(Listensock, FIONBIO, &on);

	char buffer[] = { "qqqq" };
	char ip[INET_ADDRSTRLEN] = { 0, };

	std::map<int, User> g_userList;

	int g_iNumUser = 0;

	while (1)
	{
		SOCKET client;
		SOCKADDR_IN clientaddr;
		int AddrLen = sizeof(clientaddr);
		client = accept(Listensock, (sockaddr*)&clientaddr, &AddrLen);
		if (client != SOCKET_ERROR)
		{
			printf("%s, %d\n", inet_ntop(AF_INET, &clientaddr.sin_addr, ip, sizeof(ip)), ntohs(clientaddr.sin_port));
			DWORD threadID;
			HANDLE hThread = CreateThread(0, 0, ClientThread, (LPVOID)&client, 0, &threadID);
		}
	}

	closesocket(Listensock);
	if (EndWinSock() == false)
	{
		return -1;
	}
	return 0;
}


bool BeginWinSock()
{
	WSADATA wsaData;
	int iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != 0) return false;
	return true;
}


bool EndWinSock()
{
	int iRet = WSACleanup();
	if (iRet != 0) return false;
	return true;
}