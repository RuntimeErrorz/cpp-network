#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
    printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());//父进程启动时打印内容
    int fd1_td1 = open("td1.txt", O_RDONLY);//以只读模式打开文件
    
    if(fd1_td1 == -1){//异常处理
        printf("Fail to open the file\n");
        return 1;
    }
    
    printf("[pid](%d) [ppid](%d): [fd1_td1](%d) is got\n", getpid(), getppid(), fd1_td1);//成功获得fd1_td1时打印
    int fd1_td2;
    int num;
    sscanf(argv[1], "%d", &num);//读取并发进程总数
    //printf("%d", num);
    pid_t pid;
    for(int i = 0; i < num - 1; i++){//派生子进程
        pid = fork();
        if(pid == -1){
            perror("Fail to fork\n");
            exit(1);
        }
        else if(pid == 0){
            break;
        }
    }
    if(pid > 0){//父进程
        char buf[6];//用于存储从文件中读取的数据，只读5个字符
        buf[5] = '\0';
        int len1 = read(fd1_td1, buf, 5);//len1没什么用，可以去掉
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", getpid(), getppid(), buf, fd1_td1);
        close(fd1_td1);
        fd1_td2 = open("td2.txt", O_RDONLY);
        printf("[pid](%d) [ppid](%d): [fd1_td2](%d) is got\n", getpid(), getppid(), fd1_td2);
        int len2 = read(fd1_td2, buf, 5);
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td2](%d)\n", getpid(), getppid(), buf, fd1_td2);
        close(fd1_td2);
        sleep(2);
        printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
        exit(0);
    }
    else if(pid == 0){//子进程
        printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());
        char buffer[6];//用于存储从文件中读取的数据，只读5个字符
        buffer[5] = '\0';
        read(fd1_td1, buffer, 5);
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd1_td1](%d)\n", getpid(), getppid(), buffer, fd1_td1);
        int fd2_td1 = open("td1.txt", O_RDONLY);
        printf("[pid](%d) [ppid](%d): [fd2_td1](%d) is got\n", getpid(), getppid(), fd2_td1);
        read(fd2_td1, buffer, 5);
        printf("[pid](%d) [ppid](%d): [read](%s) by [fd2_td1](%d)\n", getpid(), getppid(), buffer, fd2_td1);
        close(fd2_td1);
        close(fd1_td1);
        printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
        exit(0);
    }
    return 0;
}
