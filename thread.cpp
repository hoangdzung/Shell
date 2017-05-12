#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
void T1(){
	while(1){ 
		printf("1 ");
		Sleep(100);
	}
}
void T2(){
while(1){
	printf("2 ");
	Sleep(100);
	}
}

int main(){
HANDLE h1, h2; DWORD Id;
h1=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)T1,NULL,0,&Id);
h2=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)T2,NULL,0,&Id);

WaitForSingleObject(h1,INFINITE);
WaitForSingleObject(h2,INFINITE);

return 0;
}
