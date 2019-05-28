#include "random.h"

int * getRandom(int m=100, int e=15, int t=0.05, int N=64) {
    int p = 0;
    int num = 0;
    int v[100000];
    for (int i = 0; i < 100000; i++) {
        v[i] = ((rand() * e) + p) % N;
        num++;
        if (num >= m) {
            if (rand() < t) {
                p = rand() * N;
            }
            else {
                p = (p + 1) % N;
            }
            num = 0;
        }
    }

    return v;
}
