#ifndef LECS_PARS_LCOMP_H
#define LECS_PARS_LCOMP_H

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Syntax.h"
#include "GenCode.h"


int Parse(const std::string& file_path, const std::string& path_dest);
#endif
