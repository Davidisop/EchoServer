#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// ���� ���� ������ ����  ����ü

struct SOCKET_INFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recv_bytes;
	int send_bytes;
	WSABUF wsabuf;
	
};

// �۾��� ������ �Լ�

DWORD WINAPI WorkerThread(LPVOID arg);

// ���� ����Լ�
void err_quit(char *msg);
void err_display(char *msg);


