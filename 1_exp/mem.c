#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int *p = malloc(sizeof(int)); // a1
    assert(p != NULL);
    printf("(%d) address pointed to by p: %p\n", getpid(), p); // a2
    *p = 0; // a3
    while (*p < 20) {
        sleep(1);
        *p = *p + 1;
        printf("(%d) p: %2d addr_p: %p\n", getpid(), *p, p); // a4
    }
    return 0;
}