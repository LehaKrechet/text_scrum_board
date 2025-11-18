#include <gtest/gtest.h>
#include "manager.h"
#include "board.h"

class ManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board("Test Board");
        board->add_column(new Column("Backlog"));
        board->add_column(new Column("In Progress"));
        board->add_column(new Column("Done"));
    }

    void TearDown() override {
        for (auto col : board->get_columns()) {
            delete col;
        }
        for (auto dev : board->get_developer()) {
            delete dev;
        }
        delete board;
    }

    Board* board;
};

TEST_F(ManagerTest, CreateTask) {
    create_task(*board, "Backlog", "New Task");
    
    bool taskFound = false;
    for (auto col : board->get_columns()) {
        if (col->get_name() == "Backlog") {
            for (auto task : col->get_tasks()) {
                if (task->get_title() == "New Task") {
                    taskFound = true;
                    break;
                }
            }
        }
    }
    EXPECT_TRUE(taskFound);
}

TEST_F(ManagerTest, CreateTaskInNonExistentColumn) {

    create_task(*board, "NonExistent", "New Task");
    

    int totalTasks = 0;
    for (auto col : board->get_columns()) {
        totalTasks += col->get_tasks().size();
    }
    EXPECT_EQ(totalTasks, 0);
}

TEST_F(ManagerTest, CreateDeveloper) {
    create_developer(*board, "New Developer");
    
    bool developerFound = false;
    for (auto dev : board->get_developer()) {
        if (dev->get_name() == "New Developer") {
            developerFound = true;
            break;
        }
    }
    EXPECT_TRUE(developerFound);
}