# 操作系统 实验一 进程管理
## 韩坤洋 16281038

## 1）通过fork的方式，产生4个进程P1,P2,P3,P4，每个进程打印输出自己的名字，例如P1输出“I am the process P1”。要求P1最先执行，P2、P3互斥执行，P4最后执行。通过多次测试验证实现是否正确。

这里4个线程执行的任务各不相同，并且对于信号量的等待也不相同所以共写出4个函数，分别让p1, p2, p3, p4执行。信号量关系如下图所示：

![](Untitled&#32;Diagram_exp3.png)

然后分别给出每个进程具体的执行过程：
``` c
// 初始化各个信号量的值
sem_t *sem23 = sem_open("sem23", O_CREAT, 0666, 0);
sem_t *sem24 = sem_open("sem24", O_CREAT, 0666, 0);
sem_t *sem34 = sem_open("sem34", O_CREAT, 0666, 0);

void worker1(void *arg) {
    printf("I am the Process 1\n");

    sem_post(sem23);        // P2 或 P3 可以执行
}
void worker2(void *arg) {        
    sem_wait(sem23);        // 等待 P1，与 P3 互斥
    printf("I am the Process 2\n");
    sem_post(sem23);        // P3 可以执行

    sem_post(sem24);        // P4 可以执行(必要不充分条件)
}
void worker3(void *arg) {        
    sem_wait(sem23);        // 等待 P1，与 P2 互斥
    printf("I am the Process 3\n");
    sem_post(sem23);        // P2 可以执行

    sem_post(sem34);        // P4 可以执行(必要不充分条件)
}
void worker4(void *arg) {        
    sem_wait(sem34);        // 等待 P3
    sem_wait(sem24);        // 等待 P2
    printf("I am the Process 4\n");
}
```
编译运行的结果没有问题，P1 总是第一个执行，P4 总是最后一个执行，P2 和 P3 在中间执行两者的相对顺序不固定。以下为部分执行次数的结果截图：

![](Screenshot&#32;from&#32;2019-03-30&#32;11-38-02.png)


## 2）火车票余票数ticketCount 初始值为1000，有一个售票线程，一个退票线程，各循环执行多次。添加同步机制，使得结果始终正确。要求多次测试添加同步机制前后的实验效果。

为了验证两个线程多次执行之后的结果是否是正确的，首先取消了两个线程任务的无条件循环，并分别设置了各自的执行次数（这里设定的是初始 1000 张票，共有 800 次购票和 400 次退票），所以如果程序执行无误最终票数应该为 600 张。  
首先是没有同步机制的情况，这里直接使用的老师给的实验报告中的卖票处理逻辑和退票处理逻辑，所以不再贴出源码直接给出结果截图：

![](Screenshot&#32;from&#32;2019-03-30&#32;14-39-26.png)

可以看到在截图的七次中，剩余的票数都是大于 600 的，最接近正确的一次也多出了 8 张。  
然后开始加入同步机制，这里因为是卖票和退票两个方式，两者共同影响了剩余的总票数这一个变量，并且是一个使增加 1 一个使减少 1。所以最先想到的是一个“消费者——生产者”模式，但是这个模式有一个问题：剩余的票数实际上变成了信号量的形式来控制两个进程，这样的话就没有一个整性变量去表示剩余票数了。是不符合买票程序的逻辑的，如果一个买票系统连剩余票数有多少都不能直接显示。  
所以这里直接使用了进程锁，在两个线程各自的临界区进行上锁。

``` c
pthread_mutex_t lock;

void *sell(void *arg) {
    int sellTime = 800;                 // 卖票 800 次
    while (sellTime > 0) {
        pthread_mutex_lock(&lock);      // 上锁 / 等待解锁

        int temp = ticketCount;
        pthread_yield();
        temp = temp - 1;
        pthread_yield();
        ticketCount = temp;

        pthread_mutex_unlock(&lock);    // 解锁

        sellTime -= 1;
    }
    return NULL;
}
void *buy(void *arg) {
    int buyTime = 400;                  // 退票 400 次
    while (buyTime > 0) {   
        pthread_mutex_lock(&lock);      // 上锁 / 等待解锁

        int temp = ticketCount;     
        pthread_yield();
        temp = temp + 1;
        pthread_yield();
        ticketCount = temp;             

        pthread_mutex_unlock(&lock);    // 解锁

        buyTime -= 1;
    }
    return NULL;
}
```
在加了锁之后再次运行程序，得到如下结果：

![](Screenshot&#32;from&#32;2019-03-30&#32;14-39-51.png)

可以看到每次执行都得到了 600 这一结果，程序执行正确，完成了同步。

## 3）一个生产者一个消费者线程同步。设置一个线程共享的缓冲区， char buf[10]。一个线程不断从键盘输入字符到buf,一个线程不断的把buf的内容输出到显示器。要求输出的和输入的字符和顺序完全一致。（在输出线程中，每次输出睡眠一秒钟，然后以不同的速度输入测试输出是否正确）。要求多次测试添加同步机制前后的实验效果。

从题干中也可知，这是一个典型的“生产者——消费者”问题，所以这里就申请两个个信号量来记录已经有的资源数量和还可以盛放资源数量。具体操作见代码及注释：

``` c
sem_t *resource = sem_open("bufRemained", O_CREAT, 0666, 10); 
sem_t *character = sem_open("bufUsed", O_CREAT, 0666, 0); 
char buf[10] = {'\0'};

void *reader(void *arg){
    int p = 0;
    char temp[50];
    while (1) {
        scanf("%s", temp);          // 接受一行输入到本地缓存
        for (int i = 0; temp[i] != '\0'; i++) {
            sem_wait(resource);     // 有空间资源时 减一并进入
            
            buf[p] = temp[i];       // 逐个写入公共缓存 buf 中
            p = (p + 1) % 10;

            sem_post(character);    // 增加字符资源
        }
    }
}
void *writer(void *arg){
    int p = 0;
    while(1) {
        sem_wait(character);        // 有字符资源时 减一并进入

        printf("(%c)\n", buf[p]);   // 输出单个字符到控制台
        p = (p + 1) % 10;

        sem_post(resource);         // 增加空间资源
        sleep(1);
    }
}
```
为了测试同步是否正确，应当从两个方面进行测试：  
1. 在输入速度大于输出速度时，buf 被填满了之后键盘上已输入字符是否会丢失，或者是否会覆盖掉未输出字符。
2. 在输入速度小于输出速度时，buf 空了之后是否还会输出已输出字符或错误字符。  

所以这里采取了一次性输入 17 个字符，测试是否满足（1），然后不再输入任何字符，测试是否满足(2)。

![](Screenshot&#32;from&#32;2019-03-30&#32;15-22-49.png)

结果可见，在接受了 17 个字符之后程序逐个输出（使用小括号扩了起来便于区分）了全部的字符，并且在输出完了之后就停止了输出。工作正常。

## 4) 进程通信问题。阅读并运行共享内存、管道、消息队列三种机制的代码

### a）通过实验测试，验证共享内存的代码中，receiver能否正确读出sender发送的字符串？如果把其中互斥的代码删除，观察实验结果有何不同？如果在发送和接收进程中打印输出共享内存地址，他们是否相同，为什么？

首先直接编译并在两个 terminal 中运行 sender 和 receiver，receiver 能够正确的读出 sender 所发送的字符串，并且字符串以空格回车制表符中断。

![](Screenshot&#32;from&#32;2019-04-08&#32;10-19-52.png)

可以看到接受程序以正确的顺序输出了“ein swei dier”，执行正确。

在这一对程序中，用于控制执行同步的关键代码，以及其执行逻辑为下：
``` c
/* sender */

struct sembuf sem_b;
sem_b.sem_op = 1;              // 每次对 sem_val 操作时，加 1

while(1) {
    if(0 == (value = semctl(sem_id, 0, GETVAL))) {  // 读取 sem_val，为 0 进入

        // 临界区

        if(-1 == semop(sem_id, &sem_b, 1)) {        // 更改 sem_val (+1)
        }
    }
}

/* reciver */

struct sembuf sem_b;
sem_b.sem_op = -1;             // 每次对 sem_val 操作时，减一 1（有限制）

while(1) {
    if(1 == (value = semctl(sem_id, 0, GETVAL))) {  // 读取 sem_val，为 1 进入
        
        // 临界区

        if(-1 == semop(sem_id, &sem_b, 1)) {        // 更改 sem_val (-1)
            perror("semop");
            exit(EXIT_FAILURE);
        }
    }
}
```
可以看到这里对于发送程序和接受程序的同步还是采用了信号量的机制，但是由于是两个程序之间的通信就不能够像之前在多线程中使用公共信号量直接控制，而是用了 shmat 将共享内存空间挂载到进程中。最关键的操作分别有两个：
1.  通过 semctl 读取 sem 的值，然后决定是否进入每个程序自己的临界区。因此保证了两个程序不会同时进入临界区且 sender 进入自己临界区的时候 semval 一定为 0；reciver进入自己临界区的时候 semval 一定为 1。
2.  通过 semop 更改 sem 的值，来控制另一个程序是否可以进入临界区。这个操作在 sender 中由于 sem_op 为正数 1，且 sem_flag 为 SEM_UNDO，所以会将对应的第 0 个 semval 进行加 1 操作，对 semadj 进行减 1 操作，所以这时 sender 中的 semval 最终一定为 1。而在 reciver 中 sem_op 为负数 1，且 sem_flag 为 SEM_UNDO，所以会将对应的第 0 个 semval 进行减 1 操作，对 semadj 进行加 1 操作，注释中有说操作有限制是指“上述操作产生负数结果时发生错误”，而 reciver 中 semval 初始为 1，会立即执行操作变成 0。

那么直接删除进入的限制 semctl 之后，运行程序：

![](Screenshot&#32;from&#32;2019-04-08&#32;10-28-48.png)

可以看到明显的是由于 sender 发送（覆写共享内存内容）过快而导致 reciver 没有得到前面的两个单词信息。  
最后添加代码，使两个程序输出共享变量的地址：
``` c
printf("\tAddress of shm_ptr is %lld\n", &shm_ptr);
```
![](Screenshot&#32;from&#32;2019-04-08&#32;10-35-35.png)
   
发现两个程序的共享变量对应地址并不一样，我认为这是由于使用共享变量时其实是将一个本地（该程序）内存映射到了一个公共内存区域，因此打印该变量的地址时，实际打印的是一个本地地址的变量。

### b)有名管道和无名管道通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？

以下为编译后直接运行截图，其中选中（反色）部分为先执行的语句：

![](Screenshot&#32;from&#32;2019-04-08&#32;10-49-27.png)

由上图知，管道工作正常。打乱顺序之后再执行：

![](Screenshot&#32;from&#32;2019-04-08&#32;10-50-27.png)

发现程序发生了卡死，使用 ^c 强制退出。然后 rm /tmp/my_fifo，再次按照接受、发送顺序执行程序。

![](Screenshot&#32;from&#32;2019-04-08&#32;10-51-27.png)

发现不再是两个程序卡死，而是接受程序直接报错退出。  
所以基本得出了结论，管道程序有对于同步的控制机制。进行第 2、3 张图操作的本意是测试如果开一个读进程读取上一次的管道文件，然后再开一个新的写程序覆盖掉原有的管道文件会发生什么。结果是读的程序没有等到在这个管道中写的程序，对于写程序也是相同状况。就导致两个程序同时阻塞。  
控制主要依赖于管道实现机制本身，管道的通信要求发送方和接收方同时存在，然后使用同一个管道通信，并且对于只存在发送方和只存在接收方这两种情况 FIFO 本身的处理都是发生阻塞。感觉就是一种强制阻塞等待对应的通讯程序的机制来实现同步。  

### c）消息通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？

![](Screenshot&#32;from&#32;2019-04-08&#32;11-06-01.png)

本身也有了同步，这个主要是依靠队列这一数据结构实现的，针对两种易发生问题的情况分别讨论:
1.  发送速度快于接受速度时，如果没有接收方接受信息那么信息会储存在消息队列尾部。当消息队列满了的时候，写进程再继续调用 msgsnd 方法就会出错，此时 errno 值为 EINTR。
2.  接受方速度过快时，调用 msgrcv 方法时会检测该信道中是否有对应的消息，如果没有则会发生等待。

也是通过等待实现了两者的同步机制。

## 5）阅读Pintos操作系统，找到并阅读进程上下文切换的代码，说明实现的保存和恢复的上下文内容以及进程切换的工作流程。

首先从资源树中确定到实现这一功能的文件——“pintos/src/threads/switch.S”，这是一个用汇编实现的线程切换保护过程。  
整个过程和使用汇编使用函数、中断时处理方式类似，都是通过 push 进入栈中，在需要的时候再按照一定顺序 pop 出来。但是这里不同的是：在使用 push 保存了当前进程的寄存器信息之后，就要立刻 pop 来恢复下一个进程的寄存器信息，所以如果是同一个栈顶是不能实现的。
这里采用的是在将寄存器 push 进入栈顶 (%esp)
``` S
pushl %ebx
pushl %ebp
pushl %esi
pushl %edi
```
接下来为了防止之前说的问题出现,选择把当前进程的栈指针 esp 储存在 thread 结构体中的 stack 中。  
``` c
/* thread.h */
struct thread
  {
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */

    /* etc. */
  }
```
但是有一个问题--在汇编中无法直接调用 cur->stack 得到当前进程的 stack 变量进行赋值操作，所以在 c 中事前先获得相对于 struct  的 stack 内存地址偏移量。
``` c
uint32_t thread_stack_ofs = offsetof (struct thread, stack);
```
在汇编中调用该变量，得到 stack 的相对偏移量并放在 %edx 中。
``` S
.globl thread_stack_ofs
	mov thread_stack_ofs, %edx
```
但是只有 stack 相对于结构体的偏移量不够，还需要知道当前进程在整个内存中的偏移量才能确定 stack 的位置，从而进行赋值操作。而实现进程上下文切换的这段函数本身就接受了**当前进程的指针**和**要切换的进程指针**，由 c 的函数调用机制可以知道这些都被压入了栈中。
``` c
struct thread *switch_threads (struct thread *cur, struct thread *next);
```
为了更清晰的了解目前栈的内容，找到目标地址 cur，先画一个表格。
|     |     |
| --- | --- |
| 地址 | 内容 |
| esp + 24 | next |
| esp + 20 | cur |
| esp + 16 | return |
| esp + 12 | ebx |
| esp + 8 | ebp |
| esp + 4 | esi |
| esp + 0 | edi |

很明显为了得到 cur 的值，只需要访问 esp + 20 就可以了。由于在 switch.h 中已经定义了 SWITCH_CUR:
``` c
#define SWITCH_CUR      20
#define SWITCH_NEXT     24
```
所以汇编中直接使用该宏标识，将该进程 thread 的地址存入 %eax 中。这样只要将 %eax 和 %edx 相加就可以表示在整个内存中的 stack 的地址了，以及提一下原文中使用的 (%eax, %edx, 1)就是做 %eax + %edx * 1 操作。
``` S
movl SWITCH_CUR(%esp), %eax
movl %esp, (%eax,%edx,1)
```
这个是整个现进程的保存，而对下一进程的恢复就是一个逆过程，只不过栈偏移量为 24，使用标识符 SWITCH_NEXT，得到下一进程的 stack 地址，恢复 %esp 然后 pop 恢复其寄存器的值。
``` S
movl SWITCH_NEXT(%esp), %ecx
movl (%ecx,%edx,1), %esp

popl %edi
popl %esi
popl %ebp
popl %ebx
```

## 遇到的问题

遇到的问题主要还是出现在了新加的第 4 题中，当然之前的第 4 题中问题更多。比如第 2 小问出现了意料之外的情况，就是先运行接受程序再运行发送程序时双方同时阻塞这种情况，按照对代码的理解应该是接收方报错“FIFO %s does not existed”然后退出才对。后来发现是因为之前执行程序时遗留了 FIFO 文件导致的。  
还有就是第 3 小问产生了一个疑惑，为什么两个程序都有发消息还能准确接受，之后查看 msgsnd 和 msgrcv 定义时发现两个都接受参数 msgbuf.mtype，决定了发送和接受的消息类。  
还有第 5 题对于寄存器内容的保存和恢复，本身程序做了什么是很好理解的——需要保存寄存器值，需要保存栈指针，以及恢复。但是具体的实现就比较难以理解，比如本身 Intel 汇编就和之前学的有些不同，同时还要利用到栈来找到 cur，也是想了好久怎么实现的。