#include "lab_01.h"
#include "C:/Users/sasha/source/repos/laboratory1/SyntaxTree.h"


int Parse_01(const std::string& file_path) {

    Lexer lex(file_path.c_str());
    auto table = lex.ScanCode(); //table of lexemes

    /*SyntaxNode* tree = new SyntaxNode(NODE_PROGRAM);
    tree->ShowTree();*/

    Syntax syntx(std::move(table));
    syntx.ParseCode();


    return EXIT_SUCCESS;
}