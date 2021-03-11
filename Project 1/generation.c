#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
void spawnChild(int counter);
int main(){
    int counter;
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

    int numOfChildren = (rand() % 4) + 7;
    printf("Random Descendant Count: %d\n", numOfChildren);
    counter = numOfChildren;
    spawnChild(counter);
    
}

void spawnChild(int counter){
    if(counter == 0){
        exit(0);
    }
    int childID = fork();
    if(childID == 0){
        printf("    [Child, PID: %d]: I was called with descendant count=%d. I’ll have %d descendant(s).\n",getpid(), counter, counter -1);
        spawnChild(counter- 1);
    }else{
        printf("[Parent, PID: %d]: I am waiting for PID %d to finish.\n",getpid(),childID);


        int status;
        waitpid(childID, &status, 0);
        int exitCode = WEXITSTATUS(status);
        printf("[Parent, PID: %d]: Child %d finished with status code %d. It's now my turn to exit\n",getpid(),childID,exitCode);
        exit(exitCode + 1);
    }
}