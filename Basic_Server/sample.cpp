#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>

#include <iostream>

#include <stdlib.h>

#include <stdio.h>

#include <WS2tcpip.h> //InetNtop(), inet_ntop()

#pragma comment(lib, "ws2_32")



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



	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listen_sock == INVALID_SOCKET)

	{

		return 1;

	}



	// bind()



	SOCKADDR_IN Server_addresse;

	ZeroMemory(&Server_addresse, sizeof(Server_addresse));



	Server_addresse.sin_family = AF_INET;

	Server_addresse.sin_addr.s_addr = htonl(INADDR_ANY);

	Server_addresse.sin_port = htons(SERVERPORT);



	return_value = bind(listen_sock, (sockaddr*)&Server_addresse, sizeof(Server_addresse));



	// listen()



	return_value = listen(listen_sock, SOMAXCONN);





	// 데이터 통신에 사용할 변수



	SOCKET client_sock;

	SOCKADDR_IN client_addresse;

	int addlength;

	char buffer[BUFSIZE + 1];



	while (1)

	{



		//accept()



		addlength = sizeof(client_addresse);

		client_sock = accept(listen_sock, (SOCKADDR*)&client_addresse, &addlength);





		// 접속한 클라이언트 정보 출력



		printf("[TCP/%s:%d] %s, \n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port), buffer);





		//클라이언트와의 데이터 통신



		while (1)



		{



			//데이터 받기



			return_value = recv(client_sock, buffer, BUFSIZE, 0); //  : 받을 데이터(바이트)의 최대 크기를 가정. = 실제 보낸 것보다 훨씬 커도 된다.
			printf("\n 실제 받은 데이터 크기 %d", return_value);
																  // 반환값 int = 실제 받은 바이트 수. (최소 1. 최대 strlen() )







																  // 받은 데이터 출력하기



			buffer[return_value] = '\0';

			printf("\n yang[TCP / %s : %d] %s size=%d \n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port), buffer, sizeof(buffer));



			// 데이터 보내기ㅁ



			return_value = send(client_sock, buffer, return_value, 0);
			printf("\n 실제 보낸 데이터 크기 %d", return_value);



		}







		/// closersocket()



		closesocket(client_sock);

		printf("[TCP 서버] 클라이언트 종료 : IP주소 = %s, 포트번호 = %d\n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port));



	}



	closesocket(listen_sock);

	//// 윈도우 소켓종료()

	WSACleanup();

	return 0;

}