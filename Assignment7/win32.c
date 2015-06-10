//Hossein Niazmandi
//CSC 415
//Hw7
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MBCS
#define _MBCS
#endif

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define bufferSize 1024

int main(){
    
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
    HANDLE hStdin;
    HANDLE hStdout;
    HANDLE hStderr;

	while (TRUE){
		char buffer[bufferSize] = { 0 };

		printf("Myshell> ");
		scanf("%[^\n]", buffer);
		getchar();
		if (strcmp(buffer, "quit") == 0){
			printf("Quiting the Shell, Thanks");
			return 0;
		}

		char C[bufferSize] = "/C ";
		strcat(C, buffer);

		if (!CreateProcess(
			"C:\\Windows\\System32\\cmd.exe", C, NULL, NULL, FALSE, 0,NULL, NULL, &si, &pi)
			)
		{
			printf("Failed to create the child process(%d)\n", GetLastError());
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return 0;
}