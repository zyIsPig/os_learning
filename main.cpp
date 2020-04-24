#include "bits/stdc++.h"
#include <unistd.h>

using namespace std;

#define PROCESS_NAME_LEN 32 //进程名最大长度
#define MIN_SLICE 10 //内碎片最大大小
#define DEFAULT_MEM_SIZE 1024  //总内存大小
#define DEFAULT_MEM_START 0  //内存开始分配时的起始地址

typedef pair<int, string> My_algo;

int mem_size = DEFAULT_MEM_SIZE;
bool flag = 0; //当内存以及被分配了之后，不允许更改总内存大小的flag
static int pid = 2;
My_algo algo[123];
int alg=1;
struct free_block{	//空闲数据块
    int size;
    int start_addr;
    struct free_block *next;
    struct free_block *last;

    friend bool operator >(const struct free_block &block1, const struct free_block &block2);
};

inline bool operator > (const struct free_block &block1, const struct free_block &block2){
    return block1.size > block2.size;
}

struct allocated_block{ //已分配的数据块
    int pid;
    int size;
    int start_addr;
    int *data;
    struct allocated_block *next;
};

priority_queue<free_block,vector<free_block>,greater<free_block>> pq;
int total_free_size=mem_size;
free_block *free_block_head; //空闲数据块首指针
free_block *free_block_tail;//空闲数据快尾指针
free_block *logical_free_block_tail;
allocated_block *allocated_block_head = NULL; //分配块首指针
allocated_block *allocated_block_tail=NULL;
allocated_block *logical_all_block_tail=NULL;

allocated_block *find_process(int id); //寻找pid为id的分配块
free_block *init_free_block(int mem_size); //空闲块初始化
void display_menu(); //显示选项菜单
void set_mem_size(); //设置内存大小
int allocate_mem(allocated_block *ab); //为制定块分配内存
void rearrange(); // 对块进行重新分配
int create_new_process(); //创建新的进程
int free_mem(allocated_block *ab); //释放分配块
void swap(int *p, int *q); //交换地址
int dispose(allocated_block *ab); //释放分配块结构体
void display_mem_usage(); //显示内存情况
void kill_process(); //杀死对应进程并释放其空间与结构体
void Usemy_algo(int id); //使用对应的分配算法
void dispose_free(free_block *ab);
int best_fit(allocated_block *ab);
int worst_fit(allocated_block *ab);
int first_fit(allocated_block *ab);
int buddy_fit(allocated_block *ab);

//主函数
int main(){
    int op;
    pid = 1;
    free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
    free_block_tail=free_block_head;
    logical_free_block_tail=free_block_head;
    for(;;){
//        sleep(1);
        display_menu();
        fflush(stdin);
        scanf("%d", &op);
        switch (op){
            case 1:{
                set_mem_size();
                break;
            }
            case 2:{
                printf("Choose an algorithm\n");
                printf("1: Best Fit\n 2: Worst Fit\n 3: First Fit\n 4: Buddy System\n");
                scanf("%d", &alg);
                break;
            }
            case 3:{ create_new_process(); break; }
            case 4:{ kill_process(); break; }
            case 5:{ display_mem_usage(); break; }
            case 233:{ puts("bye...."); sleep(1); return 0; }
            defaut: break;
        }
    }
}

allocated_block *find_process(int id){ //循环遍历分配块链表，寻找pid=id的进程所对应的块

    for (allocated_block *pointer=allocated_block_head; pointer!=nullptr; pointer=pointer->next) {
            if(pointer->pid==id){
                return pointer;
            }
    }
    puts("we don't have a process with the certain pid");
    return nullptr;
}

free_block *init_free_block(int mem_size){ //初始化空闲块，这里的mem_size表示允许的最大虚拟内存大小
    free_block *p;
    p = (free_block *)malloc(sizeof(free_block));
    if (p == nullptr){
        puts("No memory left");
        return nullptr;
    }
    p->size = mem_size;
    p->start_addr = DEFAULT_MEM_START;
    p->next = nullptr;
    p->last=NULL;
    return p;
}

void display_menu(){
    puts("\n\n******************menu*******************");
    printf("1) Set memory size (default = %d)\n", DEFAULT_MEM_SIZE);
    printf("2) Set memory allocation algorithm\n");
    printf("3) Create a new process\n");
    printf("4) Kill a process\n");
    printf("5) Display memory usage\n");
    printf("233) Exit\n");
}

void set_mem_size() { //更改最大内存大小
    int size;
    scanf("%d",&size);

    int sub=size-mem_size;

    if(sub>=0){//变大->直接扩展尾巴的size即可
        if(logical_all_block_tail==NULL || logical_all_block_tail->start_addr<logical_free_block_tail->start_addr){
            logical_free_block_tail->size+=sub;

        } else{//free list 的最后一个节点在中间->需要新建free node
            free_block *cur=(free_block*)malloc(sizeof(free_block));
            cur->size=sub;
            cur->start_addr=mem_size;
            free_block_tail->next=cur;
            cur->last=free_block_tail;
            free_block_tail=cur;
            logical_free_block_tail=cur;
        }
        total_free_size+=sub;
    }

    else{//变小
        if(logical_all_block_tail->start_addr>logical_free_block_tail->start_addr){
            //最后一个是已经分配的，此时无法缩小除非压缩(不考虑)
            puts("can't reduce the size except redirect the memory");
        } else{
            if(logical_free_block_tail->size>-sub){//大于，减少size即可
                logical_free_block_tail->size+=sub;
            } else if(logical_free_block_tail->size==-sub){//删掉该节点
                free_block *temp=logical_free_block_tail->last;
                temp->next=logical_free_block_tail->next;
                if(logical_free_block_tail->next!=NULL) {
                    logical_free_block_tail->next->last=logical_free_block_tail->last;
                }
                free(logical_free_block_tail);
                logical_free_block_tail=temp;
                free_block_tail=temp;
            } else{
                puts("can't reduce the size except redirect the memory");
            }
        }

    }
    mem_size=size;
    //将free_list最后的size换掉
}

void allocated_manage(allocated_block *ab){

    if(allocated_block_head==NULL){
        allocated_block_head=ab;
        allocated_block_tail=ab;
        logical_all_block_tail=ab;
    } else{
        allocated_block_tail->next=ab;
        allocated_block_tail=ab;
        if(ab->start_addr>logical_all_block_tail->start_addr){
            logical_all_block_tail=ab;
        }
    }
}

int first_fit(allocated_block *ab){

    rearrange();

    free_block *cur=free_block_head;

    while(cur!=NULL&&cur->size<ab->size){
        cur=cur->next;
    }
    if(cur==NULL){//找不到
        return 0;
    }
    ab->start_addr=cur->start_addr;
    cur->start_addr+=ab->size;
    cur->size-=ab->size;
    total_free_size-=ab->size;
    allocated_manage(ab);

    return 1;
}
int best_fit(allocated_block *ab){

    free_block *pointer=free_block_head;

    free_block *best=nullptr;

   while(pointer!=NULL){
       if(pointer->size>ab->size){
           if(best== nullptr || pointer->size<best->size){
               best=pointer;
           }
       } else if (pointer->size==ab->size){//直接break，刚好匹配
           best=pointer;
           break;
       }
       pointer=pointer->next;
   }

   if(best->size<ab->size){
       return 0;
   }
   ab->start_addr=best->start_addr;
   if(best->size==ab->size){//删掉free节点
       dispose_free(best);
   } else{
       best->start_addr+=ab->size;
       best->size-=ab->size;
       total_free_size-=ab->size;

   }
    allocated_manage(ab);


    return 1;
}

void dispose_free(free_block *fb){
    if(fb==free_block_head){
        fb->next->last=NULL;
        free_block_head=fb->next;
        free(fb);
    } else if(fb==free_block_tail){
        fb->last->next=NULL;
        free_block_tail=fb->last;
        free(fb);
    } else{
        fb->next->last=fb->last;
        fb->last->next=fb->next;
        free(fb);
    }
}

int worst_fit(allocated_block *ab){

    free_block *pointer=free_block_head;
    free_block *worst=free_block_head;
    while(pointer!=NULL){
        if(pointer->size>ab->size){
            if(pointer->size>worst->size){
                worst=pointer;
            }
        }
        pointer=pointer->next;
    }

    if(worst->size<ab->size){
        return 0;
    }
    ab->start_addr=worst->start_addr;
    if(worst->size==ab->size){//删掉free节点
        dispose_free(worst);
    } else{
        worst->start_addr+=ab->size;
        worst->size-=ab->size;
        total_free_size-=ab->size;
    }
    allocated_manage(ab);
    return 1;
}


int allocate_mem(allocated_block *ab){ //为块分配内存，真正的操作系统会在这里进行置换等操作

    switch (alg){
        case 1: {
            return best_fit(ab);
        }
        case 2:{
            return worst_fit(ab);
        }
        case 3:{
            return first_fit(ab);
        }
        case 4:{
            return buddy_fit(ab);
        }
        default:
            return -1;
    }
}

int create_new_process(){ //创建新进程
    int mem_sz = 0;
    printf("Please input memory size\n");
    scanf("%d", &mem_sz);

    if(mem_sz>total_free_size){//分配失败
        puts("The memory is not enough,you need to increase it");
        return -1;
    } else{//分配成功

        allocated_block *ab=(allocated_block*)malloc(sizeof(allocated_block));
        ab->pid=pid++;
        ab->size=mem_sz;
        ab->next=NULL;

        return allocate_mem(ab);
    }
    // Write your code here
}

void swap(int *p, int *q){
    int tmp = *p;
    *p = *q;
    *q = tmp;
    return;
}

void rearrange(){ //将块按照地址大小进行排序
    free_block *tmp, *tmpx;
    puts("Rearrange begins...");
    puts("Rearrange by address...");
    tmp = free_block_head;
    while(tmp != NULL){
        tmpx = tmp->next;
        while (tmpx != NULL){
            if (tmpx->start_addr < tmp->start_addr){
                swap(&tmp->start_addr, &tmpx->start_addr);
                swap(&tmp->size, &tmpx->size);
            }
            tmpx = tmpx->next;
        }
        tmp = tmp->next;
    }
    usleep(500);
    puts("Rearrange Done.");
}

void merge(){
    free_block *cur=free_block_head->next;
    free_block *pre=free_block_head;
    while(cur!=NULL){
        if(cur->last->start_addr+cur->last->size==cur->start_addr){
            cur->last->size+=cur->size;
            cur->last->next=cur->next;
            if(cur->next!=NULL){
                cur->next->last=cur->last;
            }
            if(cur==free_block_tail){
                free_block_tail=cur->last;
            }
            pre=cur->next;
            free(cur);
            cur=pre;
        } else {
            cur = cur->next;
        }
    }
}


int free_mem(allocated_block *ab){ //释放某一块的内存

    free_block *cur=(free_block*)malloc(sizeof(free_block));
    cur->size=ab->size;
    cur->start_addr=ab->start_addr;
    free_block_tail->next=cur;
    cur->last=free_block_tail;
    cur->next=NULL;
    free_block_tail=cur;
    total_free_size+=ab->size;
    rearrange();
    merge();
    return 1;

}

int dispose(allocated_block *fab){ //释放结构体所占的内存
    allocated_block *pre, *ab;
    if(fab==allocated_block_tail){
       pre=allocated_block_head;
       if(pre==fab){
           allocated_block_tail=NULL;
       } else{
           while(pre->next!=allocated_block_tail){
               pre=pre->next;
           }
           allocated_block_tail=pre;
       }
    }

    if (fab == allocated_block_head){
        allocated_block_head = allocated_block_head->next;
        free(fab);
        return 1;
    }
    pre = allocated_block_head;
    ab = allocated_block_head->next;
    while (ab != fab){ pre = ab; ab = ab->next;}
    pre->next = ab->next;
    free(ab);
    return 2;
}

void display_mem_usage(){
    free_block *fb = free_block_head;
    allocated_block *ab = allocated_block_head;
    puts("*********************Free Memory*********************");
    printf("%20s %20s\n", "start_addr", "size");
    int cnt = 0;
    while (fb != NULL){
        cnt++;
        printf("%20d %20d\n", fb->start_addr, fb->size);
        fb = fb->next;
    }
    if (!cnt) puts("No Free Memory");
    else printf("Totaly %d free blocks\n", cnt);
    puts("");
    puts("*******************Used Memory*********************");
    printf("%10s %10s %20s\n", "PID", "start_addr", "size");
    cnt = 0;
    while (ab != NULL){
        cnt++;
        printf("%10d %10d %20d\n", ab->pid, ab->start_addr, ab->size);
        ab = ab->next;
    }
    if (!cnt) puts("No allocated block");
    else printf("Totaly %d allocated blocks\n", cnt);
    return;
}

void kill_process(){ //杀死某个进程
    allocated_block *ab;
    int pid;
    puts("Please input the pid of Killed process");
    scanf("%d", &pid);
    ab = find_process(pid);
    if (ab != NULL){
        free_mem(ab);
        dispose(ab);
    }
}

int buddy_fit(allocated_block *ab){

    int size=1;

    while(size < ab->size){
        size=size<<1;
    }
    ab->size=size;

    return best_fit(ab);

}



