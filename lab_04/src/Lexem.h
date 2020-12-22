#ifndef LECS_PARS_LEXEM_H
#define LECS_PARS_LEXEM_H

#include <string>


enum tokens {
    unknown_tk = -1,    // we get unknown token
    program_tk = 0,     // 'program'
    var_tk,             // 'var'
    begin_tk,           // 'begin'
    end_tk,             // 'end'
    type_tk,            // 'type'
    id_tk       = 8,    // any [aA-zZ][0-9]
    constant_tk = 9,    // 0-9
    dot_tk,             // '.'
    comma_tk,           // ','
    ddt_tk,             // ':'
    semi_tk,            // ';'
    eqv_tk,             // '='
    ass_tk,             // ':='
    add_tk,             // '+'
    sub_tk,             // '-'
    mul_tk,             // '*'
    opb_tk,             // '('
    cpb_tk,             // ')'
    div_op_tk,          // 'div'
    if_tk,              // 'if'
    then_tk,            // 'then'
    else_tk,            // 'else'
    true_tk,            // 'true'
    false_tk,           // 'false'
    while_tk,           // 'while'
    do_tk,              // 'do'
    for_tk,             // 'for'
    to_tk,              // 'to'
    downto_tk,          // 'downto'
    or_tk,              // 'or'
    and_tk,             // 'and'
    xor_tk,             // 'xor'
    array_tk,           // 'array'
    of_tk,              // 'of'
    sqbrleft_tk,        // '['
    sqbrright_tk,       // ']'
    comp_tk,

    eof_tk              // end of file
};


enum errors {
    UNKNOWN_LEXEM,   // Unknown lexeme
    EOF_ERR,         // End of file error
    MUST_BE_ID,      // Error: Must be identifier
    MUST_BE_CONST,   // Error: Must be const
    MUST_BE_SEMI,    // Error: Must be ';'
    MUST_BE_PROG,    // Error: Must be 'program'
    MUST_BE_COMMA,   // Error: Must be ','
    MUST_BE_TYPE,    // Error: Must be type of identifier
    MUST_BE_DOT,     // Error: Must be '.'
    MUST_BE_ASS,     // Error: Must be ':='
    MUST_BE_BKT_END, // Error: Must be ')'
    MUST_BE_BKT_BGN, // Error: Must be '('
    MUST_BE_THEN,    // Error: Must be 'then'
    MUST_BE_DO,      // Error: Must be 'do'
    MUST_BE_TO,      // Error: Must be 'to'
    MUST_BE_OF,      // Error: Must be 'of'
    MUST_BE_SQBRLEFT,   // Error: Must be '['
    MUST_BE_SQBRRIGHT,  // Error: Must be ']'
    DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
    UNKNOWN_ID,      // Error: Undefined identifier
    INCOMP_TYPES,    // Error: Incompatible types
    OUT_RANGE,       // Error: Out range array
};


class Lexem {
public:
    Lexem() : name(""), token(unknown_tk), line(0) {};
    Lexem(std::string &&t_name, tokens t_tok, int t_line) : name(t_name),
        token(t_tok), line(t_line) {};

    const std::string GetName();
    tokens      GetToken() const;
    int         GetLine();
    void        RenameLex(std::string name_);

private:
    std::string name;
    tokens      token;
    int         line;
};


#endif //LECS_PARS_LEXEM_H
