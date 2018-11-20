#include <windows.h>
#include <stdio.h>

struct Point3D  // ﻿구조체를 정의.
{
	int x, y, z;
};


DWORD WINAPI MyThread(LPVOID arg)// LPVOID 는 void* 입니다.(캐스팅을 위한)
{
	Point3D* pt = (Point3D*)arg;
	while (1)
	{
		printf("Running MyThread() %d : %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);    // GetCurrentThreadId()는 API 함수로, 현재 사용되는 쓰레드의 ID를 반환합니다.
		Sleep(1000); //﻿쓰레드 교환 입니다.
	}
	return 0;
}

int main(int argc, char* argv[])
{
	// 첫번째 스레드 생성
	Point3D pt1 = { 10,20,30 };
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL) { return 1; }
	CloseHandle(hThread1);

	// 두번째 스레드 생성
	Point3D pt2 = { 40,50,60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL) { return 1; }
	CloseHandle(hThread2);

	while (1)
	{
	//	/*printf("Running main() %d \n", GetCurrentThreadId());
	//	Sleep(1000);*/
	}
	return 0;
}

