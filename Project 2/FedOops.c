#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h> 
#include <time.h>
#include <semaphore.h>

/*
Tasks:
    Weighing:
    Barcoding:
    X-Raying:
    Jostling:

Stations (General):
    - Each station has 4 teams
    - each team can have one package at a time
    - once they are done they tag in the next teamate
    - each team has 10 workers
    - packages can't be put down
    - Workers move between stations with packages
    - One Package per station
    - no breaks for the packages between stations
    - target destination must be empty
    - only one move at a time (atomic)

Package:
    - One to Four Steps
    - a "Pile" will be delivered to each station
    - each package has a set of instructions
    - Customers now get to pick the steps that these things happen in

*/
enum steps {NO_STEP, WEIGH, BARCODE, XRAY, JOSTL};
enum groupColor {NO_TEAM, RED, BLUE, GREEN, YELLOW};

typedef struct package
{
    int id;
    enum steps steps[4];
}Package;

typedef struct employee
{
    enum groupColor group;
    int team;
    int id;

}Employee;

#define TOTAL_PACKAGES 200
Package pileOfPendingPackages[TOTAL_PACKAGES];
int currentPileIndex = 0;
sem_t mutex, pile;
sem_t onBoarding, checkingLineSpot;
//Weigh Barcode X-Ray Jostel, Convayer
sem_t W,B,X,J,C;
//Group(Color) team
int currrentRunner[4][4];

void* employeeBrain(void* arg)
{
    Employee data = *((Employee*)arg);
    int id = data.id - 1;
    int team = data.team - 1;
    int group = data.group - 1;
    //printf("%d %d %d\n", group, team,id);
    sem_wait(&mutex);//wait for the right to grab a package
    sem_post(&mutex);
    while(1){// LOOP
        sem_wait(&checkingLineSpot);
        int lineSpot = currrentRunner[group][team];
        //printf("CHECKED %d %d\n",id,lineSpot);
        usleep(1000 * 50);
        sem_post(&checkingLineSpot);
        if(lineSpot == id){// while tagged in
            sem_wait(&pile);
            Package myPackage = pileOfPendingPackages[currentPileIndex];
            currentPileIndex = currentPileIndex + 1;
            if(currentPileIndex == TOTAL_PACKAGES){
                exit(0);
            }
            switch (group + 1)
            {
            case RED:
                printf("[RED #%d.%d] picked up package %d with steps %d %d %d %d\n",team, id, myPackage.id, myPackage.steps[0], myPackage.steps[1], myPackage.steps[2], myPackage.steps[3]);
                break;
            case GREEN:
                printf("[GREEN #%d.%d] picked up package %d with steps %d %d %d %d\n",team, id, myPackage.id, myPackage.steps[0], myPackage.steps[1], myPackage.steps[2], myPackage.steps[3]);
                break;
            case BLUE:
                printf("[BLUE #%d.%d] picked up package %d with steps %d %d %d %d\n",team, id, myPackage.id, myPackage.steps[0], myPackage.steps[1], myPackage.steps[2], myPackage.steps[3]);
                break;
            case YELLOW:
                printf("[YELLOW #%d.%d] picked up package %d with steps %d %d %d %d\n",team, id, myPackage.id, myPackage.steps[0], myPackage.steps[1], myPackage.steps[2], myPackage.steps[3]);
                break;
            
            default:
                printf("ERROR TEAM NOT FOUND");
                break;
            }
            sem_post(&pile);//wait for the right to grab a package
            int notDone = 1;
            while(notDone){
                sem_wait(&onBoarding);
                int * weighing,* barcode,* xray,* jostl;
                int one = 1;
                weighing = &one;
                barcode = &one;
                xray = &one;
                jostl = &one;
                for(int curStep = 0; curStep < 4; curStep++){//check to see if all stations needed can be claimed
                    if(myPackage.steps[curStep] != 0){
                        switch (myPackage.steps[curStep])
                        {
                        case WEIGH:
                        sem_getvalue(&W, weighing);
                            break;
                        case BARCODE:
                        sem_getvalue(&B, barcode);
                            break;
                        case XRAY:
                        sem_getvalue(&X,xray);
                            break;
                        case JOSTL:
                        sem_getvalue(&J,jostl);
                            break;
                        default:
                        printf("ERROR STATION NOT FOUND");
                            break;
                        }
                    }
                }
                //printf("%d %d %d %d\n", *weighing, *barcode, *xray, *jostl);
                if(weighing != 0 && barcode != 0 && xray !=0 && jostl != 0){//if all stations can be claimed
                    
                    for(int curStep = 0; curStep < 4; curStep++){
                        if(myPackage.steps[curStep] != 0){
                            switch (myPackage.steps[curStep])
                            {
                            case WEIGH:
                            sem_wait(&W);
                                break;
                            case BARCODE:
                            sem_wait(&B);
                                break;
                            case XRAY:
                            sem_wait(&X);
                                break;
                            case JOSTL:
                            sem_wait(&J);
                                break;
                            }
                        }

                    
                    
                    }//done claiming all spots
                    sem_post(&onBoarding);
                    for(int curStep = 0; curStep < 4; curStep ++){
                        if(myPackage.steps[curStep] != 0){
                            switch (myPackage.steps[curStep])
                            {
                            case WEIGH:
                            printf("[%d #%d.%d] Placing package %d at WEIGING\n",group,team, id, myPackage.id);
                                break;
                            case BARCODE:
                            printf("[%d #%d.%d] Placing package %d at BARCODING\n",group,team, id, myPackage.id);
                                break;
                            case XRAY:
                            printf("[%d #%d.%d] Placing package %d at XRAYING\n",group,team, id, myPackage.id);
                                break;
                            case JOSTL:
                            printf("[%d #%d.%d] Placing package %d at JOSTELING\n",group,team, id, myPackage.id);
                                break;
                            }
                            usleep(1000 * 5);//sleep
                            sem_wait(&C);//wait for convayer
                            printf("[%d #%d.%d] Moving package %d \n",group,team, id, myPackage.id);
                            switch (myPackage.steps[curStep])
                            {
                            case WEIGH:
                            sem_post(&W);
                                break;
                            case BARCODE:
                            sem_post(&B);
                                break;
                            case XRAY:
                            sem_post(&X);
                                break;
                            case JOSTL:
                            sem_post(&J);
                                break;
                            }
                            sem_post(&C);
                        }
                    }
                    printf("[%d #%d.%d] Placing package %d onto the truck\n",group,team, id, myPackage.id);
                    notDone = 0;
                }else{//if all needed spots cant be claimed
                    sem_post(&onBoarding);
                }
            }//end of the isDone loop
            currrentRunner[group][team] = (currrentRunner[group][team] + 1) % 10;
        }//end of the is spot in line loop
        sleep(1);
    }//end of while 1
    //printf("Hi I am on the %d group team #%d id #%d\n",data.group,team,id);

    //get in queue
    //if first and signaled take package

}

int main(){
    int counter;
    int randomNumbers[15];
    //TODO: read the seed from seed.txt
    FILE *fp;
    char buffer[255];
    int seed;

    fp = fopen("./seed.txt", "r");
    fscanf(fp, "%s", buffer);
    seed = atoi(buffer);
    printf("Read seed value: %s\n\n", buffer);
    printf("Read seed value (converted to integer): %d\n", seed);

    srand(seed);

    //make packages
    for(int i = 0; i < TOTAL_PACKAGES; i++){
        int numSteps = ((rand() % 4));
        int stepOne = ((rand() % 4) + 1);
        int stepTwo = 0, stepThree = 0, stepFour = 0;
        if(numSteps >= 1){
            stepTwo = (rand() % 4) + 1;
            while (stepTwo == stepOne){
                stepTwo = (rand() % 4) + 1;
            }
        }
        if(numSteps >= 2){
            stepThree = (rand() % 4) + 1;
            int flag = 1;
            while (flag){
                if(stepThree != stepOne && stepThree != stepTwo){
                    flag = 0;
                }else{
                stepThree = (rand() % 4) + 1;
                }
            }
        }
        if(numSteps == 3){
            stepFour = 10 - (stepOne + stepTwo + stepThree);
        }
        pileOfPendingPackages[i].id = i+1;
        pileOfPendingPackages[i].steps[0] = stepOne;
        pileOfPendingPackages[i].steps[1] = stepTwo;
        pileOfPendingPackages[i].steps[2] = stepThree;
        pileOfPendingPackages[i].steps[3] = stepFour;
    }
    /*
    for (int i = 0; i < 50; i++){
        printf("PACKAGE CREATED ID:%d steps[%d,%d,%d,%d]\n", pileOfPendingPackages[i].id,pileOfPendingPackages[i].steps[0],pileOfPendingPackages[i].steps[1],pileOfPendingPackages[i].steps[2],pileOfPendingPackages[i].steps[3]);
    }*/
    
    //make the employees
    sem_init(&mutex, 0, 1);
    sem_init(&onBoarding, 0, 1);
    sem_init(&checkingLineSpot, 0, 1);
    sem_init(&pile, 0, 1);
    sem_init(&W , 0, 1);
    sem_init(&B , 0, 1);
    sem_init(&X , 0, 1);
    sem_init(&J , 0, 1);
    sem_init(&C , 0, 1);

    pthread_t allEmployees[160];
    Employee data[150];
    sem_wait(&mutex);
    //make employees
    for(int group = 1; group < 5; group++){
        for(int team = 1; team < 5; team++){
            for(int id = 1; id <= 10; id++){
                data[(group * team * id) - 1].group = group;
                data[(group * team * id) - 1].team = team;
                data[(group * team * id) - 1].id = id;
                pthread_create(&allEmployees[(group * team * id) - 1],NULL,employeeBrain,&data[(group * team * id) - 1]);
            }
        }
    }
    sleep(3);
    sem_post(&mutex);
    for(int i = 0; i < 160; i ++){
        pthread_join(allEmployees[i],NULL);
    }
    sleep(4);//let the print statements finsh
}