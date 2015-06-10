#include <stdio.h>
#include <windows.h>
#include <string.h>
#define NAME "Hossein Niazmandi"

int main(){

    char buffer[50];
//Handle us define output
    //DWORD store bytesWritten
    HANDLE outs = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD write;
    
    sprintf(buffer, "Hello 415 \n, I am %s", NAME);
    
    //WriteFile used for output of string
    WriteFile(outs, buffer, sizeof(buffer), &write, NULL);
    
    return 0;

}
