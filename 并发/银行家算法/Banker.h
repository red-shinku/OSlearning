#ifndef BANKER_H
#define BANKER_H

#include <vector>
#include <map>
#include <queue>
#include <mutex>

class Banker 
{
public:
    Banker(
        int thread_nums,
        int src_nums
    );
    Banker(const Banker&) = delete;
    Banker& operator=(const Banker&) = delete;
    ~Banker() = default;

private:
    int total_src;
    int available_src;
    std::map<int, int> PID_func_index;
    std::vector<int> each_max_request;
    std::vector<int> each_now_have;
    std::vector<int> each_now_need;

    bool is_safe(int PID);
    void reset_each_now_have(std::vector<int>& have);

public:
    bool request(int PID, int need);
    void release_src(int PID);
    void add_thread(int PID);
    void remove_thread(int PID);
};

Banker::Banker(
    int thread_nums,
    int src_nums
):
    total_src(src_nums),
    available_src(src_nums)
{
    
    each_max_request.reserve(thread_nums);
    each_now_have.reserve(thread_nums);
    each_now_need.reserve(thread_nums);
}

struct Details
{
    int max_request;
    int now_have;
    int now_need;
};


#endif //BANKER_H