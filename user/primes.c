#include "kernel/types.h"
#include "user.h"

void generate_nums(int nums[]);
void send_primes(int pd[], int infos[], int infoslen);
void process(int pd[]);

int
main(int argc, char **argv) {
   
    int pd[2];  // pipe descriptor

    pipe(pd);

    int pid;

    // Child Process
    if ((pid = fork()) == 0) {
        close(pd[1]);  
        process(pd);
        exit(0);
    } 
    // Parent Process
    else {
        int nums[34];
        generate_nums(nums);
        close(pd[0]);  
        send_primes(pd, nums, 34);
        close(pd[1]);  
        wait(0);       
        exit(0);
    }
}


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
        if (n % p != 0) {  
            infos[infos_i++] = n;
        }
    }

    close(pd[0]);
    
   
    if (infos_i == 0) {
        close(pd_child[0]);
        close(pd_child[1]);
        exit(0);
    }

    // Child Process
    if ((pid = fork()) == 0) {
        close(pd_child[1]);  
        process(pd_child);
        exit(0);
    } 
    // Parent Process
    else {
        close(pd_child[0]);  
        send_primes(pd_child, infos, infos_i);  
        close(pd_child[1]);  
        wait(0);            
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


void
send_primes(int pd[], int infos[], int infoslen) {
    for (int i = 0; i < infoslen; i++) {
        write(pd[1], &infos[i], sizeof(infos[i]));
    }
}