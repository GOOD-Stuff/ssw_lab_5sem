#include "Lexer.h"

Lexer::Lexer(const char *file_path) {
    try {
        code.open(file_path);
    } catch (const std::exception &exp) {
        std::string lel(exp.what());
        std::string what = "<E> Lexer: Catch exception in constructor: ";
        throw std::runtime_error(what + lel);
    }
}

Lexer::~Lexer() {
    code.close();
}

std::vector<Lexem> Lexer::ScanCode() {
    try {
        if (!code.is_open()) {
            std::cerr << "<E> Can't open file" << std::endl;
            return lex_table;
        }

        while(!code.eof()) {
            lex_table.emplace_back(GetLex());
        }
        return lex_table;
    }
    catch (const std::exception &exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what() << std::endl;
        return lex_table;
    }
}

Lexem Lexer::GetLex() {
    try {
        auto ch = GetCurrentCurs();
        while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            if (ch == '\n') line++;

            if (code.eof()) // if end of file
                return Lexem("EOF", TOKEN_EOF, line);
            ch = GetChar();
        }

        auto isId = [](char ch) {
            return std::isalpha(static_cast<unsigned char>(ch)) ||
                   std::isdigit(static_cast<unsigned char>(ch));
        };

        std::string lex;
        if (std::isdigit(static_cast<unsigned char>(ch))) { // Constants (Numbers)
            while (std::isdigit(static_cast<unsigned char>(ch))) {
                lex += ch;
                ch = GetChar();
            }

            return Lexem(std::move(lex), TOKEN_CONSTANT, line);
        } 
        else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
            while(isId(ch)) {
                lex += ch;
                ch = GetChar();
            }

            if (lex == "program")      { return Lexem(std::move(lex), TOKEN_PROGRAM, line); }
            else if (lex == "var")     { return Lexem(std::move(lex), TOKEN_VAR, line);     }
            else if (lex == "begin")   { return Lexem(std::move(lex), TOKEN_BEGIN, line);   }
            else if (lex == "integer") { return Lexem(std::move(lex), TOKEN_TYPE, line);    }
            else if (lex == "end")     { return Lexem(std::move(lex), TOKEN_END, line);     }

            else { // it is ID
                return Lexem(std::move(lex), TOKEN_ID, line);
            }
        }
        else if (std::ispunct(static_cast<unsigned char>(ch))) { // Other symbols
            Tokens tok;
            switch (ch) {
                case ',' : tok = TOKEN_COMMA; break;
                case '.' : tok = TOKEN_DOT;   break;
                case ':' : tok = TOKEN_DDT;   break;
                case ';' : tok = TOKEN_SEMI;  break;
                case '=' : tok = TOKEN_EQV;   break;
                case '+' : tok = TOKEN_ADD;   break;
                case '-' : tok = TOKEN_SUB;   break;
                case '*' : tok = TOKEN_MUL;   break;
                case '/' : tok = TOKEN_DIV;   break; // TODO: rewrite for Pascal div
                case '(' : tok = TOKEN_OPB;   break; // TODO: rewrite for Pascal div
                case ')' : tok = TOKEN_CPB;   break; // TODO: rewrite for Pascal div
                //case 'd' : tok = div_tk;   break; // TODO: rewrite for Pascal div
                //case 'mod' : tok = div_tk;   break; // TODO: rewrite for Pascal div
                default: {
                    std::cerr << "<E> Unknown token " << ch << std::endl;
                    tok = TOKEN_UNKNOWN;
                    break;
                }

            }
               lex += ch;

            if (tok == TOKEN_DDT) {
                ch = GetChar();
                if (ch == '=') {
                    lex += ch;
                    tok = TOKEN_ASS;
                }
            }

            GetChar(); // some kind of k o s t y l; here we look on \n
            return Lexem(std::move(lex), tok, line);
        }
        else {
            std::cerr << "<E> Unknown token " << ch << std::endl;
        }

        return Lexem("", TOKEN_UNKNOWN, line);
    }
    catch (const std::exception &exp) {
        return Lexem("", TOKEN_UNKNOWN, line);
    }
}

char Lexer::GetChar() {
    if (code.fail()) {
        std::cerr << "<E> Can't read from the file" << std::endl;
        throw std::runtime_error("File doesn't available");
    }

    if (!code.eof()) {
        code >> std::noskipws >> cursor;
    } else {
        std::cerr << "<E> File is EOF early" << std::endl;
        throw std::runtime_error("File is EOF early");
    }

    return cursor;
}