#include "Lexem.h"
/**
 * @brief Return name of lexeme
 * @param none
 *
 * @return name of lexeme
 */
const std::string Lexem::GetName() 
{
    return name;
}
tokens Lexem::GetToken() const 
{
    return token;
}

int Lexem::GetLine() 
{
        return line;
}
void Lexem::RenameLex(std::string name_) 
{
    name = name_;
}