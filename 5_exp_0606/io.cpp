#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void IO::l2chb(int l, int *a) {
    int a[3] = {0};
    a[0] = l % B;
    a[0] = (l - B) % (H * B);
    a[1] = (l - a[0] * H * B) % B;
    a[2] = l - a[0] * H * B - a[1] * B;
}

int IO::chb2l(int c, int h, int b) {
    return b + b * h + b * h * c;
}

void IO::readBlock(int l, char *ch) {
    int a[3];
    l2chb(l, a);

    strcpy(ch, ldisk[a[0]][a[1]][a[2]].content);
}

void IO::writeBlock(int l, char *ch) {
    int a[3];
    l2chb(l, a);

    strcpy(ldisk[a[0]][a[1]][a[2]].content, ch);
}

void IO::disk2File(char name[]) {
    FILE *fp;
    fp = fopen(name, "wb");

    if (fp == NULL) {
        printf("File Open Fail");
        exit(1);
    }

    for (int i = 0; i < C; i++) {
        for (int j = 0; j < H; j++) {
            for (int k = 0; k < B; k++) {
                fwrite(ldisk[i][j][k].content, 512, 1, fp);
            }
        }
    }

    fclose(fp);
}

void IO::file2Disk(char name[]) {
    FILE *fp;
    fp = fopen(name, "rb");
    if (fp == NULL) {
        printf("File Open Fail");
        exit(1);
    }

    int ind = 0;
    while (!feof(fp)) {
        int a[3];
        l2chb(ind, a);
        BLOCK *tmp = &ldisk[a[0]][a[1]][a[2]];

        fread(tmp->content, 512, 1, fp);

        ind++;
    }
    fclose(fp);
}

void IO::setBitmap(int l, char ch) {
    int b = l / 512;
    int ind = l % 512;
    ldisk[0][0][b].content[ind] = ch;
}

int IO::getFree() {
    int result = -1;
    for (int i = 14; i < C * H * B; i++) {
        if (ldisk[0][0][i / 512].content[i % 512] == '0') {
            result = i;
            break;
        }
    }

    return result;
}

void IO::initDisk() {
    for (int i = 0; i < C; i++) {
        for (int j = 0; j < H; j++) {
            for (int k = 0; k < B; k++) {
                ldisk[i][j][k].setVar(i, j, k, chb2l(i, j, k));
            }
        }
    }
}

void IO::initBitmap() {
    setBitmap(0, '1');
    setBitmap(1, '1');
    for (int i = 0; i < C*H*B; i++) {
        setBitmap(i, '0');
    }
}

