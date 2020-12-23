#ifndef LECS_PARS_VARIABLE_H
#define LECS_PARS_VARIABLE_H

#include <string>

class Variable {
public:
    explicit Variable(std::string t_type, std::string t_value) : type(t_type),
        value(t_value) {};
    std::string type;
    std::string value;
    bool mas_is{false};
    bool label_is{ false };
    std::pair<int, int>range;
};


#endif //LECS_PARS_VARIABLE_H
