#include <developer.h>
#include <string>

std::string Developer::get_name(){
    if (this != nullptr) {
        return name;
    }else{
        return "No developer";
    }
}
void Developer::set_name(std::string n){
    if (this != nullptr) {
        name = n;
    }
}
