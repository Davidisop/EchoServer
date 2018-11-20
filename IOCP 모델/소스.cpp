#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512


// ���� ���� ������ ���� ����ü
struct SOCKET_INFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recv_bytes;
	int send_bytes;
	WSABUF wsabuf;
};


// �۾��� ������ 
DWORD WINAPI WorkerThread_F(LPVOID arg);
{
	HANDLE hcp = (HANDLE)arg;
	return 1;
}



// �����Լ�

int main(int argc, char* argv[])
{

	int return_value;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ����¿Ϸ� ��Ʈ ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // �ʹݺ��� ����. �� ���� �� NULL�� ����ִ´�.

	// �۾��� ������ ����

	HANDLE Worker_Thread;
	Worker_Thread = CreateThread(NULL, 0, WorkerThread_F, hcp, 0, NULL); // ����� �Ϸ���Ʈ�� �ѱ��.


	 // ���� : ���� ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
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


	// ������ ��ſ� ����� ����

	SOCKET client_sock;
	SOCKADDR_IN client_addresse;
	int addlength;
	char buffer[BUFSIZE + 1];
	DWORD recv_bytes;
	DWORD flags;


	while (1)
	{	//accept()
		addlength = sizeof(client_addresse);
		client_sock = accept(listen_sock, (SOCKADDR*)&client_addresse, &addlength); // ACCEPT�� clinet_sock�� ��ȯ�޴´�.


						// ������ Ŭ���̾�Ʈ ���� ���
		printf("Ŭ���̾�Ʈ ����[TCP/%s:%d]\n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port));



		// ��ȯ�� Ŭ���̾�Ʈ ���ϰ� ����� �Ϸ� ��Ʈ�� ���� ���ε�.
		
		::CreateIoCompletionPort(
			(HANDLE)client_sock, //���� ������ �аų� ���� �۾��� �ؼ�, �ڵ��� �޾ƾ� �Ѵ�.
			hcp,                 //����� �Ϸ���Ʈ�ڵ� 
			client_sock,         // CompletionKey. �񵿱� ����� �۾��� �Ϸᶧ����  ����¿Ϸ���Ʈ�� ����Ǵ�.
			0); //               // ���ÿ� ����� �� �ִ� �۾��� ������ ����. 0 �ϸ� �� ���.
	
	}





