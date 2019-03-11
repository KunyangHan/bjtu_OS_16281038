#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: cpu <string>\n");
        exit(1);
    }
    char *str = argv[1];
    int coun = 0;
    while (coun < 20) {
        coun++;
        sleep(1);
        printf("%s\n", str);
    }
    return 0;
}