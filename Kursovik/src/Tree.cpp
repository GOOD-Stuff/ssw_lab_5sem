#include "Tree.h"

Tree::Tree() {
    value = "";
    alloc = true;
}

Tree::Tree(const std::string& val) {
    value = val;
    alloc = true;
}

/**
 * @brief Sets the priority of the tree node's support
 * @param[in] priority_ - operation priority
 *
 * @return none
 */
void Tree::SetPriority(int priority_) {
    priority = priority_;
}

/**
 * @brief Get the priority of the operation
 *
 * @return Priority of the operation
*/
int Tree::GetPriority() {
    return priority;
}


/**
 * @brief Add left the Node in Tree
 * @param[in] val - value
 *
 * @param priority_ - The priority of the operation
*/
void Tree::AddLeftNode(const std::string& val, int priority_) {
    this->left = CreateNode(this, val, priority_);
}

void Tree::AddRightNode(const std::string& val, int priority_) {
    this->right = CreateNode(this, val, priority_);
}

void Tree::AddLeftNode(const std::string& val) {
    this->left = CreateNode(this, val);
}

void Tree::AddRightNode(const std::string& val) {
    this->right = CreateNode(this, val);
}

/**
 * @brief Get left Node of Tree
 *
 * @return Node of Tree
*/
Tree* Tree::GetLeftNode() const {
    return this->left;
}

/**
 * @brief Add left Tree in global tree
 * @param[inout] tree - left tree for add
 *
 * @return none
*/
void Tree::AddLeftTree(Tree* tree) {
    if (tree != nullptr) {
        tree->parent = this;
        this->left = tree;
    }
}


void Tree::AddRightTree(Tree* tree) {
    if (tree != nullptr) {
        tree->parent = this;
        this->right = tree;
    }
}


/**
 * @brief Change Value Node of Tree
 * @param[in] val - new value
 *
 * @return none
*/
void Tree::ChangeValue(const std::string& val) {
    value = val;
}


/**
 * @brief Get right Node of Tree
 *
 * @return Right Node
*/
Tree* Tree::GetRightNode() const {
    return this->right;
}


/**
 * @brief Get Parent Node
 *
 * @return Parent Node
*/
Tree* Tree::GetParentNode() const {
    return this->parent;
}


std::string Tree::GetValue() {
    return this->value;
}


/**
 * @brief Print full Tree
 * @param[in] tab - tabulation for correct out Tree
 *
 * @return none
*/
void Tree::PrintTree(int tab) {
    for (auto i = 0; i < tab; i++) {
        std::cout << "    ";
    }

    std::cout << this->value << std::endl;

    if (this->left != nullptr) {
        this->left->PrintTree(tab + 1);
    } else {
        for (auto i = 0; i < tab + 1; i++) {
            std::cout << "    ";
        }

        std::cout << "NULL" << std::endl;
    };

    if (this->right != nullptr) {
        this->right->PrintTree(tab + 1);
    } else {
        for (auto i = 0; i < tab + 1; i++) {
            std::cout << "    ";
        }

        std::cout << "NULL" << std::endl;
    }
}


/**
 * @brief Create like a root node of syntax tree
 * @param[in] node_name - name of the node (value)
 *
 * @return node of tree
 */
Tree* Tree::CreateNode(const std::string& val) {
    auto* node = new Tree(val);
    return node;
}


/**
 * @brief Create node of syntax tree
 * @param[in] t_tree    - parent node
 * @param[in] node_name - name of the created node (value)
 *
 * @return node of tree
 */
Tree* Tree::CreateNode(Tree* parent_tree, const std::string& val) {
    auto* node = new Tree(val);
    node->parent = std::addressof(*parent_tree);
    return node;
}

/**
 * @brief Create node of syntax tree
 * @param[in] t_tree    - parent node
 * @param[in] node_name - name of the created node (value)
 *
 * @return node of tree
 */
Tree* Tree::CreateNode(Tree* parent_tree, const std::string& val, int& priority_) {
    auto* node = new Tree(val);
    node->parent = std::addressof(*parent_tree);
    node->SetPriority(priority_);
    return node;
}



void Tree::FreeLeftNode() {
    FreeTree(this->left);
}


void Tree::FreeRightNode() {
    FreeTree(this->right);
}

/**
 * @brief Delete tree
 *
 * @param[inout] t_tree - Tree
*/
void Tree::FreeTree(Tree*& t_tree) {
    try {
        if (t_tree->left != nullptr) FreeTree(t_tree->left);

        if (t_tree->right != nullptr) FreeTree(t_tree->right);

        delete t_tree;
        t_tree = nullptr;
    } catch (const std::exception& exp) {
        std::cerr << "<E> Tree: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Tree::FreeTree() {
    if (this->left != nullptr) this->left->FreeTree();

    if (this->right != nullptr) this->right->FreeTree();

    parent = nullptr;
    value = "";
    alloc = false;
}