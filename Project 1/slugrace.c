#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define BILLION  1000000000L;

int main(){
    int masterID = getpid();
    char buffer[255];
    int children[4];

    for(int i = 0; i < 4 && (getpid() == masterID); i++){
        int childID = fork();
        if(childID == 0 ){
            printf("    [Child, PID: %d]: Executing ’./slug %d’ command....\n",getpid(), i + 1);
            sprintf(buffer, "%d", i + 1);
            char *argv[] = {"./slug",buffer,NULL};
            execvp(argv[0], argv);
        }else{
            printf("[Parent]: I forked off child %d.\n",childID);
            children[i] = childID;
        }
    }

    usleep(250000);

    int flag = 1;
    int racers[4] = {children[0],children[1],children[2],children[3]};
    int running[4];
    int lastRunState[4];
    int status[4];
    
    struct timespec start, stop;
    double time;
    clock_gettime( CLOCK_REALTIME, &start);
    
    while(flag && (getpid() == masterID)){
        
        for (int i = 0; i < 4; i++){
            waitpid(children[i], &status[i], WNOHANG);
            lastRunState[i] = running[i];
            running[i] = WIFSIGNALED(status[i]);
        }

        for (int i = 0; i < 4; i++){
            if(lastRunState[i] && !running[i]){
                clock_gettime( CLOCK_REALTIME, &stop);
                time = ( stop.tv_sec - start.tv_sec )+ ( stop.tv_nsec - start.tv_nsec ) / BILLION;
                printf("Child %d has crossed the finish line! It took %f seconds\n", children[i],time);
            }
        }
        
        if((!running[0]) && (!running[1]) && (!running[2]) && (!running[3])){
            flag = 0;
        }else{
            printf("The race is ongoing. The following children are still racing: ");
            
            for(int i = 0; i < 4; i++){
                if (running[i]){
                printf(" %d", children[i]);
                }
            }

            printf("\n");
        }
        
        usleep(250000);
    }
    return 0;
}