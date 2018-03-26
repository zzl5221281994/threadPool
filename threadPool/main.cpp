#include <iostream>
#include <chrono>
#include "threadpool.h"
using namespace std;
mutex m;
void fun(int a,int b)
{
        m.lock();
        cout<<"thread id:"<<this_thread::get_id()<<" "<<a<<":"<<b<<endl;
        m.unlock();
}
void callback1(int var)
{
    cout<<"callback1:"<<var<<endl;
}
void callback2(int var)
{
    cout<<"callback2:"<<var<<endl;
}
/******************************************/
void fune()
{
    vector<int> a;
    //cout<<"fune"<<endl;
    lock_guard<mutex> lck(m);
    cout<<a.at(1)<<endl;
}
void funf(const string &str)
{
    while(true)
    {
        lock_guard<mutex> lck(m);
        cout<<str<<endl;
        this_thread::sleep_for(chrono::microseconds(50));
    }
}
int main()
{
    string str;
    while(cin>>str)
    {
        if(str=="exception")
            threadPool::getInstance()->add_task(fune);
        else
            threadPool::getInstance()->add_task(funf,str);
    }
    while(true);
    return 0;
}
