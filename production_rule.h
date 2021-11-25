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
#include <functional>

#include "id_node.h"
#include "semantical_rule.h"

//testing libraries

#include <vector>
#include <cassert>

struct SyntacticalNode {
            int symbol;
            std::vector<SyntacticalNode*> adjacent;
            std::string id_content; //empty if  ///this is repetitive because id_node has it
            IdNode * id_node = NULL;
            std::pair<int,int> location = {-1,-1};
            CompleteType * type; //at beginning , by default all have void if not literal
            SemanticalRule * semantical_rule;
            static std::unordered_map<int,TypeValue> symbol_to_type;
            SyntacticalNode(int symbol):
                symbol(symbol)
            {
                handle_initial_type();
            }
            SyntacticalNode(int symbol,std::string id_content,std::pair<int,int> location):
                id_content(id_content)
                ,symbol(symbol)
                ,location(location) 
            {
                handle_initial_type();
            }
            private:
                void handle_initial_type(){
                    auto it = symbol_to_type.find(symbol);
                    type = new CompleteType(var_entity, it==symbol_to_type.end() ? void_type : it->second,{});
                }
};

class ProductionRule{
    public:
        
        ProductionRule(int rule_label,int left_non_terminal,SemanticalRule * semantical_rule);
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
        SemanticalRule * semantical_rule;
};


#endif // PRODUCTION_RULE_H