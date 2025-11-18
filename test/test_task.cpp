#include <gtest/gtest.h>
#include "task.h"
#include "developer.h"

class TaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        task = new Task("Test Task");
        developer = new Developer("Test Developer");
    }

    void TearDown() override {
        delete task;
        delete developer;
    }

    Task* task;
    Developer* developer;
};

TEST_F(TaskTest, ConstructorAndGetters) {
    EXPECT_EQ(task->get_title(), "Test Task");
    EXPECT_FALSE(task->get_id().empty());
    EXPECT_EQ(task->get_prioriy(), 0);
    EXPECT_EQ(task->get_description(), "");
    EXPECT_EQ(task->get_developer(), nullptr);
}

TEST_F(TaskTest, SetAndGetDescription) {
    std::string desc = "Test Description";
    task->set_description(desc);
    EXPECT_EQ(task->get_description(), desc);
}

TEST_F(TaskTest, SetAndGetPriority) {
    task->set_priority(5);
    EXPECT_EQ(task->get_prioriy(), 5);
    
    task->set_priority(-1);
    EXPECT_EQ(task->get_prioriy(), -1);
    
    task->set_priority(100);
    EXPECT_EQ(task->get_prioriy(), 100);
}

TEST_F(TaskTest, SetAndGetDeveloper) {
    task->set_developer(developer);
    EXPECT_EQ(task->get_developer(), developer);
}

TEST_F(TaskTest, SetAndGetTitle) {
    std::string new_title = "New Title";
    task->set_title(new_title);
    EXPECT_EQ(task->get_title(), new_title);
}

TEST_F(TaskTest, UniqueIDGeneration) {
    Task task1("Task1");
    Task task2("Task2");
    
    EXPECT_NE(task1.get_id(), task2.get_id());
    EXPECT_FALSE(task1.get_id().empty());
    EXPECT_FALSE(task2.get_id().empty());
}