#pragma once

#include <memory>
#include "task.h"
#include "board.h"
#include "column.h"
#include "developer.h"

// Функции-менеджеры для создания сущностей на доске

// Создание новой задачи в указанной колонке
// Автоматически создает задачу и добавляет ее в нужную колонку
void create_task(Board& board, const std::string& col, const std::string& title);

// Создание нового разработчика в команде
// Создает разработчика и добавляет его на доску
void create_developer(Board& board, const std::string& name);