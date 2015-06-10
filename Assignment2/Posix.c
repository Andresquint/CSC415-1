//Hossein Niazmandi

//913064794

#include <windows.h>

#define BUFFERSIZE 1024 //Defined Buffer of 256 as instructed in the hw

/* Create necessary variables for reading and writing data from two different files*/

int main(int argc, char ** argv)

{
    
    char buff1[BUFFERSIZE];
    
    char buff2[BUFFERSIZE];
    
    DWORD BytRead;
    
    DWORD BytWrite;
    
    HANDLE file1;
    
    HANDLE file2;
    
    /* This first block of code will read in the second arguement in the CL and using the
     
     the first buffer and flag OF_Read will allow the file to be read by permissions*/
    
    file1 = OpenFile(argv[1], buff1, OF_READ);
    
    if (file1 == HFILE_ERROR)
    
    {
        
        perror("The Source file Could not be opened");
        
        exit(-1);
        
    }
    
    //check if the second file exists
    
    file2 = OpenFile(argv[2], buff2, OF_CREATE);
    
    if (file2 == HFILE_ERROR)
    
    {
        
        perror("unable to open File");
        
        exit(-1);
        
    }
    
    else
    
    printf("The Desintation File was Created succesfully");
    
    
    
    //Copy bytes from the existing file to the created file
    
    /*while (ReadFile(file1, buff1, BUFFERSIZE, &BytRead, NULL) && BytRead > 0)
     
     {
     
     WriteFile(file2, buff1, BytRead, &BytWrite, NULL);
     
     }*/
    
    ReadFile(file1, buff1, BUFFERSIZE, &BytRead, NULL);
    
    WriteFile(file2, buff1, BytRead, &BytWrite, NULL);
    
    //Close files
    
    CloseHandle(file1);
    
    CloseHandle(file2);
    
    
    
    //This will print the number of bytes that were copied from the source
    
    // to the desintation folder
    
    printf("The number of Bytes Copied were %d", BytWrite);
    
    
    
    return 0;
    
}