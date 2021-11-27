#ifndef SEMANTICAL_ANALYZER_H
#define SEMANTICAL_ANALYZER_H

#include "syntactical_analyzer.h"
#include "lexical_analyzer.h"
#include "scopes.h"

#include <fstream>


struct Arity{
    int 
        id_symbol_arity=0
        ,expr_symbol_arity=0
    ;
    Arity(){}
    Arity(
        int id_symbol_arity
        ,int expr_symbol_arity
    ):
        id_symbol_arity(id_symbol_arity)
        ,expr_symbol_arity(expr_symbol_arity)
    {

    }
    Arity& operator+=(const Arity & other){
        this->expr_symbol_arity += other.expr_symbol_arity;
        this->id_symbol_arity += other.id_symbol_arity;
        return *this;
    }
};

class SemanticalAnalyzer{
    public:
        SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_map<int,Mode> special_symbols);
        virtual ~SemanticalAnalyzer();
        void create_and_print_symbol_table_and_extend_syntactical_tree(std::function<void(std::string)> & f_out);
        void calculate_types();
        void check_other_validations();
    private:
        SyntacticalAnalyzer & syntactical_analyzer;
        SyntacticalNode * syntactical_tree_root;
        std::unordered_map<int,Mode> special_symbols;
        std::vector<std::pair<std::string,int> > API_functions = {
            {"printi",1}
            ,{"printc",1}
            ,{"prints",1}
            ,{"println",0}
            ,{"readi",0}
            ,{"reads",0}
            ,{"new",1}
            ,{"size",1}
            ,{"add",2}
            ,{"get",2}
            ,{"set",3}
        };
        bool is_main_function_defined = false;
        //int return value is arity useful for <id> defined as immediate child of <fun-def>
        Arity create_and_print_symbol_table_and_extend_syntactical_tree(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,Mode mode,Scopes & scopes,bool is_global_transverse,bool while_or_do_while_as_ancestor); // true if definition, false if reference 
        void print_table_entry(IdNode * id_node,std::function<void(std::string)> & f_out);
        //mode -> does not matter initial value but could be dangerous because it depends on the grammar
        void calculate_types(SyntacticalNode * node);
        Mode assign_mode(SyntacticalNode * node,Mode cur_mode);
        void check_other_validations(SyntacticalNode * node);

        
        // std::unordered_map<int,IdType> symbol_valex_to_id_type;
};

#endif // SEMANTICAL_ANALYZER_H