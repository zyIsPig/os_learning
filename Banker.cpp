#include "iostream"
#include "string"
#include "vector"
#include "map"
#define NEW "new"
#define REQUEST "request"
#define TERMINATE "terminate"
using namespace std;

int pro_num=0;

struct process
{
    int pid;
   vector <int> max;
   vector <int> allocation;
   vector <int> need;

};
map <int,process> pro_map;
vector <process> process_list;

int resour_num=0;
vector <int> available;
vector <int> resour_max;

process * find_by_pid(int pid){
    for (int i = 0; i <pro_num ; ++i) {
        if(process_list[i].pid==pid){
            return &process_list[i];
        }
    }

    return NULL;
}
bool check_need(vector <int > &v,process *cur_pro){

    for (int i = 0; i < resour_num; ++i) {
        if(v[i]>cur_pro->need[i]){
            return false;
        }
    }
    return true;

}

bool check_avail(vector <int> &v){
    for (int i = 0; i < resour_num; ++i) {
        if(v[i]>available[i]){
            return false;
        }
    }
    return true;

}

bool check_small(vector <int> &a,vector <int> &b,int len){//vector a <= vector b
    for (int i = 0; i < len; ++i) {
        if(a[i]>b[i]){
            return false;
        }
    }
    return true;
}

bool safety(){
    vector <int> work;
    vector <bool> finish;
    for (int i = 0; i < resour_num; ++i) {
        work.push_back(available[i]);
    }
    for (int j = 0; j < pro_num; ++j) {
        finish.push_back(false);
    }

    int cc=0;
    for (int i = 0; i < pro_num; ++i) {
        if(finish[i])continue;
        process &cur_pro=process_list[i];
        if(check_small(cur_pro.need,work,resour_num)){
            for (int j = 0; j < resour_num; ++j) {
                work[j]+=cur_pro.allocation[j];
            }
            finish[i]=true;
            i=-1;
            cc++;
        }
    }
    return cc == pro_num;
}
bool resource_request(vector <int > &v,int pid){
    process *cur_pro=find_by_pid(pid);
    if(cur_pro==NULL)return false;

    if(!check_need(v,cur_pro)){//超出最大需求
        return false;
    }

    if(!check_avail(v)){//超出可用
        return false;
    }
    //尝试分配

    for (int i = 0; i < resour_num; ++i) {
        available[i]=available[i]-v[i];
        cur_pro->allocation[i]+=v[i];
        cur_pro->need[i]-=v[i];
        pro_map[pid]=*cur_pro;
    }

    if(safety()){
        return true;
    } else{//回滚
        for (int i = 0; i < resour_num; ++i) {
            available[i]=available[i]+v[i];
            cur_pro->allocation[i]-=v[i];
            cur_pro->need[i]+=v[i];
        }
        return false;
    }
}

bool request(int pid,int r){
    vector < int > v_temp;
    int temp;
    for (int i = 0; i < r; ++i) {
        cin >> temp;
        v_temp.push_back(temp);
    }
    return resource_request(v_temp, pid);
    //resource request

}

bool add_process(int pid,int r){
    process *new_pro=new process;
    vector <int> temp_vec;
    bool check= true;

    int temp;
    for (int i = 0; i < r; i++)
    {
        cin >>temp;
        temp_vec.push_back(temp);
        if(temp>resour_max[i])check = false;
    }
    if(!check){
        return false;
    }
    for (int j = 0; j < r; ++j) {
        temp=temp_vec[j];
        new_pro->max.push_back(temp);
        new_pro->allocation.push_back(0);
        new_pro->need.push_back(temp);
    }
    new_pro->pid=pid;
    process_list.push_back(*new_pro);
//    pro_map[pid]=*new_pro;
    pro_num++;
    return true;
}
void delete_process(int pid){
    process *cur_pro=find_by_pid(pid);
    if(cur_pro==NULL)return;

    for (int i = 0; i < resour_num; ++i) {
        available[i]+=cur_pro->allocation[i];
        cur_pro->allocation[i]=0;
    }
    cur_pro->pid=-1;
}

int main(){
    int r=0,pid;
    char type[10];
    cin >>r;

    //分配resources
    resour_num=r;
    //输入资源
    int t;
    for (int i = 0; i < r; i++)
    {
        cin >> t;
        available.push_back(t);
        resour_max.push_back(t);
    }
    // 输入操作
    while(cin.get()){
        cin >>pid;
        cin >>type;
        vector <int> cur_vec;
        if(strcmp(type,TERMINATE)==0){
            delete_process(pid);
            cout <<"OK"<<endl;
        }
        else if (strcmp(type,NEW)==0)
        {
            if(add_process(pid,r)){
                cout <<"OK"<<endl;
            } else{
                cout <<"NOT OK"<<endl;
            }
        }
        else if(strcmp(type,REQUEST)==0)
        {
           if(request(pid,r)){
               cout <<"OK"<<endl;
           } else{
               cout <<"NOT OK"<<endl;
           }
        }
    }
    return 0;

}