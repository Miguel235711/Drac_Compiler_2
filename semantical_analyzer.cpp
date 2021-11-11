#include "semantical_analyzer.h"

SemanticalAnalyzer::SemanticalAnalyzer(SyntacticalAnalyzer & syntactical_analyzer,std::unordered_map<int,Mode> special_symbols):
        syntactical_analyzer(syntactical_analyzer)
        ,special_symbols(special_symbols){}
        
SemanticalAnalyzer::~SemanticalAnalyzer(){

}
void SemanticalAnalyzer::create_and_print_symbol_table_and_extend_syntactical_tree(std::function<void(std::string)> & f_out){
    syntactical_tree_root = syntactical_analyzer.get_syntatical_tree_root();
    int scope = 0;
    //create scope 0
    scopes_and_entries.push_back({scope,std::unordered_set<std::string>()});
    create_and_print_symbol_table_and_extend_syntactical_tree(syntactical_tree_root,NULL,f_out,none,scope); //mode -> does not matter initial value but could be dangerous because it depends on the grammar
    scopes_and_entries.pop_back();
}
void SemanticalAnalyzer::create_and_print_symbol_table_and_extend_syntactical_tree(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,Mode mode,int & scope){
    //std::cout << "recursive call\n";
    auto next_mode = assign_mode(node,mode);
    auto symbol = node->symbol;
    //std::cout << "symbol: " << LexicalAnalyzer::symbol_lexval_to_name[symbol] << "\n";
    if(symbol==LexicalAnalyzer::curly_close){ // handle going to parent if token }
        assert(!scopes_and_entries.empty()); 
        auto scope_and_entry_ref = scopes_and_entries.back();
        auto scope = scope_and_entry_ref.first;
        auto & entry_refs = scope_and_entry_ref.second;
        for(auto & entry_ref: entry_refs){
            auto & in_symbol_track = symbol_track[entry_ref];
            for(;!in_symbol_track.empty()&&in_symbol_track.back()->scope == scope;in_symbol_track.pop_back()); //pop all matching with scope
        }
        scopes_and_entries.pop_back();
    }
    //manage scope change begin
    if((symbol==LexicalAnalyzer::curly_open&&parent&&parent->symbol!=LexicalAnalyzer::fun_def) || symbol == LexicalAnalyzer::fun_def ){
        scope++;
        scopes_and_entries.push_back({scope,std::unordered_set<std::string>()});
    }
    //manage scope change end
    if(symbol==LexicalAnalyzer::id_symbol){
        ///it's an id
        auto & content = node->id_content;
        auto it = symbol_track.find(content); 
        auto not_found = it == symbol_track.end();
        ///handle symbol search or insertion
        /*if(content=="x")
            std::cout << "mode of x: " << mode << "\n";*/
        if(mode == var_def || mode == fun_def){
            //definition
            if(not_found || it->second.empty() || it->second.back()->scope != scope ){
                if(not_found)
                    it = symbol_track.insert({content,std::vector<IdNode*>()}).first;
                ///reference to this IdNode !!!!
                it->second.push_back(node->id_node = new IdNode(scope,mode,content));
                //print node->id_node
                auto id_node =  node->id_node;
                std::cout << "type: " << ( id_node->id_type == var_def ? "var" : "fun" ) << " scope: " << id_node->scope << " name: " << id_node->name << " index: " << id_node->index << "\n";
                //insert in scopes and entries
                scopes_and_entries.back().second.insert(content); // to keep track
            }else{
                ///handle error later in a proper way, probably not necessary to stop compilation
                std::cout << "redefinition of " << content << "\n";
                //assert(false);
            }
        }else if(mode == ref){
            //find
            if(not_found || it->second.empty()){
                ///handle error later in a proper way, probably not necessary to stop compilation
                std::cout << content << " is not defined" << "\n";
                //assert(false);
            }else{
                ///reference to this IdNode !!!!
                //std::cout << "node->id_node:\n";
                node->id_node = it->second.back();
            }
        }
    }
    for(auto adjacent: node->adjacent){
        create_and_print_symbol_table_and_extend_syntactical_tree(adjacent,node,f_out,next_mode,scope);
    }

}

Mode SemanticalAnalyzer::assign_mode(SyntacticalNode * node,Mode cur_mode){
    auto symbol = node->symbol;
    auto it = special_symbols.find(symbol);
    if(it!=special_symbols.end())
        return it->second;
    return cur_mode;
}