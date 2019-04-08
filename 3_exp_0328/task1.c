#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *sem23 = NULL;
sem_t *sem24 = NULL;
sem_t *sem34 = NULL;

void worker1(void *arg) {
    printf("I am the Process 1\n");

    sem_post(sem23);
}
void worker2(void *arg) {        
    sem_wait(sem23);
    printf("I am the Process 2\n");
    sem_post(sem23);

    sem_post(sem24);
}
void worker3(void *arg) {        
    sem_wait(sem23);
    printf("I am the Process 3\n");
    sem_post(sem23);

    sem_post(sem34);
}
void worker4(void *arg) {        
    sem_wait(sem34);
    sem_wait(sem24);
    printf("I am the Process 4\n");
}

int main(int argc, char *argv[]) {
    sem23 = sem_open("sem23", O_CREAT, 0666, 0);
    sem24 = sem_open("sem24", O_CREAT, 0666, 0);
    sem34 = sem_open("sem34", O_CREAT, 0666, 0);

    pid_t fpid1 = fork();
    if (fpid1 == 0) {
        worker1();          // P1
    }
    else {
        pid_t fpid2 = fork();
        if (fpid2 == 0) {
            worker2();      // P2
        }
        else {
            pid_t fpid3 = fork();
            if (fpid3 == 0) {
                worker1();  // P3
            }
            else {
                worker3()   // P4
            }
        }
    }

    sem_close(sem23);
    sem_close(sem24);
    sem_close(sem34);
    sem_unlink("sem23");
    sem_unlink("sem24");
    sem_unlink("sem34");

    return 0;
}