typdef struct pageitem {
    int page;
    int exist;
    int block;
} PAGEITEM;

struct table {
    PAGEITEM item[31];
    int findPage(int);
    int replacePage(int, int);
}

int table::findPage(int page) {
    int block = -1;
    for (int i = 0; i < 31; i++) {
        if (item[i].exist == 1 && item[i].block == page) {
            block = i;
            break;
        }
    }
    return block;
}

int table::replacePage(int origin, int target) {
    int success = 1;
    int index = findPage(origin);
    if (index == -1) {
        success = 0;
    }
    else {
        item[index].page = target;
    }
    return success;
}


