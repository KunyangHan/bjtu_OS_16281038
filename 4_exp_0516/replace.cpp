#include "replace.h"
#include <stdio.h>

void Pageitem::init() {
    page = 0;
    exist = 0;
    block = 0;
    modi = 0;
}

Table::Table() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        item[i].init();
    }
}

int Table::findPage(int page) {
    int block = -1;
    for (int i = 0; i < 31; i++) {
        if (item[i].exist == 1 && item[i].page == page) {
            block = i;
            break;
        }
    }
    return block;
}

void Table::replacePage(int block, int target) {
    item[block].page = target;
    item[block].exist = 1;
    item[block].modi = 0;
}

Log::Log() {
    fault = 0;
    total = 0;
}

double Log::getFaultRate() {
    return (double)fault / (double)total;
}

void Log::wrong() {
    fault++;
    total++;
}

void Log::correct() {
    total++;
}

void Best::run(int len, int visit[]) {
    for (int i = 0; i < len; i++) {
        if (table.findPage(visit[i]) != -1) {
            log.correct();
            continue;
        }
        int max = 0;
        int block = 0;

        for (int j = 0; j < BLOCK_NUM; j++) {
            if (table.item[j].exist == 0) {
                block = j;
                break;
            }
            int temp = i;
            while (temp < len) {
                if (table.item[j].page == visit[temp]) {
                    break;
                }
                temp++;
            }
            if (temp > max) {
                max = temp;
                block = j;
            }
        }

        table.replacePage(block, visit[i]);
        log.wrong();
    }

    printf("Best: %lf\n", log.getFaultRate());
}

FIFO::FIFO() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        timeLine[i] = 0;
    }
}

void FIFO::run(int len, int visit[]) {
    for (int i = 0; i < len; i++) {
        if (table.findPage(visit[i]) != -1) {
            log.correct();
            continue;
        }
        int min = 0;
        int block = 0;

        for (int j = 0; j < BLOCK_NUM; j++) {
            if (table.item[j].exist == 0) {
                block = j;
                break;
            }
            if (timeLine[j] < min) {
                min = timeLine[j];
                block = j;
            }
        }

        table.replacePage(block, visit[i]);
        timeLine[block] = i;
        log.wrong();
    }

    printf("FIFO: %lf\n", log.getFaultRate());
}

LRU::LRU() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        timeLine[i] = 0;
    }
}

void LRU::run(int len, int visit[]) {
    for (int i = 0; i < len; i++) {
        int index = table.findPage(visit[i]);
        if (index != -1) {
            timeLine[index] = i;
            log.correct();
            continue;
        }
        int min = 0;
        int block = 0;

        for (int j = 0; j < BLOCK_NUM; j++) {
            if (table.item[j].exist == 0) {
                block = j;
                break;
            }
            if (timeLine[j] < min) {
                min = timeLine[j];
                block = j;
            }
        }

        table.replacePage(block, visit[i]);
        timeLine[block] = i;
        log.wrong();
    }

    printf("LRU : %lf\n", log.getFaultRate());
}

Clock::Clock() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        visit[i] = 0;
    }
    ptr = 0;
}

void Clock::run(int len, int v[]) {
    for (int i = 0; i < len; i++) {
        if (table.findPage(v[i]) != -1) {
            log.correct();
            continue;
        }
        int block = find();
        visit[i] = 1;
        table.replacePage(block, v[i]);
        table.item[i].modi = (v[i] % 3 == 0 ? 1 : 0);
        log.wrong();
    }

    printf("LRU : %lf\n", log.getFaultRate());
}

void Clock::next() {
    ptr = (ptr + 1) % BLOCK_NUM;
}

int Clock::find() {
    int block = findFir();
    if (block != -1) {
        goto endp;
    }
    block = findSec();
    if (block != -1) {
        goto endp;
    }
    block = findFir();
    if (block != -1) {
        goto endp;
    }
    block = findSec();
    if (block != -1) {
        goto endp;
    }

endp:
    return block;
}

int Clock::findFir() {
    int block = -1;
    for (int i = 0; i < BLOCK_NUM; next()) {
        if (table.item[ptr].modi == 0 && visit[ptr] == 0) {
            block = ptr;
            break;
        }
    }
    return block;
}

int Clock::findSec() {
    int block = -1;
    for (int i = 0; i < BLOCK_NUM; next()) {
        if (table.item[ptr].modi == 1 && visit[ptr] == 0) {
            block = ptr;
            break;
        }
        visit[ptr] = 0;
    }
    return block;
}

PBA::PBA() {
    for (int i = 0; i < CHAIN_LEN; i++) {
        visited[i] = 0;
    }
    for (int i = 0; i < TABLE_LEN; i++) {
        timeLine[i] = 0;
    }
}

void PBA::run(int len, int v[]) {
    for (int i = 0; i < len; i++) {
        int index = table.findPage(v[i]);
        if (index != -1 && index < TABLE_LEN) {
            log.correct();
            continue;
        }
        else if (findInChain(v[i]) != -1) {
            log.correct();
            continue;
        }
        int block = find();

        addToChain(block);
        table.replacePage(block, v[i]);
        timeLine[block] = i;
        table.item[i].modi = (v[i] % 3 == 0 ? 1 : 0);
        log.wrong();
    }

    printf("LRU : %lf\n", log.getFaultRate());
}

int PBA::find() {
    int min = 0;
    int block = 0;

    for (int j = 0; j < BLOCK_NUM; j++) {
        if (table.item[j].exist == 0) {
            block = j;
            break;
        }
        if (timeLine[j] < min) {
            min = timeLine[j];
            block = j;
        }
    }

    return block;
}

int PBA::findInChain(int page) {
    int index = -1;
    for (int i = 0; i < CHAIN_LEN; i++) {
        if (visited[i] == page) {
            int block = find();
            deleteChain(i);
            addToChain(table.item[block].page);
            table.replacePage(block, page);

            index = i;
            break;
        }
    }
    return index;
}

void PBA::addToChain(int page) {
    int full = 1;
    for (int i = 0; i < CHAIN_LEN; i++) {
        if (visited[i] == -1) {
            visited[i] = -1;
            full = 0;
        }
    }
    if (full == 1) {
        for (int i = 0; i < CHAIN_LEN; i++) {
            visited[i] = 0;
        }
        visited[0] = page;
    }
}

void PBA::deleteChain(int i) {
    visited[i] = 0;
}
