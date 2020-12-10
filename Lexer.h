#ifndef LECS_PARS_LEXER_H
#define LECS_PARS_LEXER_H

#include <fstream>
#include <iostream>
#include <vector>
#include "Lexem.h"

class Lexer {
public:
    explicit Lexer(const char *file_path);
    std::vector<Lexem> ScanCode();
    ~Lexer();
private:
    std::ifstream code;
    char cursor {-1};
    int line {1};
    std::vector<Lexem> lex_table;

    Lexem GetLex();
    char GetChar();

    inline char GetCurrentCurs(){
        return cursor;
    }
};


#endif //LECS_PARS_LEXER_H
