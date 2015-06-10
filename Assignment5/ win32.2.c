//Hossein Niamzandi
//913064794
//Win32 p2
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <stdio.h>

// global variables
HANDLE	semaphoreFull;
HANDLE	semaphoreEmpty;
HANDLE	semaphoreMutex;
int		*buffer;
int		finishedProducers = 0;
int		totalProducers;

// struct to hold arguments
typedef struct arguments{
	WORD threadNum;
	WORD numberOfThreads;
	WORD produced;
};

// funtion to create the item
int createItem(const WORD counter, const WORD num_threads, const WORD thread_number){
	return (counter * num_threads + thread_number);
}

// function to insert item in buffer
void insertItem(int newItem){
	if (buffer[3] == -1){
		buffer[3] = buffer[2];
		buffer[2] = buffer[1];
		buffer[1] = buffer[0];
		buffer[0] = newItem;
	}
	else
		printf("BUFFER FULL CANNOT WRITE\n");
}

// function to get item from buffer
int getItem(){
	if (buffer[3] != -1){
		int temp = buffer[3];
		buffer[3] = -1;
		return temp;
	}
	else if (buffer[2] != -1){
		int temp = buffer[2];
		buffer[2] = -1;
		return temp;
	}
	else if (buffer[1] != -1){
		int temp = buffer[1];
		buffer[1] = -1;
		return temp;
	}
	else if (buffer[0] != -1){
		int temp = buffer[0];
		buffer[0] = -1;
		return temp;
	}
	else{
		printf("BUFFER IS EMPTY\n");
		return 0;
	}
}

bool isBufferEmpty(){
	if (buffer[0] == -1 && buffer[1] == -1 && buffer[2] == -1 && buffer[3] == -1)
		return true;
	return false;
}

DWORD WINAPI producer(LPVOID lpParameter){
	struct arguments * args = (struct arguments *) lpParameter;
	WORD counter = 0;

	while (true){
		int newItem = createItem(counter, args->numberOfThreads, args->threadNum);
		WaitForSingleObject(semaphoreEmpty, INFINITE);
		WaitForSingleObject(semaphoreMutex, INFINITE);
		insertItem(newItem);
		counter++;
		ReleaseSemaphore(semaphoreMutex, 1, NULL);
		ReleaseSemaphore(semaphoreFull, 1, NULL);
		if (counter == args->produced){
			finishedProducers++;
			ExitThread(1);
		}
	}
}

DWORD WINAPI consumer(LPVOID lpParameter){
	printf("Consumer thread <%i> created.\n", GetCurrentThreadId());
	Sleep(5000);

	while (true){
		WaitForSingleObject(semaphoreFull, INFINITE);
		WaitForSingleObject(semaphoreMutex, INFINITE);
		int item = getItem();
		ReleaseSemaphore(semaphoreMutex, 1, NULL);
		ReleaseSemaphore(semaphoreEmpty, 1, NULL);
		printf("Item %i Consumed by thread <%i> \n", item, GetCurrentThreadId());
		Sleep(1000);
		if (isBufferEmpty && finishedProducers == totalProducers){
			ExitThread(1);
		}
	}
}

int main(int argc, char ** argv){
	// variable declaration
	WORD				bufferSlots		= atoi(argv[1]);
	WORD				consumersNum	= atoi(argv[2]);
	WORD				producersNum	= atoi(argv[3]);
	WORD				produced		= atoi(argv[4]);
	WORD				i;
	struct arguments	*consArgs;
	struct arguments	*prodArgs;
	HANDLE				*consThreads;
	HANDLE				*prodThreads;

	printf("Buffer slots: %d\n", bufferSlots);
	printf("Number of Consumer: %d\n", consumersNum);
	printf("Number of Producer: %d\n", producersNum);
	printf("Number of produced per producer : %d\n", produced);

	// memory allocation
	consArgs = (struct arguments*) malloc(consumersNum);
	prodArgs = (struct arguments*) malloc(producersNum);
	buffer = (int*) malloc(bufferSlots);
	prodThreads = (HANDLE*) malloc(producersNum);
	consThreads = (HANDLE*) malloc(consumersNum);
	totalProducers = producersNum;

	// initialize buffer with zeroes
	for (i = 0; i < 4; i++){
		buffer[i] = -1;
	}

	// create semaphores
	semaphoreFull = CreateSemaphore(NULL, 0, bufferSlots, "Full");
	semaphoreEmpty = CreateSemaphore(NULL, bufferSlots, bufferSlots, "Empty");
	semaphoreMutex = CreateSemaphore(NULL, 1, 1, NULL);

	// for loop for creating producers
	for (i = 0; i < producersNum; i++){
		prodArgs[i].numberOfThreads = producersNum;
		prodArgs[i].produced = produced;
		prodArgs[i].threadNum = i;
		prodThreads[i] = CreateThread(NULL, 0, &producer, &prodArgs[i], 0, NULL);
	}

	// for loop for creating consumers
	for (i = 0; i < consumersNum; i++){
		consArgs[i].numberOfThreads = consumersNum;
		consArgs[i].produced = produced;
		consArgs[i].threadNum = i;
		consThreads[i] = CreateThread(NULL, 0, &consumer, &consArgs[i], 0, NULL);
	}

	// wait for all threads to complete
	WaitForMultipleObjects(producersNum, prodThreads, TRUE, INFINITE);
	WaitForMultipleObjects(consumersNum, consThreads, TRUE, INFINITE);

	// for loop to close handles for threads
	for (i = 0; i < consumersNum; i++){
		CloseHandle(consThreads[i]);
	}
	for (i = 0; i < producersNum; i++){
		CloseHandle(prodThreads[i]);
	}

	// close handles for semaphores
	CloseHandle(semaphoreEmpty);
	CloseHandle(semaphoreFull);
	CloseHandle(semaphoreMutex);

	printf("All threads completed\n");
	printf("Press <RETURN> to continue...\n");
	getchar();

	return 0;
}