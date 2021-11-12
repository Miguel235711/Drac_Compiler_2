#include "scopes.h"

Scopes::Scopes(){
    symbol_track.resize(2);
    scopes_and_entries.resize(2);
    for(int i = 0 ; i < 2 ; i ++)
        scopes_and_entries[i].push({scope_counter++,std::unordered_set<std::string>()});
}
Scopes::~Scopes(){

}

IdNode * Scopes::get_lca_symbol(std::string name,ScopeType scope_type){
    auto & symbol_track_focus = symbol_track[scope_type];
    auto it = symbol_track_focus.find(name);
    if(it==symbol_track_focus.end())
        return NULL;
    assert(!it->second.empty());
    return it->second.top();
}

void Scopes::open_var_scope(){
    auto & target_scopes_and_entries = scopes_and_entries[var_index];
    target_scopes_and_entries.push({scope_counter++,std::unordered_set<std::string>()});
}
void Scopes::close_var_scope(){
    //remove all scope related
    auto & target_scopes_and_entries = scopes_and_entries[var_index];
    auto & target_symbol_track = symbol_track[var_index];
    assert(!target_scopes_and_entries.empty());
    auto & target_scope = target_scopes_and_entries.top();
    auto scope = target_scope.first;
    for(auto & name: target_scope.second){
        auto & symbol_track_name = target_symbol_track[name];
        for(;!symbol_track_name.empty()&&symbol_track_name.top()->scope==scope;symbol_track_name.pop());
    }
    target_scopes_and_entries.pop();
}


bool Scopes::try_insert_in_scope(ScopeType scope_type,std::string name,IdNode * & id_node){
    auto & target_scopes_and_entries = scopes_and_entries[scope_type];
    auto & target_symbol_track = symbol_track[scope_type];
    auto & target_scope = target_scopes_and_entries.top();
    auto scope = target_scope.first;
    auto it = target_symbol_track.find(name);
    if(it!=target_symbol_track.end()&&!it->second.empty()&&it->second.top()->scope==scope){
        //invalid, improve later
        //assign found node
        id_node = it->second.top();
        return false;
    }
    //insert in corresponding scope
    //std::cout << "try_insert_in_scopes\n";
    id_node = new IdNode(target_scope.first,scope_type==var_index ? var_def : fun_def,name);
    id_node->scope=target_scope.first;
    target_scope.second.insert(id_node->name);
    target_symbol_track[name].push(id_node);
    return true;
}

size_t Scopes::var_scopes(){
    return scopes_and_entries[var_index].size();
}