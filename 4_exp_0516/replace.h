#pragma once

#define BLOCK_NUM 31
#define CHAIN_LEN 4
#define TABLE_LEN BLOCK_NUM - CHAIN_LEN

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
    Table table;
    Log log;
    int timeLine[TABLE_LEN];
    int visited[CHAIN_LEN];
    PBA();
    void run(int, int[]);
    int find();
    int findInChain(int);
    void addToChain(int);
    void deleteChain(int);
};