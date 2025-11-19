#pragma once

#include <memory>
#include "task.h"
#include "board.h"
#include "column.h"
#include "developer.h"

// Функции-менеджеры для создания сущностей на доске
// Эти функции предоставляют высокоуровневый интерфейс для работы с доской

// Создание новой задачи в указанной колонке
void create_task(Board& board, const std::string& col, const std::string& title);

// Создание нового разработчика в команде
void create_developer(Board& board, const std::string& name);