#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){

    printf("Hello, please help me schedule!\n");
    //input parsing
    char policy[100];
    char jobPath[100];
    char timeSlice[100];
    if(argc >= 2){
        strcpy(policy, argv[1]);
    } if(argc >= 3){
        strcpy(jobPath, argv[2]);
    } if(argc >= 4){
        strcpy(timeSlice, argv[3]);
    }
    //Figure out what policy to use hacky solution but it works
    int policyFlag = 0;
    if(policy[0] == 'F'){
        policyFlag = 1;
    }else if(policy[0] == 'S'){
        policyFlag = 2;
    }else if(policy[0] == 'R'){
        policyFlag = 3;
    }

    int timeSliceInt= atoi(timeSlice);
    printf("%d\n",timeSliceInt);
}