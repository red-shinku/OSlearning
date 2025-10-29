#include "Banker.h"

bool Banker::request(int PID, int need)
{
    int index = PID_func_index[PID];
    if(need + each_now_have[index] > each_max_request[index])
    {
        return false;
    }
    if(need > available_src)
    {
        each_now_need[index] = need;
        return false;
    }
    available_src -= need;
    each_now_have[index] += need;
    if(is_safe(PID))
    {
        each_now_need[index] = 0;
        return true;
    }
    else
    {
        available_src += need;
        each_now_have[index] -= need;
        return false;
    }
}

bool Banker::is_safe(int PID)
{
    
}