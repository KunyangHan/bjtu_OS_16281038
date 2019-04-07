#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

volatile int counter = 0;
sem_t *resource = NULL;
sem_t *character = NULL;
char buf[10] = {'1'};

void print() {
    for (int i = 0; i < 10; i++) {
        printf("\n%d : %c", i, buf[i]);
    }
    printf("\n");
}
void *reader(void *arg){
    int p = 0;
    char temp[50];
    while (1) {
        scanf("%s", temp);
        for (int i = 0; temp[i] != '\0'; i++) {
            sem_wait(resource);
            
            buf[p] = temp[i];
            p = (p + 1) % 10;
            // printf("\nreader p:%d\n", p);

            sem_post(character);
        }
    }
}
void *writer(void *arg){
    int p = 0;
    while(1) {
        sem_wait(character);

        printf("(%c)\n", buf[p]);
        p = (p + 1) % 10;
        // printf("\nwriter p:%d\n", p);  

        sem_post(resource);

        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    resource = sem_open("bufRemained", O_CREAT, 0666, 10); 
    character = sem_open("bufUsed", O_CREAT, 0666, 0); 

    pthread_t p1, p2;
    pthread_create(&p1, NULL, reader, NULL);
    pthread_create(&p2, NULL, writer, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    sem_close(resource);
    sem_close(character);
    sem_unlink("bufRemained");
    sem_unlink("bufUsed");

    return 0;
}
