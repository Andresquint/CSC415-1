//Hossein Niazmandi
//913064794
//Hw5 Sync condition

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
	HANDLE mutex;
};

int readDataFromFile(const char *fileName,char *buff, int *size)
{
	int numbyteread;
	int totalBytes = 0;
	int bytesRead = 0;

	//opening the file in read mode

	FILE *fp = fopen(fileName,"rb");
	if(fp == NULL)
		return -1;
	numbyteread = *size;
	//this loop will read the chars into the buffer
	while((bytesRead = fread(buff+totalBytes,sizeof(char), numbyteread,fp)))
	{
		totalBytes += bytesRead;
		numbyteread -= bytesRead;
	}
	//Closes the input file
	fclose(fp);
	*size = totalBytes;
	return 0;
}
//this function will return the thread index for each thread used
void divide_buff(struct size *t_size , int lengthofbuffer, char *buffer,int *charCounts,HANDLE mutex){

    int startLoc = 0;
    int len = lengthofbuffer / numofthreads;
    //this loop decides where to start each partition
    int i;
    for( i = 0; i < numofthreads; i++){
	t_size[i].buffer = buffer;
	t_size[i].indexstart =  startLoc;
	t_size[i].indexend = startLoc + len;
	t_size[i].charCounts = charCounts;
	t_size[i].thrdNum = i;
	t_size[i].mutex = mutex;
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
		WaitForSingleObject(t_size->mutex,INFINITE);
		t_size->charCounts[(int)c]++;
		ReleaseMutex(t_size->mutex);
	}
	return 0;
}
int main(int argc, char *argv[])
{
	char buffer[BUFF_SIZE];
	int  lengthofbuffer = BUFF_SIZE;
	struct size t_size[numofthreads];
	int charCounts[128] = {0};
	 DWORD   dwThreadIdArray[numofthreads];
    HANDLE  hThreadArray[numofthreads]; 
	HANDLE mutex;
		int i;
			int count;

	/*validating the arguements*/
	if(argc < 2){
    	    printf("Incomplete Input");
        	return(1);
    	}
	//read data from the file
    if( -1 == readDataFromFile(argv[1],buffer,&lengthofbuffer))
	{
		perror("Cannot open input file\n");
	        return(2);
	}
	mutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex
	
	divide_buff(t_size,lengthofbuffer,buffer,charCounts,mutex);

	//Spawn the threads
	for( i = 0; i < numofthreads; i++){
			  hThreadArray[i] = CreateThread( 
            NULL,                   // default attrib
            0,                      // use default stack size  
            charCount,       // thread function name
            (void *) &t_size[i],          // arguement passed to thread function 
            0,                      // defualt flags
            &dwThreadIdArray[i]);   // thrd ID 

    }
		//This loop makes sure threads finish job
	for( i = 0; i < numofthreads; i++){
			WaitForSingleObject(hThreadArray[i],INFINITE);
	        CloseHandle(hThreadArray[i]);
	}
	CloseHandle(mutex);
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
