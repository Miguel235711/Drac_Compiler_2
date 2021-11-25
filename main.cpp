#include <vector>
#include <utility>
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <functional>
#include <unordered_map>

#include "lexical_analyzer.h"
#include "syntactical_analyzer.h"
#include "semantical_analyzer.h"
#include "production_rule.h"
#include "out_files.h"
#include "in_file_names.h"
#include "token.h"

#define BEGIN_SEMANTIC_RULE new SemanticalRule([](std::function<CompleteType *(int)>get_rule_type_at){
#define END_SEMANTIC_RULE })
#define ERROR_SEMANTIC_ANS new CompleteType(var_entity,error_type,{})
#define RETURN_VAR_ENTITY_AND_VOID_TYPE return new CompleteType(var_entity,void_type,{});

bool testing = false;
auto cases_path_c_str = std::string("./cases/").c_str();
int SyntacticalAnalyzer::end_symbol = 40;
char subbuff[10000];
std::unordered_map<int,std::string> LexicalAnalyzer::symbol_lexval_to_name;
int LexicalAnalyzer::id_symbol = 1,LexicalAnalyzer::curly_open = 24, LexicalAnalyzer::curly_close=25, LexicalAnalyzer::fun_def=-5;
int IdNode::index_counter = 0;
//int ProductionRule::min_symbol_value = 44, ProductionRule::max_symbol_value = 40;
auto in_file_names = InFileNames("./syntactical_table.txt","./grammar.txt","./symbols.txt");
std::unordered_map<int,TypeValue> SyntacticalNode::symbol_to_type={
    {2,int_type}
    ,{3,char_type}
    ,{4,string_type}
    ,{18,bool_type}
};

bool not_var_entity_or_not_void_value(CompleteType * x){
    return x->symbol_entity != var_entity || x->type_value != void_type;
}

bool  not_var_entity_or_not_bool_value(CompleteType * x){ 
    return x->symbol_entity != var_entity || x->type_value != bool_type;
}

bool numeric_variable_type(CompleteType * type){
    return
        type->type_value == int_type
        ||
        type->type_value == bool_type
    ;
}

bool any_variable_type(CompleteType * type){
    return 
        type->symbol_entity == var_entity
        &&
        (
            numeric_variable_type(type)
            ||
            type->type_value == array_type
            ||
            type->type_value == char_type
            ||
            type->type_value == string_type
        )
    ;
}

CompleteType * numeric_variable_single(CompleteType * type){
    auto ans = ERROR_SEMANTIC_ANS;
    if(!numeric_variable_type(type))
        return ans;
    ans->type_value = void_type;
    return ans;
}

CompleteType * any_variable_single(CompleteType * type){
    auto ans = ERROR_SEMANTIC_ANS;
    if(!any_variable_type(type))
        return ans;
    ans->type_value = void_type;
    return ans;
}

CompleteType * empty_single(CompleteType * type){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        type->symbol_entity != var_entity || type->type_value != void_type
    )
        return ans;
    ans->type_value = void_type;
    return ans;
}
CompleteType * expr_list_handler(CompleteType * expr, CompleteType * expr_list_cont){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        !any_variable_single(expr)
        ||
        not_var_entity_or_not_void_value(expr_list_cont)
    )
        return ans;
    ans->nested_elements = expr_list_cont->nested_elements;
    ans->nested_elements.push_back(expr); // the vector is reversed
    return ans;
}

std::vector<SemanticalRule*> SyntacticalAnalyzer::semantical_rules={
    //1 ‹program› → ‹def-list›
    BEGIN_SEMANTIC_RULE
        auto def_list_type  = get_rule_type_at(0);
        return new CompleteType(var_entity,def_list_type->symbol_entity == var_entity && def_list_type->type_value == void_type ? void_type : error_type,{});
    END_SEMANTIC_RULE,
    //2 ‹def-list› → ‹def-list› ‹def›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto def_list_type = get_rule_type_at(0);
        if(def_list_type->type_value!=void_type)
            return ans;
        auto def_type = get_rule_type_at(1);
        if(def_type->type_value!=void_type)
            return ans;
        //extend <def-list> with <def>
        ans->type_value = void_type;
        /*ans->nested_elements = def_list_type->nested_elements;
        ans->nested_elements.push_back(def_type);
        ans->type_value = deflist_type;*/
        return ans;
    END_SEMANTIC_RULE,
    //3 ‹def-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //4 ‹def› →	‹var-def›
    BEGIN_SEMANTIC_RULE
        auto var_def = get_rule_type_at(0);
        return new CompleteType(var_entity, var_def->symbol_entity == var_entity &&  var_def->type_value == void_type ? void_type : error_type,{});
    END_SEMANTIC_RULE,
    //5 ‹def› → ‹fun-def›
    BEGIN_SEMANTIC_RULE
        auto fun_def = get_rule_type_at(0);
        return new CompleteType(fun_entity,fun_def->symbol_entity == fun_entity && fun_def->type_value == void_type ? void_type : error_type,{});
    END_SEMANTIC_RULE,
    //6 ‹var-def› →	var ‹var-list› ;
    BEGIN_SEMANTIC_RULE
        auto var_list = get_rule_type_at(1);
        return new CompleteType(var_entity,var_list->symbol_entity == var_entity && var_list->type_value == void_type ? void_type : error_type,{});
    END_SEMANTIC_RULE,
    //7 ‹var-list› → ‹id-list›
    BEGIN_SEMANTIC_RULE
        auto var_list = get_rule_type_at(0);
        return new CompleteType(var_entity,var_list->symbol_entity == var_entity && var_list->type_value == idlist_type ? void_type : error_type,{});
    END_SEMANTIC_RULE,
    //8 ‹id-list› → ‹id› ‹id-list-cont›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto id = get_rule_type_at(0);
        if(not_var_entity_or_not_void_value(id))  //be careful!!! depends if normal or function call
            return ans;
        auto id_list_cont_type = get_rule_type_at(1);
        if(id_list_cont_type->symbol_entity != var_entity || id_list_cont_type->type_value != idlist_type)
            return ans;
        //extend <id-list-cont>  with <id>
        ans->nested_elements = id_list_cont_type->nested_elements;
        ans->nested_elements.push_back(id);  // the vector is reversed
        ans->type_value = idlist_type;
        return ans;
    END_SEMANTIC_RULE,
    //9 ‹id-list-cont› → , ‹id› ‹id-list-cont›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;;
        auto id = get_rule_type_at(1);
        if(not_var_entity_or_not_void_value(id))  //be careful!!!!
            return ans;
        auto id_list_cont_type = get_rule_type_at(2);
        if(id_list_cont_type->symbol_entity != var_entity || id_list_cont_type->type_value != idlist_type)
            return ans;
        //extend <id-list-cont>  with <id>
        ans->nested_elements = id_list_cont_type->nested_elements;
        ans->nested_elements.push_back(id);  // the vector is reversed
        ans->type_value = idlist_type;
        return ans;
    END_SEMANTIC_RULE,
    //10 ‹id-list-cont› → ε
    BEGIN_SEMANTIC_RULE
        return new CompleteType(var_entity,idlist_type,{});
    END_SEMANTIC_RULE,
    //11 ‹fun-def› → ‹id› ( ‹param-list› ) { ‹var-def-list› ‹stmt-list› }    same type as <param-list> dimension
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;;
        auto id = get_rule_type_at(0),
            param_list = get_rule_type_at(2),
            var_def_list = get_rule_type_at(5),
            stmt_list = get_rule_type_at(6)
        ;
        if(
            id->symbol_entity != var_entity || id->type_value!=void_type
            || 
            param_list->symbol_entity != var_entity || param_list->type_value != idlist_type
            ||
            not_var_entity_or_not_void_value(var_def_list)
            ||
            not_var_entity_or_not_void_value(stmt_list)
           
        )
            return ans;
        ans->nested_elements = param_list->nested_elements;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //12 ‹param-list› → ‹id-list›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto id_list = get_rule_type_at(0);
        if(id_list->symbol_entity!=var_entity || id_list->type_value!= idlist_type)
            return ans;
        return id_list;
    END_SEMANTIC_RULE,
    //13 ‹param-list› → ε
    BEGIN_SEMANTIC_RULE
        return new CompleteType(var_entity,idlist_type,{});
    END_SEMANTIC_RULE,
    //14 ‹var-def-list› → ‹var-def-list› ‹var-def› 
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            var_def_list = get_rule_type_at(0),
            var_def = get_rule_type_at(1)
        ;
        if(
            not_var_entity_or_not_void_value(var_def_list)
            ||
            not_var_entity_or_not_void_value(var_def)
        )
            return ans;
        //ans->nested_elements = var_def_list->nested_elements;
        //ans->nested_elements.push_back(var_def);
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //15 ‹var-def-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //16 ‹stmt-list› →	‹stmt-list› ‹stmt›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            stmt_list = get_rule_type_at(0),
            stmt = get_rule_type_at(1)
        ;
        if(
            not_var_entity_or_not_void_value(stmt_list)
            ||
            not_var_entity_or_not_void_value(stmt)
        )
         ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //17  ‹stmt-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //18 ‹stmt› → ‹stmt-assign›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //19 ‹stmt› → ‹stmt-incr›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //20 ‹stmt› → ‹stmt-decr›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //21 ‹stmt› → ‹stmt-fun-call›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //22 ‹stmt› → ‹stmt-if›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //23 ‹stmt› → ‹stmt-while›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //24 ‹stmt› → ‹stmt-do-while›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //25 ‹stmt› → ‹stmt-break›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //26 ‹stmt› → ‹stmt-return›
    BEGIN_SEMANTIC_RULE
        return any_variable_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //27 ‹stmt› →‹stmt-empty›
    BEGIN_SEMANTIC_RULE
        return empty_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //28 ‹stmt-assign› → ‹id› = ‹expr› ;
    BEGIN_SEMANTIC_RULE
        //check that ‹id› has void type or type is already equal to that of ‹expr›
        auto ans = ERROR_SEMANTIC_ANS;
        auto id = get_rule_type_at(0),
            expr = get_rule_type_at(2)
        ;
        if(
            id->symbol_entity!=var_entity || (id->type_value != void_type && id->type_value != expr->type_value)
            ||
            expr->symbol_entity!=var_entity || !any_variable_type(expr)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //29 ‹stmt-incr› →	inc ‹id› ;
    BEGIN_SEMANTIC_RULE
        return numeric_variable_single(get_rule_type_at(1));
    END_SEMANTIC_RULE,
    //30 ‹stmt-decr› → dec ‹id› ;
    BEGIN_SEMANTIC_RULE
        return numeric_variable_single(get_rule_type_at(1));
    END_SEMANTIC_RULE,
    //31 ‹stmt-fun-call› → ‹fun-call› ;
    BEGIN_SEMANTIC_RULE
        return any_variable_single(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //32 ‹fun-call› → ‹id› ( ‹expr-list› )
    BEGIN_SEMANTIC_RULE
        //assign types to elements and call transversal of the syntax tree from this - HARD!!!
    END_SEMANTIC_RULE,
    //33 ‹expr-list› →	‹expr› ‹expr-list-cont›
    BEGIN_SEMANTIC_RULE
        return expr_list_handler(get_rule_type_at(0),get_rule_type_at(1));
    END_SEMANTIC_RULE,
    //34 ‹expr-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //35 ‹expr-list-cont› →	, ‹expr› ‹expr-list-cont›
    BEGIN_SEMANTIC_RULE
        return expr_list_handler(get_rule_type_at(1),get_rule_type_at(2));
    END_SEMANTIC_RULE,
    //36 ‹expr-list-cont› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //37 ‹stmt-if› → if ( ‹expr› ) { ‹stmt-list› } ‹else-if-list› ‹else›
    BEGIN_SEMANTIC_RULE // here <expr> should already be casted to bool
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            expr = get_rule_type_at(2)
            ,stmt_list = get_rule_type_at(5)
            ,else_if_list = get_rule_type_at(7)
            ,else_ = get_rule_type_at(8)
        ;
        if(
            not_var_entity_or_not_bool_value(expr)
            || 
            not_var_entity_or_not_void_value(stmt_list)
            ||
            not_var_entity_or_not_void_value(else_if_list)
            ||
            not_var_entity_or_not_void_value(else_)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //38 ‹else-if-list› → ‹else-if-list› elif ( ‹expr› ) { ‹stmt-list› }
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto
            else_if_list = get_rule_type_at(0)
            ,expr = get_rule_type_at(3)
            ,stmt_list = get_rule_type_at(6);
        if(
            not_var_entity_or_not_void_value(else_if_list)
            ||
            not_var_entity_or_not_bool_value(expr)
            ||
            not_var_entity_or_not_void_value(stmt_list)
        )
            return ans;
        ans->type_value = void_type;
    END_SEMANTIC_RULE,
    //39 ‹else-if-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE;
    END_SEMANTIC_RULE,
    //40 ‹else› → else { ‹stmt-list› }
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto stmt_list = get_rule_type_at(2);
        if(
            not_var_entity_or_not_void_value(stmt_list)
        )
            return ans;
        ans->type_value = void_type;
    END_SEMANTIC_RULE,
    //41 ‹else› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE;
    END_SEMANTIC_RULE
}; //handle later

void load_lex_names(){
    std::fstream in(in_file_names.symbols_file_name); 
    //load symbol_valex_to_name
    int symbol_lexval;
    std::string symbol_name;
    while(in >> symbol_lexval >> symbol_name)
        LexicalAnalyzer::symbol_lexval_to_name[symbol_lexval]=symbol_name;
    in.close();
}

int main(int argc,char ** argv){
    load_lex_names();
    LexicalAnalyzer lexical_analyzer;
    std::vector<std::string> filenames;

    OutFiles out_files;

    for(int i = 1 ; i < argc; i ++){
        auto str_n = strlen(argv[i]);
        memcpy(subbuff,argv[i],3);
        //std::cout << "subbfuff: " << argv[i] << "\n";
        if(subbuff!=NULL && strlen(argv[i])>3 && subbuff[0]=='-' && subbuff[2]=='='){
            char flag = subbuff[1];
            //std::cout << "flag: " << flag << "\n";
            if(out_files.is_flag_valid(flag)){
                std::string ans = &argv[i][3];
                //std::cout << "ans: " << ans << "\n"; 
                out_files[flag] = ans;
            }
        }else
            //filename
            filenames.push_back(argv[i]);
    }
    //std::cout << "lexical_log_file_name: " << out_file_names.lexical_log_file_name << " syntatical_tree_file_name: " << out_file_names.syntactical_tree_file_name << " symbol_table_file_name: " << out_file_names.symbol_table_file_name << "\n";
    DIR *dir;
    dirent * ent;

    if(testing){
        if((dir=opendir(cases_path_c_str))!=NULL){
            while((ent=readdir(dir))!=NULL){
                //std::cout << "*** filename: " << ent->d_name << "\n";
                if(strcmp(ent->d_name,".")&&strcmp(ent->d_name,"..")){
                    char path[1024];
                    sprintf(path,"%s%s",cases_path_c_str,ent->d_name);
                    filenames.push_back(std::string(path));
                }
            }
        }else{
            std::cout << "Error getting testing directory\n";
            return EXIT_FAILURE;
        }
    }
    //init syntatical analyzer
    SyntacticalAnalyzer syntactical_analyzer(in_file_names);
    //init semantical analyzer
    SemanticalAnalyzer semantical_analyzer(syntactical_analyzer,{{-6,var_def},{-9,var_def},{-5,fun_def},{-12,var_ref},{-25,fun_ref},{-26,var_ref}});
    out_files.open();
    auto lexical_f_out = out_files.get_func('l'), tree_f_out = out_files.get_func('t'), symbol_f_out = out_files.get_func('s');
    for(auto file_name: filenames){
        std::vector<Token> tokens;
        bool failure = false;
        if(!lexical_analyzer.get_tokens(file_name,tokens)){
            lexical_f_out(std::to_string(lexical_analyzer.get_line())+":"+std::to_string(lexical_analyzer.get_column())+" Token "+lexical_analyzer.get_last_token()+" is not valid\n");
            failure = true;
        }
        lexical_f_out(file_name+"\n");
        for(auto token: tokens)
            lexical_f_out("label: "+LexicalAnalyzer::symbol_lexval_to_name[token.label]+" content: "+token.content+" line: "+std::to_string(token.location.first)+" col: "+std::to_string(token.location.second)+"\n");
        if(failure)
            return EXIT_FAILURE;
        ///
        //add 
        tokens.push_back(Token(SyntacticalAnalyzer::end_symbol,"",{-1,-1})); // special symbol for syntactical analysis
        bool is_correct = syntactical_analyzer.is_correct(tokens);
        std::cout << file_name << "\n";
        std::string preffix_msg = is_correct ? "Correct" : "Incorrect";
        std::cout << preffix_msg +  " syntaxis\n";
        // tree_f_out("test\n");
        // return 0;
        tree_f_out(file_name+"\n");
        symbol_f_out(file_name+"\n");
        if(is_correct){
            ///print tree
            semantical_analyzer.create_and_print_symbol_table_and_extend_syntactical_tree(symbol_f_out);
            syntactical_analyzer.print_syntatical_tree(tree_f_out);
        }
    }
    out_files.close();
    std::cout << "End of compiler\n";
    return 0;
}