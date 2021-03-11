#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

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
    //TODO: plug seed into srand()
    srand(seed);

    //TODO: using rand pick a number between 10-15
    int numOfChildren = (rand() % 6) + 10;
    printf("Random Child Count: %d\n", numOfChildren);
    printf("I'm feeling prolific!\n");

    //TODO: Generate and store a random number for each child in an array
    randomNumbers[numOfChildren];
    for(int i = 0; i < numOfChildren; i++){
        randomNumbers[i] = rand();
    }
    //TODO: spawn children
    int masterID = getpid();
    for(int i = 0; i < numOfChildren && (getpid() == masterID); i++){
        counter = i;
        int childID = fork();
        if(childID == 0 ){
            int myNum = randomNumbers[counter];
            int seconds = (myNum % 3)+ 1;
            int code = (myNum % 50) + 1;
            printf("    [Child, PID: %d]: I am the child and I will wait %d seconds and exit with code %d.\n",getpid(), seconds, code );
            sleep(seconds);
            printf("    [Child, PID: %d]: Now exiting...\n",getpid());
            exit(code);
        }else{
            printf("[Parent]: I am waiting for PID %d to finish.\n",childID);
            int status;
            waitpid(0, &status, 0);
            int exitCode = WEXITSTATUS(status);
            
            printf("[Parent]: Child %d finished with status code %d. Onward!\n", childID, exitCode);
        }
    }
}