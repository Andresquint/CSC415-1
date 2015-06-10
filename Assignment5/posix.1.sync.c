//Hossein Niazmandi
//913064794
// Synchornization
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 65536
#define numofthreads 4

struct size{
    char *buffer;
    int indexstart, indexend, thrdNum;
    int *charCounts;
    pthread_mutex_t *mutex;
};

int readDataFromFile(const char *fileName,char *buff, int *size)
{
	//open the file read only mode
	FILE *fp = fopen(fileName,"rb");
	if(fp == NULL)
		return -1;
	int toBread = *size;
	int totalBytes = 0;
	int bytesRead = 0;
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
void divide_buff(struct size *t_size , int buff_length, char *buffer,int *charCounts,pthread_mutex_t *mutex){

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
	t_size[i].mutex = mutex;
	startLoc += len;

    }
}
//Simple Thread function
void* charCount(void *ptr)
{
	struct size *t_size = (struct size*)ptr;
	int count;
	for(count = t_size->indexstart; count < t_size->indexend;count++)
	{
		char c = t_size->buffer[count];
		pthread_mutex_lock(t_size->mutex);
		t_size->charCounts[(int)c]++;
		pthread_mutex_unlock(t_size->mutex);
	}
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	char buffer[BUFF_SIZE];
	int  buff_length = BUFF_SIZE;
        int  thread_id[numofthreads];
	struct size t_size[numofthreads];
	int charCounts[128] = {0};
	pthread_t p_thread[numofthreads];
	pthread_mutex_t mutex;
	/*validating the arguements*/
	if(argc < 2){
    	    printf("Incomplete Input");
        	return(1);
    	}
	struct timespec prev,now;
	clock_gettime(CLOCK_REALTIME, &prev);
	//read data from the file
    	if( -1 == readDataFromFile(argv[1],buffer,&buff_length))
	{
		perror("Cannot open input file\n");
	        return(2);
	}
	//allocating quota for each thread
	divide_buff(t_size,buff_length,buffer,charCounts,&mutex);
	int i;
	//spawn the threads
	for( i = 0; i < numofthreads; i++){
        	thread_id[i] = pthread_create(&p_thread[i], NULL, charCount, (void *) &t_size[i]);
    	}
	//This loop makes sure treads finish job
	for( i = 0; i < numofthreads; i++){
	        pthread_join(p_thread[i], NULL);
	}
	//Print the output of the program on the console
	int count;
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
	
	clock_gettime(CLOCK_REALTIME, &now);
	long long diff = (now.tv_sec * 1000000000 + now.tv_nsec) - (prev.tv_sec * 1000000000 + prev.tv_nsec);
	printf("it took %ld micro seconds to execute \n", diff/1000);
	return 0;
}
