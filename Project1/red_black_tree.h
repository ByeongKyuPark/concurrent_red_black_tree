#include "node.h"
#include <queue>

class RBTree {
    Node* root;
public:
    RBTree() :root{ new Node{0} } {
    }
    ~RBTree() {
        delete root;
        root = nullptr;
    }
    bool Contains(int element) {
        bool found{ false };
        Node* node{ root };
        node->contains_mtx.lock();
        while ((node != nullptr && node->isLeaf() == false) && !found) {
            Node* parent = node;
            if (element == node->data) {
                found = true;
            }
            else if (element < node->data) {
                node = node->left;
            }
            else {
                node = node->right;
            }
            if (found == false) {
                node->contains_mtx.lock();
                parent->contains_mtx.unlock();
            }
        }
        node->contains_mtx.unlock();
        return found;
    }
    void ConnectLeftChild(Node* node, Node* left) {
        left->parent = node;
        node->left = left;
    }
    void ConnectRightChild(Node* node, Node* right) {
        right->parent = node;
        node->right = right;
    }

    bool Add(int element) {
        bool found{ false };
        Node* node{ root };
        node->insert_mtx.lock();
        Node* locked{ node };
        if (root != node) {
            root->insert_mtx.lock();
            node->insert_mtx.unlock();
            locked = node = root;
        }

        while (node->isLeaf() == false && !found) {
            Node* parent = node;
            if (element == node->data) {
                found = true;
            }
            else if (element < node->data) {
                node = node->left;
            }
            else {
                node = node->right;
            }
            if (node->color == BLACK && parent->color == BLACK && parent != locked) {
                parent->insert_mtx.lock();
                locked->insert_mtx.unlock();
                locked = parent;
            }
        }
        if (found == false) {
            node->rot_mtx.lock();
            Node* oldNode{ node };
            Node* left = new Node{ 0,nullptr,nullptr,node };
            Node* right = new Node{ 0,nullptr,nullptr,node };
            node->color = RED;
            node->data = element;
            node->left = left;
            node->right = right;
            //node->color = RED;
            //node->data = element;
            node->left->color = BLACK;
            node->right->color = BLACK;
            ConnectLeftChild(node, node->left);
            ConnectRightChild(node, node->right);
            oldNode->rot_mtx.unlock();
            while (node != root && (node->parent != nullptr && node->parent->color == RED)) {
                Node* parent = node->parent;
                Node* grandParent = parent->parent;
                if (parent == grandParent->left) {
                    Node* aunt = grandParent->right;
                    if (aunt->color == RED) {
                        aunt->color = BLACK;
                        parent->color = BLACK;
                        grandParent->color = RED;
                        node = grandParent;
                    }
                    else if (node == parent->left) {
                        parent->color = BLACK;
                        grandParent->color = RED;
                        Node* sister = parent->right;
                        if (grandParent == root) {
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            sister->rot_mtx.lock();
                            root = parent;
                            ConnectRightChild(parent, grandParent);
                            ConnectLeftChild(grandParent, sister);
                            sister->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                        }
                        else {
                            Node* grandGrandParent = grandParent->parent;
                            grandGrandParent->rot_mtx.lock();
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            sister->rot_mtx.lock();
                            if (grandParent == grandGrandParent->left) {
                                ConnectLeftChild(grandGrandParent, parent);
                            }
                            else {
                                ConnectRightChild(grandGrandParent, parent);
                            }
                            ConnectRightChild(parent, grandParent);
                            ConnectLeftChild(grandParent, sister);
                            sister->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                            grandGrandParent->rot_mtx.unlock();
                        }
                    }
                    else {//node is right of parent
                        node->color = BLACK;
                        grandParent->color = RED;
                        Node* left = node->left;
                        Node* right = node->right;
                        if (grandParent == root) {
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            node->rot_mtx.lock();
                            left->rot_mtx.lock();
                            right->rot_mtx.lock();
                            root = node;
                            ConnectLeftChild(node, parent);
                            ConnectRightChild(node, grandParent);
                            ConnectRightChild(parent, left);
                            ConnectLeftChild(grandParent, right);
                            right->rot_mtx.unlock();
                            left->rot_mtx.unlock();
                            node->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                        }
                        else {
                            Node* grandgrandParent = grandParent->parent;
                            grandgrandParent->rot_mtx.lock();
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            node->rot_mtx.lock();
                            left->rot_mtx.lock();
                            right->rot_mtx.lock();
                            if (grandParent == grandgrandParent->left)
                                ConnectLeftChild(grandgrandParent, node);
                            else
                                ConnectRightChild(grandgrandParent, node);

                            ConnectLeftChild(node, parent);
                            ConnectRightChild(node, grandParent);
                            ConnectRightChild(parent, left);
                            ConnectLeftChild(grandParent, right);
                            right->rot_mtx.unlock();
                            left->rot_mtx.unlock();
                            node->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                            grandgrandParent->rot_mtx.unlock();
                        }

                    }
                }
                else {//parent is right child of grandparent
                    Node* aunt = grandParent->left;
                    if (aunt != nullptr && aunt->color == RED) {
                        aunt->color = BLACK;
                        parent->color = BLACK;
                        grandParent->color = RED;
                        node = grandParent;
                    }
                    else if (node == parent->right) {
                        parent->color = BLACK;
                        grandParent->color = RED;
                        Node* sister = parent->left;
                        if (grandParent == root) {
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            sister->rot_mtx.lock();
                            root = parent;
                            ConnectLeftChild(parent, grandParent);
                            ConnectRightChild(grandParent, sister);
                            sister->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                        }
                        else {
                            Node* grandGrandParent = grandParent->parent;
                            if (grandGrandParent)
                                grandGrandParent->rot_mtx.lock();
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            sister->rot_mtx.lock();
                            if (grandParent == grandGrandParent->left) {
                                ConnectLeftChild(grandGrandParent, parent);
                            }
                            else {
                                ConnectRightChild(grandGrandParent, parent);
                            }
                            ConnectLeftChild(parent, grandParent);
                            ConnectRightChild(grandParent, sister);
                            sister->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                            if (grandGrandParent)
                                grandGrandParent->rot_mtx.unlock();
                        }
                    }
                    else {//node is left child of parent
                        node->color = BLACK;
                        grandParent->color = RED;
                        Node* left = node->left;
                        Node* right = node->right;
                        if (grandParent == root) {
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            node->rot_mtx.lock();
                            left->rot_mtx.lock();
                            right->rot_mtx.lock();
                            root = node;
                            ConnectRightChild(node, parent);
                            ConnectLeftChild(node, grandParent);
                            ConnectLeftChild(parent, right);
                            ConnectRightChild(grandParent, left);
                            right->rot_mtx.unlock();
                            left->rot_mtx.unlock();
                            node->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                        }
                        else {
                            Node* grandGrandParent = grandParent->parent;
                            grandGrandParent->rot_mtx.lock();
                            grandParent->rot_mtx.lock();
                            parent->rot_mtx.lock();
                            node->rot_mtx.lock();
                            left->rot_mtx.lock();
                            right->rot_mtx.lock();
                            if (grandParent == grandGrandParent->left) {
                                ConnectLeftChild(grandGrandParent, node);
                            }
                            else {
                                ConnectRightChild(grandGrandParent, node);
                            }
                            ConnectRightChild(node, parent);
                            ConnectLeftChild(node, grandParent);
                            ConnectLeftChild(parent, right);
                            ConnectRightChild(grandParent, left);
                            right->rot_mtx.unlock();
                            left->rot_mtx.unlock();
                            node->rot_mtx.unlock();
                            parent->rot_mtx.unlock();
                            grandParent->rot_mtx.unlock();
                            grandGrandParent->rot_mtx.unlock();
                        }
                    }
                }
            }
            root->color = BLACK;//always
            locked->insert_mtx.unlock();
            return !found;
        }
        return found;
    }


    void CreatePrintQueueByDepth(Node* root, std::queue<Node*>& printQueue, int depth) const
    {
        if (depth == 0) {
            printQueue.push(root);
        }
        else if (root == nullptr) {
            CreatePrintQueueByDepth(nullptr, printQueue, depth - 1);
            CreatePrintQueueByDepth(nullptr, printQueue, depth - 1);
        }
        else {
            if (root->left == nullptr && root->right == nullptr) {
                CreatePrintQueueByDepth(nullptr, printQueue, depth - 1);
                CreatePrintQueueByDepth(nullptr, printQueue, depth - 1);
            }
            else {
                CreatePrintQueueByDepth(root->left, printQueue, depth - 1);
                CreatePrintQueueByDepth(root->right, printQueue, depth - 1);
            }
        }
    }

    std::queue<Node*> CreatePrintQueue() const
    {
        if (root == nullptr) {
            return std::queue<Node*>();
        }

        std::queue<Node*> result;
        const int height = root->Height();
        for (int depth = 0; depth <= height; depth++) {
            CreatePrintQueueByDepth(root, result, depth);
        }

        return result;
    }

    void PrintTree() const
    {
        if (root != nullptr) {
            std::queue<Node*> printQueue = CreatePrintQueue();

            int nodeRowCount = 0;
            int nodeRowTarget = 1;
            int coutWidth = 4 << root->Height();
            int offset{};
            while (printQueue.empty() == false) {
                if (nodeRowCount == 0) {
                    std::cout.width(coutWidth / 2);
                }
                else {
                    std::cout.width(coutWidth - 1 + offset);
                }

                if (printQueue.front() != nullptr && printQueue.front()->isLeaf() == false) {
                    Node* node = printQueue.front();
                    std::cout << node->data << "(";
                    if (node->color == Color::BLACK) {
                        std::cout << "B)";
                    }
                    else {
                        std::cout << "R)";
                    }
                    offset = -3;
                }
                else {
                    std::cout << "X";
                    offset = 0;
                }

                // check if we are done with the current row
                if (++nodeRowCount == nodeRowTarget) {
                    std::cout << std::endl;
                    nodeRowTarget *= 2;
                    coutWidth /= 2;
                    nodeRowCount = 0;
                }
                else {
                    std::cout << ' ';
                }
                printQueue.pop();
            }
        }
        else {
            std::cout << "Tree is Empty" << std::endl;
        }
        std::cout << std::endl;
    }
};
