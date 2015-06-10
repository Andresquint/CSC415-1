//Hossein Niazmandi
//913064794
//415 Hw 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#define BUFF_SIZE 65536
#define NUM_THREADS 1

struct size{
    char *buffer;
    int beginning_of_index, end_of_index, threadNumber;
    int *charCounts;
};

int readDataFromFile(const char *fileName,char *buff, int *size)
{
	int numOfBytesToRead;
	int totBytesRead = 0;
	int bytesRead = 0;

	//opening the file in read mode

	FILE *fp = fopen(fileName,"rb");
	if(fp == NULL)
		return -1;
	numOfBytesToRead = *size;
	/*The following loop reads the chars from the file and put in to the buffer*/
	while((bytesRead = fread(buff+totBytesRead,sizeof(char), numOfBytesToRead,fp)))
	{
		totBytesRead += bytesRead;
		numOfBytesToRead -= bytesRead;
	}
	/*releasing the file resource*/
	fclose(fp);
	*size = totBytesRead;
	return 0;
}
//this function will return the thread index for each thread used
void divide_buff(struct size *t_size , int buff_length, char *buffer,int charCounts[NUM_THREADS][128]){

    int startLoc = 0;
    int len = buff_length / NUM_THREADS;
    //this loop decides where to start each partition
    int i;
    for( i = 0; i < NUM_THREADS; i++){
	t_size[i].buffer = buffer;
	t_size[i].beginning_of_index =  startLoc;
	t_size[i].end_of_index = startLoc + len;
	t_size[i].charCounts = &charCounts[i][0];
	t_size[i].threadNumber = i;
	startLoc += len;

    }
}
/*Thread function*/
DWORD WINAPI charCount( LPVOID lpParam)
{
	struct size *t_size = (struct size*)lpParam;
	int count;
	for(count = t_size->beginning_of_index; count < t_size->end_of_index;count++)
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
	struct size t_size[NUM_THREADS];
	int charCounts[NUM_THREADS][128] = {{0,0}};
	 DWORD   dwThreadIdArray[NUM_THREADS];
    HANDLE  hThreadArray[NUM_THREADS]; 
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
	memset(charCounts,0,sizeof(int)*NUM_THREADS*128);
	//allocating quota for each thread
	divide_buff(t_size,buff_length,buffer,charCounts);

	/*spawning the threads*/
	for( i = 0; i < NUM_THREADS; i++){
			  hThreadArray[i] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            charCount,       // thread function name
            (void *) &t_size[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 

    }
	/*wait the threads to finish it's job*/
	for( i = 0; i < NUM_THREADS; i++){
			WaitForSingleObject(hThreadArray[i],INFINITE);
	        CloseHandle(hThreadArray[i]);
	}
	/*printing the char counts on the console*/
	for(count = 0; count < 128;count++)
	{
	
		int totCount = 0;
    		for(i = 0; i < NUM_THREADS;i++)
		{
			totCount +=charCounts[i][count];
		}
		if(count <= 32)
		{
			printf("%d occurances of '0x%x'\n",totCount,(char)count);
		}else
		{
			printf("%d occurances of '%c'\n",totCount,(char)count);
		}
	}
	system("pause");
	
	return 0;
	
}
