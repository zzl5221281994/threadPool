#include "threadpool.h"

threadPool::threadPool()
{
    auto threadNums=std::thread::hardware_concurrency();
    std::cout<<"thread pool size:"<<threadNums<<std::endl;
    for(int i=0;i<threadNums;i++)
    {
        thread_pool.push_back(std::thread(run,this));
    }
}
threadPool::~threadPool()
{

}
threadPool* threadPool::pool=new threadPool;
