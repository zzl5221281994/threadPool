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
#include <exception>

/*生产者消费者模式线程池，通过add_task,可以添加任意参数的任务。
 * 1:第一个参数固定的为运行函数，其余参数为运行函数的参数，可以有任意个。
 * 2:当运行任务是对象的成员函数时，需要将对象本身当作第二个参数传入。
 * 3:线程池实现使用单例模式,需要通过getInstance函数获取线程池实例。
 * 4:线程池大小为所运行机器支持的物理线程个数
 * 5:当线程池运行线程发生异常时,所执行的任务会失败,但不会影响线程池正常使用，。
 * */
class threadPool
{
protected:
    threadPool();
private:
    static threadPool*pool;

    /*任务队列*/
    std::queue<std::function<void()>> task_queue ;

    /*线程池*/
    std::vector<std::thread>          thread_pool;

    /*线程池同步设施*/
    std::mutex task_mx;
    std::mutex exception_mx;
    std::condition_variable taskNotEmpty;


public:
    /*单例模式*/
    static threadPool* getInstance(){return pool;}

    threadPool(const threadPool&)            =delete;
    threadPool(threadPool&&)                 =delete;
    threadPool& operator =(const threadPool&)=delete;
    threadPool& operator =(threadPool&&)     =delete;


    /*添加任务，生产者*/
    template<typename Functor,typename ...Args>
    void add_task(Functor &&fun,Args&&...args)
    {
        std::unique_lock<std::mutex> lck(this->task_mx);
        auto f=std::bind(std::forward<Functor>(fun),std::forward<Args>(args)...);
        task_queue.push(f);
        taskNotEmpty.notify_one();
    }

    /*线程运行函数，消费者*/
    void run()
    {
        while(true)
        {
            std::function<void()> f;
            {
                std::unique_lock<std::mutex> lck(this->task_mx);
                taskNotEmpty.wait(lck,[this]()->bool{return !task_queue.empty();});
                f=task_queue.front();
                task_queue.pop();
            }
            try
            {
                f();
            }
            catch(std::exception e)
            {
                std::lock_guard<std::mutex> lck(exception_mx);
                std::cout<<"task failed case of exception:"<<std::endl;
                std::cout<<"thread id \t"<<std::this_thread::get_id()<<"\nexception:\t"<<e.what()<<std::endl;
            }
        }
    }
    ~threadPool();
};

#endif // THREADPOOL_H
