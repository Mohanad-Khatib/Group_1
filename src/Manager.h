// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014
#pragma once
#include <vector>
#include <unordered_map>

#include "ManagerInterface.h"

namespace ClassProject {
    struct BDDNode {
        std::string label;
        BDD_ID id;
        BDD_ID high;
        BDD_ID low;
        BDD_ID topVar;
    };
    class Manager final : public ManagerInterface {
    public:
        std::vector<BDDNode> uniqueTable;
        std::unordered_map<size_t, BDD_ID> uniqueTableMap;
        std::unordered_map<size_t, BDD_ID> computedTable;
        std::string label;
        BDD_ID falseNodeID = 0;
        BDD_ID trueNodeID = 1;
        static size_t hashFunction(BDD_ID f, BDD_ID g, BDD_ID h);
        BDD_ID find_or_add_unique_table(BDD_ID v, BDD_ID high, BDD_ID low);
        BDD_ID createVar(const std::string &label) override;
        const BDD_ID &True() override;
        const BDD_ID &False() override;
        bool isConstant(BDD_ID f) override;
        bool isVariable(BDD_ID x) override;
        BDD_ID topVar(BDD_ID f) override;
        BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;
        BDD_ID or2(BDD_ID a, BDD_ID b) override;
        BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;
        BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;
        BDD_ID coFactorTrue(BDD_ID f) override;
        BDD_ID coFactorFalse(BDD_ID f) override;
        BDD_ID and2(BDD_ID a, BDD_ID b) override;
        BDD_ID xor2(BDD_ID a, BDD_ID b) override;
        BDD_ID neg(BDD_ID a) override;
        BDD_ID nand2(BDD_ID a, BDD_ID b) override;
        BDD_ID nor2(BDD_ID a, BDD_ID b) override;
        BDD_ID xnor2(BDD_ID a, BDD_ID b) override;
        std::string getTopVarName(const BDD_ID &root) override;
        void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;
        void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;
        size_t uniqueTableSize() override;
        void visualizeBDD(const std::string &filepath, const BDD_ID &root) override;
        Manager();
        ~Manager();
    };
}
