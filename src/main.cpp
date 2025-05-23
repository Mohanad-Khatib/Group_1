//
// Created by Carolina P. Nogueira 2016
//

#include <iostream>

#include "Manager.h"
using namespace ClassProject;

int main(int argc, char* argv[])
{
    Manager manager;
    manager.createVar("a");
    manager.createVar("b");
    manager.createVar("c");
    manager.createVar("d");
    manager.or2(2,3);
    manager.and2(4,5);
    manager.and2(6,7);
    for (const auto& node : manager.uniqueTable) {
        std::cout << "Label: " << node.label
                  << ", ID: " << node.id
                  << ", High: " << node.high
                  << ", Low: " << node.low
                  << ", TopVar: " << node.top_var
                  << std::endl;
    }
    std::cout << "Computed Table:\n";
    for (const auto &entry : manager.computedTable) {
        std::cout << "f: " << entry.f
                  << ", g: " << entry.g
                  << ", h: " << entry.h
                  << ", r: " << entry.r << '\n';
    }
    manager.visualizeBDD("C:/Users/mohan/Desktop/bdd.dot", 9);
}
