#include "random.h"
#include "replace.h"

int main() {
    int *a = getRandom();
    Best b;
    FIFO f;
    LRU l;
    Clock c;
    PBA p;

    b.run(100000, a);
    f.run(100000, a);
    l.run(100000, a);
    c.run(100000, a);
    p.run(100000, a);

    return 0;
}