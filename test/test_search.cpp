#include <gtest/gtest.h>
#include <column.h>
#include <board.h>

TEST(SearchTaskTest, FindExistingTask) {
    Board board("Test Board");
    Column backlog("Backlog");
    Task task("Find Me");
    
    backlog.add_task(&task);
    board.add_column(&backlog);
    
    Task* found = search_task(board, "Backlog", "Find Me");
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->get_title(), "Find Me");
}

TEST(SearchTaskTest, TaskNotFound) {
    Board board("Test Board");
    Column backlog("Backlog");
    board.add_column(&backlog);
    
    EXPECT_THROW({
        search_task(board, "Backlog", "NonExistent");
    }, std::out_of_range);
}

TEST(SearchTaskTest, ColumnNotFound) {
    Board board("Test Board");
    
    EXPECT_THROW({
        search_task(board, "NonExistent", "Any Task");
    }, std::out_of_range);
}