#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <WS2tcpip.h> //InetNtop(), inet_ntop()
#pragma comment(lib, "ws2_32")

#define SERVERPORT 5000
#define BUFSIZE 512



// 쓰레드 함수 1
DWORD WINAPI ProcessClient(LPVOID arg)
{
	// 소켓을 쓰레드 함수에 인자로 넘겨서,  쓰레드에서 소켓을 처리하게 함.
	SOCKET client_socket = (SOCKET)arg; // 소켓 복구
	int return_value;
	SOCKADDR_IN client_addr;
	int addr_length;
	char buf[MAX_BUFFER_SIZE + 1];
	// 클라이언트 정보 얻기.

	addr_length = sizeof(client_addr);
	getpeername(client_socket, (SOCKADDR*)&client_addr, &addr_length);
	//
	while (1)
	{
		// 데이터 받기
		return_value = recv(client_socket, buf, MAX_BUFFER_SIZE, 0);

		// 받은 데이터 출력
		buf[return_value] = '\0'; // 이래야, 문장으로 출력이 됨.
		printf("\n [TCP 클라이언트 IP주소 = %s, 포트번호 = %d] : %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);

		// 데이터 클라이언트에게 보내기
		return_value = send(client_socket, buf, return_value, 0);
		Sleep(1);
	}

	// close 소켓

	closesocket(client_socket);
	printf("\n [TCP 서버] 클라이언트 접속종료. 주소 = %S, 포트번호 = %d\n ", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	return 0;
}


int main()
{
	int return_value;
	// 윈도우 소켓초기화
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
	{
	return 1;
	}
	// socket() 
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return 1;
	}
	// connect()
	SOCKADDR_IN Server_addresse;
	ZeroMemory(&Server_addresse, sizeof(Server_addresse));
	Server_addresse.sin_family = AF_INET;
	Server_addresse.sin_addr.s_addr = inet_addr("192.168.0.111");
	Server_addresse.sin_port = htons(SERVERPORT);
	return_value = connect(sock, (sockaddr*)&Server_addresse, sizeof(Server_addresse));

	// 데이터 통신에 사용할 변수
	char buffer[BUFSIZE + 1];
	int length;


	// 서버와의 데이터 통신
	while (1)
	{
		//데이터 입력
		printf("\n [보낼 데이터] : ");
		if (fgets(buffer, BUFSIZE + 1, stdin) == NULL) break;
		/*gets(str, sizeof(str), stdin);

		stdin으로 부터 문자열을 입력받아 배열 str에 저장하되, sizeof(strO0만큼만 저장해라!

		﻿가운데 인자가 7이라고 하면, 글자는 6개가 저장된다.문자열의 끝에는 반드시 'NULL'이 포함되기 때문이다.*/


		// 문자열 끝에 Enter 때문에 자동으로 삽입되는 '\n' 을 제거하자

		length = strlen(buffer);
		if (buffer[length - 1] == '\n') { buffer[length - 1] = '\0'; }
		if (strlen(buffer) == 0) break;

		////////////// 데이터 보내기.

		return_value = send(sock, buffer, strlen(buffer), 0);
		printf("[TCP 클라이언트] %d 바이트를 보냈습니다.\n", return_value);
		//// 데이터 받기
		return_value = recv(sock, buffer, return_value, 0);
		// 받은 데이터 출력
		buffer[return_value] = '\0';
		printf("[TCP 클라이언트] %d 바이트를 받앗습니다. \n", return_value);
		printf("[받은 데이터] %s \n", buffer);

		//// 데이터 받기
		return_value = recv(sock, buffer, BUFSIZE, 0);
		// 받은 데이터 출력
		buffer[BUFSIZE] = '\0';
		printf("[TCP 클라이언트] %d 바이트를 받앗습니다. \n", return_value);
		printf("[받은 데이터] %s \n", buffer);
	}
	/// closersocket()



	closesocket;

	//// 윈도우 소켓종료()
	WSACleanup();
	return 0;
}