#include "Lexem.h"

const std::string& Lexem::GetName() const {
    return name;
}

Tokens Lexem::GetToken() const {
    return token;
}

int Lexem::GetLine() const {
    return line;
}