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

DWORD WINAPI WorkerThread(LPVOID arg) // �۾��ڴ� ����¿Ϸ���Ʈ�� ����� ������ ������, DISPATCH �����ش�.
{
	int return_value;
	HANDLE hcp = (HANDLE)arg; // ����¿Ϸ���Ʈ �ڵ�

	while (1)
	{
		// �񵿱� ����� �Ϸ� ��ٸ��� & ��� Ȯ��, �޾ƿ���.

		DWORD cbTransferred; // �񵿱� ����� �۾����� ���۵� ����Ʈ ��
		SOCKET client_sock;
		SOCKET_INFO *ptr; //���� ������ ���� ���� ����ü

		return_value = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPOVERLAPPED*)&client_sock, (LPDWORD*)&ptr, INFINITE);

		// Ŭ���̾�Ʈ ���� ���
		SOCKADDR_IN client_addr;
		int addr_length = sizeof(client_addr);
		getpeername(ptr->sock, (SOCKADDR*)&client_addr, &addr_length);


		// ������ ��쿡, �񵿱� ����� ��� Ȯ��

		if (return_value == 0 || cbTransferred == 0) //GetQueuedCompletionStatus() ��ȯ���� 0�̸�, �����ߴٴ� ���̿���. cbTransferred ����� �۾����� ���۵� ����Ʈ�� 0�̴�.
		{
			if (return_value == 0) // ��ȯ���� 0�̸�, �ᱹ �����ߴٴ� ���̿���. GetQueuedCompletionStatus()�� �����ߴ�.
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &temp1, FALSE, &temp2);	//  �̹� ���������� ��ٸ� �ʿ� ����.
				//err ó��
			}

			closesocket(ptr->sock); // ���� �������� �����ϴ� �� ����.
			printf("Ŭ���̾�Ʈ ����[TCP/%s:%d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			delete ptr;
			continue;

		}

		// ������ ���۷� ����

		if (ptr->recv_bytes == 0)
		{
			ptr->recv_bytes = cbTransferred;
			ptr->send_bytes = 0;

			// ���� ������ ���

			ptr->buf[ptr->recv_bytes] = '\0';
			printf("[TCP/%s:%d]�� ���� ���� %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), ptr->buf); // �� buf����? wsabuf�� �ƴϰ�? 
		}
		else
		{
			ptr->send_bytes += cbTransferred; // ���� �� �̸� �����ϰ�.
		}


		// ������ ������.

		if (ptr->recv_bytes > ptr->send_bytes)
		{
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->send_bytes; // �̹� ���� ����Ʈ ����Ͽ�
			ptr->wsabuf.len = ptr->recv_bytes - ptr->send_bytes; // ������ ���� ���� = ���� �� - ���ݱ��� ���� �� 

			DWORD send_bytes;
			return_value = WSASend(ptr->sock, &ptr->wsabuf, 1, &send_bytes, 0, &ptr->overlapped, NULL);

		}

		else if(ptr->recv_bytes == ptr->send_bytes) // �̰� ���� �����?
		{
			ptr->recv_bytes = 0;

			//������ �ޱ�
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf;
			ptr->wsabuf.len = BUFSIZE;

			DWORD recv_bytes;
			DWORD flags;

			return_value = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recv_bytes, &flags, &ptr->overlapped, NULL);
		}
	}
	return 0;
}


// ���� ����Լ�
void err_quit(char *msg);
void err_display(char *msg);



int main(int argc, char* argv[])
{

	int return_value;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ����¿Ϸ� ��Ʈ ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// �۾��� ������ ����

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
	

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
		client_sock = accept(listen_sock, (SOCKADDR*)&client_addresse, &addlength);
		

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("Ŭ���̾�Ʈ ����[TCP/%s:%d]\n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port));


		// ��ȯ�� ���ϰ� ����� �Ϸ� ��Ʈ�� ����.

		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		// ���� ���� ����ü �Ҵ�

		

		struct SOCKET_INFO
		{
			OVERLAPPED overlapped;
			SOCKET sock;
			char buf[BUFSIZE + 1];
			int recv_bytes;
			int send_bytes;
			WSABUF wsabuf;
		}; 

		SOCKET_INFO* ptr = new SOCKET_INFO;
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recv_bytes = ptr->send_bytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;


		// �񵿱� ����� ����

		flags = 0;
		return_value = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recv_bytes, &flags, &ptr->overlapped, NULL); //


	}
	WSACleanup();
	return 0;
}


