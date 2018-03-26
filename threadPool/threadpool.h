#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <memory>
#include <utility>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <ostream>
#include <exception>
class threadPool
{
protected:
    threadPool();
private:
    static threadPool*pool;
    std::queue<std::function<void()>> task_queue;
    std::vector<std::thread>    thread_pool ;   //线程池

    std::mutex task_mx;
    std::mutex exception_mx;
    std::condition_variable taskNotEmpty;


public:
    static threadPool* getInstance(){return pool;}
    template<typename Functor,typename ...Args>
    void add_task(Functor &&fun,Args&&...args)
    {
        std::unique_lock<std::mutex> lck(this->task_mx);
        auto f=std::bind(std::forward<Functor>(fun),std::forward<Args>(args)...);
        task_queue.push(f);
        taskNotEmpty.notify_one();
    }
    void run()
    {
        while(true)
        {
            std::unique_lock<std::mutex> lck(this->task_mx);
            taskNotEmpty.wait(lck,[this](){return !task_queue.empty();});
            auto f=task_queue.front();
            task_queue.pop();
            lck.unlock();
            try
            {
                f();
            }
            catch(std::exception e)
            {
                exception_mx.lock();
                std::cout<<"task failed case of exception:"<<std::endl;
                std::cout<<"thread id \t"<<std::this_thread::get_id()<<"\nexception:\t"<<e.what()<<std::endl;
                exception_mx.unlock();
            }

        }
    }
    ~threadPool();
};

#endif // THREADPOOL_H
