#include "production_rule.h"


ProductionRule::ProductionRule(int rule_label,int left_non_terminal,SemanticalRule * semantical_rule):
    rule_label(rule_label)
    ,left_non_terminal(left_non_terminal)
    ,semantical_rule(semantical_rule)
{

}

ProductionRule::~ProductionRule(){

}

void ProductionRule::add_right_symbol(int symbol){
    right_symbols.push_back(symbol);
    //std::cout << "right symbols of size " << left_non_terminal << " is " << right_symbols.size() << "\n";
    //std::cout << "\n";   
}

int ProductionRule::get_ith_right_symbol(int i){
    assert(is_there_symbol_at(i));
    /*if(rule_label==2)
        std::cout << "get_ith_right_symbol: " << left_non_terminal << " " << right_symbols[0] << "\n";*/
    return right_symbols[extended_index_to_simple(i)];
}

int ProductionRule::get_left_non_terminal(){
    return left_non_terminal;
}

/*int ProductionRule::get_hash(){
    int x = normalize_symbol_for_hash(left_non_terminal);
    int p = hash_symbol_count;
    for(auto symbol: right_symbols){
        x += (int64_t) p * normalize_symbol_for_hash(symbol) % hash_modulo;
        x %= hash_modulo;
        p = (int64_t) p * hash_symbol_count % hash_modulo;
    }
    return x;
}*/
// int ProductionRule::normalize_symbol_for_hash(int symbol){
//     return symbol + min_symbol_value;
// }

int ProductionRule::get_rule_label(){
    return rule_label;
}
bool ProductionRule::is_there_symbol_at(int i){
    auto n_i = extended_index_to_simple(i);
    return -1 < n_i && n_i < right_symbols.size();
}
int ProductionRule::extended_index_to_simple(int i){
    return i +( i < 0 ?  right_symbols.size() : 0);
}
