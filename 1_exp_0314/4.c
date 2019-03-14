#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int myFork(int, int);
void suiside();
char type;

int main (int argc, char *argv[]){
    type = argv[1][0];

    printf("I'm father, my pid : %d\r\n", getpid());
    myFork(1, 2);
    
    return 0;
}

int myFork(int counter, int step) {
    pid_t fpid = fork();

    if (fpid < 0) 
        printf("error in fork!");
    else if (fpid == 0) {
        printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
        if (counter > 0) {
            myFork(counter - 1, 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
        }
    }
    else {
        if (counter >= 0) {
            myFork(counter - step, step + 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            if (step == 3) {
                printf("I'm father, my pid : %d\r\n", getpid());
            }
            else {
                // printf("pid : %d suiside\r\n", getpid());
                suiside();
                printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
            }
        }
    }
    return 0;
}

void suiside() {
    int num[2] = {0, 0};
    int *p = 0x0;
    switch (type) {
    case '1':
        printf("self exit\r\n");
        exit(0);
        break;
    case '2':
        printf("illegal address accessing\r\n");
        num[1000] = *p;
        printf("didn't stop\r\n");
        break;
    default:
        // printf("others\r\n");
        break;
    }
}