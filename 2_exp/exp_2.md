# 操作系统 实验一 进程管理
## 韩坤洋 16281038

## 1、打开一个vi进程。通过ps命令以及选择合适的参数，只显示名字为vi的进程。寻找vi进程的父进程，直到init进程为止。记录过程中所有进程的ID和父进程ID。将得到的进程树和由pstree命令的得到的进程树进行比较。

使用vi指令打开上一次实验的mem.c

![](Screenshot&#32;from&#32;2019-03-14&#32;08-24-29.png)

打开另一个终端，使用 ps -c 或者借助 grep 对结果进行筛选名字为 vi 的进程

![](Screenshot&#32;from&#32;2019-03-14&#32;08-27-02.png)

借助 ps 的 -f 参数，使用 grep 过滤得到目标 pid 对应的列数据。  
可以多次执行上述命令得到它所有父节点:vi 命令(22183)，zsh控制台(10013)，yakuake终端(1508)，然后是图形化界面的gnome-shell(1056)，gnome-session-binary(1015)，接着出现gdm有关的进程，pid分别为 1002，979,513，最后出现了init(1)。  

![](Screenshot&#32;from&#32;2019-03-14&#32;08-27-54.png)

![](Screenshot&#32;from&#32;2019-03-14&#32;08-28-23.png)

![](Screenshot&#32;from&#32;2019-03-14&#32;08-29-16.png)

![](Screenshot&#32;from&#32;2019-03-14&#32;08-38-28.png)

接着使用 pstree 查看对应的进程树，发现与通过 ps 命令查找到的进程号一致。

![](Screenshot&#32;from&#32;2019-03-14&#32;08-49-42.png)

## 2、编写程序，首先使用fork系统调用，创建子进程。在父进程中继续执行空循环操作；在子进程中调用exec打开vi编辑器。然后在另外一个终端中，通过ps –Al命令、ps aux或者top等命令，查看vi进程及其父进程的运行状态，理解每个参数所表达的意义。选择合适的命令参数，对所有进程按照cpu占用率排序。

对应程序代码：
``` c
# include <unistd.h>
# include <stdio.h>

int main (){
    pid_t fpid; //fpid表示fork函数返回的值  
    fpid = fork();

    if (fpid < 0)   
        printf("error in fork!");   
    else if (fpid == 0) {  
        int ret;
        ret = execl("/bin/vi", "vi", "/home/knavit/Documents/_University/Junoir/OS/bjtu_OS_16281038/2_exp/2.c");
        if (ret == -1) {
            perror("execl");
        }
    }  
    else {  
        for (int i = 0; ; i < 1) {

        }
    }
    
    return 0;
}
```
### ps -Al 命令
执行 man 指令得到 ps 的详细信息：  
&nbsp;&nbsp;&nbsp;&nbsp; -A     Select all processes.  Identical to -e  
&nbsp;&nbsp;&nbsp;&nbsp; -l     Long format.  The -y option is often useful with this.  
那么 -Al 则是选取所有进程以详细格式表示。利用 grep 筛选所需信息。得到了 b.out 及其子进程的相关信息。

![](Screenshot&#32;from&#32;2019-03-14&#32;09-20-07.png)

### ps axu 命令
同上得到 axu 指令的信息：  
&nbsp;&nbsp;&nbsp;&nbsp; To see every process on the system using BSD syntax:  
查看所有使用 BSD 参数的进程。同样使用 grep 进行筛选得到 b.out 相关的进程信息。  

![](Screenshot&#32;from&#32;2019-03-14&#32;09-25-38.png)

### top 命令

使用 top 命令之后得到如下状态，加入 -p 指令筛选对应线程之后只显示了一个空循环的父进程。猜测是由于目前 vi 没有占用cpu的操作所以没有被列出来。

![](Screenshot&#32;from&#32;2019-03-14&#32;09-32-29.png)

在 直接使用 top 命令之后，键入 P 会按照CPU的使用率进行降序排列。并且进程的数量不断浮动。

![](Screenshot&#32;from&#32;2019-03-14&#32;09-33-37.png)

## 3、使用fork系统调用，创建如下进程树，并使每个进程输出自己的ID和父进程的ID。观察进程的执行顺序和运行状态的变化。

编写程序
``` c
#include <unistd.h>
#include <stdio.h>

int myFork(int, int);

int main (int argc, char *argv[]){

    printf("I'm father, my pid : %d\r\n", getpid());
    myFork(1, 2);
    
    return 0;
}

int myFork(int counter, int step) {
    pid_t fpid = fork();

    if (fpid < 0) 
        printf("error in fork!");
    else if (fpid == 0) {
        printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
        if (counter > 0) {
            myFork(counter - 1, 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
        }
    }
    else {
        if (counter >= 0) {
            myFork(counter - step, step + 1);
        }
        for (int i = 0; ; i < 1) {
            sleep(5);
            if (step == 3) {
                printf("I'm father, my pid : %d\r\n", getpid());
            }
            else {
                printf("Father's pid : %d my pid : %d\r\n", getppid(), getpid());
            }
        }
    }
    return 0;
}
```

编译并运行，利用 pstree 命令得到进程树，可以看到符合题目要求的结构。

![](Screenshot&#32;from&#32;2019-03-14&#32;10-13-07.png)

![](Screenshot&#32;from&#32;2019-03-14&#32;10-14-23.png)

从第一张图片中输出的顺序中可以看出是先执行了父进程的代码，创建了 9556、9557，又执行了子进程的代码，创建了 9558、9559。  

## 4、修改上述进程树中的进程，使得所有进程都循环输出自己的ID和父进程的ID。然后终止p2进程(分别采用kill -9 、自己正常退出exit()、段错误退出)，观察p1、p3、p4、p5进程的运行状态和其他相关参数有何改变。

代码是在第三题基础上进行了更改，增加了从命令行读取参数的过程，然后编写了根据参数执行 exit、段错误、等待kill等操作的函数，并放在了合适的位置。

``` c
...
void suiside();
char type;

int main (int argc, char *argv[]){
    type = argv[1][0];

    ...
    
    return 0;
}

int myFork(int counter, int step) {
    ...

    if (fpid < 0) 
        ...
    else if (fpid == 0) {
        ...
    }
    else {
        ...
        for (int i = 0; ; i < 1) {
            ...
            else {
                // p2 位置
                suiside();
                ...
            }
        }
    }
    return 0;
}

void suiside() {
    int num[2] = {0, 0};
    switch (type) {
    case '1':
        printf("self exit\r\n");
        exit(0);
        break;
    case '2':
        printf("illegal address accessing\r\n");
        for (int i = 0; ; i < 10000) {
            num[0] = num[i];
        }
        printf("didn't stop\r\n");
        break;
    default:
        printf("others\r\n");
        break;
    }
}
```

### kill 终止程序