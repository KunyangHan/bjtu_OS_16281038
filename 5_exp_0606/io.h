#pragma once

//#define C 50
//#define H 2
//#define B 30
//#define SIZE 512

using namespace std;

struct BLOCK {
    char content[512];
};

class IO {
public:
    IO() {}
    ~IO() {}

    const static int C = 50;
    const static int H = 2;
    const static int B = 10;
    const static int SIZE = 512;

    BLOCK ldisk[C][H][B];

    void l2chb(int, int *);
    int chb2l(int, int, int);
    void readBlock(int, char*);
    void writeBlock(int, char*);
    void disk2File(char[]);
    void file2Disk(char[]);
    void setBitmap(int, char);
    int getFree();

    void initDisk();
    void initBitmap();
};