#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
 
//Global variable
//pagesize, memory, which mode we are using, allocated of framestack
//
int SzofPage, SzofMemory, pMode, NumOfFrame;
int *framestack;
//check for page
int checkpg[101];
 

//Compares the size verses the token looking for marker
int checkforindex(int marker, int checking[], int capactiy){
    int token = 0;
     
    do{
        if (checking[token] == marker)
            return token;
        token++;
    } while (token < capactiy);
     
    return -1;
}
 
//this function reads through the bottom of the contents of the array until
// the token finds the marker
int Iteratefromend(int marker, int start, int checking[], int capactiy){
    int token = start;
     
    do{
        if (checking[token] == marker)
            return token;
        token--;
    } while (token >= 0);
     
    return -1;
}

//this algorithim calculates the FIFO 

int Firstinfirstout(int argsread[], int arraySize){
    int newframe = 0, numOfPageFaults = 0;
    int y;
     
    for (y = 0; y < NumOfFrame; y++){
        framestack[y] = -1;
    }
     
    for (y = 0; y < arraySize; y++){
        if (checkforindex(argsread[y], framestack, NumOfFrame) < 0){
            framestack[newframe % NumOfFrame] = argsread[y];
            newframe++;
            numOfPageFaults++;
        }
    }
    return numOfPageFaults;
}
//this algorithim calculates the the least used 
int lastrecentused(int argsread[], int arraySize){
    int numOfPageFaults = 0, token, y, x, indextoken = 0, point, t;
     
    for (y = 0; y < 3; y++){
        framestack[y] = -1;
    }
    //two arrays to keep track of the last used frame
    int *check1;
	//allocated memory for first array
    check1 = (int *)malloc(NumOfFrame * sizeof(int));
     
    int *check2;
	//allocated memory for second array
    check2 = (int *)malloc(NumOfFrame * sizeof(int));
     
    framestack[indextoken] = argsread[indextoken];
     
    numOfPageFaults++;
    indextoken++;
     
    for (y = 1; y< arraySize; y++){
        token = 0;
         
        if (checkforindex(argsread[y], framestack, NumOfFrame) < 0){
            token = NumOfFrame;
            numOfPageFaults++;
             
            if (indextoken < NumOfFrame){
                framestack[indextoken] = argsread[y];
                indextoken++;
            }
            else{
                for (point = 0; point < NumOfFrame; point++){
                    check2[point] = 0;
                    for (x = y - 1; x < SzofPage; x--){
                        if (framestack[point] != argsread[x])
                            check2[point]++;
                        else
                            break;
                    }
                }
                 
                for (point = 0; point < NumOfFrame; point++)
                    check1[point] = check2[point];
                for (point = 0; point < NumOfFrame; point++){
                    for (x = point; x < NumOfFrame; x++){
                        if (check1[point]<check1[x]){
                            t = check1[point];
                            check1[point] = check1[x];
                            check1[x] = t;
                        }
                    }
                }
                 
                for (point = 0; point < NumOfFrame; point++){
                    if (check2[point] == check1[0])
                        framestack[point] = argsread[y];
                }
            }
        }
    }
    return numOfPageFaults;
}
 

//read in command line arguements from user 
// there needs to be 5 arguements for program to run
int main(int argc, char *argv[]){
    if (argc < 5){
        printf("\n Not enough Args. [program name] [page size][memsize] [Algo] [frame allocated]\n");
        return 1;
    }
     //allocate each CL to respetive origin
    SzofPage = pow(2, atoi(argv[1]));
    SzofMemory = pow(2, atoi(argv[2]));
	//Check to see if user entered FIFO
    if (strcmp(argv[3], "FIFO") == 0){
        pMode = 0;
	//check to see if user entered LRU
    }
    else{
        pMode = 1;
    }
	
    NumOfFrame = atoi(argv[4]);
     
    framestack = (int *)malloc(NumOfFrame * sizeof(int));
  
    int count = 0, allocated;
    char reftomem[100];
     
    printf("\nAlgorithm used: %s\t #ofFrames: %d", argv[3], NumOfFrame);
     
    while (fgets(reftomem, sizeof(reftomem), stdin)){
        reftomem[strlen(reftomem) - 1] = '\0';
         
        if (strcmp(reftomem, "") == 0)
            break;
         
        allocated = (int)strtol(reftomem, NULL, 0) / SzofPage;
         

        checkpg[count] = allocated;
        count++;
    }
     
	
    if (pMode == 1){
        printf("\n # LRU PAGE FAULTS: %d\n", lastrecentused(checkpg, count));
    }
    else if (pMode == 0){
        printf("\n # FIFO PAGE FAULTS: %d\n", Firstinfirstout(checkpg, count));
    }
     
   
     
    return 0;
}
