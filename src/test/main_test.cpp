//
// Created by ludwig on 22.11.16.
//

#include <gtest/gtest.h>
#include "Manager.h"

class testManager : public ::testing::Test{
protected:
    ClassProject::Manager manager;
    ClassProject::BDD_ID trueTerminal, falseTerminal,a,b,c,d;
    void SetUp() override {
        trueTerminal = manager.True();
        falseTerminal = manager.False();
        a = manager.createVar("a");
        b = manager.createVar("b");
        c = manager.createVar("c");
        d = manager.createVar("d");
    }
};

TEST_F(testManager, TrueAndFalseNodesTest) {
    ASSERT_EQ(manager.topVar(trueTerminal), trueTerminal); //top variable for true terminal is true terminal
    ASSERT_EQ(manager.uniqueTable[trueTerminal].high, trueTerminal); //high for the true terminal is true terminal
    ASSERT_EQ(manager.uniqueTable[trueTerminal].low, trueTerminal); //low for the true terminal is true terminal
    ASSERT_EQ(manager.topVar(falseTerminal), falseTerminal); //top variable for false terminal is false terminal
    ASSERT_EQ(manager.uniqueTable[falseTerminal].high, falseTerminal); //high for the false terminal is false terminal
    ASSERT_EQ(manager.uniqueTable[falseTerminal].low, falseTerminal); //low for the false terminal is false terminal
    ASSERT_NE(trueTerminal, falseTerminal); //true and false terminals are not equal
}

TEST_F(testManager, creatVariableTest) {
    EXPECT_EQ(a,2);
    EXPECT_EQ(b,3);
    EXPECT_EQ(c,4);
    EXPECT_EQ(d,5);
    EXPECT_EQ(manager.topVar(a),a);
    EXPECT_EQ(manager.topVar(b),b);
    EXPECT_EQ(manager.topVar(c),c);
    EXPECT_EQ(manager.topVar(d),d);
    EXPECT_EQ(manager.uniqueTable[a].high,trueTerminal);
    EXPECT_EQ(manager.uniqueTable[a].low,falseTerminal);
    EXPECT_EQ(manager.uniqueTable[b].high,trueTerminal);
    EXPECT_EQ(manager.uniqueTable[b].low,falseTerminal);
    EXPECT_EQ(manager.uniqueTable[c].high,trueTerminal);
    EXPECT_EQ(manager.uniqueTable[c].low,falseTerminal);
    EXPECT_EQ(manager.uniqueTable[d].high,trueTerminal);
    EXPECT_EQ(manager.uniqueTable[d].low,falseTerminal);
}

TEST_F(testManager, uniqueTableSizeIncreaseByOneAfterCreateVar) {
    const ClassProject::BDD_ID size = manager.uniqueTableSize();
    manager.createVar("sizeTest");
    ASSERT_EQ(manager.uniqueTableSize(), size + 1);
}

TEST_F(testManager, isConstantTest) {
    const ClassProject::BDD_ID size = manager.uniqueTableSize() - 1;
    for (ClassProject::BDD_ID i = 0; i < size; i++) {
        bool isConstant = (i == manager.True() || i == manager.False());
        ASSERT_EQ(manager.isConstant(i), isConstant);
    }
}

TEST_F(testManager, isVariableTest) {
    const ClassProject::BDD_ID size = manager.uniqueTableSize();
    for (ClassProject::BDD_ID i = 0; i < size; i++) {
        bool isVariable = (i == a || i == b || i == c || i == d);
        ASSERT_EQ(manager.isVariable(i), isVariable);
    }
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
