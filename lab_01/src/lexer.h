#ifndef LAB_01_Lexer_H
#define LAB_01_Lexer_H

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "Lexem.h"

class Lexer 
{
public:

    explicit Lexer(const char* file_path);
    std::vector<Lexem>	ScanCode();
    ~Lexer();

private:

    char				cursor{-1};
    char				GetChar();
    int					line{0};
    std::vector<Lexem>	lex_table;
    std::ifstream		code;
    Lexem				GetLex();
    inline char			GetCurrentCurs() {return cursor;}
};

#endif //LAB_01_LEXER_H
