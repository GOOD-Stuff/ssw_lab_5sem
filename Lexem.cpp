//
// Created by vovan on 28.09.2019.
//

#include "Lexem.h"


tokens Lexem::GetToken() const {
    return token;
}


/**
 * @brief Return line (from pascal file) of lexeme
 * @param none
 *
 * @return line of lexeme
 */
int Lexem::GetLine() const {
    return line;
}