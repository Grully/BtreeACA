#include <iostream>

struct Node {
    int* values;
    Node** pointers;
    bool isLeaf;
    int size;

    Node(int loadfactor, bool isLeaf) {
        this->isLeaf = isLeaf;
        size = 0;
        values = new int[2 * loadfactor - 1];
        pointers = new Node * [2 * loadfactor];

        for (int i = 0; i < 2 * loadfactor - 1; i++) {
            values[i] = 0;
        }

        for (int i = 0; i < 2 * loadfactor; i++) {
            pointers[i] = nullptr;
        }
    }

    ~Node() {
        for (int i = 0; i <= size; i++) {
            delete pointers[i];
        }
        delete[] pointers;
        delete[] values;
    }

    void printNode() {
        std::cout << "[";
        for (int i = 0; i < size; i++) {
            std::cout << values[i];
            if (i < size - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]";
    }
};
