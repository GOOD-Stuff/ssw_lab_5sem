//
// Created by Home on 06.12.2020.
//

#ifndef LAB_01_VARIABLE_H
#define LAB_01_VARIABLE_H

#include <string>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <list>
#include <map>
#include <vector>
class Variable 
{
public:
    explicit Variable(const std::string& t_type, const std::string& t_value) : type(t_type), value(t_value) {};
    ~Variable() = default;
    std::string type;
    std::string value;
};


#endif //LAB_01_VARIABLE_H
