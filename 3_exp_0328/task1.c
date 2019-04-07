#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *sem23 = NULL;
sem_t *sem24 = NULL;
sem_t *sem34 = NULL;

void *worker1(void *arg) {
    printf("I am the Process 1\n");

    sem_post(sem23);

    return NULL;
}
void *worker2(void *arg) {        
    sem_wait(sem23);
    printf("I am the Process 2\n");
    sem_post(sem23);

    sem_post(sem24);
    
    return NULL;
}
void *worker3(void *arg) {        
    sem_wait(sem23);
    printf("I am the Process 3\n");
    sem_post(sem23);

    sem_post(sem34);

    return NULL;
}
void *worker4(void *arg) {        
    sem_wait(sem34);
    sem_wait(sem24);
    printf("I am the Process 4\n");

    return NULL;
}

int main(int argc, char *argv[]) {
    sem23 = sem_open("sem23", O_CREAT, 0666, 0);
    sem24 = sem_open("sem24", O_CREAT, 0666, 0);
    sem34 = sem_open("sem34", O_CREAT, 0666, 0);

    pthread_t p1, p2, p3, p4;
    // printf("Initial value : %d\n", counter);
    pthread_create(&p1, NULL, worker1, NULL);
    pthread_create(&p2, NULL, worker2, NULL);
    pthread_create(&p3, NULL, worker3, NULL);
    pthread_create(&p4, NULL, worker4, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);

    sem_close(sem23);
    sem_close(sem24);
    sem_close(sem34);
    sem_unlink("sem23");
    sem_unlink("sem24");
    sem_unlink("sem34");

    return 0;
}