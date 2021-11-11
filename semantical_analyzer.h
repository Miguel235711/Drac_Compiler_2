#ifndef SEMANTICAL_ANALYZER_H
#define SEMANTICAL_ANALYZER_H

#include "syntactical_analyzer.h"
#include "lexical_analyzer.h"
#include <fstream>



class SemanticalAnalyzer{
    public:
        SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_map<int,Mode> special_symbols);
        virtual ~SemanticalAnalyzer();
        void create_and_print_symbol_table_and_extend_syntactical_tree(std::function<void(std::string)> & f_out);
    private:
        SyntacticalAnalyzer & syntactical_analyzer;
        SyntacticalNode * syntactical_tree_root;
        std::unordered_map<int,Mode> special_symbols;
        std::unordered_map<std::string,std::vector<IdNode*> > symbol_track; 
        std::vector<std::pair<int,std::unordered_set<std::string> > > scopes_and_entries;
        void create_and_print_symbol_table_and_extend_syntactical_tree(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,Mode mode,int & scope); // true if definition, false if reference 
        //mode -> does not matter initial value but could be dangerous because it depends on the grammar
        Mode assign_mode(SyntacticalNode * node,Mode cur_mode);
        // std::unordered_map<int,IdType> symbol_valex_to_id_type;
};

#endif // SEMANTICAL_ANALYZER_H