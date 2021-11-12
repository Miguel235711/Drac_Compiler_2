#include "semantical_analyzer.h"

SemanticalAnalyzer::SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_map<int,Mode> special_symbols):
        syntactical_analyzer(syntactical_analyzer)
        ,special_symbols(special_symbols){
        }
        
SemanticalAnalyzer::~SemanticalAnalyzer(){
}
void SemanticalAnalyzer::create_and_print_symbol_table_and_extend_syntactical_tree(std::function<void(std::string)> & f_out){
    syntactical_tree_root = syntactical_analyzer.get_syntatical_tree_root();
    Scopes scopes;
    ///API functions begin
    for(auto & API_function : API_functions){
        IdNode * node;
        scopes.try_insert_in_scope(fun_index,API_function,node);
        print_table_entry(node,f_out);
    }
    ///API functions end
    ///transverse globally and then (globally and locally)
    create_and_print_symbol_table_and_extend_syntactical_tree(syntactical_tree_root,NULL,f_out,none,scopes,true); //mode -> does not matter initial value but could be dangerous because it depends on the grammar
    //std::cout << "var_scopes(): " << scopes.var_scopes() << "\n";
    create_and_print_symbol_table_and_extend_syntactical_tree(syntactical_tree_root,NULL,f_out,none,scopes,false);
}
void SemanticalAnalyzer::create_and_print_symbol_table_and_extend_syntactical_tree(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,Mode mode,Scopes & scopes,bool is_global_transverse){
    //std::cout << "recursive call\n";
    auto next_mode = assign_mode(node,mode);
    auto symbol = node->symbol;
    //std::cout << "symbol: " << LexicalAnalyzer::symbol_lexval_to_name[symbol] << "\n";
    //std::cout << "sizes: " << scopes_and_entries[0].size() << " " << scopes_and_entries[1].size() << "scope_counter: " << scope << "\n";
    if(symbol==LexicalAnalyzer::curly_close){ // handle going to parent if token }
        scopes.close_var_scope();
    }
    //manage scope change begin
    if((symbol==LexicalAnalyzer::curly_open &&parent&&parent->symbol!=LexicalAnalyzer::fun_def)|| (parent&&parent->symbol==LexicalAnalyzer::fun_def && symbol == LexicalAnalyzer::id_symbol) || symbol == LexicalAnalyzer::curly_close){
        if(symbol!=LexicalAnalyzer::curly_close || scopes.var_scopes()>1) //mantaining a single var global scope
            scopes.open_var_scope();
    }
    //manage scope change end
    auto index = mode == var_def || mode == var_ref ? var_index : fun_index;
    if(symbol==LexicalAnalyzer::id_symbol){
        //std::cout << "It's an ID\n";
        ///it's an id0
        auto & content = node->id_content;
        if(mode == var_def || mode == fun_def){
            //definition
            auto & id_node = node->id_node;
            //std::cout << "mode: " << mode << " index: " << index << "\n";
            auto was_insertion_successful = (!is_global_transverse && (scopes.var_scopes()==1||mode == fun_def)) || scopes.try_insert_in_scope(index,content,id_node); //special case for globality handling . Dangerous!!!
            if((scopes.var_scopes()==1||mode == fun_def)^!is_global_transverse){
                if(was_insertion_successful){
                    print_table_entry(id_node,f_out);
                }else
                    std::cout << node->location.first << ":" << node->location.second << ":Error: redefinition of " << content << "\n";
            }
            //std::cout << "insert in scope\n";
        }else if(mode == fun_ref || mode == var_ref){
            //find
            auto id_node= scopes.get_lca_symbol(content,index);
            if(id_node == NULL&&!is_global_transverse) ///on global transverse don't display this
                std::cout << node->location.first << ":" << node->location.second << ":Error: " << content << " is not defined\n";
            else
                node->id_node = id_node;
        }
    }
    for(auto adjacent: node->adjacent)
        create_and_print_symbol_table_and_extend_syntactical_tree(adjacent,node,f_out,next_mode,scopes,is_global_transverse);

}

Mode SemanticalAnalyzer::assign_mode(SyntacticalNode * node,Mode cur_mode){
    auto symbol = node->symbol;
    auto it = special_symbols.find(symbol);
    if(it!=special_symbols.end())
        return it->second;
    return cur_mode;
}

void SemanticalAnalyzer::print_table_entry(IdNode * id_node,std::function<void(std::string)> & f_out){
    std::string s = id_node->id_type  == var_def ? "var" : "fun";
    f_out("type: "+s+" scope: "+std::to_string(id_node->scope)+" name: "+id_node->name+" index: "+std::to_string(id_node->index)+"\n");
}