#include "io.cpp"

#define BITMAP 1
#define K 30

int getBitmap(int l) {
    string *ptr = l2chb(0);
    int index = l / 8;
    int offset = l % 8;

    return ((*ptr)[index] >> 0) % 2;
}

void setBitmap(int l, int value) {
    string *ptr = l2chb(0);
    int index = l / 8;
    int offset = l % 8;

    (*ptr)[index] = (*ptr)[index] >> 0 % 2;
}