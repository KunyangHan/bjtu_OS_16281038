#pragma once

#define BLOCK_NUM 31

struct Pageitem {
    int page;
    int exist;
    int block;
    int modi;
    void init();
};

struct Table {
    Pageitem item[BLOCK_NUM];
    Table();
    int findPage(int);
    void replacePage(int, int);
};

struct Log {
    int total;
    int fault;
    Log();
    double getFaultRate();
    void wrong();
    void correct();
};

struct Best {
    Table table;
    Log log;
    void run(int, int []);
};

struct FIFO {
    Table table;
    Log log;
    int timeLine[BLOCK_NUM];
    FIFO();
    void run(int, int []);
};

struct LRU {
    Table table;
    Log log;
    int timeLine[BLOCK_NUM];
    LRU();
    void run(int, int[]);
};

struct Clock {
    Table table;
    Log log;
    int ptr;
    int visit[BLOCK_NUM];
    Clock();
    void run(int, int[]);
    void next();
    int find();
    int findFir();
    int findSec();
};

struct PBA {

};