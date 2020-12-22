#include "lab_04.h"


/**
 *
 * program name;
 * var a : integer;
 *     b : integer;
 *     c : array[1..5] of integer;
 * begin
 * b := 1;
 * a := 2;
 * end.
 *
 *
 *             table of lexem:
 *  <lexem_str>, <lexem_id>, <line in code>
 */


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
    auto table = lex.ScanCode();

    Syntax syntx(std::move(table));
    syntx.ParseCode();
    auto tree = syntx.retTree();

    if (tree == nullptr) {
        std::cerr << "<E> Incorrect syntax tree, abort!" << std::endl;
        return -EXIT_FAILURE;
    }

    GenCode genCode(std::move(*tree));
    genCode.GenerateAsm();

    return EXIT_SUCCESS;
}
