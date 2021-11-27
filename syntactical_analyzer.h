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
#include "id_node.h"
#include "token.h"
#include "incomplete_production_handler.h"





class SyntacticalAnalyzer{
    public: 
        SyntacticalAnalyzer(const InFileNames & in_file_names);
        virtual ~SyntacticalAnalyzer();
        bool is_correct(std::vector<Token> & tokens);
        void print_syntatical_tree(std::function<void(std::string)> & f_out);
        SyntacticalNode * get_syntatical_tree_root();

        //bool has_main_function();
        
        static int end_symbol;

        static std::vector<SemanticalRule*> semantical_rules;

    private:

        struct State{
            std::unordered_map<int,std::pair<OpType,int> > adjacent;
            State(){}
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
            StackElement(bool is_state,int element,std::string id_content,std::pair<int,int> location):is_state(is_state),element(element){
                if(!is_state)
                    node = new SyntacticalNode(element,id_content,location);
            }
            StackElement(){}
        };

        std::vector<State*> table;
        std::vector<ProductionRule*> rules;
        
        StackElement * parent_stack_element;
        IncompleteProductionHandler * incomplete_production_handler;

        std::pair<OpType,int> get_mov(int state,int symbol);
        bool handle_reduction(std::stack<StackElement*> & st,int rule_number);   
        void print_syntatical_tree(SyntacticalNode * node,std::function<void(std::string)> & f_out,int level);

};

#endif // SYNTACTICAL_ANALYZER_H