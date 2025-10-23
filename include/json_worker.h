#pragma once
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;
class Board;

class Json_worker{
    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    std::string save_path;

    public:
        Json_worker(std::string sp){
            doc.SetObject();
            save_path = sp;
        }
        void save();
        Value ids_add(std::vector<std::string> id);
        std::vector<std::string> ids_get();
        void board_add(Board board, Value ids);
        void board_load(Board& board);
        void clear_ids();
};
