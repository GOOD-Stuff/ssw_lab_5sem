#include "compiller.h"


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


int Parse(const std::string& file_path, const std::string& path_dest) {
    Lexer lex(file_path.c_str());
    auto table = lex.ScanCode();

    if (table.empty()) {
        return 0;
    }

    std::cout <<
              "***************************Table of lexem ************************************\n";
    std::setiosflags(std::ios::left);

    for (auto i = 0; i < table.size(); i++) {//out lex
        std::cout << "Name: " <<  std::setw(10) << table[i].GetName() << std::setw(
                      10) << "Token: " << std::setw(10)
                  << table[i].GetToken() << std::setw(10) << "Line: " << std::setw(
                      10) << table[i].GetLine() << std::endl;
    }

    std::cout <<
              "******************************************************************************" <<
              std::endl;

    Syntax syntx(std::move(table));
    auto status = syntx.ParseCode();
    auto tree = syntx.retTree();

    if ((tree == nullptr)||(status)) {
        std::cerr << "<E> Incorrect syntax tree, abort!" << std::endl;
        return -EXIT_FAILURE;
    }

    GenCode genCode(std::move(*tree), path_dest);
    genCode.GenerateAsm();

    return EXIT_SUCCESS;
}
