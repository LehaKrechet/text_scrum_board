#pragma once

#include <string>
#include <vector>
#include <task.h>

class Task{
    std::string name;
    std::string description;
    std::vector<Task> tasks;
};