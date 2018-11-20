#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <WS2tcpip.h> //InetNtop(), inet_ntop()
#pragma comment(lib, "ws2_32")

#define SERVERIP "192.168.0.111"
#define SERVERPORT 9000
#define BUFSIZE 512

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
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		return 1;
	}



	// 서버의 주소 입력
	SOCKADDR_IN Server_addresse;
	ZeroMemory(&Server_addresse, sizeof(Server_addresse));
	Server_addresse.sin_family = AF_INET;
	Server_addresse.sin_addr.s_addr = inet_addr(SERVERIP);
	Server_addresse.sin_port = htons(SERVERPORT);


	// 데이터 통신에 사용할 변수
	char buffer[BUFSIZE + 1];
	int addr_length;
	int length;

	SOCKADDR_IN peer_addresse;




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
		if (buffer[length - 1] == '\n') { buffer[length - 1] = '\0'; } // Enter를 \0 으로 바꿔치기 했다.

		if (strlen(buffer) == 0) break;



		////////////// 데이터 보내기.



		return_value = sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&Server_addresse, sizeof(Server_addresse));
		printf("[UDP 클라이언트] %d 바이트를 보냈습니다.\n", return_value);





		//// 데이터 받기

		addr_length = sizeof(peer_addresse);
		return_value = recvfrom(sock, buffer, BUFSIZE, 0, (SOCKADDR*)&peer_addresse, &addr_length);



		// 서버로 부터 받은 게 맞는지 IP 체크.
		if (memcmp(&peer_addresse, &Server_addresse, sizeof(peer_addresse)))
		{
			printf("[오류 잘못된 데이터 입니다.");
			continue;
		}





		// 받은 데이터 출력
		buffer[return_value] = '\0';  // 이걸 해줘야, 마지막이 문자열로 인식됩니다.
		
		printf("[UDP 클라이언트] %d 바이트를 받앗습니다. \n", return_value);
		printf("[UDP 서버로부터 /%s:%d] %s \n", inet_ntoa(peer_addresse.sin_addr), ntohs(peer_addresse.sin_port), buffer);
		//printf("[받은 데이터] %s \n", buffer);

	}


	closesocket(sock);
	WSACleanup();

	return 0;

}

