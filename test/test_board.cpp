#include <gtest/gtest.h>
#include "board.h"
#include "column.h"
#include "developer.h"

class BoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        board = new Board("Test Board");
        column = new Column("Test Column");
        developer = new Developer("Test Developer");
    }

    void TearDown() override {
        delete board;
        delete column;
        delete developer;
    }

    Board* board;
    Column* column;
    Developer* developer;
};

TEST_F(BoardTest, ConstructorAndGetters) {
    EXPECT_EQ(board->get_name(), "Test Board");
    EXPECT_TRUE(board->get_columns().empty());
    EXPECT_TRUE(board->get_developer().empty());
}

TEST_F(BoardTest, SetName) {
    board->set_name("New Board Name");
    EXPECT_EQ(board->get_name(), "New Board Name");
}

TEST_F(BoardTest, AddAndGetColumns) {
    board->add_column(column);
    auto columns = board->get_columns();
    EXPECT_EQ(columns.size(), 1);
    EXPECT_EQ(columns[0], column);
}

TEST_F(BoardTest, AddAndGetDevelopers) {
    board->add_developer(developer);
    auto developers = board->get_developer();
    EXPECT_EQ(developers.size(), 1);
    EXPECT_EQ(developers[0], developer);
}

TEST_F(BoardTest, DeleteColumn) {
    board->add_column(column);
    EXPECT_EQ(board->get_columns().size(), 1);
    
    board->delete_column(column);
    EXPECT_TRUE(board->get_columns().empty());
}

TEST_F(BoardTest, DeleteDeveloper) {
    board->add_developer(developer);
    EXPECT_EQ(board->get_developer().size(), 1);
    
    board->delete_developer(developer);
    EXPECT_TRUE(board->get_developer().empty());
}