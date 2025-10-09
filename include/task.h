#include <string>
#include <vector>

extern std::vector<std::string> ids;

class Task{
    public:
        std::string id;
        std::string title;
        std::string description;
        int priority;
        std::string status;
        std::string generate_id();
};