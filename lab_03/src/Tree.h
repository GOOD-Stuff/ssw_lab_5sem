#ifndef LECS_PARS_TREE_H
#define LECS_PARS_TREE_H

#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <memory>
#include <string>
#include <queue>
#include <utility>
#include <iomanip>

class Tree {

public:
    Tree();
    explicit Tree(const std::string& val);

    void SetPriority(int priority_);
    void AddLeftNode(const std::string& val, int priority_);
    void AddRightNode(const std::string& val, int priority_);
    void AddLeftNode(const std::string& val);
    void AddRightNode(const std::string& val);
    void AddLeftTree(Tree* tree);
    void AddRightTree(Tree* tree);
    void ChangeValue(const std::string& val);
    Tree* GetRightNode() const;
    Tree* GetParentNode() const;
    std::string GetValue();
    int GetPriority();
    Tree* GetLeftNode() const;

    void PrintTree(int tab);

    static Tree* CreateNode(const std::string& val);
    static Tree* CreateNode(Tree* parent_tree, const std::string& val);
    static Tree* CreateNode(Tree* parent_tree, const std::string& val, int& priority_);

    void FreeLeftNode();
    void FreeRightNode();
    static void FreeTree(Tree*& t_tree);

    virtual ~Tree() {
        FreeTree();
    }

private:
    Tree *left   { nullptr };
    Tree *right  { nullptr };
    Tree *parent { nullptr };
    std::string value;
    bool alloc { false };
    int priority{ 0 };

    void FreeTree();
};


#endif //LECS_PARS_TREE_H
