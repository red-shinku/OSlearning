#pragma once

class Handler
{
public:
    virtual ~Handler() = default;
    virtual void handler(int connfd) = 0;
};