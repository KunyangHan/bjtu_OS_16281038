#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t lock; 
int ticketCount = 1000;

void *sell(void *arg) {
    int sellTime = 800;
    while (sellTime > 0) {
        pthread_mutex_lock(&lock);

        int temp = ticketCount;
        pthread_yield();
        temp = temp - 1;
        pthread_yield();
        ticketCount = temp;

        pthread_mutex_unlock(&lock); 

        sellTime -= 1;
    }
    return NULL;
}
void *buy(void *arg) {
    int buyTime = 400;
    while (buyTime > 0) {   
        pthread_mutex_lock(&lock);

        int temp = ticketCount;
        pthread_yield();
        temp = temp + 1;
        pthread_yield();
        ticketCount = temp;

        pthread_mutex_unlock(&lock); 

        buyTime -= 1;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1;
    }

    pthread_t p1, p2;
    // printf("Initial value : %d\n", counter);
    pthread_create(&p1, NULL, sell, NULL);
    pthread_create(&p2, NULL, buy, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("ticket remained : %d\n", ticketCount);

    pthread_mutex_destroy(&lock); 

    return 0;
}