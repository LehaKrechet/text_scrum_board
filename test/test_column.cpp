#include <gtest/gtest.h>
#include "column.h"
#include "task.h"
#include "board.h"

class ColumnTest : public ::testing::Test {
protected:
    void SetUp() override {
        column = new Column("Test Column");
        task1 = new Task("Task 1");
        task2 = new Task("Task 2");
    }

    void TearDown() override {
        delete column;
        delete task1;
        delete task2;
    }

    Column* column;
    Task* task1;
    Task* task2;
};

TEST_F(ColumnTest, ConstructorAndGetters) {
    EXPECT_EQ(column->get_name(), "Test Column");
    EXPECT_TRUE(column->get_tasks().empty());
}

TEST_F(ColumnTest, AddTask) {
    column->add_task(task1);
    auto tasks = column->get_tasks();
    EXPECT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0], task1);
}

TEST_F(ColumnTest, DeleteTask) {
    column->add_task(task1);
    column->add_task(task2);
    
    EXPECT_EQ(column->get_tasks().size(), 2);
    
    column->delete_task(task1);
    auto tasks = column->get_tasks();
    EXPECT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0], task2);
}

TEST_F(ColumnTest, DeleteNonExistentTask) {
    column->delete_task(task1);
    EXPECT_TRUE(column->get_tasks().empty());
}

TEST_F(ColumnTest, SetName) {
    column->set_name("New Column Name");
    EXPECT_EQ(column->get_name(), "New Column Name");
}

TEST(MoveTaskTest, MoveBetweenColumns) {
    Column start("Start");
    Column end("End");
    Task task("Movable Task");
    
    start.add_task(&task);
    EXPECT_EQ(start.get_tasks().size(), 1);
    EXPECT_TRUE(end.get_tasks().empty());
    
    move_task(&start, &end, &task);
    
    EXPECT_TRUE(start.get_tasks().empty());
    EXPECT_EQ(end.get_tasks().size(), 1);
    EXPECT_EQ(end.get_tasks()[0], &task);
}