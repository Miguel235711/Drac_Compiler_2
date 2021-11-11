#ifndef IN_FILE_NAMES_H
#define IN_FILE_NAMES_H

#include <string>

struct InFileNames {
    std::string syntactical_file_name, rule_file_name, symbols_file_name;
    InFileNames(std::string syntactical_file_name, std::string rule_file_name, std::string symbols_file_name):
        syntactical_file_name(syntactical_file_name)
        ,rule_file_name(rule_file_name)
        ,symbols_file_name(symbols_file_name)
    {}
};


#endif // IN_FILE_NAMES_H