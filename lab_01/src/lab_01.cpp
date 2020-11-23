#include "lab_01.h"


/**
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <your_other_operations>
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */


int Parse(const std::string& file_path) {

    Lexer lex(file_path.c_str());
    auto table = lex.ScanCode(); // create table of lexemes

    Syntax syntx(std::move(table));
    syntx.ParseCode();          // start parse code

    return EXIT_SUCCESS;
}
