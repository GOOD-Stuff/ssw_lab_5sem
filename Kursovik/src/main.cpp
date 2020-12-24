#include <iostream>
#include <regex>
#include <map>
#include <string>
#include "compiller.h"


int main(int argc, char* argv[]){
    if (argc == 3) {
        std::cout << "Start..." << std::endl;
        std::string source = argv[1];
        std::string dest = argv[2];
        
        std::cout << source << std::endl;
        Parse(source, dest);
    }
    else {
        if (argc == 1) {
            std::string source = "source.p";
            std::string dest = "dest";
            Parse(source, dest);
        }
        if (argc == 2) {
            std::string help = argv[1];
            if (help == "-help") {
                std::cout << "To run the compiler, you need to specify the path to the Pascal code, \n\
                              and also specify the path to write the .C file\n Example:\n gay_compill <path source>/<name_file> <path destination>/<name file>";
                std::getchar();
                return 0;
            }
        }
        std::cout << "Incorrect args\n";
    }
    std::getchar();
    return 0;
}
