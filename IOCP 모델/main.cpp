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

DWORD WINAPI WorkerThread(LPVOID arg) // 작업자는 입출력완료포트에 저장된 정보를 가지고, DISPATCH 시켜준다.
{
	int return_value;
	HANDLE hcp = (HANDLE)arg; // 입출력완료포트 핸들

	while (1)
	{
		// 비동기 입출력 완료 기다리기 & 결과 확인, 받아오기.

		DWORD cbTransferred; // 비동기 입출력 작업으로 전송된 바이트 수
		SOCKET client_sock;
		SOCKET_INFO *ptr; //내가 정의한 소켓 정보 구조체

		return_value = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPOVERLAPPED*)&client_sock, (LPDWORD*)&ptr, INFINITE);

		// 클라이언트 정보 얻기
		SOCKADDR_IN client_addr;
		int addr_length = sizeof(client_addr);
		getpeername(ptr->sock, (SOCKADDR*)&client_addr, &addr_length);


		// 끝났을 경우에, 비동기 입출력 결과 확인

		if (return_value == 0 || cbTransferred == 0) //GetQueuedCompletionStatus() 반환값이 0이면, 실패했다는 뜻이에요. cbTransferred 입출력 작업으로 전송된 바이트가 0이다.
		{
			if (return_value == 0) // 반환값이 0이면, 결국 실패했다는 뜻이에요. GetQueuedCompletionStatus()가 실패했다.
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &temp1, FALSE, &temp2);	//  이미 실패했으니 기다릴 필요 없다.
				//err 처리
			}

			closesocket(ptr->sock); // 소켓 끊어졌다 생각하는 거 같다.
			printf("클라이언트 종료[TCP/%s:%d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			delete ptr;
			continue;

		}

		// 데이터 전송량 갱신

		if (ptr->recv_bytes == 0)
		{
			ptr->recv_bytes = cbTransferred;
			ptr->send_bytes = 0;

			// 받은 데이터 출력

			ptr->buf[ptr->recv_bytes] = '\0';
			printf("[TCP/%s:%d]가 보낸 정보 %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), ptr->buf); // 왜 buf이지? wsabuf가 아니고? 
		}
		else
		{
			ptr->send_bytes += cbTransferred; // 보낼 거 미리 생각하고.
		}


		// 데이터 보내기.

		if (ptr->recv_bytes > ptr->send_bytes)
		{
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->send_bytes; // 이미 보낸 바이트 고려하여
			ptr->wsabuf.len = ptr->recv_bytes - ptr->send_bytes; // 보낼는 남은 버퍼 = 받은 거 - 지금까지 보낸 거 

			DWORD send_bytes;
			return_value = WSASend(ptr->sock, &ptr->wsabuf, 1, &send_bytes, 0, &ptr->overlapped, NULL);

		}

		else if(ptr->recv_bytes == ptr->send_bytes) // 이건 무슨 경우지?
		{
			ptr->recv_bytes = 0;

			//데이터 받기
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


// 오류 출력함수
void err_quit(char *msg);
void err_display(char *msg);



int main(int argc, char* argv[])
{

	int return_value;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 입출력완료 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// 작업자 스레드 생성

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
	

	// 소켓 : 리슨 소켓 생성

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



	// 데이터 통신에 사용할 변수

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
		

		// 접속한 클라이언트 정보 출력
		printf("클라이언트 접속[TCP/%s:%d]\n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port));


		// 반환된 소켓과 입출력 완료 포트를 연결.

		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		// 소켓 정보 구조체 할당

		

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


		// 비동기 입출력 시작

		flags = 0;
		return_value = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recv_bytes, &flags, &ptr->overlapped, NULL); //


	}
	WSACleanup();
	return 0;
}


