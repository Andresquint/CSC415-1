/*
	Hossein Niazmandi
 
*/
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define bufferSize 2048

int main(){
	// variables
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	while (TRUE){
		// create and clear buffer every loop
		char buffer[bufferSize] = { 0 };

		printf("Myshell> ");
		scanf("%[^\n]", buffer);
		getchar();

		// check if entry is "exit"
		if (strcmp(buffer, "exit") == 0){
			printf("Exit registered, exiting myShell");
			return 0;
		}

		// append /C into the beginning of string
		char C[bufferSize] = "/C ";
		strcat(C, buffer);

		// Start the child process. 
		if (!CreateProcess(
			"C:\\Windows\\System32\\cmd.exe",			// No module name (use command line)
			C,				// Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			NULL,           // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)			// Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d)\n", GetLastError());
		}

		// wait for process to finish
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return 0;
}