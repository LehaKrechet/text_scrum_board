#pragma once

#include <string>
#include <vector>
#include <column.h>  
#include <developer.h>

class Column;

class Board {
    private:
        std::string name;
        std::vector<Column*> columns;
        std::vector<Developer*> developers;
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

        void add_developer(Developer* develop);
        void delete_developer(Developer* develop);
        std::vector<Developer*>& get_developer();


};