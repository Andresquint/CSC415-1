//Hossein Niazmandi
//913064794
//Hw5 Race condition

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#define BUFF_SIZE 65536
#define numofthreads 4

struct size{
    char *buffer;
    int indexstart, indexend, thrdNum;
    int *charCounts;
};

int readDataFromFile(const char *fileName,char *buff, int *size)
{
	int toBread;
	int totalBytes = 0;
	int bytesRead = 0;

	//opening the file in read mode

	FILE *fp = fopen(fileName,"rb");
	if(fp == NULL)
		return -1;
	toBread = *size;
	//this loop will read the chars into the buffer
	while((bytesRead = fread(buff+totalBytes,sizeof(char), toBread,fp)))
	{
		totalBytes += bytesRead;
		toBread -= bytesRead;
	}
//Closes the input file
	fclose(fp);
	*size = totalBytes;
	return 0;
}
//this function will return the thread index for each thread used
void divide_buff(struct size *t_size , int buff_length, char *buffer,int *charCounts){

    int startLoc = 0;
    int len = buff_length / numofthreads;
    //this loop decides where to start each partition
    int i;
    for( i = 0; i < numofthreads; i++){
	t_size[i].buffer = buffer;
	t_size[i].indexstart =  startLoc;
	t_size[i].indexend = startLoc + len;
	t_size[i].charCounts = charCounts;
	t_size[i].thrdNum = i;
	startLoc += len;

    }
}
//Simple thread function
DWORD WINAPI charCount( LPVOID lpParam)
{
	struct size *t_size = (struct size*)lpParam;
	int count;
	for(count = t_size->indexstart; count < t_size->indexend;count++)
	{
		char c = t_size->buffer[count];
		t_size->charCounts[(int)c]++;
	}
	return 0;
}
int main(int argc, char *argv[])
{
	char buffer[BUFF_SIZE];
	int  buff_length = BUFF_SIZE;
	struct size t_size[numofthreads];
	int charCounts[128] = {0};
	 DWORD   dwThreadIdArray[numofthreads];
    HANDLE  hThreadArray[numofthreads]; 
		int i;
			int count;

	/*validating the arguements*/
	if(argc < 2){
    	    printf("Incomplete Input");
        	return(1);
    	}
	//read data from the file
    if( -1 == readDataFromFile(argv[1],buffer,&buff_length))
	{
		perror("Cannot open input file\n");
	        return(2);
	}
	
	//allocating quota for each thread
	divide_buff(t_size,buff_length,buffer,charCounts);

	//Spawn the threads
	for( i = 0; i < numofthreads; i++){
			  hThreadArray[i] = CreateThread( 
            NULL,                   // default security
            0,                      // default value  
            charCount,       // thread function name
            (void *) &t_size[i],          // argument passed to thread function 
            0,                      // default flag
            &dwThreadIdArray[i]);   // thrd ID 

    }
	//This loop makes sure threads finish job
	for( i = 0; i < numofthreads; i++){
			WaitForSingleObject(hThreadArray[i],INFINITE);
	        CloseHandle(hThreadArray[i]);
	}
	//Print the output of the program on the console
	for(count = 0; count < 128;count++)
	{
	
		if(count <= 32)
		{
			printf("%d occurances of '0x%x'\n",charCounts[count],(char)count);
		}else
		{
			printf("%d occurances of '%c'\n",charCounts[count],(char)count);
		}
	}
	
	return 0;
}
