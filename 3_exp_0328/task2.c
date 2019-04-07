#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *writelock = NULL;
int ticketCount = 1000;

void *sell(void *arg) {
    int sellTime = 800;
    while (sellTime > 0) {
        sem_wait(writelock);

        int temp = ticketCount;
        pthread_yield();
        temp = temp - 1;
        pthread_yield();
        ticketCount = temp;

        sem_post(writelock);

        sellTime -= 1;
    }
    return NULL;
}
void *buy(void *arg) {
    int buyTime = 400;
    while (buyTime > 0) {   
        sem_wait(writelock);

        int temp = ticketCount;
        pthread_yield();
        temp = temp + 1;
        pthread_yield();
        ticketCount = temp;

        sem_post(writelock);

        buyTime -= 1;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    writelock = sem_open("ticketLock", O_CREAT, 0666, 2);

    pthread_t p1, p2;
    // printf("Initial value : %d\n", counter);
    pthread_create(&p1, NULL, sell, NULL);
    pthread_create(&p2, NULL, buy, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("ticket remained : %d\n", ticketCount);

    sem_close(writelock);
    sem_unlink("ticketLock");

    return 0;
}