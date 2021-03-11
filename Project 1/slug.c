#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    FILE *fp;
    char buffer[255];
    int seed;

    sprintf(buffer, "./seed_slug_%s.txt", argv[1]);
    fp = fopen(buffer, "r");
    fscanf(fp, "%s", buffer);
    seed = atoi(buffer);
    printf("[Slug PID: %d] Read seed value: %s\n", getpid(), buffer);
    printf("[Slug PID: %d] Read seed value (converted to integer): %d\n",getpid(), seed);

    srand(seed);

    int seconds = (rand() % 4) + 1;
    int coinFlip = rand() % 2;

    printf("[Slug PID: %d] Delay time is %d seconds. Coin flip: %d\n",getpid(), seconds, coinFlip);
    printf("[Slug PID: %d] I’ll get the job done. Eventually...\n",getpid());

    sleep(seconds);

    if(coinFlip == 1){
        printf("[Slug PID: %d] Break time is over! I am running the ’id -u’ command.\n",getpid());
        char *argv[] = {"id", "-u", NULL};
        execvp(argv[0], argv);
    }else{
        printf("[Slug PID: %d] Break time is over! I am running the ’last -d --fulltimes’ command.\n",getpid());
        char *argv[] = {"last", "-d","--fulltimes", NULL};
        execvp(argv[0], argv);
    }
}