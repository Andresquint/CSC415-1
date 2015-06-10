//
//  main.c
//  Homework1
//
//  Created by Hossein Niazmandi on 1/28/15.
//  Copyright (c) 2014 Hossein Niazmandi. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define NAME "Hossein Niazmandi"

int main(){
    
	char buffer[100];
	int fd =1;
	
    
	sprintf(buffer, "Hello 415, I am %s", NAME);
    write(fd, buffer, strlen(buffer));
    
		return 0;
}