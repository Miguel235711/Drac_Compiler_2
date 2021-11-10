#ifndef PRODUCTION_RULE_H
#define PRODUCTION_RULE_H

#include <string>
#include <utility>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <cwchar>
#include <vector>

//testing libraries

#include <vector>
#include <cassert>

class ProductionRule{
    public:
        
        ProductionRule(int rule_label,int left_non_terminal);
        virtual ~ProductionRule();
        void add_right_symbol(int symbol);
        int get_left_non_terminal();
        int get_ith_right_symbol(int i); ///manage -1
        //int get_hash();
        int get_rule_label();
        bool is_there_symbol_at(int i);///manage -1
        //static int min_symbol_value,max_symbol_value;
        
    protected:
       
        std::vector<int> right_symbols;
        int rule_label,left_non_terminal;

        //int normalize_symbol_for_hash(int symbol);
    private:

        int extended_index_to_simple(int i);
};

#endif // PRODUCTION_RULE_H