#include "error.h"
#include <iostream>
void err_sys(const std::string detail)
{
    std::cout << detail << '\n';
}