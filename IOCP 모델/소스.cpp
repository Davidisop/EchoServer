#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512


// 소켓 정보 저장을 위한 구조체
struct SOCKET_INFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recv_bytes;
	int send_bytes;
	WSABUF wsabuf;
};


// 작업자 스레드 
DWORD WINAPI WorkerThread_F(LPVOID arg);
{
	HANDLE hcp = (HANDLE)arg;
	return 1;
}



// 메인함수

int main(int argc, char* argv[])
{

	int return_value;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 입출력완료 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // 초반부터 생성. 이 때는 다 NULL을 집어넣는다.

	// 작업자 스레드 생성

	HANDLE Worker_Thread;
	Worker_Thread = CreateThread(NULL, 0, WorkerThread_F, hcp, 0, NULL); // 입출력 완료포트를 넘긴다.


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
		client_sock = accept(listen_sock, (SOCKADDR*)&client_addresse, &addlength); // ACCEPT로 clinet_sock을 반환받는다.


						// 접속한 클라이언트 정보 출력
		printf("클라이언트 접속[TCP/%s:%d]\n", inet_ntoa(client_addresse.sin_addr), ntohs(client_addresse.sin_port));



		// 반환된 클라이언트 소켓과 입출력 완료 포트를 완전 바인딩.
		
		::CreateIoCompletionPort(
			(HANDLE)client_sock, //실제 파일을 읽거나 쓰는 작업을 해서, 핸들을 받아야 한다.
			hcp,                 //입출력 완료포트핸들 
			client_sock,         // CompletionKey. 비동기 입출력 작업이 완료때마다  입출력완료포트에 저장되는.
			0); //               // 동시에 실행될 수 있는 작업자 쓰레드 개수. 0 하면 다 사용.
	
	}





