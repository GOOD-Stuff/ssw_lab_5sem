#include "Syntax.h"

/**
 * TODO: Update grammar for your variant of tasks
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> [ = <exp> ] ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <your_other_operations>
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant> | <your_other_operations>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */
Syntax::Syntax(std::vector<Lexem>&& t_lex_table) {
    if (t_lex_table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty");

    if (t_lex_table.at(0).GetToken() == eof_tk) {
        printError(MUST_BE_PROG, t_lex_table.at(0));
        throw std::runtime_error("<E> Syntax: Code file is empty");
    }

    lex_table = t_lex_table;
    cursor    = lex_table.begin();

    operations.emplace(":=", 0);

    operations.emplace("+",   2);
    operations.emplace("-",   2);

    operations.emplace("*",   3);
    operations.emplace("div", 3);

    operations.emplace("=", 1);
    operations.emplace("<", 1);
    operations.emplace(">", 1);
    operations.emplace("<=", 1);
    operations.emplace(">=", 1);
    operations.emplace("<>", 1);
}


Syntax::~Syntax() {
    Tree::FreeTree(root_tree);
}


/**
 * Каждый блок (..Parse) строит своё поддерево (и возвращает его),
 *  которое затем добавляется на уровне выше, в месте вызова метода.
 */

/**
 * @brief Start parse incoming pascal file
 * @param none
 *
 * @return  EXIT_SUCCESS - if file was successful parsed
 * @return -EXIT_FAILURE - if can't parse incoming file
 */
int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto &it = cursor;

    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while (it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);

    std::cout << std::endl << std::endl;
    std::cout << "**************************************TREE************************************";
    std::cout << "\r\n";

    if (!error_state) { root_tree->PrintTree(0); 
        return EXIT_SUCCESS;
    }
    
    return -EXIT_FAILURE;
}


/**
 * @brief Parse entry point in grammar
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if input program part is correct
 * @return -EXIT_FAILURE - if input program part is incorrect
 * @note Wait input like: program <id_tk> ;
 */
int Syntax::programParse(lex_it &t_iter) {
    if (!checkLexem(t_iter, program_tk)) {
        printError(MUST_BE_PROG, *t_iter);
        return -EXIT_FAILURE;
    }

    auto iter = getNextLex(t_iter);

    if (!checkLexem(iter, id_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        } else {
            printError(MUST_BE_ID, *iter);
            return -EXIT_FAILURE;
        }
    }

    auto root_name = iter->GetName(); // save the name of program

    iter = getNextLex(t_iter);

    if (!checkLexem(iter, semi_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        } else {
            printError(MUST_BE_SEMI, *iter);
            return -EXIT_FAILURE;
        }
    }

    // First phase is OK, we can start to build the tree
    root_tree = Tree::CreateNode(root_name);

    return EXIT_SUCCESS;
}


/**
 * @brief Parse block part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if block part is matched to grammar
 * @return -EXIT_FAILURE - if block part doesn't matched to grammar
 */
int Syntax::blockParse(lex_it &t_iter) {
    try {
        auto iter = getNextLex(t_iter);

        switch (iter->GetToken()) {
            case var_tk: {
                root_tree->AddLeftNode("var", 0);
                vardpParse(t_iter, root_tree->GetLeftNode());
                break;
            }
            
            case label_tk: {
                if (root_tree->GetLeftNode()->GetValue() != "var") {
                    printError(MUST_BE_VAR, *iter);
                }
                else {
                    auto t_tree = root_tree->GetLeftNode();
                    auto var_list = vardParse(t_iter);
                    updateVarTypes(var_list, "label");
                    while (t_tree->GetLeftNode() != nullptr)
                        t_tree = t_tree->GetRightNode();
                    buildVarTree(var_list, t_tree);
                }
                break;
            }

            case begin_tk: {
                root_tree->AddRightTree(compoundParse(t_iter, 0));
                break;
            }

            case dot_tk: {
                if (!error_state) {
                    std::cout << "Program was parse successfully" << std::endl;
                }

                break;
            }

            default: {

                break;
            }
        }
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse variable declaration part
 * @param[inout] t_iter - iterator of table of lexeme
 * @param[inout] t_tree - subtree of part of variables
 *
 * @return  EXIT_SUCCESS - if variable declaration part is matched to grammar
 * @return -EXIT_FAILURE - if variable declaration part doesn't matched to grammar
 */
int Syntax::vardpParse(Syntax::lex_it &t_iter, Tree *t_tree) {
    // var_list contains list of variables from current code line
    auto var_list = vardParse(t_iter);
    auto* tree_value = Tree::CreateNode("");
    bool array_true{ false };

    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);


    if (!checkLexem(t_iter, type_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    getNextLex(t_iter);
    
    bool is_value{ false };
    auto lex_value{ t_iter };
    if (t_iter->GetName() == "=") {
        getNextLex(t_iter);
        if (type_iter->GetName() == "integer") {
            if (!checkLexem(t_iter, constant_tk)) {
                printError(MUST_BE_CONST, *t_iter);
            }
        }
        else {
            if ((!checkLexem(t_iter, false_tk)&& (!checkLexem(t_iter, true_tk)))) {
                printError(MUST_BE_CONST, *t_iter);
            }
        }
        lex_value = t_iter;
        getNextLex(t_iter);
        is_value = true;
    }

    if (!checkLexem(t_iter, semi_tk)) {
        printError(MUST_BE_SEMI, *t_iter);
    }



        updateVarTypes(var_list, type_iter->GetName());
        if (is_value) updateVarValue(var_list, lex_value->GetName());
    



        if (t_tree->GetValue() == "var") {
            while (t_tree->GetLeftNode() != nullptr)
                t_tree = t_tree->GetRightNode();
            buildVarTree(var_list, t_tree);
        }
        else {
            while(t_tree->GetLeftNode() != nullptr)
            t_tree = t_tree->GetRightNode();
            buildVarTree(var_list, t_tree);
        }
        Tree::FreeTree(tree_value);

    auto forwrd_lex = peekLex(1, t_iter);

    if (checkLexem(forwrd_lex, var_tk) || checkLexem(forwrd_lex, id_tk)) {
        if (checkLexem(forwrd_lex, var_tk))
            getNextLex(t_iter);

        vardpParse(t_iter, t_tree->GetRightNode());

    } else if ((forwrd_lex->GetToken() != begin_tk)&&(forwrd_lex->GetToken() != label_tk)) {
        printError(MUST_BE_ID, *forwrd_lex);

    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse line of variables
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return list of variables
 * @note For example, we have next code lines:
 *   program exp1;
 *   var
 *     a, b : integer;    // return { a, b }
 *     c, d, e : integer; // return { c, d, e }
 *   ...
 */
std::list<std::string> Syntax::vardParse(lex_it &t_iter) {
    auto iter = getNextLex(t_iter);

    if (!checkLexem(iter, id_tk)) {
        printError(MUST_BE_ID, *iter);
        return std::list<std::string>();
    }

    if (isVarExist(iter->GetName())) printError(DUPL_ID_ERR, *iter);
    else
        id_map.emplace(iter->GetName(), Variable("?", "?"));

    std::list<std::string> var_list;
    var_list.push_back(t_iter->GetName());

    iter = getNextLex(t_iter);

    if (checkLexem(iter, comma_tk))
        var_list.splice(var_list.end(), vardParse(t_iter));

    return var_list;
}


/**
 * @brief Parse compound part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if compound part is matched to grammar
 * @return -EXIT_FAILURE - if compound part doesn't matched to grammar
 */
Tree *Syntax::compoundParse(lex_it &t_iter, int compound_count) {
    compound_count++;
    int local_lvl = compound_count; // save current compound level
    int sec_prm   = 0;

    auto label = [&]() -> std::string {
        return "_op" + std::to_string(local_lvl) + "." +
        std::to_string(sec_prm);
    };

    auto is_end = [&]() -> bool {
        return (checkLexem(peekLex(1, t_iter), end_tk)
                || checkLexem(peekLex(1, t_iter), eof_tk));
    };

    Tree *tree               = Tree::CreateNode(t_iter->GetName()); // 'begin'
    auto *root_compound_tree = tree; // save the pointer of start of subtree

    while (t_iter->GetToken() != end_tk) {
        if (t_iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *t_iter);
            return nullptr;
        }

        auto *subTree = stateParse(t_iter, compound_count);

        if (subTree != nullptr) {
            if ((subTree->GetLeftNode() == nullptr) && (subTree->GetRightNode() == nullptr)) {
                tree->AddRightNode(subTree->GetValue());
                tree = tree->GetRightNode();
                //meybi///////////////////////////
            }
            else {
                tree->AddRightNode(label(), 0);
                tree->GetRightNode()->AddLeftTree(subTree);
                tree = tree->GetRightNode();
            }

            if (!is_end()) sec_prm++;
        }
    }


    if (compound_count == 1) { // XXX: How can this be replaced?
        if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
                checkLexem(peekLex(1, t_iter), eof_tk)    ||
                !checkLexem(peekLex(1, t_iter), dot_tk)) {
            printError(MUST_BE_DOT, *t_iter);
            return nullptr;
        }

        tree->AddRightNode(t_iter->GetName() + ".", 0);
    } else
        tree->AddRightNode(t_iter->GetName(), 0);

    return root_compound_tree;
}


/**
 * @brief Parse state part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
Tree* Syntax::stateParse(lex_it &t_iter, int compound_count_f) {
    Tree *result_tree = nullptr;
    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case id_tk: {
            if (!isVarExist(iter->GetName())) {
                printError(UNKNOWN_ID, *t_iter);
                return nullptr;
            }

            if ((id_map.find(t_iter->GetName())->second.type == "label")) {
                if (id_map.find(t_iter->GetName())->second.label_is == true) {
                    printError(LABEL_OVER, *t_iter);
                    return nullptr;
                }
                if ((getNextLex(t_iter))->GetToken() != ddt_tk) {
                    printError(MUST_BE_DDT, *t_iter);
                    return nullptr;
                }
                result_tree = Tree::CreateNode(iter->GetName());
                
                id_map.find(getPrevLex(t_iter)->GetName())->second.label_is = true;
                getNextLex(t_iter);

                return result_tree;
            }

            auto var_iter = iter;
            getNextLex(t_iter);
            
            auto* tree_exp = Tree::CreateNode(t_iter->GetName());

            tree_exp = Tree::CreateNode(t_iter->GetName());
            tree_exp->AddLeftNode(var_iter->GetName(), 0);

            auto mult = 0;
            expressionParse(t_iter, tree_exp, mult, id_map.find(var_iter->GetName())->second.type);
            if (tree_exp->GetRightNode() == nullptr) {
                printError(MUST_BE_CONST, *t_iter);
                Tree::FreeTree(tree_exp);
                return nullptr;
            }

            if (!checkLexem(t_iter, semi_tk)&&(!checkLexem(t_iter, else_tk))){
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }
            result_tree = tree_exp;

            break;
        }

        case if_tk:{

            auto* tree_exp = Tree::CreateNode(t_iter->GetName());
            auto mult = 0;
            expressionParse(t_iter, tree_exp, mult, "if");
            if (tree_exp->GetRightNode() == nullptr) { return nullptr; };
            auto sr = tree_exp->GetRightNode()->GetValue();
            if ((sr != "<") && (sr != ">") && (sr != "<=") && (sr != ">=") && (sr != "<>") && (sr != "=")&& (sr != "true") && (sr != "false")) {
                if (!std::isdigit(static_cast<unsigned char>(sr[0]))){
                    if(id_map.find(sr)->second.type != "boolean"){
                        printError(MUST_BE_COMP, *t_iter);
                        Tree::FreeTree(tree_exp);
                        return nullptr;
                    }
                }
                else {
                    printError(MUST_BE_COMP, *t_iter);
                    Tree::FreeTree(tree_exp);
                    return nullptr;
                }
            }


            tree_exp->AddLeftTree(tree_exp->GetRightNode());
            if (iter->GetToken() == if_tk) {
                if (t_iter->GetToken() != then_tk) {
                    printError(MUST_BE_THEN, *t_iter);
                    return nullptr;
                }
            }
            tree_exp->AddRightNode("then");
            auto then_exp = tree_exp->GetRightNode();
            auto var_iter = getNextLex(t_iter);
            result_tree = tree_exp;

            if ((var_iter->GetToken() == id_tk)||(var_iter->GetToken() == begin_tk)|| (var_iter->GetToken() == goto_tk)||(var_iter->GetToken() == if_tk)) {
                var_iter = getPrevLex(var_iter);
                result_tree->GetRightNode()->AddLeftTree(stateParse(var_iter, compound_count_f));
                var_iter->GetName();
            }

            if (var_iter->GetToken() == else_tk) {
                then_exp->AddRightNode("else");
                getNextLex(var_iter);
                if ((var_iter->GetToken() == id_tk) || (var_iter->GetToken() == begin_tk)||(var_iter->GetToken() == goto_tk)) {
                    var_iter = getPrevLex(var_iter);
                    then_exp->GetRightNode()->AddLeftTree(stateParse(var_iter, compound_count_f));
                }
            }
            t_iter = var_iter;
            break;
        }

        

        case begin_tk: {
            auto *tree_comp = compoundParse(t_iter, compound_count_f);
            getNextLex(t_iter);

            if (!checkLexem(t_iter, semi_tk)&&(!checkLexem(t_iter, else_tk))) {
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }

            if (tree_comp != nullptr)
                result_tree = tree_comp;

            break;
        }

        case goto_tk: {
            auto go_to = t_iter;
            if (getNextLex(t_iter)->GetToken() != id_tk) {
                printError(MUST_BE_ID, *t_iter);
                return nullptr;
            }
            auto num_label{ 0 };
            for (auto iter = lex_table.begin(); iter != lex_table.end(); iter++) {
                if (iter->GetName() == t_iter->GetName()) num_label++;
            }
            if (num_label < 2) {
                printError(UNDEF_LABEL, *t_iter);
                return nullptr;
            }
            auto search = id_map.find(t_iter->GetName());
            if (search != id_map.end()) {
                if ((search->second.type) != "label") {
                    printError(INCOMP_TYPES, *t_iter);
                    return nullptr;
                }
                auto* tree_goto = Tree::CreateNode(go_to->GetName());
                tree_goto->AddRightNode(t_iter->GetName());
                result_tree = tree_goto;
            }
            else {
                printError(UNKNOWN_ID, *t_iter);
                return nullptr;
            }
            getNextLex(t_iter);
            if ((t_iter->GetToken() != semi_tk) && (t_iter->GetToken() != else_tk)) {
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }
            

            break;
        }

        default: {
            if (t_iter->GetToken() == else_tk) {
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }
            break;
        }
    }

    return result_tree;
}


/**
 * @brief Parse expression part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */
int Syntax::expressionParse(lex_it &t_iter, Tree *tree, int& mult, std::string var_type) {
    lex_it var_iter = t_iter;
    getPrevLex(var_iter);
    Tree *subTree;

    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case id_tk: {
            if (!isVarExist(iter->GetName()))
                printError(UNKNOWN_ID, *t_iter);
        }

        case constant_tk: {  // like a := 3 ...

            if (var_type == "integer" || var_type == "comp_integer") {//проверка типов
                if (iter->GetToken() != constant_tk) {
                    if (iter->GetToken() != id_tk) {
                        printError(INCOMP_TYPES, *t_iter);
                    }
                    else {
                        if (id_map.find(iter->GetName())->second.type != "integer") {
                            printError(INCOMP_TYPES, *t_iter);
                        }
                    }
                }
            }

            if (var_type == "boolean"||var_type == "comp_boolean") {//проверка типов
                if ((iter->GetToken() != false_tk) && (iter->GetToken() != true_tk)) {
                    if (iter->GetToken() != id_tk) {
                        printError(INCOMP_TYPES, *t_iter);
                    }
                    else {
                        if (id_map.find(iter->GetName())->second.type != "boolean") {
                            printError(INCOMP_TYPES, *t_iter);
                        }
                    }
                }
            }

            var_iter = iter; // save variable/constant value
            getNextLex(iter);
                if (var_type == "if") { 
                    if (var_iter->GetToken() == id_tk) subTree = simplExprParse(var_iter, t_iter, tree, mult, id_map.find(var_iter->GetName())->second.type);
                    if (var_iter->GetToken() == constant_tk) subTree = simplExprParse(var_iter, t_iter, tree, mult, "integer");
                    
                }
                else {
                    if ((var_type == "comp_integer") || (var_type == "comp_boolean")) {
                        subTree = simplExprParse(var_iter, t_iter, tree, mult, "comp_integer");
                    }
                    else {
                        subTree = simplExprParse(var_iter, t_iter, tree, mult, "integer");
                    }
                }

            break;
        }

        case true_tk:
        case false_tk: {

            if (var_type == "integer") {//проверка типов
                if (iter->GetToken() != constant_tk) {
                    if (iter->GetToken() != id_tk) {
                        printError(INCOMP_TYPES, *t_iter);
                    }
                    else {
                        if (id_map.find(iter->GetName())->second.type != "integer") {
                            printError(INCOMP_TYPES, *t_iter);
                        }
                    }
                }
            }

            if (var_type == "boolean") {//проверка типов
                if ((iter->GetToken() != false_tk) && (iter->GetToken() != true_tk)) {
                    if (iter->GetToken() != id_tk) {
                        printError(INCOMP_TYPES, *t_iter);
                    }
                    else {
                        if (id_map.find(iter->GetName())->second.type != "boolean") {
                            printError(INCOMP_TYPES, *t_iter);
                        }
                    }
                }
            }

            var_iter = iter; // save variable/constant value

            subTree = simplExprParse(var_iter, t_iter, tree, mult, "boolean");

            break;
        }

        case sub_tk: { // like a := -3;
            var_iter = t_iter;

            if (getNextLex(t_iter)->GetToken() != constant_tk) {
                if (t_iter->GetToken() == id_tk) {
                    if (id_map.find(t_iter->GetName())->second.type != "integer") {
                        printError(MUST_BE_ID, *t_iter);
                        return -EXIT_FAILURE;
                    }
                }
                else {
                    printError(MUST_BE_ID, *t_iter);
                    return -EXIT_FAILURE;
                }
            }

            tree->AddRightNode(var_iter->GetName());
            tree->GetRightNode()->AddLeftNode("0");
            var_iter = t_iter;
            subTree = simplExprParse(var_iter, t_iter, tree->GetRightNode(), mult, var_type);
            break;
        }

        case opb_tk: { // like a := ( ... );
            mult += 3;
            expressionParse(t_iter, tree, mult, var_type);
            break;

            case cpb_tk: {
                if (getNextLex(t_iter)->GetToken() != semi_tk) {
                    mult -= 3;
                    t_iter = getPrevLex(iter);
                    lex_table.erase(getNextLex(iter));
                    getPrevLex(t_iter);
                    expressionParse(t_iter, tree, mult, var_type);
                } else {
                    mult -= 3;
                    var_iter = getPrevLex(iter);
                    t_iter = var_iter;
                    getNextLex(iter);
                    lex_table.erase(iter);
                    simplExprParse(var_iter, t_iter, tree, mult, var_type);
                }

                break;
            }

            case semi_tk: {
                if (mult > 0) {
                    printError(MUST_BE_BKT_END, *t_iter);
                    return -EXIT_FAILURE;
                }

                if (mult < 0) {
                    printError(MUST_BE_BKT_BGN, *t_iter);
                    return -EXIT_FAILURE;
                }

                break;
            }
        }

        default: {
            printError(MUST_BE_ID, *t_iter);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse subexpression part
 * @param[in]    var_iter - iterator, which point to the variable (id/number)
 * @param[inout] t_iter   - iterator of table of lexeme
 * @param[inout] tree     - current subtree
 *
 * @return subtree of subexpression
 */
Tree *Syntax::simplExprParse(const Syntax::lex_it &var_iter,
                             Syntax::lex_it &t_iter, Tree *tree, int& mult, std::string var_type) {
    Tree *subTree;

    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case add_tk:
        case sub_tk:
        case mul_tk:
        case div_op_tk:
        case comp_tk:{
            if (((iter->GetName() != "=")&&(iter->GetName() != "<>")&&(iter->GetName() != "<") && (iter->GetName() != ">") 
                 && (iter->GetName() != "<=") && (iter->GetName() != ">=")) && (var_type == "boolean")) {
                printError(INCOMP_TYPES, *t_iter);
            }
            if ((iter->GetToken() == comp_tk) && (var_type == "comp_integer") || (var_type == "comp_boolean")) {
                printError(INCOMP_TYPES, *t_iter);//возможно не та ошибка
            }
            if (iter->GetToken() == comp_tk) {
                if (var_type == "integer") {
                    var_type = "comp_integer";
                }
                if (var_type == "boolean") {
                    var_type = "comp_boolean";
                }
            }

            if (operations.at(iter->GetName()) + mult <=
                    (tree->GetPriority())) {       // Priority of current <=
                tree->AddRightNode(var_iter->GetName());
                subTree = tree->GetParentNode();

                while (operations.at(iter->GetName()) + mult <= // go through parents
                        (subTree->GetPriority()))
                    subTree = subTree->GetParentNode();

                subTree = createLowestOpTree(subTree, iter->GetName(),
                                             operations.at(iter->GetName()) + mult);
            } else { // if Priority of current >
                /******* Create a new node of subexpression ************/
                tree->AddRightNode(iter->GetName(),
                                   operations.at(iter->GetName()) + mult);            //     <oper> <- subTree
                subTree = tree->GetRightNode();                 //      /  /
                subTree->AddLeftNode(var_iter->GetName(), 0);      //    val  nullptr
                /********************************************************/
            }

            expressionParse(t_iter, subTree, mult, var_type);
            break;
        }

        default: { // any other lexem, expression is over
            if (iter->GetToken() == cpb_tk) {

                getPrevLex(t_iter);
                expressionParse(t_iter, tree, mult, "");
            } else {
                if (mult != 0) {
                    getPrevLex(t_iter);
                    expressionParse(t_iter, tree, mult, var_type);
                }

                tree->AddRightNode(var_iter->GetName(), 0);
            }

            break;
        }
    }

    return tree;
}


/**
 * @brief Parse subexpression part
 * @param[in]    var_tree - Tree, which the variable
 * @param[inout] t_iter   - iterator of table of lexeme
 * @param[inout] tree     - current subtree
 *
 * @return subtree of subexpression
 */
Tree* Syntax::simplExprParse(Tree* var_tree,
    Syntax::lex_it& t_iter, Tree* tree, int& mult, std::string type_var) {
    Tree* subTree;

    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
    case add_tk:
    case sub_tk:
    case mul_tk:
    case div_op_tk:
    case comp_tk: {

        if (operations.at(iter->GetName()) + mult <=
            (tree->GetPriority())) {       // Priority of current <=
            tree->AddRightTree(var_tree);
            subTree = tree->GetParentNode();

            while (operations.at(iter->GetName()) + mult <= // go through parents
                (subTree->GetPriority()))
                subTree = subTree->GetParentNode();

            subTree = createLowestOpTree(subTree, iter->GetName(),
                operations.at(iter->GetName()) + mult);
        }
        else { // if Priority of current >
         /******* Create a new node of subexpression ************/
            tree->AddRightNode(iter->GetName(),
                operations.at(iter->GetName()) + mult);            //     <oper> <- subTree
            subTree = tree->GetRightNode();                 //      /  /
            subTree->AddLeftTree(var_tree);      //    val  nullptr
            /********************************************************/
        }

        expressionParse(t_iter, subTree, mult, "");
        break;
    }

    default: { // any other lexem, expression is over
        if (iter->GetToken() == cpb_tk) {

            getPrevLex(t_iter);
            expressionParse(t_iter, tree, mult, type_var);
        }
        else {
            if (mult != 0) {
                getPrevLex(t_iter);
                expressionParse(t_iter, tree, mult, type_var);
            }

            tree->AddRightTree(var_tree);
        }

        break;
    }
    }

    return tree;
}

/**
 * @brief Print information about error
 * @param[in] t_err - error type
 * @param[in] lex   - error lexeme
 *
 * @return none
 */
void Syntax::printError(errors t_err, Lexem lex) {
    error_state = 1;

    switch (t_err) {
        case UNKNOWN_LEXEM: {
            std::cerr << "<E> Lexer: Get unknown lexem '" << lex.GetName()
                      << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case EOF_ERR: {
            std::cerr << "<E> Syntax: Premature end of file" << std::endl;
            break;
        }

        case MUST_BE_ID: {
            std::cerr << "<E> Syntax: Must be identifier or begin instead '" << lex.GetName()
                      << "' on " << lex.GetLine() << " line"       << std::endl;
            break;
        }

        case MUST_BE_SEMI: {
            std::cerr << "<E> Syntax: Must be ';' instead '" << lex.GetName()
                      << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case MUST_BE_PROG: {
            std::cerr << "<E> Syntax: Program must start from lexem 'program' ("
                      << lex.GetLine() << ")" << std::endl;
            break;
        }

        case MUST_BE_COMMA: {
            std::cerr << "<E> Syntax: Must be ',' instead '" << lex.GetName()
                      << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case DUPL_ID_ERR: {
            std::cerr << "<E> Syntax: Duplicate identifier '" << lex.GetName()
                      << "' on " << lex.GetLine() << " line"  << std::endl;
            break;
        }

        case UNKNOWN_ID: {
            std::cerr << "<E> Syntax: Undefined variable '"  << lex.GetName()
                      << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case MUST_BE_DOT: {
            std::cerr << "<E> Syntax: Program must be end by '.'" << std::endl;
            break;
        }

        case MUST_BE_BKT_BGN: {
            std::cerr << "<E> Syntax: Must be '(' on "  << lex.GetLine() << " line" << std::endl;
            break;
        }

        case MUST_BE_BKT_END: {
            std::cerr << "<E> Syntax: Must be ')' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case MUST_BE_THEN: {
            std::cerr << "<E> Syntax: Must be 'then' on " << lex.GetLine() << " line" <<
                      std::endl;
            break;
        }

        case INCOMP_TYPES: {
            std::cerr << "<E> Syntax: Incompatible types " << lex.GetLine() << " line" <<
                      std::endl;
            break;
        }

        case LABEL_OVER: {
            std::cerr << "<E> Syntax: Label already defined " << lex.GetLine() << " line" <<
                std::endl;
            break;
        }
        case UNDEF_LABEL: {
            std::cerr << "<E> Syntax: Undefined label " << lex.GetLine() << " line" <<
                std::endl;
            break;
        }

        case MUST_BE_VAR: {
            std::cerr << "<E> Syntax: Must be  'var' on " << lex.GetLine() << " line" <<
                std::endl;
            break;
        }
        case MUST_BE_COMP: {
            std::cerr << "<E> Syntax: Must be  '>, <, <= ...' on " << lex.GetLine() << " line" <<
                std::endl;
            break;
        }
        case MUST_BE_TYPE: {
            std::cerr << "<E> Syntax: Must be type of id instead '" << lex.GetName()
                << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }
        case MUST_BE_CONST:{
             std::cerr << "<E> Syntax: Must be constant or id instead '" << lex.GetName()
             << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        default: {
            std::cerr << "<E> Syntax: Undefined type of error" << std::endl;
            break;
        }
    }
}


/**
 * @brief Get next lexeme
 * @param[inout] iter - cursor-iterator of lexeme table
 *
 * @return iterator on next lexeme
 */
Syntax::lex_it Syntax::getNextLex(lex_it &iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }

    return iter;
}


/**
 * @brief Get prev lexeme
 * @param[inout] iter - cursor-iterator of lexeme table
 *
 * @return iterator on prev lexeme
 */
Syntax::lex_it Syntax::getPrevLex(lex_it& iter) {
    try {
        if (iter != lex_table.begin())
            iter--;
    } catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }

    return iter;
}


/**
 * @brief Peek to forward on the N lexeme
 * @param[in] N      - the number of skipped tokens
 * @param[in] t_iter - copy of cursor-iterator of lexeme table
 *
 * @return copy of iterator on N-th lexeme (token)
 * @note If catch exception, return copy of iterator
 */
Syntax::lex_it Syntax::peekLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;

        while (iter != lex_table.end()) {
            if (N == 0) return iter;

            iter++;
            N--;
        }

        return iter;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Can't peek so forward" << std::endl;
        return t_iter;
    }
}


/**
 * @brief Check does current lexeme is match to needed token
 * @param[in] t_iter - current lexem
 * @param[in] t_tok  - needed token
 *
 * @return true  - if lexeme is match
 * @return false - if lexeme is unreachable (end) or if doesn't match
 */
bool Syntax::checkLexem(const Syntax::lex_it &t_iter, const tokens &t_tok) {
    if (t_iter == lex_table.end())   return false;

    if (t_iter->GetToken() != t_tok) return false;

    return true;
}


/**
 * @brief Check existence of variable
 * @param[in] t_var_name - variable for check
 *
 * @return true  - if variable is exist
 * @return false - if unknown variable (doesn't exist)
 */
bool Syntax::isVarExist(const std::string &t_var_name) {
    auto map_iter = id_map.find(t_var_name);
    return !(map_iter == id_map.end());
}

/**
 * @brief Update information about type in map of identifiers
 * @param[in] t_var_list  - list of variables
 * @param[in] t_type_name - type of variables
 *
 * @return none
 */
void Syntax::updateVarTypes(const std::list<std::string>& t_var_list,
    const std::string& t_type_name) {
    try {
        for (auto& el : t_var_list)
            id_map.at(el).type = t_type_name;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }
}

/**
 * @brief Update information about value in map
 * @param[in] t_var_list  - list of variables
 * @param[in] t_value - value of variables
 *
 * @return none
 */
void Syntax::updateVarValue(const std::list<std::string>& t_var_list,
    const std::string& t_value) {
    try {
        for (auto& el : t_var_list)
            id_map.at(el).value = t_value;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }
}


/**
 * @brief Build subtree of variable declaration part
 * @param[in]  t_var_list - list of variable
 * @param[out] t_tree     - subtree of variable part
 *
 * @return none
 * @note If we firstly call this method:
 *                              program_name
 *                               /         \
 *             t_tree here ->  var         <block>
 */
void Syntax::buildVarTree(const std::list<std::string> &t_var_list, Tree *t_tree) {
    try {
        auto i = 0;

        for (auto &el : t_var_list) {
            auto *tmp_tree  = Tree::CreateNode(el);
            tmp_tree->AddRightNode(id_map.at(el).type, 0);
            if(id_map.at(el).value != "?") tmp_tree->AddLeftNode(id_map.at(el).value, 0);
            createVarTree(t_tree, tmp_tree, i++);
        }
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}


/**
 * @brief Build subtree of variable declaration part
 * @param[in]  t_var_list - list of variable
 * @param[out] t_tree     - subtree of variable part
 * @param[in] array_tree  - addition tree for array
 *
 * @return none
 * @note If we firstly call this method:
 *                              program_name
 *                               /         \
 *             t_tree here ->  var         <block>
 */
void Syntax::buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree,
                          Tree* array_tree) {
    try {
        auto i = 0;

        for (auto& el : t_var_list) {
            auto* tmp_tree = Tree::CreateNode(el);
            tmp_tree->AddRightTree(array_tree);
            array_tree->AddRightNode(id_map.at(el).type, 0);
            createVarTree(t_tree, tmp_tree, i++);
        }
    } catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}


/**
 * @brief Insert subtree of <var dec> part to tree <var part>
 * @param[out] t_tree       - current node (look on var/$ root)
 * @param[in]  t_donor_tree - tree with information about identifier
 * @param[in]  lvl          - level of recursion
 *
 * @return none
 * @note How look t_tree:
 *                      program_name
 *                       /      \
 *                     var     <block>
 *                     / \
 *       <t_donor_tree>  <t_tree>
 *                       / \
 *         <t_donor_tree>  $
 *                        etc.
 *
 * How look t_donor_tree:
 *                  a           <id>
 *                   \             \
 *                   integer       <type>
 */
void Syntax::createVarTree(Tree *t_tree, Tree *t_donor_tree, int lvl) {
    if (lvl > 0) {
        lvl--;
        createVarTree(t_tree->GetRightNode(), t_donor_tree, lvl);
    } else {
        t_tree->AddLeftTree(t_donor_tree);
        t_tree->AddRightNode("$", 0);
    }
}


/**
 * @brief Create subtree with lowest operator priority
 * @param[in] t_parent_tree - pointer to parent tree of subtree with lowest operator
 * @param[in] value         - value of the lowest subtree
 *
 * @return pointer to subtree with the lowest operator
 * @note We find in main tree the subtree with equal operator (between value)
 *   and set this subtree like the parent tree (t_parent_tree) for new
 *   lowest subtree.
 *   All children nodes of t_parent_tree will be set like children nodes of
 *   the lowest operator subtree
 */
Tree* Syntax::createLowestOpTree(Tree *t_parent_tree, std::string value,
                                 int priority_) {
    auto *lowest_tree = Tree::CreateNode(t_parent_tree, value, priority_);
    lowest_tree->AddLeftTree(t_parent_tree->GetRightNode());
    t_parent_tree->AddRightTree(lowest_tree);

    return lowest_tree;
}