#include <ftxui.h>
#include <iostream>

int main() {
    try {
        ScrumBoardUI app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
