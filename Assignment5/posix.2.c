//Hossein Niazmandi
//913064794
//Hw5 p2

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>

// global variables
sem_t semFUll;
sem_t semEmpty;
pthread_mutex_t semaphoreMutex;
int *buffer;
unsigned int bufLoc = 0;

// data structure to hold arguements
struct arguments {
    int thrdNum;
    int numofThrds;
    int itemsproduced;
};

// funtion to create the item

unsigned int createItem(const int counter, const int numitemsproduced, const int thread_number) {
    return (counter + thread_number*numitemsproduced);
}

// InsertItem will push Item into buffer

void insertItem(unsigned int newItem) {
 	buffer[bufLoc++] = newItem;
}

// getItem will retrieve items from buffer

unsigned int getItem() {
	bufLoc--;
	return buffer[bufLoc];
}

// this checks to see if the buffer is empty

int isBufferEmpty() {
    return bufLoc == 0 ? 1 : 0;


}

// Here is where the producer thread starts

void * producer(void * lpParameter) {
    int threadID = (int) pthread_self();
    struct arguments * args = (struct arguments *)lpParameter;
    int counter = 0;

    int thrdNum = args->thrdNum;
    int itemsproduced = args->itemsproduced;

    while (counter < itemsproduced) {
        sem_wait(&semEmpty);
	//protecting the buffer
        pthread_mutex_lock(&semaphoreMutex);
        int newItem = createItem(counter,itemsproduced,thrdNum);
        counter++;
        insertItem(newItem);
        pthread_mutex_unlock(&semaphoreMutex);
	//protecting the buffer is done
        sem_post(&semFUll);

    }
    
     printf("Producer thread <%u> exits \n", threadID);
     pthread_exit(NULL);
     return NULL;
    //pthread_exit(NULL);
}

// consumer function

void * consumer(void * lpParameter) {
    int howManyToConsume = *((int*) lpParameter);
    int threadID = (int) pthread_self();
    printf("Consumer thread <%i> created.\n", threadID);
    int count = 0;
    while (count < howManyToConsume) {
        sem_wait(&semFUll);
        pthread_mutex_lock(&semaphoreMutex);
        int item = getItem();
        printf("Item %i Consumed by thread <%i> \n", item, threadID);
	count++;
        pthread_mutex_unlock(&semaphoreMutex);
        sem_post(&semEmpty);

    }
    
     printf("Consumer thread <%u> exits \n", threadID);
     pthread_exit(NULL);
     return NULL;
    //pthread_exit(NULL);
}

int main(int argc, char ** argv) {
    // variable declaration
    if(argc < 4)
    {
		fprintf(stderr,"Please Enter <number of producers> <num of consumers> <number of itemsproduced items per thread>"); 
		return -1;
    }
    int bufferSlots = 16;
    int producersNum = atoi(argv[1]);
    int consumersNum = atoi(argv[2]);
    int itemsproduced = atoi(argv[3]);
	
    producersNum = pow(2,producersNum);
    consumersNum = pow(2,consumersNum);
    itemsproduced = pow(2,itemsproduced);


    int i;
    struct arguments *prodArgs;
    pthread_t *consThreads;
    pthread_t *prodThreads;
    pthread_attr_t attr;

    // create thread attribute
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // print results 
    printf("Buffer slots: %d\n", bufferSlots);
    printf("Number of Consumer: %d\n", consumersNum);
    printf("Number of Producer: %d\n", producersNum);
    printf("Number of itemsproduced per producer : %d\n", itemsproduced);

    // memory allocation
    prodArgs = (struct arguments*) malloc(producersNum*sizeof(struct arguments));
    buffer = (int*) malloc(bufferSlots*sizeof(int));
    prodThreads = (pthread_t*) malloc(producersNum*sizeof(pthread_t));
    consThreads = (pthread_t*) malloc(consumersNum*sizeof(pthread_t));

    // initialize the buffer
    for (i = 0; i < bufferSlots; i++) {
        buffer[i] = -1;
    }

    // semaphores created
    pthread_mutex_init(&semaphoreMutex, NULL);
    sem_init(&semFUll, 0, 0);
    sem_init(&semEmpty, 0, bufferSlots);

    int howManyItemsToConsume = (producersNum*itemsproduced)/consumersNum;
    for(i = 0; i < consumersNum; i++)
    {
	        pthread_create(&consThreads[i], &attr, &consumer, &howManyItemsToConsume);
    }	
    // loop that creates producers/consumers
    for (i = 0; i < producersNum; i++) {
        prodArgs[i].numofThrds = producersNum;
        prodArgs[i].itemsproduced = itemsproduced;
        prodArgs[i].thrdNum = i;
        pthread_create(&prodThreads[i], &attr, &producer, &prodArgs[i]);
    }

    // loop that checks to make sure threads complete
    for (i = 0; i < producersNum; i++) {
        pthread_join(prodThreads[i], NULL);
    }
    for(i = 0; i < consumersNum; i++)
    {
	    pthread_join(consThreads[i], NULL);
    }
    // close handles for semaphores
    sem_destroy(&semFUll);
    sem_destroy(&semEmpty);
    pthread_mutex_destroy(&semaphoreMutex);
    free(prodThreads);
    free(consThreads);
    free(buffer);
    printf("All threads completed\n");
    printf("Press <RETURN> to continue...\n");
    getchar();

    return 0;
}
