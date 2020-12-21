#include <iostream>
#include <string>
#include "lab_04.h"
/*#include "GenCode.cpp"
#include "Lexem.cpp"
#include "Lexer.cpp"
#include "Syntax.cpp"
#include "Tree.cpp"
#include "Variable.cpp"*/

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string path = "test.p";
    Parse4(path);
    return 0;
}
