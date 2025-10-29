#include "Banker.hpp"

bool Banker::request(int PID, int need)
{
    std::unique_lock<std::mutex> lock(mtx);
    Details& details = PID_func_details[PID];
    if(need + details.now_have > details.max_request)
    {
        return false;
    }
    if(need > available_src)
    {
        details.now_need = need;
        return false;
    }
    available_src -= need;
    details.now_have += need;
    if(is_safe(PID))
    {
        details.now_need = 0;
        return true;
    }
    else
    {
        available_src += need;
        details.now_have -= need;
        return false;
    }
}

bool Banker::is_safe(int PID)
{
    int available = available_src;
    DetailsQueue pqueue(detailsQueue);
    while (!pqueue.empty())
    {
        if(test_and_remove())
        {
            continue;
        }
        int cur_thread = pqueue.top();
        Details &details = PID_func_details[cur_thread];
        if(available < details.now_need)
        {
            return false;
        }
        available += details.now_have;
        pqueue.pop();
    }
    return true;
}

void Banker::update_src(int PID, int nums = 0)
{
    std::unique_lock<std::mutex> lock(mtx);
    auto details = PID_func_details[PID];
    if(nums == 0)
    {
        available_src += details.now_have;
        details.now_have = 0;
    }
    else if(nums <= details.now_have && nums > 0)
    {
        available_src += nums;
        details.now_have -= nums;
    }
    else
    {
        return;
    }
}

void Banker::add_thread(int PID, int max_request)
{
    std::unique_lock<std::mutex> lock(mtx);
    PID_func_details[PID] = {max_request, 0, 0};
    detailsQueue.push(PID);
}

bool Banker::test_and_remove()
{
    if(waitTORemove.top() != detailsQueue.top())
    {
        return false;
    }
    if(PID_func_details.count(waitTORemove.top()) == 0)
    {
        detailsQueue.pop();
        waitTORemove.pop();
        return true;
    }
    else
    {
        waitTORemove.pop();
        return false;
    }
}
