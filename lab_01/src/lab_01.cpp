#include "lab_01.h"


int Parse(const std::string& file_path) {
	Lexer lex(file_path.c_str());
	auto table = lex.ScanCode(); // create table of lexemes
	Syntax syntx(std::move(table));
	syntx.ParseCode();          // start parse code

	return EXIT_SUCCESS;
}