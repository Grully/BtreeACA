#include "Node.h"

class Btree {
private:
    int loadfactor;
    Node* root;

    void splitChild(Node* parent, int index, Node* child) {
        Node* newNode = new Node(loadfactor, child->isLeaf);
        newNode->size = loadfactor - 1;

        for (int i = 0; i < loadfactor - 1; i++) {
            newNode->values[i] = child->values[i + loadfactor];
        }

        if (!child->isLeaf) {
            for (int i = 0; i < loadfactor; i++) {
                newNode->pointers[i] = child->pointers[i + loadfactor];
                child->pointers[i + loadfactor] = nullptr;
                child->pointers[i + loadfactor] = nullptr;
            }
        }

        child->size = loadfactor - 1;

        for (int i = parent->size; i > index; i--) {
            parent->pointers[i + 1] = parent->pointers[i];
        }
        parent->pointers[index + 1] = newNode;

        for (int i = parent->size - 1; i >= index; i--) {
            parent->values[i + 1] = parent->values[i];
        }
        parent->values[index] = child->values[loadfactor - 1];
        parent->size++;
    }

    void insertNonFull(Node* node, int value) {
        int i = node->size - 1;

        if (node->isLeaf) {
            while (i >= 0 && value < node->values[i]) {
                node->values[i + 1] = node->values[i];
                i--;
            }
            node->values[i + 1] = value;
            node->size++;
        }
        else {
            while (i >= 0 && value < node->values[i]) {
                i--;
            }
            i++;

            if (node->pointers[i]->size == 2 * loadfactor - 1) {
                splitChild(node, i, node->pointers[i]);
                if (value > node->values[i]) {
                    i++;
                }
            }
            insertNonFull(node->pointers[i], value);
        }
    }

    void removeFromLeaf(Node* node, int index) {
        for (int i = index + 1; i < node->size; ++i) {
            node->values[i - 1] = node->values[i];
        }
        node->size--;
    }

    int getPreviousVal(Node* node, int index) {
        Node* current = node->pointers[index];
        while (!current->isLeaf) {
            current = current->pointers[current->size];
        }
        return current->values[current->size - 1];
    }

    int getNextVal(Node* node, int index) {
        Node* current = node->pointers[index + 1];
        while (!current->isLeaf) {
            current = current->pointers[0];
        }
        return current->values[0];
    }

    void removeFromNonLeaf(Node* node, int index) {
        int k = node->values[index];

        if (node->pointers[index]->size >= loadfactor) {
            int pred = getPreviousVal(node, index);
            node->values[index] = pred;
            deleteValue(node->pointers[index], pred);
        }
        else if (node->pointers[index + 1]->size >= loadfactor) {
            int nextVal = getNextVal(node, index);
            node->values[index] = nextVal;
            deleteValue(node->pointers[index + 1], nextVal);
        }
        else {
            mergeNodes(node, index);
            deleteValue(node->pointers[index], k);
        }
    }

    void mergeNodes(Node* parent, int index) {
        Node* child = parent->pointers[index];
        Node* sibling = parent->pointers[index + 1];

        child->values[child->size++] = parent->values[index];

        for (int i = 0; i < sibling->size; ++i) {
            child->values[child->size + i] = sibling->values[i];
        }

        if (!child->isLeaf) {
            for (int i = 0; i <= sibling->size; ++i) {
                child->pointers[child->size + i] = sibling->pointers[i];
                sibling->pointers[i] = nullptr;
            }
        }

        child->size += sibling->size;

        for (int i = index + 1; i < parent->size; ++i) {
            parent->values[i - 1] = parent->values[i];
        }
        for (int i = index + 2; i <= parent->size; ++i) {
            parent->pointers[i - 1] = parent->pointers[i];
        }
        parent->size--;

        sibling->pointers[0] = nullptr;
        delete sibling;
    }

    void takeValFromLeftSon(Node* node, int index) {
        Node* child = node->pointers[index];
        Node* sibling = node->pointers[index - 1];

        for (int i = child->size - 1; i >= 0; --i) {
            child->values[i + 1] = child->values[i];
        }
        if (!child->isLeaf) {
            for (int i = child->size; i >= 0; --i) {
                child->pointers[i + 1] = child->pointers[i];
            }
        }

        child->values[0] = node->values[index - 1];

        if (!child->isLeaf) {
            child->pointers[0] = sibling->pointers[sibling->size];
            sibling->pointers[sibling->size] = nullptr;
        }

        node->values[index - 1] = sibling->values[sibling->size - 1];

        child->size++;
        sibling->size--;
    }

    void takeValFromRightSon(Node* node, int index) {
        Node* child = node->pointers[index];
        Node* sibling = node->pointers[index + 1];

        child->values[child->size] = node->values[index];

        if (!child->isLeaf) {
            child->pointers[child->size + 1] = sibling->pointers[0];
        }

        node->values[index] = sibling->values[0];

        for (int i = 1; i < sibling->size; ++i) {
            sibling->values[i - 1] = sibling->values[i];
        }
        if (!sibling->isLeaf) {
            for (int i = 1; i <= sibling->size; ++i) {
                sibling->pointers[i - 1] = sibling->pointers[i];
            }
        }

        child->size++;
        sibling->size--;
    }

    void fill(Node* node, int index) {
        if (index > 0 && node->pointers[index - 1]->size >= loadfactor) {
            takeValFromLeftSon(node, index);
        }
        else if (index < node->size && node->pointers[index + 1]->size >= loadfactor) {
            takeValFromRightSon(node, index);
        }
        else {
            if (index != node->size) {
                mergeNodes(node, index);
            }
            else {
                mergeNodes(node, index - 1);
            }
        }
    }

    bool deleteValue(Node* node, int value) {
        int index = 0;
        while (index < node->size && node->values[index] < value) {
            index++;
        }

        if (index < node->size && node->values[index] == value) {
            if (node->isLeaf) {
                removeFromLeaf(node, index);
            }
            else {
                removeFromNonLeaf(node, index);
            }
            return true;
        }
        else {
            if (node->isLeaf) {
                return false;
            }

            bool flag = (index == node->size);
            if (node->pointers[index]->size < loadfactor) {
                fill(node, index);
            }

            if (flag && index > node->size) {
                return deleteValue(node->pointers[index - 1], value);
            }
            else {
                return deleteValue(node->pointers[index], value);
            }
        }
    }
    void printLevel(Node* node, int level) const {
        if (!node) return;

        if (level == 1) {
            node->printNode();
            std::cout << " ";
        }
        else {
            for (int i = 0; i <= node->size; ++i) {
                printLevel(node->pointers[i], level - 1);
            }
        }
    }

    int getHeight(Node* node) const {
        if (!node) return 0;
        if (node->isLeaf) return 1;
        return 1 + getHeight(node->pointers[0]);
    }

    void deleteTree(Node* node) {
        if (!node) return;

        if (!node->isLeaf) {
            for (int i = 0; i <= node->size; ++i) {
                deleteTree(node->pointers[i]);
            }
        }
        delete node;
    }

public:
    Btree() {
        this->loadfactor = 2;
        root = new Node(loadfactor, true);
    }
    Btree(int loadfactor) {
        this->loadfactor = loadfactor;
        root = new Node(loadfactor, true);
    }

    ~Btree() {
        deleteTree(root);
    }

    bool findValue(int value) const {
        Node* currentNode = root;

        while (currentNode != nullptr) {
            int counter = 0;
            while (counter < currentNode->size && currentNode->values[counter] < value) {
                counter++;
            }

            if (counter < currentNode->size && currentNode->values[counter] == value) {
                return true;
            }

            if (currentNode->isLeaf) {
                break;
            }
            else {
                currentNode = currentNode->pointers[counter];
            }
        }

        return false;
    }

    void insertValue(int value) {
        if (root->size == 2 * loadfactor - 1) {
            Node* newRoot = new Node(loadfactor, false);
            newRoot->pointers[0] = root;
            root = newRoot;
            splitChild(newRoot, 0, newRoot->pointers[0]);
            insertNonFull(newRoot, value);
        }
        else {
            insertNonFull(root, value);
        }
    }

    bool deleteValue(int value) {
        if (!root) return false;

        bool result = deleteValue(root, value);

        if (root->size == 0 && !root->isLeaf) {
            Node* oldRoot = root;
            root = root->pointers[0];
            oldRoot->pointers[0] = nullptr;
            delete oldRoot;
        }

        return result;
    }

    void printTree() const {
        int height = getHeight(root);
        for (int i = 1; i <= height; i++) {
            printLevel(root, i);
            std::cout << std::endl;
        }
    }


};