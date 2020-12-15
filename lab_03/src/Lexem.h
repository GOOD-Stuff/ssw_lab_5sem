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
    div_tk,             // '/'
    opb_tk,             // '('
    cpb_tk,             // ')'
    div_op_tk,          // 'div'
    mod_op_tk,          // 'mod'
    if_tk,
    then_tk,
    else_tk,
    true_tk,
    false_tk,
    while_tk,
    do_tk,
    for_tk,
    to_tk,
    or_tk,
    and_tk,
    xor_tk,
    array_tk, 
    of_tk,
    sqbrleft_tk,
    sqbrright_tk,
    eof_tk              // end of file
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
    MUST_BE_BKT_END, // Error: Must be ')'
    MUST_BE_BKT_BGN, // Error: Must be '('
    MUST_BE_THEN, 
    MUST_BE_DO,
    MUST_BE_TO,
    MUST_BE_OF,
    MUST_BE_SQBRLEFT,
    MUST_BE_SQBRRIGHT,
    DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
    UNKNOWN_ID,      // Error: Undefined identifier
};


class Lexem {
public:
    Lexem() : name(""), token(unknown_tk), line(0) {};
    Lexem(std::string &&t_name, tokens t_tok, int t_line) : name(t_name),
                                              token(t_tok), line(t_line){};

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
