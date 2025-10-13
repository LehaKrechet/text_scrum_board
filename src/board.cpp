#include <string>
#include <vector>
#include <board.h> 
#include <algorithm> 


void Board::set_name(std::string n){
    name = n;
}
std::string Board::get_name(){
    return name;
}

std::vector<Column*>& Board::get_columns() {
    return columns;
}

// const std::vector<Column>& Board::get_columns() const {
//     return columns;
// }
void Board::add_column(Column* col){
    columns.push_back(col);
}

void Board::delete_column(Column* col){
    auto it = std::find(columns.begin(), columns.end(), col);
    columns.erase(it);
}
