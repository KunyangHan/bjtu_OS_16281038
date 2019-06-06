#include <string>

#define C 50
#define H 2
#define B 30
#define SIZE 512

using namespace std;

extern (string*) ldisk[C][H][B];

void read_block(int l, string *p) {
    int a[3] = l2chb(l);
    string *ptr = ldisk[a[0]][a[1]][a[2]];
    for (int i = 0; i < SIZE; i++) {
        (*p)[i] = ptr[i];
    }
}

void write_block(int l, string p) {
    string *ptr = l2chb(l);
    for (int i = 0; i < SIZE; i++) {
        ptr[i] = p[i];
    }
}

int chb2l(int c, int h, int b) {

}

int *l2chb(int l) {
    int a[3] = 0;


    return ldisk[a[0]][a[1]][a[2]];
}