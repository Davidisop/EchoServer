#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

int main(int argc, char* argv[])
{


	int return_value;

	// ������ �����ʱ�ȭ
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
	{
		return 1;
	}



	// socket() 
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);    // �̰��� �ٸ���.SOCK_DGRAM | 0
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

	return_value = bind(sock, (sockaddr*)&Server_addresse, sizeof(Server_addresse)); // �����ּ� �������ش�.



	SOCKADDR_IN client_addresse;  // �̰� �߿��ϴ�. ��ſ��� �ݵ�� Server_addresse�� client_addresse�� ���ÿ� �־�� �Ѵ�.
	int addlength;
	char buffer[BUFSIZE + 1];


	while (1)
	{

		// ������ �ޱ�.

		addlength = sizeof(client_addresse);
		return_value = recvfrom(sock, buffer, BUFSIZE, 0, (SOCKADDR*)&client_addresse, &addlength); // TCP�� recv()�� Ŭ���̾�Ʈ �ּҰ� ����! ����� �ִ�! ������ ����ǳ� ����!

		// ���� ������ ���

		buffer[return_value] = '\0';
		printf("[UDP Ŭ���̾�Ʈ�κ���/%s:%d] %s \n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port), buffer); // �׷��ϱ� ���� ���� ��������!

		// ������ ������.
		return_value = sendto(sock, buffer, return_value, 0, (SOCKADDR*)&client_addresse, sizeof(client_addresse)); //  TCP�� send()�� Ŭ���̾�Ʈ �ּҰ� ����!
		

	}


	closesocket(sock);

	WSACleanup();
	return 0;
}