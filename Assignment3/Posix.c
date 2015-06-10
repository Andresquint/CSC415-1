
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

//First intiliaze the buffers that will be used
#define BUFFERSIZE 1024
#define input 16
#define string 16

//Parsing the input string
// Code is readily avaible online
void parse(char *line, char *argc[]){
    while(*line != '\0'){
        
        while(*line == ' ' || *line == '\t' || *line == '\n'){
            *line++ = '\0';
        }
        
        if(*line){
            *argc++ = line;
        }
        while(*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n'){
            line++;
        }
    }
    *argc = '\0';
    
}

//this function will execute any command given to it
//it will allow the process to fork and wait for the child process to finish.
void cmdline(char *argc[]){
    pid_t childprocess;
    int running;
    
    if((childprocess = fork()) < 0){
        printf("Fork() process failed.\n");
        exit(1);
    } else if (childprocess == 0){
        if(execlp(*argc, *argc, (char*)NULL ) < 0){
            printf("Invalid input Execlp() failed.\n");
            exit(1);
        }
        //wait for parent
    } else{
        while(wait(&running) != childprocess)
        ;
    }
}


int main(void){
    char buff1[BUFFERSIZE];
    char *argc[input];
    
    // infinite while loop until "exit" command is given
    // Compare the specific Keywords to strcmp if match then break
    // otherwise continue running
    while(1) {
        printf("Myshell> ");
        fgets(buff1, BUFFERSIZE, stdin);
        printf("\n");
        parse(buff1, argc);
        //Prevents breaking incase of user pressing return
        if(argc[0] == NULL){
            continue;
        }
        // If user enters exit terminate the program
        if((strcmp(argc[0], "exit")) == 0){
            printf("Terminating Child process \n");
            exit(0);
        }
        cmdline(argc);
    }
    
}