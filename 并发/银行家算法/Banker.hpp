#ifndef BANKER_H
#define BANKER_H

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include <mutex>

/**
 * Details：保存线程资源使用情况
 * 每个注册线程拥有一个该结构体
 * 成员变量：
 *     max_request：该线程允许的最大资源数
 *     now_have：该线程当前持有的资源
 *     now_need：该线程当前需要的资源
 */

struct Details
{
    int max_request;
    int now_have;
    int now_need;
};

/**
 * Banker类（死锁预防）
 * 在线程申请资源前，检查是否会出现死锁
 * 注册了一组使用同一批资源的线程id
 * 申请资源的线程调用request方法，若安全，
 * 返回true允许申请，否则返回false拒绝。
 * 释放资源的线程需要调用update_src方法，更新
 * 资源使用信息。
 * 该类以禁止拷贝操作，未定义移动操作
 * 请在堆内存上创建该对象。
 * 
 * 成员变量：
 *     私有：
 *     mtx：同步原语，互斥量，控制该对象的入口
 *     total_src：总的资源数
 *     available_src：当前可用资源数
 *     PID_func_details：线程id到线程Details的映射
 *     cmp_func：可调用对象，用于优先队列的比较方法
 *     detailsQueue：实时维护的，线程资源信息的优先队列。
 *              内部存储的是线程id，通过map映射到相关数据结构
 *     waitTORemove：
 *           待删除PID队列，带优先级。辅助移除detailsQueue
 *           中的PID。
 * 
 * 成员函数：
 *     私有：
 *     is_safe(int PID)：
 *          检查分配资源后，线程是否安全
 *     test_and_remove(int PID)：
 *          检查待删队列内的PID是否能移除。若
 *          有实际数据位于映射表，则不能移除。                       
 * 
 *     对外：
 *     request(int PID, int need)：
 *          线程请求资源时调用。先预分配，检查是否安全。
 *     update_src(int PID, int nums):
 *          线程释放资源时调用。更新其资源持有信息。
 *     add_thread(int PID, int max_request)：
 *          注册新的线程id，提供对该线程的死锁检查服务。
 *     remove(K&&... pid)：
 *          取消对线程的检查服务。使用可变参数模版。
 *          同时提供对给定实参合法性的检查。若不合法则
 *          忽略。若合法，添加至待删除队列，并从
 *          PID_func_details中移除。
 */

class Banker 
{
public:
    Banker(int thread_nums, int src_nums, std::vector<std::vector<int>>&);
    Banker(const Banker&) = delete;
    Banker& operator=(const Banker&) = delete;
    ~Banker() = default;

private:
    using DetailsQueue = 
    std::priority_queue<
        int, 
        std::vector<int>,
        std::function<bool(int, int)>
    >;

    std::mutex mtx;
    int total_src;
    int available_src;
    std::map<int, Details> PID_func_details;

    std::function<bool(int, int)> cmp_func = 
    [this](int PID1, int PID2) -> bool{
        return PID_func_details[PID1].now_need < 
            PID_func_details[PID2].now_need;
    };
    DetailsQueue detailsQueue;
    DetailsQueue waitTORemove;

    bool is_safe();
    bool test_and_remove();
    void reset_each_now_have(std::vector<int>& have);

public:
    bool request(int PID, int need);
    void update_src(int PID, int nums);
    void add_thread(int PID, int max_request);
    template<typename... K> void remove(K&&... PID);
};


template<typename... K>
void Banker::remove(K&&... PID)
{
    auto remove_thread = [&](auto&& pid){
        if(PID_func_details.count(pid) == 1)
        {
            waitTORemove.push(pid);
            PID_func_details.erase(pid);
        }
    }

    (remove_thread(PID), ...)
}


#endif //BANKER_H