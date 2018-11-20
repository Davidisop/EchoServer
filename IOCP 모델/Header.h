#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// 소켓 정보 저장을 위한  구조체

struct SOCKET_INFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recv_bytes;
	int send_bytes;
	WSABUF wsabuf;
	
};

// 작업자 스레드 함수

DWORD WINAPI WorkerThread(LPVOID arg);

// 오류 출력함수
void err_quit(char *msg);
void err_display(char *msg);


