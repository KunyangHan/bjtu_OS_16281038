#include <stdio.h>
#include <unistd.h>

int main(){
    pid_t pid;
    
    asm volatile(
        "mov $0, %%ebx;"
        "mov $0x14, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        :"=m"(pid)
    );
    printf("%d\n", pid);

    return 0;
}