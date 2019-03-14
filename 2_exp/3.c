#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int myFork(int, int, int);
void suiside();
char type;

int main (int argc, char *argv[]){
    type = argv[1][0];

    printf("I'm father, my pid : %d\r\n", getpid());
    myFork(getpid(), 1, 2);
    
    return 0;
}

int myFork(int pid, int counter, int step) {
    pid_t fpid = fork();

    if (fpid < 0) 
        printf("error in fork!");
    else if (fpid == 0) {
        printf("Father's pid : %d my pid : %d\r\n", pid, getpid());
        if (counter > 0) {
            myFork(getpid(), counter - 1, 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            printf("Father's pid : %d my pid : %d\r\n", pid, getpid());
        }
    }
    else {
        if (counter >= 0) {
            myFork(getpid(), counter - step, step + 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            if (step == 3) {
                printf("I'm father, my pid : %d\r\n", getpid());
            }
            else {
                // printf("pid : %d suiside\r\n", getpid());
                suiside();
                printf("Father's pid : %d my pid : %d\r\n", pid, getpid());
            }
        }
    }
    return 0;
}

void suiside() {
    int num[2] = {0, 0};
    switch (type) {
    case '1':
        printf("self exit\r\n");
        exit(0);
        break;
    case '2':
        printf("illegal address accessing\r\n");
        for (int i = 0; ; i < 10000) {
            num[0] = num[i];
        }
        printf("didn't stop\r\n");
        break;
    default:
        printf("others\r\n");
        break;
    }
}