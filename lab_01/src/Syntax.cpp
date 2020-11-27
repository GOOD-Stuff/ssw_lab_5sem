#include "Syntax.h"


Syntax::Syntax(std::vector<Lexem>&& t_lex_table) {
    if (t_lex_table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty");
    if (t_lex_table.at(0).GetToken() == eof_tk)
        throw std::runtime_error("<E> Syntax: Code file is empty");
    lex_table = t_lex_table;
    cursor = lex_table.begin();
}

Syntax::~Syntax() {
    // buildtree
}


int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto& it = cursor;
    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while (it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);
    std::cout << "EOF" << std::endl;

    return EXIT_SUCCESS;
}

Syntax::lex_it Syntax::getNextLex(lex_it& iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }

    return iter;
}

Syntax::lex_it Syntax::peekLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter++; N--;
        }

        return iter;
    }
    catch (const std::exception &) {
        std::cerr << "<E> Syntax: Can't peek so forward" << std::endl;
        return t_iter;
    }
}

int Syntax::programParse(lex_it& t_iter) {
    if (!checkLexem(t_iter, program_tk)) {
        printError(MUST_BE_PROG, *t_iter);
        return -EXIT_FAILURE;
    }

    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        }
        else {
            printError(MUST_BE_ID, *iter);
            return -EXIT_FAILURE;
        }
    }

    //auto root_name = iter->GetName(); // save the name of program

    iter = getNextLex(t_iter);
    if (!checkLexem(iter, semi_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        }
        else {
            printError(MUST_BE_SEMI, *iter);
            return -EXIT_FAILURE;
        }
    }

    // First phase is OK, we can start to build the tree
    //pascal_tree = createNode(root_name);

    return EXIT_SUCCESS;
}

int Syntax::blockParse(lex_it& t_iter) {
    try {
        auto iter = getNextLex(t_iter);
        switch (iter->GetToken()) {
        case var_tk: {
            // pascal_tree->left_node = buildTreeStub(pascal_tree, "var");
            vardpParse(t_iter); //(t_iter, pascal_tree->left_node);
            break;
        }
        case begin_tk: {
            compoundParse(t_iter, 0);
            break;
        }
        case dot_tk: {
            std::cout << "Program was parse successfully" << std::endl;
            break;
        }
        default: {
            break;
        }
        }
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::list<std::string> Syntax::vardParse(lex_it& t_iter) {
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

int Syntax::vardpParse(lex_it& t_iter) {
    auto var_list = vardParse(t_iter);
    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);
    if (!checkLexem(t_iter, type_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        printError(MUST_BE_SEMI, *t_iter);
    }

    updateVarTypes(var_list, type_iter->GetName());
    // buildVarTree(var_list, t_tree);

    return EXIT_SUCCESS;
}

int Syntax::stateParse(lex_it& t_iter, int c_count) {
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case id_tk: {
        if (!isVarExist(iter->GetName())) {
            printError(UNKNOWN_ID, *t_iter);
            return -EXIT_FAILURE;
        }

        getNextLex(t_iter);
        if (!checkLexem(t_iter, ass_tk)) {
            printError(MUST_BE_ASS, *t_iter);
            return -EXIT_FAILURE;
        }

        expressionParse(t_iter);
        if (!checkLexem(t_iter, semi_tk)) { // we exit from expression on the ';'
            printError(MUST_BE_SEMI, *t_iter);
            return -EXIT_FAILURE;
        }
        break;
    }
    case begin_tk: {
        compoundParse(t_iter, c_count);
        getNextLex(t_iter);
        if (!checkLexem(t_iter, semi_tk)) {
            printError(MUST_BE_SEMI, *t_iter);
            return -EXIT_FAILURE;
        }
        break;
    }
    default: {
        break;
    }
    }

    return EXIT_SUCCESS;
}

int Syntax::compoundParse(lex_it& t_iter, int c_count) {
    //static int compound_count = 0; 
    c_count++;
    while (t_iter->GetToken() != end_tk) {
        if (t_iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *t_iter);
            return -EXIT_FAILURE;
        }
        stateParse(t_iter, c_count);
    }

    if (c_count == 1) { 
        if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
            checkLexem(peekLex(1, t_iter), eof_tk)) {
            printError(MUST_BE_DOT, *t_iter);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int Syntax::expressionParse(lex_it& t_iter) {
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case id_tk: {
        if (!isVarExist(iter->GetName()))
            printError(UNKNOWN_ID, *t_iter);
        if (isVarDuplicate(iter->GetName()))
            printError(DUPL_ID_ERR, *t_iter);
    }
    case constant_tk: { // like a := 3 ...
        iter = getNextLex(t_iter);
        switch (iter->GetToken()) {
        case plus_tk:
        case minus_tk:
            expressionParse(t_iter);
            break;
        default: { // any other lexem, expression is over
            break;
        }
        }
        break;
    }
    case minus_tk: { // like a := -3;
        break;
    }
    default: {
        printError(MUST_BE_ID, *t_iter);
        return -EXIT_FAILURE;
    }
    }

    return EXIT_SUCCESS;
}

void Syntax::printError(errors t_err, Lexem lex) {
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
        std::cerr << "<E> Syntax: Must be identifier instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
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
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case UNKNOWN_ID: {
        std::cerr << "<E> Syntax: Undefined variable '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_DOT: {
        std::cerr << "<E> Syntax: Program must be end by '.'" << std::endl;
        break;
    }
                    // TODO: Add remaining error types
    default: {
        std::cerr << "<E> Syntax: Undefined type of error" << std::endl;
        break;
    }
    }
}

bool Syntax::checkLexem(const lex_it& t_iter, const tokens& t_tok) {
    if (t_iter == lex_table.end())   return false;
    if (t_iter->GetToken() != t_tok) return false;

    return true;
}

bool Syntax::isVarExist(const std::string& t_var_name) {
    auto map_iter = id_map.find(t_var_name);
    return !(map_iter == id_map.end());
}

bool Syntax::isVarDuplicate(const std::string& t_var_name) {
    auto map_iter = id_map.find(t_var_name);
    if (map_iter == id_map.end()) {
        return 0;
    }
    else return 1;
}

void Syntax::updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name) {
    try {
        for (auto& el : t_var_list)
            id_map.at(el).type = t_type_name;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }
}


