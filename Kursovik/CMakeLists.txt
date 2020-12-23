cmake_minimum_required(VERSION 3.13)
project(gay_compill)


set(CMAKE_CXX_STANDARD 14)
set(SRC ./src)

add_executable(gay_compill ${SRC}/main.cpp "src/compiller.h" "src/compiller.cpp"
               ${SRC}/Lexer.cpp ${SRC}/Lexer.h ${SRC}/Syntax.cpp ${SRC}/Syntax.h
               ${SRC}/Lexem.cpp ${SRC}/Lexem.h ${SRC}/Variable.cpp
               ${SRC}/Variable.h ${SRC}/Tree.cpp ${SRC}/Tree.h  ${SRC}/GenCode.h
               ${SRC}/GenCode.cpp)
