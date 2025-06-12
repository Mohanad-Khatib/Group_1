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
    variable.topVar = uniqueTable.size();
    uniqueTable.push_back(variable);
    uniqueTableMap.insert({hashFunction(variable.topVar,variable.high,variable.low), variable.id});
    return uniqueTable.size() - 1;
}

const BDD_ID &Manager::True() {
    return trueNodeID;
}

const BDD_ID &Manager::False() {
    return falseNodeID;
}

bool Manager::isConstant(BDD_ID f) {
    return (f == uniqueTable[f].high && f == uniqueTable[f].low);

}

bool Manager::isVariable(BDD_ID f) {
    return (uniqueTable[f].high == True() && uniqueTable[f].low == False());
}

BDD_ID Manager::topVar(BDD_ID f) {
    return uniqueTable[f].topVar;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    if (i == True()) {
        return t;
    }
    if (i == False() || t == e) {
		return e;
	}
    if (isVariable(i) && t == True() && e == False()) return i;
    if (auto search = computedTable.find(hashFunction(i, t, e)); search != computedTable.end()) {
        return search->second;
    }
    BDD_ID bigNumber = std::numeric_limits<BDD_ID>::max();
    BDD_ID topF = isConstant(i) ? bigNumber : topVar(i);
    BDD_ID topG = isConstant(t) ? bigNumber : topVar(t);
    BDD_ID topH = isConstant(e) ? bigNumber : topVar(e);
    BDD_ID topVariable = std::min({topF, topG, topH});
    BDD_ID high = ite(coFactorTrue(i,topVariable),coFactorTrue(t,topVariable),coFactorTrue(e,topVariable));
    BDD_ID low = ite(coFactorFalse(i,topVariable),coFactorFalse(t,topVariable),coFactorFalse(e,topVariable));
    if (high == low) return high;
    BDD_ID newID = find_or_add_unique_table(topVariable, high, low);
    label = "";
    computedTable.insert({hashFunction(i, t, e), newID});
    return newID;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (isConstant(f)) return f;
    if (topVar(f) == topVar(x)) return uniqueTable[f].high;
    BDD_ID high = coFactorTrue(uniqueTable[f].high, x);
    BDD_ID low = coFactorTrue(uniqueTable[f].low, x);
    return ite(topVar(f), high, low);
}

BDD_ID Manager::coFactorTrue(BDD_ID f) {
    return coFactorTrue(f, topVar(f));
}

BDD_ID Manager::coFactorFalse(BDD_ID f,BDD_ID x) {
    if (isConstant(f)) return f;
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
    nodes_of_root.insert(root); // pointer and bool return
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
            vars_of_root.insert(uniqueTable[id].topVar);
        }
    }
}

size_t Manager::uniqueTableSize() {
    return uniqueTable.size();
}

void Manager::visualizeBDD(const std::string &filepath, const BDD_ID &root) {
    std::ofstream dot_file(filepath);
    if (!dot_file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }
    dot_file << "digraph BDD {\n";
    dot_file << "  rankdir=TB;\n";
    dot_file << "  size=\"4,6\";\n";
    dot_file << "  node [shape = circle];\n";
    std::set<BDD_ID> visited;
    std::function<void(BDD_ID)> writeNode;
    writeNode = [&](BDD_ID id) {
        if (visited.count(id)) return;
        visited.insert(id);
        const BDDNode &node = uniqueTable[id];
        if (node.topVar == 0 || node.topVar == 1) {
            dot_file << "  " << id << " [shape=box, label=\"" << node.topVar << "\"];\n";
        } else {
            dot_file << "  " << id << " [label=\"" << node.topVar << "\"];\n";
        }
        if (node.low != id) {
            dot_file << "  " << id << " -> " << node.low << " [style=dotted label=\"0\"];\n";
            writeNode(node.low);
        }
        if (node.high != id) {
            dot_file << "  " << id << " -> " << node.high << " [label=\"1\"];\n";
            writeNode(node.high);
        }
    };
    writeNode(root);
    dot_file << "}\n";
    dot_file.close();
}

BDD_ID Manager::find_or_add_unique_table(BDD_ID v, BDD_ID high, BDD_ID low) {
    if (auto search = uniqueTableMap.find(hashFunction(v, high, low)); search != uniqueTableMap.end()) {
        return uniqueTable[search->second].id;
    }
    const BDD_ID id = uniqueTableSize();
    BDDNode node = {.label = getTopVarName(v), .id = id, .high = high, .low = low, .topVar = v};
    uniqueTable.push_back(node);
    uniqueTableMap.insert({hashFunction(node.topVar, node.high, node.low), node.id});
    return id;
}

size_t Manager::hashFunction(BDD_ID f, BDD_ID g, BDD_ID h) {
    return (((f << 21) + g) << 21) + h;
}

Manager::Manager() {
    uniqueTable.push_back(BDDNode {"False",falseNodeID,falseNodeID,falseNodeID,falseNodeID});
    uniqueTable.push_back(BDDNode {"True",trueNodeID,trueNodeID,trueNodeID,trueNodeID});
    uniqueTableMap.insert({hashFunction(falseNodeID, falseNodeID, falseNodeID), falseNodeID});
    uniqueTableMap.insert({hashFunction(trueNodeID, trueNodeID, trueNodeID), trueNodeID});
}

Manager::~Manager() = default;

