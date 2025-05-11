#include "Manager.h"

#include <algorithm>
#include <limits>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>

using namespace ClassProject;

BDD_ID Manager::createVar(const std::string &label) {
    BDDNode variable;
    variable.label = label;
    variable.high = True();
    variable.low = False();
    variable.id = uniqueTable.size();
    variable.top_var = uniqueTable.size();
    uniqueTable.push_back(variable);
    return uniqueTable.size();
}

const BDD_ID &Manager::True() {
    return trueNodeID;
}

const BDD_ID &Manager::False() {
    return falseNodeID;
}

bool Manager::isConstant(BDD_ID f) {
    if (f == uniqueTable[f].high && f == uniqueTable[f].low)
        return true;
    return false;
}

bool Manager::isVariable(BDD_ID f) {
    if (uniqueTable[f].high == True() && uniqueTable[f].low == False())
        return true;
    return false;
}

BDD_ID Manager::topVar(BDD_ID f) {
    return uniqueTable[f].top_var;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    if (isConstant(i) && i == 1) return t;
    if ((isConstant(i) && i == 0) || t == e) return e;
    for (const auto& entry : computedTable) {
        if (entry.f == i && entry.g == t && entry.h == e) {
            return entry.r;
        }
    }

    BDD_ID bigNumber = std::numeric_limits<BDD_ID>::max();
    BDD_ID topF = isConstant(i) ? bigNumber : topVar(i);
    BDD_ID topG = isConstant(t) ? bigNumber : topVar(t);
    BDD_ID topH = isConstant(e) ? bigNumber : topVar(e);
    BDD_ID topVariable = std::min({topF, topG, topH});
    BDD_ID high = ite(coFactorTrue(i,topVariable),coFactorTrue(t,topVariable),coFactorTrue(e,topVariable));
    BDD_ID low = ite(coFactorFalse(i,topVariable),coFactorFalse(t,topVariable),coFactorFalse(e,topVariable));
    if (high == low) return high;
    const BDD_ID newID = uniqueTable.size();
    uniqueTable.push_back({label, newID, high, low, topVariable});
    label = "";
    computedTable.push_back({i, t, e, newID});
    return newID;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || isConstant(f)) return f;
    if (topVar(f) == x) return uniqueTable[f].high;
    BDD_ID high = coFactorTrue(uniqueTable[f].high, x);
    BDD_ID low = coFactorTrue(uniqueTable[f].low, x);
    return ite(topVar(f), high, low);
}

BDD_ID Manager::coFactorTrue(BDD_ID f) {
    return coFactorTrue(f, topVar(f));
}

BDD_ID Manager::coFactorFalse(BDD_ID f,BDD_ID x) {
    if (topVar(f) > x || isConstant(f)) return f;
    if (topVar(f) == x) return uniqueTable[f].low;
    BDD_ID high = coFactorFalse(uniqueTable[f].high, x);
    BDD_ID low = coFactorFalse(uniqueTable[f].low, x);
    return ite(topVar(f), high, low);
}

BDD_ID Manager::coFactorFalse(BDD_ID f) {
    return coFactorFalse(f, topVar(f));
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
    label = uniqueTable[a].label + "+" + uniqueTable[b].label;
    return ite(a,True(),b);
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
    label = uniqueTable[a].label + "*" + uniqueTable[b].label;
    return ite(a,b,False());
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), b);
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), True());
}

BDD_ID Manager::neg(BDD_ID a) {
    label = "~" + uniqueTable[a].label;
    return ite(a,False(),True());
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
    return ite(a, False(), neg(b));
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
    return ite(a, b, neg(b));
}

std::string Manager::getTopVarName(const BDD_ID &root) {
    return uniqueTable[topVar(root)].label;
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
    if (nodes_of_root.count(root)) {
        return;
    }
    nodes_of_root.insert(root);
    if (isConstant(root)) {
        return;
    }
    const BDDNode &node = uniqueTable[root];
    findNodes(node.high, nodes_of_root);
    findNodes(node.low, nodes_of_root);
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
    std::set<BDD_ID> nodes_of_root;
    findNodes(root, nodes_of_root);
    for (const auto &id : nodes_of_root) {
        if (id != 0 && id != 1) {
            vars_of_root.insert(uniqueTable[id].top_var);
        }
    }
}

size_t Manager::uniqueTableSize() {
    return uniqueTable.size();
}

Manager::Manager() {
    uniqueTable.push_back(BDDNode {"False",0,0,0,0});
    uniqueTable.push_back(BDDNode {"True",1,1,1,1});
}

Manager::~Manager() = default;

