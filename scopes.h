#ifndef SCOPES_H
#define SCOPES_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <stack>


#include "id_node.h"

enum ScopeType{
    fun_index,var_index
};

class Scopes{
    public:
        typedef std::vector<std::unordered_map<std::string,std::stack<IdNode*> > > symbol_track_type;
        typedef std::vector<std::stack<std::pair<int,std::unordered_set<std::string> > > > scopes_and_entries_type;
        Scopes();
        virtual ~Scopes();
        IdNode * get_lca_symbol(std::string name,ScopeType scope_type);
        void open_var_scope();
        void close_var_scope();
        bool try_insert_in_scope(ScopeType scope_type,std::string name, IdNode * & id_node);
        size_t var_scopes();
    private:
        symbol_track_type symbol_track; /// 0 is functions and 1 is vars
        scopes_and_entries_type scopes_and_entries;
        int scope_counter = 0; 
};  


#endif // SCOPES_H