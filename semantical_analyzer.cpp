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
        scopes.try_insert_in_scope(fun_index,API_function.first,node);
        node->arity = API_function.second;
        print_table_entry(node,f_out);
    }
    ///API functions end
    ///transverse globally and then (globally and locally)
    create_and_print_symbol_table_and_extend_syntactical_tree(syntactical_tree_root,NULL,f_out,none,scopes,true,false); //mode -> does not matter initial value but could be dangerous because it depends on the grammar
    //std::cout << "finished first semantic: " << "var_scopes(): " << scopes.var_scopes() << "\n";
    create_and_print_symbol_table_and_extend_syntactical_tree(syntactical_tree_root,NULL,f_out,none,scopes,false,false);
}
Arity SemanticalAnalyzer::create_and_print_symbol_table_and_extend_syntactical_tree(SyntacticalNode * node,SyntacticalNode * parent,std::function<void(std::string)> & f_out,Mode mode,Scopes & scopes,bool is_global_transverse,bool while_or_do_while_as_ancestor){
    //std::cout << "recursive call\n";
    auto next_mode = assign_mode(node,mode);
    auto symbol = node->symbol;
    //std::cout << "symbol: " << LexicalAnalyzer::symbol_lexval_to_name[symbol] << "\n";
    //std::cout << "sizes: " << scopes_and_entries[0].size() << " " << scopes_and_entries[1].size() << "scope_counter: " << scope << "\n";
    if(symbol==LexicalAnalyzer::curly_close_symbol){ // handle going to parent if token }
        scopes.close_var_scope();
    }
    //manage scope change begin
    if((symbol==LexicalAnalyzer::curly_open_symbol &&parent&&parent->symbol!=LexicalAnalyzer::fun_def_symbol)|| (parent&&parent->symbol==LexicalAnalyzer::fun_def_symbol && symbol == LexicalAnalyzer::id_symbol) || symbol == LexicalAnalyzer::curly_close_symbol){
        if(symbol!=LexicalAnalyzer::curly_close_symbol || scopes.var_scopes()>1) //mantaining a single var global scope
            scopes.open_var_scope();
    }
    Arity arity;
    //manage scope change end
    auto index = mode == var_def || mode == var_ref ? var_index : fun_index;
    if(symbol==LexicalAnalyzer::id_symbol){
        arity.id_symbol_arity++;
        //std::cout << "in id=" << node->id_content << " arity=" << arity << "\n";
        //std::cout << "It's an ID\n";
        ///it's an id0
        auto & content = node->id_content;
        if(mode == var_def || mode == fun_def_symbol){
            //definition
            auto & id_node = node->id_node;
            //std::cout << "mode: " << mode << " index: " << index << "\n";
            auto was_insertion_successful = (!is_global_transverse && (scopes.var_scopes()==1||mode == fun_def_symbol)) || scopes.try_insert_in_scope(index,content,id_node); //special case for globality handling . Dangerous!!!
            if((scopes.var_scopes()==1||mode == fun_def_symbol)^!is_global_transverse){
                if(was_insertion_successful){
                    if(!is_global_transverse||mode!=fun_def_symbol) //avoid printing now to calculate later arity
                        print_table_entry(id_node,f_out);
                }else{
                    std::cout << node->location.first << ":" << node->location.second << ":Error: redefinition of " << content << "\n";
                }
            }
            //std::cout << "insert in scope\n";
        }else if(mode == fun_ref || mode == var_ref){
            //find
            //std::cout << "index in find: " << index << "\n";
            auto id_node= scopes.get_lca_symbol(content,index);
            if(id_node == NULL&&!is_global_transverse) ///on global transverse don't display this
                std::cout << node->location.first << ":" << node->location.second << ":Error: " << content << " is not declared\n";
            else
                node->id_node = id_node;
        }
    }else if(!is_global_transverse){
        if(symbol == LexicalAnalyzer::break_symbol && !while_or_do_while_as_ancestor)
            std::cout << node->location.first << ":" << node->location.second << ":Error: " << "'break' must be inside a <stmt-do-while> or <stmt-while> \n";
        else if(symbol == LexicalAnalyzer::expr_symbol)
            arity.expr_symbol_arity ++;
    }
    //std::cout << "node->symbol: " << node->symbol << " size: " << adjacent_nodes.size() << "\n";
    int id_symbol_arity=-1;
    for(auto adjacent: node->adjacent){
        auto arity_from_call = create_and_print_symbol_table_and_extend_syntactical_tree(adjacent,node,f_out,next_mode,scopes,is_global_transverse,while_or_do_while_as_ancestor || symbol==LexicalAnalyzer::stmt_do_while_symbol || symbol==LexicalAnalyzer::stmt_while_symbol);
        if(adjacent->symbol == LexicalAnalyzer::param_list_symbol)
            id_symbol_arity = arity_from_call.id_symbol_arity;
        arity += arity_from_call;
    }
    //set arity for fun_def id
    if(node->symbol == LexicalAnalyzer::fun_def_symbol &&is_global_transverse&&node->adjacent[0]->id_node->arity==-1){ ///assign arity to <id> that corresponds to a function in global_transverse (ULTRA DANGEROUS HACK!!!)
        //std::cout << "adjacent_nodes.size(): " << adjacent_nodes.size() << "\n";
        node->adjacent[0]->id_node->arity=id_symbol_arity;
        //std::cout << "after arity assignment\n";
        print_table_entry(node->adjacent[0]->id_node,f_out);
    }
    //check arity for fun_call
    //std::cout << "size: " << adjacent_nodes.size() << "\n";
    /*if(node->symbol&&LexicalAnalyzer::fun_call_symbol && adjacent_nodes[0]->id_node == NULL)
        std::cout << "damn\n";*/
    /*if(node==NULL)
        std::cout << "damn\n";*/
    //std::cout << node->symbol << " size: " << node->adjacent.size() << "\n";
    if(node->symbol == LexicalAnalyzer::fun_call_symbol){
        if(!is_global_transverse&&node->adjacent[0]->id_node!=NULL&&node->adjacent[0]->id_node->arity!=arity.expr_symbol_arity)
            //std::cout << "size of fun-call children: " << node->adjacent.size() << "\n",std::cout << "call argument amount error\n";
            std::cout 
                << node->adjacent[0]->location.first 
                << ":" << node->adjacent[0]->location.second 
                << ":Error: call to function '" << node->adjacent[0]->id_node->name 
                << "' expects " <<  node->adjacent[0]->id_node->arity
                << " parameters, but " << arity.expr_symbol_arity
                << " were received\n"  
            ;
        ///restart expr arity 
        //arity.expr_symbol_arity=0;
    }
    //patch for fun_call arguments
    if(node->symbol == LexicalAnalyzer::expr_primary)
        arity.expr_symbol_arity = 0;
    return arity;
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
    is_main_function_defined |= id_node->id_type == fun_def_symbol && id_node->name == "main";
    f_out("type: "+s+" scope: "+std::to_string(id_node->scope)+" name: "+id_node->name+" index: "+std::to_string(id_node->index)+(id_node->id_type == fun_def_symbol ? " arity: "+std::to_string(id_node->arity):"")+"\n");
}

void SemanticalAnalyzer::calculate_types(SyntacticalNode * node){
    auto adjacent = node->adjacent;
    for(auto adj:adjacent)
        calculate_types(adj);
    if(node->semantical_rule!=NULL){
        ///<stmt-assign> special case of changing type
        if(node->symbol==-13) // <stmt-assign>
            adjacent[0]->type = adjacent[2]->type;
        node->type = node->semantical_rule->get_type([adjacent](int i){
            assert(i<adjacent.size());
            return adjacent[i]->type;
        });
        std::cout << "Node yield type: " << node->type->type_value << "\n";
    }
    //else
        //node->type = error_type; //already calculated
}


void SemanticalAnalyzer::calculate_types(){
    calculate_types(syntactical_tree_root);
}

void SemanticalAnalyzer::check_other_validations(){
    /// 2. Every program starts its execution in a function called main. It is an error if the program does not contain a function with this name.
    if(!is_main_function_defined){
        std::cout << "Error:" << "'main' function not found\n"; 
    }
    check_other_validations(syntactical_tree_root);
}
void SemanticalAnalyzer::check_other_validations(SyntacticalNode * node){
}