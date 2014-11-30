#pragma once

#include "Task.h"

class Worker
{
public:
    Worker();
    ~Worker();

    void work(const char* filePath);
    void work(const Task& task);
};

