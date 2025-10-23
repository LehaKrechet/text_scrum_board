#pragma once
#include <task.h>
#include <board.h>
#include <column.h>
#include <developer.h>
void create_task(Board &board, std::string col, std::string title);
void create_developer(Board& board, std::string name);