#include "semantical_rule.h"

SemanticalRule::SemanticalRule(std::function<CompleteType *(std::function<CompleteType *(int)>)> rule):
    rule(rule)
{

}
SemanticalRule::~SemanticalRule(){}
CompleteType * SemanticalRule::get_type(std::function<CompleteType*(int)> get_type_at){
    return rule(get_type_at);
}