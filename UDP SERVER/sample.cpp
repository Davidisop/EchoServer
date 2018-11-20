#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[])
{


	int return_value;

	// 윈도우 소켓초기화
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
	{
		return 1;
	}



	// socket() 
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);    // 이것이 다르다.SOCK_DGRAM | 0
	if (sock == INVALID_SOCKET)
	{
	return 1;
	}


	// bind()
	
	SOCKADDR_IN Server_addresse;
	ZeroMemory(&Server_addresse, sizeof(Server_addresse));
	Server_addresse.sin_family = AF_INET;
	Server_addresse.sin_addr.s_addr = htonl(INADDR_ANY);
	Server_addresse.sin_port = htons(SERVERPORT);

	return_value = bind(sock, (sockaddr*)&Server_addresse, sizeof(Server_addresse)); // 지역주소 결정해준다.



	SOCKADDR_IN client_addresse;  // 이게 중요하다. 통신에는 반드시 Server_addresse와 client_addresse가 동시에 있어야 한다.
	int addlength;
	char buffer[BUFSIZE + 1];


	while (1)
	{

		// 데이터 받기.

		addlength = sizeof(client_addresse);
		return_value = recvfrom(sock, buffer, BUFSIZE, 0, (SOCKADDR*)&client_addresse, &addlength); // TCP의 recv()는 클라이언트 주소가 없다! 여기는 있다! 저절로 저장되나 보다!

		// 받은 데이터 출력

		buffer[return_value] = '\0';
		printf("[UDP 클라이언트로부터/%s:%d] %s \n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port), buffer); // 그러니까 여기 저장 가능하지!

		// 데이터 보내기.
		return_value = sendto(sock, buffer, return_value, 0, (SOCKADDR*)&client_addresse, sizeof(client_addresse)); //  TCP의 send()는 클라이언트 주소가 없다!
		

	}


	closesocket(sock);

	WSACleanup();
	return 0;
}