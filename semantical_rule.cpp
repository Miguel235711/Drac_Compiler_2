#include "semantical_rule.h"

SemanticalRule::SemanticalRule(std::function<CompleteType *(std::function<CompleteType *(int)>)> rule)
{
    this->rule = rule;
    /*std::function<CompleteType*(int)> get_type_at = [](int i)->CompleteType * {
            return NULL;
    };
    this->rule(get_type_at);*/
}
SemanticalRule::~SemanticalRule(){}
CompleteType * SemanticalRule::get_type(std::function<CompleteType*(int)> get_type_at){
    //get_type_at(0);
    //get_type_at(0);
    rule(get_type_at);
    rule(get_type_at);
    return rule(get_type_at);
}