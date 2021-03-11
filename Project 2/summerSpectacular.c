#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h> 
#include <time.h>

/*
Requirments

Groups: Can share the stage with people of the same group
    - Flamenco Dancers (15)
    - Torch Jugglers   (8)

Soloists: Must Be alone (2)
    - Magician          (-)
    - Opera Singer      (-)

Stage:
    - people of the same type (non-soloists) can share the stage
    - 4 person limit
    - no one group can dominate the stage (starvation)

Proformances:
    - each proformer proforms for a random amount of time (upper bounded)
    - if a proformer is ready and the stage is open the get on else the nap (non-busy wait)

    - Entering/Exit Prosses
        + Identify itself
        + Identify Stage position using

    - Proforming Prosses
        + A proformance is a sleep(rand())
*/

// declerations
#define STAGE_SIZE 4
#define MAX_PROFORMANCE_LEGTH 2 // + 3
#define REST_MAX 8 // + 3
#define PILE_ON_CHANCE_DANCER 94
#define PILE_ON_CHANCE_JUGGELER 89
enum preformers{SOLOIST,DANCER,JUGGELER,NONE};

//mutxes
pthread_mutex_t stageEntrance = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stageManager = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stage1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stage2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stage3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stage4 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t emptyStage = PTHREAD_COND_INITIALIZER;

// Declaration of thread variables
enum preformers onStage = NONE;
int peopleOnStage = 0;

typedef struct threadData
{
    int randSeed;
    int id;
}threadData;

void* soloist(void* arg)
{
    threadData data = *((threadData*)arg);
    int id = data.id;
    int proforming = 0;
    while(!proforming){
        pthread_mutex_trylock(&stageEntrance);//try and proform
        if(onStage == NONE){//proform
            onStage = SOLOIST;
            pthread_mutex_lock(&stage1);
            pthread_mutex_lock(&stage2);
            pthread_mutex_lock(&stage3);
            pthread_mutex_lock(&stage4);
            proforming = 1;
        }
    }
    pthread_mutex_unlock(&stageEntrance);

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage + 1;
    pthread_mutex_unlock(&stageManager);

    
    printf("Hi I am soloist number %d and I will be preforming at stage position... EVERYWHERE\n",id);
    sleep((rand() % MAX_PROFORMANCE_LEGTH) + 3);
    printf("Hi I am soloist number %d and my show is done\n",id);
    pthread_mutex_unlock(&stage1);
    pthread_mutex_unlock(&stage2);
    pthread_mutex_unlock(&stage3);
    pthread_mutex_unlock(&stage4);

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage - 1;
    pthread_mutex_unlock(&stageManager);

    onStage = NONE;
    pthread_cond_broadcast(&emptyStage);
}

void* dancer(void* arg) 
{ 
    threadData data = *((threadData*)arg);
    int id = data.id;
    int proforming = 0;
    while(!proforming){
        if(pthread_mutex_trylock(&stageEntrance) == 0){//try and proform
            if(onStage == NONE){//proform
                onStage = DANCER;
                if(pthread_mutex_trylock(&stage1) == 0){//found a spot to proform
                proforming = 1;
                }else if(pthread_mutex_trylock(&stage2) == 0){//found a spot to proform
                proforming = 2;
                }else if(pthread_mutex_trylock(&stage3) == 0){//found a spot to proform
                proforming = 3;
                }else if(pthread_mutex_trylock(&stage4) == 0){//found a spot to proform
                proforming = 4;
                }else{
                    //stage full go nap
                    pthread_mutex_unlock(&stageEntrance);
                    sleep((rand() % REST_MAX) + 3);
                }
            }else if(onStage == DANCER){
                int i = rand()%100;
                //printf("I rolled a %d\n",i);
                if(i > PILE_ON_CHANCE_DANCER){
                    if(pthread_mutex_trylock(&stage1) == 0){//found a spot to proform
                    proforming = 1;
                    }else if(pthread_mutex_trylock(&stage2) == 0){//found a spot to proform
                    proforming = 2;
                    }else if(pthread_mutex_trylock(&stage3) == 0){//found a spot to proform
                    proforming = 3;
                    }else if(pthread_mutex_trylock(&stage4) == 0){//found a spot to proform
                    proforming = 4;
                    }else{
                        //stage full go nap
                        pthread_mutex_unlock(&stageEntrance);
                        sleep((rand() % REST_MAX) + 3);
                    }
                }else{
                    pthread_mutex_unlock(&stageEntrance);
                    sleep((rand() % REST_MAX) + 3);
                }
            }else{
                pthread_mutex_unlock(&stageEntrance);
            }
        }else{//nap
            sleep((rand() % REST_MAX) + 3);
        }
    }
    pthread_mutex_unlock(&stageEntrance);

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage + 1;
    pthread_mutex_unlock(&stageManager);

    printf("Hi I am dancer number %d and I will be preforming at stage position %d\n",id,proforming);
    sleep((rand() % MAX_PROFORMANCE_LEGTH) + 3);
    printf("Hi I am dancer number %d and my show is done\n",id);
    switch (proforming)
    {
    case 1:
    pthread_mutex_unlock(&stage1);
        break;
    case 2:
    pthread_mutex_unlock(&stage2);
        break;
    case 3:
    pthread_mutex_unlock(&stage3);
        break;
    case 4:
    pthread_mutex_unlock(&stage4);
        break;
    }

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage - 1;
    if(peopleOnStage == 0){
        onStage = NONE;
    }
    pthread_mutex_unlock(&stageManager);
} 

void* juggeler(void* arg) 
{ 
    threadData data = *((threadData*)arg);
    int id = data.id;
    int proforming = 0;
    while(!proforming){
         if(pthread_mutex_trylock(&stageEntrance) == 0){//try and proform
            if(onStage == NONE){//proform
                onStage = JUGGELER;
                if(pthread_mutex_trylock(&stage1) == 0){//found a spot to proform
                proforming = 1;
                }else if(pthread_mutex_trylock(&stage2) == 0){//found a spot to proform
                proforming = 2;
                }else if(pthread_mutex_trylock(&stage3) == 0){//found a spot to proform
                proforming = 3;
                }else if(pthread_mutex_trylock(&stage4) == 0){//found a spot to proform
                proforming = 4;
                }else{
                    //stage full go nap
                    pthread_mutex_unlock(&stageEntrance);
                    sleep((rand() % REST_MAX) + 3);
                }
            }else if(onStage == JUGGELER){
                int i = rand()%100;
                //printf("I rolled a %d\n",i);
                if(i > PILE_ON_CHANCE_DANCER){
                    if(pthread_mutex_trylock(&stage1) == 0){//found a spot to proform
                    proforming = 1;
                    }else if(pthread_mutex_trylock(&stage2) == 0){//found a spot to proform
                    proforming = 2;
                    }else if(pthread_mutex_trylock(&stage3) == 0){//found a spot to proform
                    proforming = 3;
                    }else if(pthread_mutex_trylock(&stage4) == 0){//found a spot to proform
                    proforming = 4;
                    }else{
                        //stage full go nap
                        pthread_mutex_unlock(&stageEntrance);
                        sleep((rand() % REST_MAX) + 3);
                    }
                }else{
                    pthread_mutex_unlock(&stageEntrance);
                }
            }else{//nap
                pthread_mutex_unlock(&stageEntrance);
                sleep((rand() % REST_MAX) + 3);
            }
        }else{//nap
            sleep((rand() % REST_MAX) + 3);
        }//here
    }
    pthread_mutex_unlock(&stageEntrance);

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage + 1;
    pthread_mutex_unlock(&stageManager);

    printf("Hi I am juggeler number %d and I will be preforming at stage position %d\n",id,proforming);
    sleep((rand() % MAX_PROFORMANCE_LEGTH) + 3);
    printf("Hi I am juggeler number %d and my show is done\n", id);
    switch (proforming)
    {
    case 1:
    pthread_mutex_unlock(&stage1);
        break;
    case 2:
    pthread_mutex_unlock(&stage2);
        break;
    case 3:
    pthread_mutex_unlock(&stage3);
        break;
    case 4:
    pthread_mutex_unlock(&stage4);
        break;
    }

    pthread_mutex_lock(&stageManager);
    peopleOnStage = peopleOnStage - 1;
    if(peopleOnStage == 0){
        onStage = NONE;
    }
    pthread_mutex_unlock(&stageManager);
}


int main(){
    int stageCounter = 0;
    int randomNumbers[15];
    
    FILE *fp;
    char buffer[255];
    int seed;

    fp = fopen("./seed.txt", "r");
    fscanf(fp, "%s", buffer);
    seed = atoi(buffer);
    printf("Read seed value: %s\n\n", buffer);
    printf("Read seed value (converted to integer): %d\n", seed);

    srand(seed);
    pthread_mutex_lock(&stageEntrance);

    //Make all the proformers  
    //make the dancers
    threadData indivigualSeeds[25];
    for (int i = 0; i < 25; i++){
        indivigualSeeds[i].randSeed = rand();
    }
    pthread_t dancers[15];
    for(int i = 0; i < 15; i ++){
        indivigualSeeds[i].id = i;
        if(pthread_create(&dancers[i], NULL, dancer, &indivigualSeeds[i]) != 0){
            
            fprintf(stderr, "error: Cannot create thread # %d\n", i);
            break;
        }
    }
   
    //make juggelers 
    pthread_t juggelers[8];
    for(int i = 0; i < 8; i ++){
        indivigualSeeds[i + 15].id = i;
        if(pthread_create(&juggelers[i], NULL, juggeler, &indivigualSeeds[i + 15]) != 0){
            fprintf(stderr, "error: Cannot create thread # %d\n", i);
            break;
        }    
    } 
    //make soloists 
    pthread_t Singer, Magician;
        indivigualSeeds[23].id = 0;
        pthread_create(&Singer, NULL, soloist, &indivigualSeeds[23]);
        indivigualSeeds[24].id = 1;
        pthread_create(&Magician, NULL, soloist, &indivigualSeeds[24]);
    pthread_mutex_unlock(&stageEntrance);

    for(int i = 0; i < 15; i ++){
        pthread_join(dancers[i],NULL);
    }
    for(int i = 0; i < 8; i ++){
        pthread_join(juggelers[i],NULL);
    }
    pthread_join(Magician,NULL);
    pthread_join(Singer,NULL);
}

