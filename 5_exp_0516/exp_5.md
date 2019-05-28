# 整体结构

每种置换算法写成一个结构体，每个置换算法的结构体都带有页表结构体变量和缺页率结构体变量，同时还带有 run 函数用于将访问序列传入进行测试。其余函数和变量根据置换算法的具体需要各不同。  

# 基础结构体

## 页表项

``` c
struct Pageitem {
    int page;
    int exist;
    int block;
    int modi;
    void init();
};
```

页表项用于记录块号与页号的对应关系，同时记录了该页表项是否有效以及该页号对应数据是否被修改过。是页表的基本组成部分。  
其初始化函数 init 用于对所有变量进行置0操作。

## 页表

``` c
struct Table {
    Pageitem item[BLOCK_NUM];
    Table();
    int findPage(int);
    void replacePage(int, int);
};
```

页表中包含 BLOCK_NUM 个页表项，BLOCK_NUM 的具体值根据题目中“页面大小为1K，模拟的物理内存有32K”可知一共有32个块，其中拿1块用来存放页表本身，所以共有31个可用块。因此定义 BLOCK_NUM 为31。

``` c
#define BLOCK_NUM 31
```

接下来是对于一个页表应该实现的操作——初始化、查找页号对应块号、替换页号。  

``` c
// 构造函数 Table 执行将全部页表项进行初始化的操作
Table::Table() {
    for (int i = 0; i < BLOCK_NUM; i++) {
        item[i].init();
    }
}

// 查找页号函数失败返回 -1， 成功返回对应块号
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

// 替换函数将某一块号设定为固定页号，同时修改存在位和修改位
void Table::replacePage(int block, int target) {
    item[block].page = target;
    item[block].exist = 1;
    item[block].modi = 0;
}
```

## 记录

``` c
struct Log {
    int total;
    int fault;
    Log();
    double getFaultRate();
    void wrong();
    void correct();
};
```
记录的结构体完全是为了记录缺页和访问成功次数、计算缺页率方便而定义的结构体。拥有记录缺页次数和访问总次数两个变量，然后是计算缺页率、缺页、访问成功、构造函数四个函数。

``` c
// 构造函数，初始化变量
Log::Log() {
    fault = 0;
    total = 0;
}

// 计算并返回缺页率
double Log::getFaultRate() {
    return (double)fault / (double)total;
}

// 缺页时调用 进行记录
void Log::wrong() {
    fault++;
    total++;
}

// 访问成功时调用 进行记录
void Log::correct() {
    total++;
}
```

# 页面置换结构体
## 最佳置换

``` c
struct Best {
    Table table;
    Log log;
    void run(int, int []);
};
```
由于最佳置换算法计算出需要置换的块号是要依赖于访问序列的，而访问序列存在于 run 函数中，如果作为参数进行传递将占用大量内存资源，因此没有单独设立寻找函数。  

**run 函数是所有置换结构体中都包含的一个函数，其实现思路也是一样的所以只在这里进行介绍。**run 函数的基本思路是 for 循环遍历整个访问序列。

``` c
for (int i = 0; i < len; i++) {

}
```
然后对于每一个当前访问的页号，都在页表 table 中寻找是否存在该页号数据。存在则可以直接访问，记录为成功访问。然后继续循环。

``` c
if (table.findPage(visit[i]) != -1) {
    log.correct();
    continue;
}
```
与之对应的情况就是每种置换算法不同的地方，但是功能是一样的——使用相应的置换算法找到合适和块号进行替换。然后记录为缺页。

``` c
table.replacePage(block, visit[i]);
log.wrong();
```

对于最佳置换算法，我采用的是遍历所有的页表项，如果找到了无效的页表项就跳出循环直接进行置换，如果有效就从当前访问序列位置开始向后直到找到页号或者到达最后，记录此时的下标。最后取所有下标最大的一个，作为被置换的项目。

``` c
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
```

## 先进先出

``` c
struct FIFO {
    Table table;
    Log log;
    int timeLine[BLOCK_NUM];
    FIFO();
    void run(int, int []);
};
```
与最佳置换的区别在于， FIFO 中我申请了数组 timeLine 来记录该块号被记录时的序列值，值越小代表记录的越早。  
相应的替换算法中也是遍历所有的页表项，如果找到了无效的页表项就跳出循环直接进行置换。但是如果是有效的就记录它在timeLine 中的值，对所有的值取最小，代表是最先记录的项目，进行置换。

``` c
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

timeLine[block] = i;
```

## 最近最久未使用

``` c
struct LRU {
    Table table;
    Log log;
    int timeLine[BLOCK_NUM];
    LRU();
    void run(int, int[]);
};
```

这个算法和上一个算法 FIFO 是很像的，都是使用 timeLine 记录时间，然后根据这个时间的值决定替换哪一个块。只不过在这个算法中 timeLine 会在被访问的时候更新。即需要在访问成功时再次为 timeLine 某一项赋值。

``` c
if (index != -1) {
    timeLine[index] = i;
    log.correct();
    continue;
}
```
至于置换的过程，和 FIFO 是一样的。

``` c
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

timeLine[block] = i;
```
## 改进 Clock

``` c
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
```
整个改进 Clock 算法的成员就很多了，下面一一介绍。首先是记录最近是否被访问过的数组 visit，因为访问数组时要求记录指针位置所以有进行访问的指针 ptr，ptr 的运行是向某一方向增加越界之后返回另一端的，所以定义 next 函数进行模 BLOCK_NUM 的加 1 操作。  

``` c
void Clock::next() {
    ptr = (ptr + 1) % BLOCK_NUM;
}
```

然后是 Clock 的查找过程，查找分为了 1 过程和 2 过程。

``` c
int Clock::findFir() {
    int block = -1;
    for (int i = 0; i < BLOCK_NUM; i++) {
        // 找到第一个最近未访问 也未被修改的
        if (table.item[ptr].modi == 0 && visit[ptr] == 0) {
            block = ptr;
            next();
            break;
        }
    }
    return block;
}

int Clock::findSec() {
    int block = -1;
    for (int i = 0; i < BLOCK_NUM; i++) {
        // 找到第一个最近未访问 有被修改的
        if (table.item[ptr].modi == 1 && visit[ptr] == 0) {
            block = ptr;
            next();
            break;
        }
        // 改为最近未被访问
        visit[ptr] = 0;
    }
    return block;
}
```

由于两个搜索过程之间并不是单纯的顺序调用，因此又定义了总的搜索函数。

``` c
int Clock::find() {
    // 最近未访问 未被修改的
    int block = findFir();
    if (block != -1) {
        goto endp;
    }
    // 最近未访问 被修改的
    block = findSec();
    if (block != -1) {
        goto endp;
    }
    // 最近访问 未被修改的
    block = findFir();
    if (block != -1) {
        goto endp;
    }
    // 最近访问 被修改的
    block = findSec();
    if (block != -1) {
        goto endp;
    }

endp:
    return block;
}
```

至于 run 函数中的替换过程就简单了，直接调用 find 方法就可以得到被替换的块号。只不过需要加一步随机决定这次访问该块号是否被修改。

``` c
table.item[index].modi = ((v[i] + i) % 3 == 0 ? 1 : 0);
```
## PBA

``` c

```

``` c

```