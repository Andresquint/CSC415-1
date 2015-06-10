//Hossein Niazmandi
//913064794
//Hw 4 415
// Posix
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 65536
#define NUMOFTHRD 4
// Intialize global variables
// create indexs
struct size{
    char *buffer;
    int startindex;
    int endindex;
    int thrdNum;
    int *charCounts;
};

int readDataFromFile(const char *fileName,char *buff, int *size)
{
    //opening the file in read mode
    FILE *fp = fopen(fileName,"rb");
    if(fp == NULL)
        return -1;
    int numOfBytesToRead = *size;
    int totBytesRead = 0;
    int bytesRead = 0;
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
void divide_buff(struct size *t_size , int buff_length, char *buffer,int charCounts[NUMOFTHRD][128]){
    
    int startLoc = 0;
    int len = buff_length / NUMOFTHRD;
    //this loop decides where to start each partition
    int i;
    for( i = 0; i < NUMOFTHRD; i++){
        t_size[i].buffer = buffer;
        t_size[i].startindex =  startLoc;
        t_size[i].endindex = startLoc + len;
        t_size[i].charCounts = &charCounts[i][0];
        t_size[i].thrdNum = i;
        startLoc += len;
        
    }
}
/*Thread function*/
void* charCount(void *ptr)
{
    struct size *t_size = (struct size*)ptr;
    int count;
    for(count = t_size->startindex; count < t_size->endindex;count++)
    {
        char c = t_size->buffer[count];
        t_size->charCounts[(int)c]++;
    }
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    char buffer[BUFF_SIZE];
    int  buff_length = BUFF_SIZE;
    int  thread_id[NUMOFTHRD];
    struct size t_size[NUMOFTHRD];
    int charCounts[NUMOFTHRD][128] = {{0,0}};
    pthread_t p_thread[NUMOFTHRD];
    /*validating the arguements*/
    if(argc < 2){
        printf("Input is not Complete");
        return(1);
    }
    struct timespec prev,now;
    clock_gettime(CLOCK_REALTIME, &prev);
    //read data from the file
    if( -1 == readDataFromFile(argv[1],buffer,&buff_length))
    {
        perror("Error in opening file, please verify file. \n");
        return(2);
    }
    memset(charCounts,0,sizeof(int)*NUMOFTHRD*128);
    //allocating quota for each thread
    divide_buff(t_size,buff_length,buffer,charCounts);
    int i;
    /*spawning the threads*/
    for( i = 0; i < NUMOFTHRD; i++){
        thread_id[i] = pthread_create(&p_thread[i], NULL, charCount, (void *) &t_size[i]);
    }
    /*wait the threads to finish it's job*/
    for( i = 0; i < NUMOFTHRD; i++){
        pthread_join(p_thread[i], NULL);
    }
    /*printing the char counts on the console*/
    int count;
    for(count = 0; count < 128;count++)
    {
        
        int totCount = 0;
        for(i = 0; i < NUMOFTHRD;i++)
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
    //will record runtime and return micro second value
    clock_gettime(CLOCK_REALTIME, &now);
    long long diff = (now.tv_sec * 1000000000 + now.tv_nsec) - (prev.tv_sec * 1000000000 + prev.tv_nsec);
    printf("it took %ld micro seconds to execute \n", diff/1000);
    return 0;
}
