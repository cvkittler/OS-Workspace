#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct job_
{
    int id;
    int legth;
    int amoutDone;
    struct job *next;
}job_;


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
    //parse the time slice into an int
    int timeSliceInt= atoi(timeSlice);
    //read the job
    FILE *fp;
    fp = fopen(jobPath, "r");
    char buffer[255];
    int jobs[50];
    int counter = 0;
    for(int i = fscanf(fp,"%s",buffer); i != EOF; i = fscanf(fp,"%s",buffer)){
        jobs[counter] = atoi(buffer);
        counter = counter + 1;
    }
    //setup the workflow
    switch (policyFlag)
    {
    case 1://First in first out
        struct job_ * firstJob;
        (*firstJob).id = 0;
        (*firstJob).legth = jobs[firstJob->id];
        (*firstJob).amoutDone = 0;
        (*firstJob).next = (void*)NULL;
        struct job_ * lastJob = firstJob;
        for(int i = 1; i < counter; i ++){
            struct job_ * currJob;
            (*currJob).id = i;
            (*currJob).legth = jobs[currJob->id];
            (*currJob).amoutDone = 0;
            (*currJob).next = (void*)NULL;
            (*lastJob).next = currJob;
        }
        break;
    case 2://Shortest job first
        break;
    case 3://Round roben
        break;
    default:
        break;
    }
}