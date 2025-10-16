#pragma once

#include <string>
#include <vector>
#include <task.h>

class Developer{
    std::string name;
    public:
        Developer (std::string n){
            name = n;
        }
        std::string get_name();
        void set_name(std::string n);
};