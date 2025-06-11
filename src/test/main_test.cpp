//
// Created by ludwig on 22.11.16.
//

#include <gtest/gtest.h>
#include "Manager.h"
#include "src/gtest-internal-inl.h"

class testManager : public ::testing::Test{
protected:
    ClassProject::Manager manager;
    ClassProject::BDD_ID trueTerminal, falseTerminal,a,b,c,d,aOrb,cAndd,f;
    void SetUp() override {
        trueTerminal = manager.True();
        falseTerminal = manager.False();
        a = manager.createVar("a");
        b = manager.createVar("b");
        c = manager.createVar("c");
        d = manager.createVar("d");
        aOrb = manager.or2(a,b);
        cAndd = manager.and2(c,d);
        f = manager.and2(aOrb,cAndd);
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
    ASSERT_EQ(a,2);
    ASSERT_EQ(b,3);
    ASSERT_EQ(c,4);
    ASSERT_EQ(d,5);
    ASSERT_EQ(manager.topVar(a),a);
    ASSERT_EQ(manager.topVar(b),b);
    ASSERT_EQ(manager.topVar(c),c);
    ASSERT_EQ(manager.topVar(d),d);
    ASSERT_EQ(manager.uniqueTable[a].high,trueTerminal);
    ASSERT_EQ(manager.uniqueTable[a].low,falseTerminal);
    ASSERT_EQ(manager.uniqueTable[b].high,trueTerminal);
    ASSERT_EQ(manager.uniqueTable[b].low,falseTerminal);
    ASSERT_EQ(manager.uniqueTable[c].high,trueTerminal);
    ASSERT_EQ(manager.uniqueTable[c].low,falseTerminal);
    ASSERT_EQ(manager.uniqueTable[d].high,trueTerminal);
    ASSERT_EQ(manager.uniqueTable[d].low,falseTerminal);
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

TEST_F(testManager, topVariableTest) {
    ASSERT_EQ(manager.topVar(aOrb), a);
    ASSERT_EQ(manager.topVar(cAndd), c);
    ASSERT_EQ(manager.topVar(8), b);
    ASSERT_EQ(manager.topVar(f), a);
}

TEST_F(testManager, iteTest) {
    ASSERT_EQ(manager.ite(trueTerminal,a,b), a);
    ASSERT_EQ(manager.ite(falseTerminal,a,b), b);
    ASSERT_EQ(manager.ite(a,b,b), b);
    const ClassProject::BDD_ID existed = manager.ite(a,trueTerminal,falseTerminal);
    ASSERT_EQ(manager.ite(a,trueTerminal,falseTerminal),existed);
    ClassProject::BDD_ID result = manager.ite(a, b, trueTerminal);
    ASSERT_NE(result, a);
    ASSERT_NE(result, b);
    ASSERT_NE(result, trueTerminal);
    ASSERT_NE(result, falseTerminal);
    EXPECT_EQ(manager.ite(a, trueTerminal, falseTerminal), a);
}

TEST_F(testManager, coFactorTrueTest) {
    ASSERT_EQ(manager.coFactorTrue(manager.trueNodeID,a),manager.trueNodeID);
    ASSERT_EQ(manager.coFactorTrue(manager.falseNodeID,a),manager.falseNodeID);
    ASSERT_EQ(manager.coFactorTrue(a,a),trueTerminal);
    ASSERT_EQ(manager.coFactorTrue(b,b),trueTerminal);
    ASSERT_EQ(manager.coFactorTrue(b,a),b);
    ASSERT_EQ(manager.coFactorTrue(aOrb,a),trueTerminal);
    ASSERT_EQ(manager.coFactorTrue(cAndd,a),cAndd);
    EXPECT_EQ(manager.coFactorTrue(f, a), cAndd);
    EXPECT_EQ(manager.coFactorTrue(f, a), manager.coFactorTrue(f));
    EXPECT_EQ(manager.coFactorTrue(f, b), cAndd);
    EXPECT_EQ(manager.coFactorTrue(f, c), manager.and2(d, aOrb));
    EXPECT_EQ(manager.coFactorTrue(f, d), manager.and2(c, aOrb));
}

TEST_F(testManager, coFactorFalseTest) {
    ASSERT_EQ(manager.coFactorFalse(manager.trueNodeID,a),manager.trueNodeID);
    ASSERT_EQ(manager.coFactorFalse(manager.falseNodeID,a),manager.falseNodeID);
    ASSERT_EQ(manager.coFactorFalse(a,a),falseTerminal);
    ASSERT_EQ(manager.coFactorFalse(b,b),falseTerminal);
    ASSERT_EQ(manager.coFactorFalse(b,a),b);
    ASSERT_EQ(manager.coFactorFalse(aOrb,a),b);
    ASSERT_EQ(manager.coFactorFalse(cAndd,a),cAndd);
}

TEST_F(testManager, or2Test) {
    ASSERT_EQ(manager.uniqueTable[aOrb].low, b);
    ASSERT_EQ(manager.uniqueTable[aOrb].high, trueTerminal);
}

TEST_F(testManager, and2Test) {
    ASSERT_EQ(manager.uniqueTable[cAndd].high, d);
    ASSERT_EQ(manager.uniqueTable[cAndd].low, falseTerminal);
}

TEST_F(testManager, negTest)
{
    ASSERT_EQ(manager.neg(manager.True()), manager.False());
    ASSERT_EQ(manager.neg(manager.False()), manager.True());
    ASSERT_EQ(manager.neg(a), manager.ite(a, manager.False(), manager.True()));
}

TEST_F(testManager, xor2Test) {
    ClassProject::BDD_ID xor2 = manager.xor2(a,b);
    ASSERT_EQ(manager.uniqueTable[xor2].high, manager.neg(b));
    ASSERT_EQ(manager.uniqueTable[xor2].low, b);
}

TEST_F(testManager, nand2Test) {
    ClassProject::BDD_ID nand2 = manager.nand2(a,b);
    ASSERT_EQ(manager.uniqueTable[nand2].high, manager.neg(b));
    ASSERT_EQ(manager.uniqueTable[nand2].low, trueTerminal);
}

TEST_F(testManager, nor2Test) {
    ClassProject::BDD_ID nor2 = manager.nor2(a,b);
    ASSERT_EQ(manager.uniqueTable[nor2].high, falseTerminal);
    ASSERT_EQ(manager.uniqueTable[nor2].low, manager.neg(b));
}

TEST_F(testManager, xnor2Test) {
    ClassProject::BDD_ID xnor2 = manager.xnor2(a,b);
    ASSERT_EQ(manager.uniqueTable[xnor2].high, b);
    ASSERT_EQ(manager.uniqueTable[xnor2].low, manager.neg(b));
}

TEST_F(testManager, getTopVariableNameTest) {
    ASSERT_EQ(manager.getTopVarName(trueTerminal), "True");
    ASSERT_EQ(manager.getTopVarName(falseTerminal), "False");
    ASSERT_EQ(manager.getTopVarName(a), "a");
    ASSERT_EQ(manager.getTopVarName(b), "b");
    ASSERT_EQ(manager.getTopVarName(c), "c");
    ASSERT_EQ(manager.getTopVarName(d), "d");
    ASSERT_EQ(manager.getTopVarName(aOrb), "a");
    ASSERT_EQ(manager.getTopVarName(cAndd), "c");
    ASSERT_EQ(manager.getTopVarName(8), "b");
    ASSERT_EQ(manager.getTopVarName(9), "a");
}

TEST_F(testManager, findNodesTest) {
    std::set<ClassProject::BDD_ID> nodes;
    manager.findNodes(trueTerminal, nodes);
    std::set trueSet = {trueTerminal};
    ASSERT_EQ(nodes, trueSet);
    nodes.clear();
    manager.findNodes(a, nodes);
    std::set aSet = {falseTerminal, trueTerminal, a};
    ASSERT_EQ(nodes, aSet);
    nodes.clear();
    manager.findNodes(aOrb, nodes);
    std::set aOrbSet = {falseTerminal, trueTerminal, b, aOrb};
    ASSERT_EQ(nodes, aOrbSet);
    nodes.clear();
    manager.findNodes(cAndd, nodes);
    std::set cAnddSet = {falseTerminal, trueTerminal, d, cAndd};
    ASSERT_EQ(nodes, cAnddSet);
    nodes.clear();
    manager.findNodes(9, nodes);
    std::set<ClassProject::BDD_ID> fSet = {falseTerminal, trueTerminal, d, cAndd, 8, 9};
    ASSERT_EQ(nodes, fSet);
    nodes.clear();
}

TEST_F(testManager, findVarsTest) {
    std::set<ClassProject::BDD_ID> nodes;
    manager.findVars(trueTerminal, nodes);
    std::set<ClassProject::BDD_ID> trueSet = {};
    ASSERT_EQ(nodes, trueSet);
    nodes.clear();
    manager.findVars(a, nodes);
    std::set aSet = {a};
    ASSERT_EQ(nodes, aSet);
    nodes.clear();
    manager.findVars(aOrb, nodes);
    std::set aOrbSet = {a, b};
    ASSERT_EQ(nodes, aOrbSet);
    nodes.clear();
    manager.findVars(cAndd, nodes);
    std::set cAnddSet = {c, d};
    ASSERT_EQ(nodes, cAnddSet);
    nodes.clear();
    manager.findVars(9, nodes);
    std::set fSet = {a, b, c, d};
    ASSERT_EQ(nodes, fSet);
    nodes.clear();
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
