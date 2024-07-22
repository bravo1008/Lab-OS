#include "kernel/types.h"
#include "user.h"


void generate_nums();
void send_primes(int pd[], int infos[], int infoslen);
void check_pd(int pd[], int len);
void process(int pd[]);

int
main(int argc, char** argv){
    //声明管道
    int pd[2];  //pipe descriper
    //创建管道
    pipe(pd);
    //check_pd(pd, 2);
    int pid;

    //Child Process
    if((pid = fork()) == 0){
        process(pd);
        exit(1);
    }
    //Parent Process
    else{
        int nums[34];
        generate_nums(nums);
        send_primes(pd, nums, 34);
        sleep(10);
        exit(1);
    }
    
}
//主函数——过滤
void process(int pd[]){
    int p;
    int n;
    int len;
    int pid;
    int pd_child[2];
    int infos[34];
    int infos_i = 0;
    //create pip
    pipe(pd_child);
    //check_pd(pd_child, 2);
    
    close(pd[1]);
    len = read(pd[0], &p, sizeof(p));
    printf("prime %d\n", p);  
    
    while(len != 0) {
        len = read(pd[0], &n, sizeof(n));
        if(n % p != 0){  //判断是否是p的倍数
            *(infos + infos_i) = n;
            infos_i++;
        }
        
    }
    //读完之后将读端关闭
    close(pd[0]);
    //判断筛选完成
    if(infos_i == 0) {
        exit(1);
    }
    

    // Child Process——创建
    if((pid = fork()) == 0){
        process(pd_child);
    }
    // Parent Process
    else{
        send_primes(pd_child, infos, infos_i);  //将过滤的数组发送到下一个进程
    }
    exit(1);
}

void
generate_nums(int nums[34]){
    int i = 0;
    for(int count = 2; count <= 35; count++){
        nums[i] = count;
        i++;
    }
    
}

void 
check_pd(int pd[], int len){
    printf("pd:\n");
    for(int i = 0; i < len; i++){
        printf("%d \n", pd[i]);
    }
}
//关闭读端并写
void
send_primes(int pd[], int infos[], int infoslen){
    int info;
    close(pd[0]);
    for(int i = 0; i < infoslen; i++){
        info = infos[i];
        write(pd[1],&info,sizeof(info));
    }
}


