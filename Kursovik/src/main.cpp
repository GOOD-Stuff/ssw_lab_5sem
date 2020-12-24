#include <iostream>
#include <regex>
#include <map>
#include <string>
#include "compiller.h"


int main(int argc, char* argv[]){
    if ((argc == 3) || (argc == 1)) {
        std::string source;
        std::string dest;
        if (argc == 3) {
            std::cout << "Start..." << std::endl;
            source = argv[1];
            dest = argv[2];
        }
        else {
            source = "source.p";
            dest = "dest";
        }

        if (!Parse(source, dest)) {



            std::string str = argv[0];
            if (argc == 1) {
                auto find = str.find_last_of('\\', str.size());
                str.erase(find, str.size() - find);
            }

            std::string cin_str;
            bool open_wind{ true };
            while (open_wind) {
                std::string path;
                if (argc == 1) {
                    path = str + "\\" + dest + ".S";
                }
                else {
                    path = dest + ".S";
                }
                std::cout << "\nFile saved on path: " << path << std::endl << "Open the file?(y/n): ";
                std::cin >> cin_str;
                if (cin_str == "y") {
                    std::ifstream file_s(path);
                    if (file_s.is_open()) {
                        std::string getline;
                        std::cout << "\n***************************Assembler**************************\n";
                        while (!file_s.eof()) {
                            std::getline(file_s, getline);
                            std::cout << getline << std::endl;
                        }
                        std::cout << "\n**************************************************************\n";
                        file_s.close();
                        open_wind = false;
                    }
                    else {
                        std::cout << "\nFile not open... (";
                    }
                }
                else if (cin_str == "n") {
                    open_wind = false;
                }
            }
        }
    }
    else {
        if (argc == 2) {
            std::string help = argv[1];
            if (help == "-help") {
                std::cout << "To run the compiler, you need to specify the path to the Pascal code, \n\
                              and also specify the path to write the .C file\n Example:\n gay_compill <path source>/<name_file> <path destination>/<name file>";
                return 0;
            }
        }
        std::cout << "Incorrect args!!!\n";
    }

    std::getchar();
    return 0;
}
