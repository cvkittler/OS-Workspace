#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct job_
{
    int id;
    int legth;
    int amoutDone;
    struct job_ *next;
    int startTime;
    int endTime;
}job_;

void swap(int* first, int* last)
{
    int temp = *first;
    *first = *last;
    *last = temp;
}

int main(int argc, char **argv){
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
    struct job_ * firstJob = malloc(sizeof(job_)); //first job to be done
    int curTime = 0;
    double responseTime = 0, turnAround = 0, waitTime = 0;
    double count = 0;

    int id[counter];
    for(int i = 0; i < counter; i++){
        id[i] = i;
    }
    switch (policyFlag)
    {
    case 1://First in first out
        printf("Execution trace with FIFO:\n");
        //initilize first workload
        (*firstJob).id = 0;
        (*firstJob).legth = jobs[firstJob->id];
        (*firstJob).amoutDone = 0;
        (*firstJob).next = (void*)NULL;
        struct job_ * lastJob = firstJob;
        //initilze rest of workload
        for(int i = 1; i < counter; i ++){
            struct job_ * currJob = malloc(sizeof(job_));
            (*currJob).id = i;
            (*currJob).legth = jobs[currJob->id];
            (*currJob).amoutDone = 0;
            (*currJob).next = (void*)NULL;
            (*lastJob).next = currJob;
            lastJob = currJob;
        }

        //exacute the workload
        struct job_ *curJob = firstJob;
        while(curJob->next != NULL){
            (*curJob).startTime = curTime;
            printf("Job %d ran for: %d\n",curJob->id,curJob->legth);
            curTime = curTime + curJob->legth;
            (*curJob).endTime = curTime;
            curJob = curJob->next;
        }
        (*curJob).startTime = curTime;
        printf("Job %d ran for: %d\n",curJob->id,curJob->legth);
        curTime = curTime + curJob->legth;
        (*curJob).endTime = curTime;
        printf("End of execution with FIFO.\n");
        printf("Begin analyzing FIFO:\n");
        curJob = firstJob;
        while(curJob->next != NULL){
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob->id,curJob->startTime, curJob->endTime, curJob->startTime);
            responseTime = responseTime + curJob->startTime;
            turnAround = turnAround + curJob->endTime;
            waitTime = waitTime + curJob->startTime;
            count = count + 1;
            curJob = curJob->next;
        }
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob->id,curJob->startTime, curJob->endTime, curJob->startTime);
        responseTime = responseTime + curJob->startTime;
        turnAround = turnAround + curJob->endTime;
        waitTime = waitTime + curJob->startTime;
        count = count + 1;
        
        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\nEnd analyzing FIFO.\n",responseTime/count,turnAround/count,waitTime/count);
        break;
    case 2://Shortest job first
        printf("Execution trace with SJF:\n");
        // sort the jobs by size
        int curIndex;
        for (int i = 0; i < counter - 1; i++) {
            curIndex = i;
            for (int j = i + 1; j < counter; j++){
                if (jobs[j] < jobs[curIndex]){
                    curIndex = j;
                }
            }
            while(curIndex - i >= 1){
                swap(&jobs[curIndex], &jobs[curIndex - 1]);
                swap(&id[curIndex],&id[curIndex - 1]);
                curIndex--;
            }
        }
        //initilize first workload
        (*firstJob).id = id[0];
        (*firstJob).legth = jobs[0];
        (*firstJob).amoutDone = 0;
        (*firstJob).next = (void*)NULL;
        struct job_ * lastJob2 = firstJob;
        //initilze rest of workload
        for(int i = 1; i < counter; i ++){
            struct job_ * currJob2 = malloc(sizeof(job_));
            (*currJob2).id = id[i];
            (*currJob2).legth = jobs[i];
            (*currJob2).amoutDone = 0;
            (*currJob2).next = (void*)NULL;
            (*lastJob2).next = currJob2;
            lastJob2 = currJob2;
        }

        //exacute the workload
        struct job_ *curJob2 = firstJob;
        while(curJob2->next != NULL){
            (*curJob2).startTime = curTime;
            printf("Job %d ran for: %d\n",curJob2->id,curJob2->legth);
            curTime = curTime + curJob2->legth;
            (*curJob2).endTime = curTime;
            count = count + 1;
            curJob2 = curJob2->next;
        }
        (*curJob2).startTime = curTime;
        printf("Job %d ran for: %d\n",curJob2->id,curJob2->legth);
        curTime = curTime + curJob2->legth;
        (*curJob2).endTime = curTime;
        count = count + 1;
        printf("End of execution with SJF.\n");
        //analyzing
        double responseTime = 0, turnAround = 0, waitTime = 0;
        double count = 0;
        printf("Begin analyzing SJF:\n");
        curJob2 = firstJob;
        while(curJob2->next != NULL){
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob2->id,curJob2->startTime, curJob2->endTime, curJob2->startTime);
            responseTime = responseTime + curJob2->startTime;
            turnAround = turnAround + curJob2->endTime;
            waitTime = waitTime + curJob2->startTime;
            count = count + 1;
            curJob2 = curJob2->next;
        }
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob2->id,curJob2->startTime, curJob2->endTime, curJob2->startTime);
        responseTime = responseTime + curJob2->startTime;
        turnAround = turnAround + curJob2->endTime;
        waitTime = waitTime + curJob2->startTime;
        count = count + 1;
        
        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\nEnd analyzing SJF.\n",responseTime/count,turnAround/count,waitTime/count);
        break;
    case 3://Round roben
        printf("Execution trace with RR:\n");
        //initilize first workload
        (*firstJob).id = 0;
        (*firstJob).legth = jobs[firstJob->id];
        (*firstJob).amoutDone = 0;
        (*firstJob).startTime = -1;
        (*firstJob).endTime = -1;
        (*firstJob).next = (void*)NULL;
        struct job_ * lastJob3 = firstJob;
        //initilze rest of workload
        for(int i = 1; i < counter; i ++){
            struct job_ * currJob3 = malloc(sizeof(job_));
            (*currJob3).id = i;
            (*currJob3).legth = jobs[currJob3->id];
            (*currJob3).amoutDone = 0;
            (*currJob3).startTime = -1;
            (*currJob3).endTime = -1;
            (*currJob3).next = (void*)NULL;
            (*lastJob3).next = currJob3;
            lastJob3 = currJob3;
        }

        //exacute the workload
        struct job_ *curJob3 = firstJob;
        int flag = 1;
        while(flag){
            flag = 0;
            while(curJob3->next != NULL){
                if(curJob3->legth == curJob3->amoutDone){
                    //if current job is done skip
                    curJob3 = curJob3->next;
                } else if(curJob3->legth - curJob3->amoutDone > timeSliceInt){
                    //if amount left is more than the slice
                    //if there is no start set the start
                    if(curJob3->startTime == -1){
                        (*curJob3).startTime = curTime;
                    }
                    //do the job
                    printf("Job %d ran for: %d\n",curJob3->id,timeSliceInt);
                    curTime = curTime + timeSliceInt;
                    //update amount done
                    (*curJob3).amoutDone = curJob3->amoutDone + timeSliceInt;
                    //if the job is done set the end
                    if(curJob3->legth == curJob3->amoutDone){
                        (*curJob3).endTime = curTime;
                    }
                    //printf("ID: %d, Start: %d, End: %d, Size: %d, Finsished: %d\n", curJob3->id,curJob3->startTime,curJob3->endTime,curJob3->legth,curJob3->amoutDone);
                    //get the next job
                    curJob3 = curJob3->next;
                }else{
                    // if the amount left is less than one slice
                    //if this is the start of the current job
                    if(curJob3->startTime == -1){
                        (*curJob3).startTime = curTime;
                    }
                    //do the job
                    printf("Job %d ran for: %d\n",curJob3->id, curJob3->legth - curJob3->amoutDone);
                    curTime = curTime + (curJob3->legth - curJob3->amoutDone);
                    //update the amount done
                    (*curJob3).amoutDone = curJob3->legth;
                    //if the job is done set the end time
                    (*curJob3).endTime = curTime;
                    //printf("ID: %d, Start: %d, End: %d, Size: %d, Finsished: %d\n", curJob3->id,curJob3->startTime,curJob3->endTime,curJob3->legth,curJob3->amoutDone);
                    //get the next job
                    curJob3 = curJob3->next;
                }
            }
            //catch the last job
            if(curJob3->legth - curJob3->amoutDone <= 0){
                //if done do nothing
            } else if(curJob3->legth - curJob3->amoutDone > timeSliceInt){
                //if the amount left is more than one slice
                //if the job is unstarted start it
                if(curJob3->startTime == -1){
                    (*curJob3).startTime = curTime;
                }
                //do the job
                printf("Job %d ran for: %d\n",curJob3->id,timeSliceInt);
                curTime = curTime + timeSliceInt;
                //update the amount that has been finished
                (*curJob3).amoutDone = curJob3->amoutDone + timeSliceInt;
                //if the job is done set the end time
                if(curJob3->legth == curJob3->amoutDone){
                    (*curJob3).endTime = curTime;
                }
                //printf("ID: %d, Start: %d, End: %d, Size: %d, Finsished: %d\n", curJob3->id,curJob3->startTime,curJob3->endTime,curJob3->legth,curJob3->amoutDone);
            }else{
                //if the amount left is smaller than one slice
                //check if started
                if(curJob3->startTime == -1){
                    (*curJob3).startTime = curTime;
                }
                //do the job
                printf("Job %d ran for: %d\n",curJob3->id, curJob3->legth - curJob3->amoutDone);
                curTime = curTime + (curJob3->legth - curJob3->amoutDone);
                //set amount done
                (*curJob3).amoutDone = curJob3->legth;
                (*curJob3).endTime = curTime;
                //printf("ID: %d, Start: %d, End: %d, Size: %d, Finsished: %d\n", curJob3->id,curJob3->startTime,curJob3->endTime,curJob3->legth,curJob3->amoutDone);
            }
            //checking if all jobs are done
            struct job_ * checkingIfFinished = firstJob;
            while(checkingIfFinished->next != NULL){
                if(checkingIfFinished->legth - checkingIfFinished->amoutDone){
                    flag = 1;
                }
                
                checkingIfFinished = checkingIfFinished->next;
            }
            if(checkingIfFinished->legth - checkingIfFinished->amoutDone){
                flag = 1;
            }
            curJob3 = firstJob;
        }
        printf("End of execution with RR.\n");
        //analyzing
        responseTime = 0, turnAround = 0, waitTime = 0;
        count = 0;
        printf("Begin analyzing RR:\n");
        curJob2 = firstJob;
        while(curJob2->next != NULL){
            printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob2->id,curJob2->startTime, curJob2->endTime, curJob2->endTime - curJob2->legth);
            responseTime = responseTime + curJob2->startTime;
            turnAround = turnAround + curJob2->endTime;
            waitTime = waitTime + curJob2->endTime - curJob2->legth;
            count = count + 1;
            curJob2 = curJob2->next;
        }
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", curJob2->id,curJob2->startTime, curJob2->endTime, curJob2->endTime - curJob2->legth);
        responseTime = responseTime + curJob2->startTime;
        turnAround = turnAround + curJob2->endTime;
        waitTime = waitTime + curJob2->endTime - curJob2->legth;
        count = count + 1;
        
        printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\nEnd analyzing RR.\n",responseTime/count,turnAround/count,waitTime/count);
        break;
    default:
        break;
    }
}