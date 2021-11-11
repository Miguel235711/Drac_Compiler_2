#include "semantical_analyzer.h"

SemanticalAnalyzer::SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_set<int> id_definition_symbols,std::unordered_set<int> reference_id_symbols,int var_mode_symbol, int fun_mode_symbol):
        syntactical_analyzer(syntactical_analyzer)
        ,id_definition_symbols(id_definition_symbols)
        ,reference_id_symbols(reference_id_symbols)
        ,var_mode_symbol(var_mode_symbol)
        ,fun_mode_symbol(fun_mode_symbol){
        
}
SemanticalAnalyzer::~SemanticalAnalyzer(){

}
void SemanticalAnalyzer::print_symbol_table(std::function<void(std::string)> & f_out){
    syntactical_tree_root = syntactical_analyzer.get_syntatical_tree_root();
    int scope = 0;
    auto * root = new SymbolTableNode(scope);
    print_symbol_table(syntactical_tree_root,NULL,f_out,{0,false},scope,root); //mode -> does not matter initial value but could be dangerous because it depends on the grammar

}
void SemanticalAnalyzer::print_symbol_table(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,std::pair<int,bool> mode,int & scope,SymbolTableNode * & symbol_table_node){
    auto next_mode = assign_mode(node,mode);
    auto next_symbol_table_node = symbol_table_node;
    if() // handle going to parent if token }
    
    //manage scope change begin

    //manage scope change end

    auto & id_name_to_type = symbol_table_node->id_name_to_type;
    if(node->symbol==LexicalAnalyzer::id_symbol){
        ///it's an id
        auto & content = node->id_content;
        ///handle symbol search or insertion
        if(mode.second){
            //definition
            if(id_name_to_type.find(content)==id_name_to_type.end())
                id_name_to_type.insert({content,mode.first});
            else{
                ///handle error later in a proper way, probably not necessary to stop compilation
                std::cout << "redefinition of " << content << "\n";
                //assert(false);
            }
        }else{
            //find
            bool not_found = true;
            for(auto node = symbol_table_node; node ; node = node->parent){
                auto & id_name_to_type = node->id_name_to_type;
                if(id_name_to_type.find(content)!=id_name_to_type.end()){
                    not_found = false;
                    break;
                }
            }
            if(not_found){
                ///handle error later in a proper way, probably not necessary to stop compilation
                std::cout << content << " is not defined" << "\n";
                //assert(false);
            }
        }
    }
    for(auto adjacent: node->adjacent){
        print_symbol_table(adjacent,parent,f_out,next_mode,scope,next_symbol_table_node);
    }
}

std::pair<int,bool> SemanticalAnalyzer::assign_mode(SyntacticalNode * node,std::pair<int,bool> cur_mode){
    auto symbol = node->symbol;
    if(id_definition_symbols.find(symbol)!=id_definition_symbols.end())
        return {symbol,true};
    if(reference_id_symbols.find(symbol)!=reference_id_symbols.end())
        return {symbol,false};
    return cur_mode;
}