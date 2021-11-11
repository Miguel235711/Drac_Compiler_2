#ifndef SYNTACTICAL_ANALYZER_H
#define SYNTACTICAL_ANALYZER_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <fstream>
#include <functional>
#include <stack>
#include <algorithm>

#include "op_type.h"
#include "production_rule.h"
#include "in_file_names.h"

class SyntacticalAnalyzer{
    public: 
        
        SyntacticalAnalyzer(const InFileNames & in_file_names);
        virtual ~SyntacticalAnalyzer();
        bool is_correct(std::vector<std::pair<int,std::string> > & tokens);
        void print_syntatical_table(std::function<void(std::string)> & f_out);

        static int end_symbol;

    private:

        struct State{
            std::unordered_map<int,std::pair<OpType,int> > adjacent;
            State(){}
        };

        struct SyntacticalNode{
            int symbol;
            std::vector<SyntacticalNode*> adjacent; 
            SyntacticalNode(int symbol):symbol(symbol){
            }
            SyntacticalNode(){}
        };

        struct StackElement{
            bool is_state; //if false, it is a symbol;
            int element;
            SyntacticalNode * node=NULL;

            StackElement(bool is_state,int element):is_state(is_state),element(element){
                if(!is_state)
                    ///create node
                    node = new SyntacticalNode(element);
            }
            StackElement(){}
        };

        std::vector<State*> table;
        std::vector<ProductionRule*> rules;
        std::unordered_map<int,std::string> symbol_lexval_to_name;  
        StackElement * parent_stack_element;

        std::pair<OpType,int> get_mov(int state,int symbol);
        bool handle_reduction(std::stack<StackElement*> & st,int rule_number);   
        void print_syntatical_table(SyntacticalNode * node,std::function<void(std::string)> & f_out,int level);
};

#endif // SYNTACTICAL_ANALYZER_H