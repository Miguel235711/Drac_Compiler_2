#ifndef SEMANTICAL_RULE_H
#define SEMANTICAL_RULE_H


#include <vector>
#include <functional>

enum SymbolEntity{
    var_entity
    ,fun_entity
};

enum TypeValue{
    void_type
    //,uknown_type
    ,array_type ///only for var_entity
    ,idlist_type //only for var entity
    //,deflist_type ///only for var_entity
    //,stmtlist_type
    ,error_type
    ,int_type
    ,char_type
    ,string_type
    ,bool_type
}; ///later consider return values in functions

struct CompleteType {
    SymbolEntity symbol_entity;
    TypeValue type_value;
    std::vector<CompleteType *> nested_elements; //if type_value is array_type
    CompleteType(SymbolEntity symbol_entity,TypeValue type_value,std::vector<CompleteType *> nested_elements):
        symbol_entity(symbol_entity)
        ,type_value(type_value)
        ,nested_elements(nested_elements)
    {

    }
};

class SemanticalRule{
    public:
        SemanticalRule(std::function<CompleteType *(std::function<CompleteType *(int)>)> rule);
        virtual ~SemanticalRule();
        CompleteType * get_type(std::function<CompleteType *(int)> get_type_at);
    private:
        std::function<CompleteType * (std::function<CompleteType*(int)>)> rule;
};

#endif // SEMANTICAL_RULE_H