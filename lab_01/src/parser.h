#ifndef LAB_01_PARSER_H
#define LAB_01_PARSER_H

#include <iostream>
#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <vector>
#include "Lexem.h"
#include "Variable.h"

class Syntax
{
public:
    explicit Syntax(std::vector<Lexem>&& t_lex_table);
    ~Syntax();
    int ParseCode();

private:
    using lex_it = std::vector<Lexem>::iterator;    // alias of vector iterator
    lex_it                          cursor;         // cursor in table of lexemes
    std::vector<Lexem>              lex_table;       // our table of lexemes
    std::map<std::string, Variable> id_map;         // our table of identifiers
    bool                            error_state{0};

    int   blockParse(lex_it& t_iter);
    int   compoundParse(lex_it& t_iter, int compound_count);
    int   expressionParse(lex_it& t_iter);
    int   programParse(lex_it& t_iter);
    int   stateParse(lex_it& t_iter, int compound_count);
    int   vardpParse(lex_it& t_iter);
    std::list<std::string> vardParse(lex_it& t_iter);

    void printError(errors t_err, Lexem lex);
    void updateVarTypes(const std::list<std::string>& t_var_list,const std::string& t_type_name);

    lex_it getNextLex(lex_it& iter);
    lex_it peekLex(int N, lex_it t_iter);

    bool checkLexem(const lex_it& t_iter, const tokens& t_tok);
    bool isVarExist(const std::string& t_var_name);
};

#endif //LAB_01_PARSER_H
