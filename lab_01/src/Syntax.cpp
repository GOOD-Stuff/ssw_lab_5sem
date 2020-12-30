#include "Syntax.h"


Syntax::Syntax(std::vector<Lexem>&& t_lex_table) {
    if (t_lex_table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty.");
    if (t_lex_table.at(0).GetToken() == eof_tk)
        throw std::runtime_error("<E> Syntax: Code file is empty.");
    lex_table = t_lex_table;
    cursor = lex_table.begin();
}


Syntax::~Syntax() {
}


/**
 * XXX: Each block (..Parse) builds its subtree (and returns it),
 *   which is then added at a higher level, at the place of the method call.
 */

 /**
  * @brief Start parse incoming pascal file
  * @param none
  *
  * @return  EXIT_SUCCESS - if file was successful parsed
  * @return -EXIT_FAILURE - if can't parse incoming file
  */
int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes." << std::endl;
    auto& it = cursor;
    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while (it != lex_table.end() && it->GetToken() != dot_tk)
        blockParse(it);
    std::cout << "End of file..." << std::endl;

    return EXIT_SUCCESS;
}


/**
 * @brief Parse entry point in grammar
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if input program part is correct
 * @return -EXIT_FAILURE - if input program part is incorrect
 * @note Wait input like: program <id_tk> ;
 */
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

    return EXIT_SUCCESS;
}


/**
 * @brief Parse block part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if block part is matched to grammar
 * @return -EXIT_FAILURE - if block part doesn't matched to grammar
 */
int Syntax::blockParse(lex_it& t_iter) {
    try {
        auto iter = getNextLex(t_iter);
        switch (iter->GetToken())
        {
        case var_tk: {
            vardpParse(t_iter);
            break;
        }
        case begin_tk: {
            compoundParse(t_iter, 0);
            break;
        }
        case dot_tk: {
            if (!error_state)
                std::cout << "Program was parse successfully :)" << std::endl;

            break;
        }
        default: {
            break;
        }
        }
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << "." << std::endl;
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
int Syntax::vardpParse(Syntax::lex_it& t_iter) {
    // var_list contains list of variables from current code line
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


/**
 * @brief Parse compound part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if compound part is matched to grammar
 * @return -EXIT_FAILURE - if compound part doesn't matched to grammar
 */
int Syntax::compoundParse(lex_it& t_iter, int compound_count) {
    compound_count++;
    while (t_iter->GetToken() != end_tk) {
        if (t_iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *t_iter);
            return -EXIT_FAILURE;
        }
        stateParse(t_iter, compound_count);
    }

    if (compound_count == 1) { // XXX: How can this be replaced?
        if (!checkLexem(peekLex(1, t_iter), dot_tk)) {
            printError(MUST_BE_DOT, *t_iter);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse state part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
int Syntax::stateParse(lex_it& t_iter, int compound_count) {
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
        compoundParse(t_iter, compound_count);
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


/**
 * @brief Parse expression part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */
int Syntax::expressionParse(lex_it& t_iter) {
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken())
    {



    case id_tk: {
        if (!isVarExist(iter->GetName()))
            printError(UNKNOWN_ID, *t_iter);
    }
    case constant_tk: { // like a := 3 ...
        iter = getNextLex(t_iter);
        switch (iter->GetToken()) {
        case add_tk:
            expressionParse(t_iter);
            break;
        case sub_tk:
            expressionParse(t_iter);
            break;
        default: { // any other lexem, expression is over
            break;
        }
        }
        break;
    }
    case sub_tk: { // like a := -3;
        break;
    }
    default: {
        printError(MUST_BE_ID, *t_iter);
        return -EXIT_FAILURE;
    }
    }

    return EXIT_SUCCESS;
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
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case EOF_ERR: {
        std::cerr << "<E> Syntax: Premature end of file or perhaps another end is missing." << std::endl;
        break;
    }
    case MUST_BE_ID: {
        std::cerr << "<E> Syntax: Must be identifier instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case MUST_BE_SEMI: {
        std::cerr << "<E> Syntax: Must be ';' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case MUST_BE_PROG: {
        std::cerr << "<E> Syntax: Program must start from lexem 'program' ("
            << lex.GetLine() << ")." << std::endl;
        break;
    }
    case MUST_BE_COMMA: {
        std::cerr << "<E> Syntax: Must be ',' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case DUPL_ID_ERR: {
        std::cerr << "<E> Syntax: Duplicate identifier '" << lex.GetName()
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case UNKNOWN_ID: {
        std::cerr << "<E> Syntax: Undefined variable '" << lex.GetName()
            << "' on " << lex.GetLine() << " line." << std::endl;
        break;
    }
    case MUST_BE_DOT: {
        std::cerr << "<E> Syntax: Program must be end by '.'" << lex.GetName()
            << "' on " << lex.GetLine() << " line or he is not defined (one begin - one end)." << std::endl;
        break;
    }
                    // TODO: Add remaining error types
    default: {
        std::cerr << "<E> Syntax: Undefined type of error or some symbol is not used." << std::endl;
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
Syntax::lex_it Syntax::getNextLex(lex_it& iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << "." << std::endl;
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
            iter++; N--;
        }

        return iter;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Can't peek so forward." << std::endl;
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
bool Syntax::checkLexem(const Syntax::lex_it& t_iter, const tokens& t_tok) {
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
bool Syntax::isVarExist(const std::string& t_var_name) {
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
            << exp.what() << "." << std::endl;
    }
}