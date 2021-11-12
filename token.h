#ifndef TOKEN_H
#define TOKEN_H

#include <utility>

struct Token{
    int label;
    std::string content;
    std::pair<int,int> location;
    Token(){}
    Token(int label, std::string content, std::pair<int,int> location):
        label(label)
        ,content(content)
        ,location(location)
    {}
};

#endif // TOKEN_H