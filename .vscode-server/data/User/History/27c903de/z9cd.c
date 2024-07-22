#include "kernel/types.h"
#include "user.h"

void generate_nums(int nums[]);
void send_primes(int pd[], int infos[], int infoslen);
void process(int pd[]);

int
main(int argc, char **argv) {
    // 声明管道
    int pd[2];  // pipe descriptor
    // 创建管道
    pipe(pd);

    int pid;

    // Child Process
    if ((pid = fork()) == 0) {
        close(pd[1]);  // 子进程关闭写端
        process(pd);
        exit(0);
    } 
    // Parent Process
    else {
        int nums[34];
        generate_nums(nums);
        close(pd[0]);  // 父进程关闭读端
        send_primes(pd, nums, 34);
        close(pd[1]);  // 发送完数据后关闭写端
        wait(0);       // 等待子进程完成
        exit(0);
    }
}

// 主函数——过滤
void process(int pd[]) {
    int p;
    int n;
    int len;
    int pid;
    int pd_child[2];
    int infos[34];
    int infos_i = 0;
    // create pipe
    pipe(pd_child);
    
    len = read(pd[0], &p, sizeof(p));
    if (len == 0) {
        close(pd[0]);
        exit(0);
    }
    printf("prime %d\n", p);  
    
    while ((len = read(pd[0], &n, sizeof(n))) > 0) {
        if (n % p != 0) {  // 判断是否是p的倍数
            infos[infos_i++] = n;
        }
    }
    // 读完之后将读端关闭
    close(pd[0]);
    
    // 判断筛选完成
    if (infos_i == 0) {
        close(pd_child[0]);
        close(pd_child[1]);
        exit(0);
    }

    // Child Process——创建
    if ((pid = fork()) == 0) {
        close(pd_child[1]);  // 子进程关闭写端
        process(pd_child);
        exit(0);
    } 
    // Parent Process
    else {
        close(pd_child[0]);  // 父进程关闭读端
        send_primes(pd_child, infos, infos_i);  // 将过滤的数组发送到下一个进程
        close(pd_child[1]);  // 发送完数据后关闭写端
        wait(0);             // 等待子进程完成
        exit(0);
    }
}

void
generate_nums(int nums[]) {
    int i = 0;
    for (int count = 2; count <= 35; count++) {
        nums[i++] = count;
    }
}

// 关闭读端并写
void
send_primes(int pd[], int infos[], int infoslen) {
    for (int i = 0; i < infoslen; i++) {
        write(pd[1], &infos[i], sizeof(infos[i]));
    }
}
