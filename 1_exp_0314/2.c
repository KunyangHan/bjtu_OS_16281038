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