#pragma once
#include <mutex>

enum Color { BLACK, RED };

struct Node {
    int data;
    Color color;
    Node* left;
    Node* right;
    Node* parent;

    int numContains;
    int numInserts;

    Node(int elem,Node* l=nullptr,Node* r=nullptr, Node* p=nullptr, Color col=Color::BLACK) :data{ elem }, color{ col }, left{l}, right{r}, parent{p}, numInserts{}, numContains{} {}

    std::mutex contains_mtx;
    std::mutex insert_mtx;
    std::mutex rot_mtx;

    bool isLeaf()const {
        return left == nullptr && right == nullptr;
    }
    int Height() const {
        if (left == nullptr && right == nullptr) {
            return 0;
        }
        int heightLeft = (left == nullptr) ? 0 : left->Height();
        int heightRight = (right == nullptr) ? 0 : right->Height();
        return 1 + ((heightLeft > heightRight) ? heightLeft : heightRight);
    }
};
