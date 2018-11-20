#include <windows.h>
#include <stdio.h>

struct Point3D 
{
	int x, y, z;
};


DWORD WINAPI MyThread(LPVOID arg) 
{
	Point3D* pt = (Point3D*)arg;

	while(1)
	{ 
	printf("Running MyThread() %d : %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
		
	}
	
	return 0;
}

int main(int argc, char* argv[])
{

	// 첫번째 스레드 생성

	Point3D pt1 = { 10,20,30 };
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	

	// 두번째 스레드 생성




	Point3D pt2 = { 40,50,60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);

	
	while(1){
	printf("eeeeeeeeeeeee Running MainThread() %d \n", GetCurrentThreadId());
	}

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	

	return 0;

}

