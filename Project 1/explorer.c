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
    printf("It’s time to see the world/file system!\n");
    //plug seed into srand()
    srand(seed);

    int numOfChildren = 5;

    //Generate and store a random number for each child in an array
    randomNumbers[5];
    for(int i = 0; i < numOfChildren; i++){
        randomNumbers[i] = rand();
    }
    //spawn children
    int masterID = getpid();
    for(int i = 0; i < numOfChildren && (getpid() == masterID); i++){
        counter = i;
        int key = (randomNumbers[counter] % 6);
        int value;
        switch (key)
        {
        case 0:
            chdir("/home");
            printf("Selection #%d: /home [SUCCESS]\n", i + 1);
            break;
        case 1:
            chdir("/proc");
            printf("Selection #%d: /proc [SUCCESS]\n", i + 1);
            break;
        case 2:
            chdir("/proc/sys");
            printf("Selection #%d: /proc/sys [SUCCESS]\n", i + 1);
            break;
        case 3:
            chdir("/usr");
            printf("Selection #%d: /usr [SUCCESS]\n", i + 1);
            break;
        case 4:
            chdir("/user/bin");
            printf("Selection #%d: /user/bin [SUCCESS]\n", i + 1);
            break;
        case 5:
            chdir("/bin");
            printf("Selection #%d: /bin [SUCCESS]\n", i + 1);
            break;
        }

        printf("Current reported directory: %s\n", getcwd(buffer,255));
        int childID = fork();
        if(childID == 0 ){
            printf("    [Child, PID: %d]: Executing ’ls -alh’ command...\n",getpid());
            char *argv[] = {"ls","-alh",NULL};
            execvp(argv[0], argv);
            exit(0);
        }else{
            printf("[Parent]: I am waiting for PID %d to finish.\n",childID);
            int status;
            waitpid(0, &status, 0);
            int exitCode = WEXITSTATUS(status);
            
            printf("[Parent]: Child %d finished with status code %d. Onward!\n", childID, exitCode);
        }
    }
}
