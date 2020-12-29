#ifndef LECS_PARS_LEXEM_H
#define LECS_PARS_LEXEM_H

#include <string>

enum Tokens {
    TOKEN_UNKNOWN = -1,    // we get unknown token
    TOKEN_PROGRAM = 0,     // 'program'
    TOKEN_VAR,             // 'var'
    TOKEN_BEGIN,           // 'begin'
    TOKEN_END,             // 'end'
    TOKEN_TYPE,            // 'type'
    TOKEN_ID = 8,    // any [aA-zZ][0-9]
    TOKEN_CONSTANT = 9,    // 0-9
    TOKEN_DOT,             // '.'
    TOKEN_COMMA,           // ','
    TOKEN_DDT,             // ':'
    TOKEN_SEMI,            // ';'
    TOKEN_EQV,             // '='
    TOKEN_ASS,             // ':='
    TOKEN_ADD,             // '+'
    TOKEN_SUB,             // '-'
    TOKEN_MUL,             // '*'
    TOKEN_DIV,             // '/'
    TOKEN_OPB,             // '('
    TOKEN_CPB,             // ')'

    TOKEN_EOF
};


enum errors {
    UNKNOWN_LEXEM,   // Unknown lexeme
    EOF_ERR,         // End of file error
    MUST_BE_ID,      // Error: Must be identifier
    MUST_BE_SEMI,    // Error: Must be ';'
    MUST_BE_PROG,    // Error: Must be 'program'
    MUST_BE_COMMA,   // Error: Must be ','
    MUST_BE_TYPE,    // Error: Must be type of identifier
    MUST_BE_DOT,     // Error: Must be '.'
    MUST_BE_ASS,     // Error: Must be ':='
    DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
    UNKNOWN_ID,      // Error: Undefined identifier

};

class Lexem {
public:
    Lexem() = default;
    Lexem(std::string &&t_name, Tokens t_tok, int t_line) : name(t_name), token(t_tok), line(t_line) {};

    const std::string& GetName() const;
    Tokens GetToken() const;
    int GetLine() const;

private:
    std::string name{""};
    Tokens token{ TOKEN_UNKNOWN };
    int line{0};
};


#endif LECS_PARS_LEXEM_H