#include <iostream>

#include "Btree.h"

int main() {

    Btree* btree = new Btree();
    btree->insertValue(5);
    btree->insertValue(20);
    btree->insertValue(4);
    btree->insertValue(18);
    btree->insertValue(55);
    btree->insertValue(44);
    btree->insertValue(36);
    btree->insertValue(7);
    btree->insertValue(61);
    btree->insertValue(77);
    btree->insertValue(48);
    btree->printTree();
    std::cout << '\n';
    btree->deleteValue(4);
    btree->deleteValue(18);
    btree->deleteValue(5);
    btree->printTree();
    std::cout << "Finish";

    return 0;
}
