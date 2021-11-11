#ifndef SEMANTICAL_ANALYZER_H
#define SEMANTICAL_ANALYZER_H

#include "syntactical_analyzer.h"
#include "lexical_analyzer.h"
#include <fstream>

struct SymbolTableNode{
    std::unordered_map<std::string,int> id_name_to_type;
    SymbolTableNode * parent=NULL;
    int scope;
    SymbolTableNode(int scope):scope(scope){
        
    }
};

class SemanticalAnalyzer{
    public:
        SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_set<int> id_definition_symbols,std::unordered_set<int> reference_definition_symbols,int var_mode_symbol,int fun_mode_symbol);
        virtual ~SemanticalAnalyzer();
        void print_symbol_table(std::function<void(std::string)> & f_out);
    private:
        SyntacticalAnalyzer & syntactical_analyzer;
        SyntacticalNode * syntactical_tree_root;
        std::unordered_set<int> reference_id_symbols,id_definition_symbols; 
        int var_mode_symbol, fun_mode_symbol;

        void print_symbol_table(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,std::pair<int,bool> mode,int & scope,SymbolTableNode * & symbol_table_node); // true if definition, false if reference 
        //mode -> does not matter initial value but could be dangerous because it depends on the grammar
        std::pair<int,bool> assign_mode(SyntacticalNode * node,std::pair<int,bool> cur_mode);
        // std::unordered_map<int,IdType> symbol_valex_to_id_type;
};

#endif // SEMANTICAL_ANALYZER_H