#pragma once

#include <string>
#include <vector>
#include <column.h>  
// #include <memory>

class Board {
    private:
        std::string name;
        std::vector<Column*> columns;
    public:
        Board(std::string n){
            name = n;
            columns = {};
        }

        void set_name(std::string n);
        std::string get_name();

        std::vector<Column*>& get_columns();
        void add_column(Column* col);
        void delete_column(Column* col);

};