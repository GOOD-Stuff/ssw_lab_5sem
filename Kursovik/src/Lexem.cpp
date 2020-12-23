#include "Lexem.h"


/**
 * @brief Return name of lexeme
 * @param none
 *
 * @return name of lexeme
 */
const std::string Lexem::GetName() {
    return name;
}


/**
 * @brief Return type of lexeme
 * @param none
 *
 * @return type (token) of lexeme
 */
tokens Lexem::GetToken() const {
    return token;
}


/**
 * @brief Return line (from pascal file) of lexeme
 * @param none
 *
 * @return line of lexeme
 */
int Lexem::GetLine() {
    return line;
}

/**
 * @brief Rename lexem
 * @param[in] name_ - new name
 *
 * @return none
*/
void Lexem::RenameLex(std::string name_) {
    name = name_;
}