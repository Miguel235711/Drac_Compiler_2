#ifndef SEMANTICAL_RULES_INIT_H
#define SEMANTICAL_RULES_INIT_H

#include "semantical_rule.h"
#include "syntactical_analyzer.h"

#define BEGIN_SEMANTIC_RULE new SemanticalRule([](std::function<CompleteType *(int)>get_rule_type_at) /* -> CompleteType * */{
#define END_SEMANTIC_RULE })
#define ERROR_SEMANTIC_ANS new CompleteType(var_entity,error_type,{})
#define RETURN_VAR_ENTITY_AND_VOID_TYPE return new CompleteType(var_entity,void_type,{});


bool is_not_var_entity_or_not_void(CompleteType * x){
    return x->symbol_entity != var_entity || x->type_value != void_type;
}

bool is_not_var_entity_or_not_bool(CompleteType * x){ 
    return x->symbol_entity != var_entity || x->type_value != bool_type;
}

bool is_numeric_variable_type(CompleteType * type){
    return
        type->type_value == int_type
        ||
        type->type_value == bool_type
    ;
}

bool is_any_variable_type(CompleteType * type){
    return 
        type->symbol_entity == var_entity
        &&
        (
            is_numeric_variable_type(type)
            ||
            type->type_value == array_type
            ||
            type->type_value == char_type
            ||
            type->type_value == string_type
        )
    ;
}

CompleteType * handle_variable_type(CompleteType * type, TypeValue target_type, TypeValue result_type){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        type->type_value != target_type
    )
        return ans;
    ans->type_value = result_type;
    return ans;
}

CompleteType * handle_bool_variable_single(CompleteType * type,TypeValue result_type){
    return handle_variable_type(type,bool_type,result_type);
}

CompleteType * handle_int_variable_single(CompleteType * type,TypeValue result_type){
    return handle_variable_type(type,int_type,result_type);
}

CompleteType * handle_char_variable_single(CompleteType * type,TypeValue result_type){
    return handle_variable_type(type,char_type,result_type);
}

CompleteType * handle_string_variable_single(CompleteType * type,TypeValue result_type){
    return handle_variable_type(type,string_type,result_type);
}

CompleteType * handle_bridge_variable_type(CompleteType * type){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        type->type_value == error_type
    )
        return ans;
    ans->type_value = type->type_value;
    return ans;
}

CompleteType * handle_numeric_variable_single(CompleteType * type,TypeValue type_result){
    auto ans = ERROR_SEMANTIC_ANS;
    if(!is_numeric_variable_type(type))
        return ans;
    ans->type_value = type_result;
    return ans;
}

CompleteType * handle_any_variable_single(CompleteType * type,TypeValue type_result){
    auto ans = ERROR_SEMANTIC_ANS;
    if(!is_any_variable_type(type))
        return ans;
    ans->type_value = type_result;
    return ans;
}

CompleteType * handle_void_single(CompleteType * type,TypeValue type_result){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        type->symbol_entity != var_entity || type->type_value != void_type
    )
        return ans;
    ans->type_value = type_result;
    return ans;
}
CompleteType * handle_not_var_entity_or_not_bool_single(CompleteType * type,TypeValue type_result){
    auto ans = ERROR_SEMANTIC_ANS;
    if(is_not_var_entity_or_not_bool(type))
        return ans;
    ans->type_value = type_result;
}
CompleteType * handle_not_var_entity_or_not_bool_multiple(std::vector<size_t> indexes,std::function<CompleteType *(int)>get_rule_type_at,TypeValue type_result){
    CompleteType * ans = ERROR_SEMANTIC_ANS;
    for(const auto ind: indexes)
        if((ans = handle_not_var_entity_or_not_bool_single(get_rule_type_at(ind),type_result))->type_value==error_type)
            return ans;
    return ans;
}
CompleteType * expr_list_handler(CompleteType * expr, CompleteType * expr_list_cont){
    auto ans = ERROR_SEMANTIC_ANS;
    if(
        !is_any_variable_type(expr)
        ||
        is_not_var_entity_or_not_void(expr_list_cont)
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
        auto fun_def_symbol = get_rule_type_at(0);
        return new CompleteType(fun_entity,fun_def_symbol->symbol_entity == fun_entity && fun_def_symbol->type_value == void_type ? void_type : error_type,{});
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
        if(is_not_var_entity_or_not_void(id))  //be careful!!! depends if normal or function call
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
        if(is_not_var_entity_or_not_void(id))  //be careful!!!!
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
            is_not_var_entity_or_not_void(id)
            || 
            param_list->symbol_entity != var_entity || param_list->type_value != idlist_type
            ||
            is_not_var_entity_or_not_void(var_def_list)
            ||
            is_not_var_entity_or_not_void(stmt_list)
           
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
            is_not_var_entity_or_not_void(var_def_list)
            ||
            is_not_var_entity_or_not_void(var_def)
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
            is_not_var_entity_or_not_void(stmt_list)
            || stmt->type_value == error_type
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //17  ‹stmt-list› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //18 ‹stmt› → ‹stmt-assign›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //19 ‹stmt› → ‹stmt-incr›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //20 ‹stmt› → ‹stmt-decr›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //21 ‹stmt› → ‹stmt-fun-call›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //22 ‹stmt› → ‹stmt-if›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //23 ‹stmt› → ‹stmt-while›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //24 ‹stmt› → ‹stmt-do-while›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //25 ‹stmt› → ‹stmt-break›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //26 ‹stmt› → ‹stmt-return› ---> especial case of <stmt> type being equal a variable type !!!!!
    BEGIN_SEMANTIC_RULE
        auto stmt_return = get_rule_type_at(0);
        return handle_any_variable_single(stmt_return,stmt_return->type_value);
    END_SEMANTIC_RULE,
    //27 ‹stmt› →‹stmt-empty›
    BEGIN_SEMANTIC_RULE
        return handle_void_single(get_rule_type_at(0),void_type);
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
            expr->symbol_entity!=var_entity || !is_any_variable_type(expr)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //29 ‹stmt-incr› →	inc ‹id› ;
    BEGIN_SEMANTIC_RULE
        return handle_numeric_variable_single(get_rule_type_at(1),void_type);
    END_SEMANTIC_RULE,
    //30 ‹stmt-decr› → dec ‹id› ;
    BEGIN_SEMANTIC_RULE
        return handle_numeric_variable_single(get_rule_type_at(1),void_type);
    END_SEMANTIC_RULE,
    //31 ‹stmt-fun-call› → ‹fun-call› ;
    BEGIN_SEMANTIC_RULE
        return handle_any_variable_single(get_rule_type_at(0),void_type);
    END_SEMANTIC_RULE,
    //32 ‹fun-call› → ‹id› ( ‹expr-list› )
    BEGIN_SEMANTIC_RULE
        //assign types to elements and call transversal of the syntax tree from this - HARD!!!
        get_rule_type_at(2);
        RETURN_VAR_ENTITY_AND_VOID_TYPE
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
            is_not_var_entity_or_not_bool(expr)
            || 
            is_not_var_entity_or_not_void(stmt_list)
            ||
            is_not_var_entity_or_not_void(else_if_list)
            ||
            is_not_var_entity_or_not_void(else_)
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
            is_not_var_entity_or_not_void(else_if_list)
            ||
            is_not_var_entity_or_not_bool(expr)
            ||
            is_not_var_entity_or_not_void(stmt_list)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
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
            is_not_var_entity_or_not_void(stmt_list)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //41 ‹else› → ε
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE;
    END_SEMANTIC_RULE,
    //42 ‹stmt-while› →	 while ( ‹expr› ) { ‹stmt-list› }
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            expr = get_rule_type_at(2)
            ,stmt_list = get_rule_type_at(5)
        ;
        if(
            is_not_var_entity_or_not_bool(expr)
            ||
            is_not_var_entity_or_not_void(stmt_list)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //43 ‹stmt-do-while› → do { ‹stmt-list› } while ( ‹expr› ) ;
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            stmt_list = get_rule_type_at(5)
            ,expr = get_rule_type_at(2)
        ;
        if(
            is_not_var_entity_or_not_void(stmt_list)
            ||
            is_not_var_entity_or_not_bool(expr)
        )
            return ans;
        ans->type_value = void_type;
        return ans;
    END_SEMANTIC_RULE,
    //44 ‹stmt-break› → break ;
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //45 ‹stmt-return› → return ‹expr› ;
    BEGIN_SEMANTIC_RULE
        return handle_any_variable_single(get_rule_type_at(1),void_type);
    END_SEMANTIC_RULE,
    //46 ‹stmt-empty› → ;
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //47 ‹expr› → ‹expr-or›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //48 ‹expr-or› → ‹expr-or› or ‹expr-and›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_multiple({0,2},get_rule_type_at,bool_type);
    END_SEMANTIC_RULE,
    //49 ‹expr-or› → ‹expr-and›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //50 ‹expr-and› → ‹expr-and› and ‹expr-comp›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_multiple({0,2},get_rule_type_at,bool_type);
    END_SEMANTIC_RULE,
    //51 ‹expr-and› → ‹expr-comp›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //52 ‹expr-comp› → ‹expr-comp› ‹op-comp› ‹expr-rel› -> accept every data type to cast to boolean
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            expr_comp = get_rule_type_at(0),
            op_comp = get_rule_type_at(1),
            expr_rel = get_rule_type_at(2)
        ;
        if(
            is_not_var_entity_or_not_bool(expr_comp)
            ||
            is_not_var_entity_or_not_void(op_comp)
            ||
            is_not_var_entity_or_not_bool(expr_rel)
        )
            return ans;
        ans->type_value = bool_type;
        return ans;
    END_SEMANTIC_RULE,
    //53 ‹expr-comp› → ‹expr-rel›
    BEGIN_SEMANTIC_RULE
        return handle_not_var_entity_or_not_bool_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //54 ‹op-comp› → ==
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //55 ‹op-comp› → <>
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //56 ‹expr-rel› → ‹expr-rel› ‹op-rel› ‹expr-add›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto
            expr_rel = get_rule_type_at(0),
            op_rel = get_rule_type_at(1),
            expr_add = get_rule_type_at(2) 
        ;
        if(
            is_not_var_entity_or_not_bool(expr_rel)
            ||
            is_not_var_entity_or_not_void(op_rel)
            ||
            !is_numeric_variable_type(expr_add)
        )
            return ans;
        ans->type_value = bool_type;
        return ans;
    END_SEMANTIC_RULE,
    //57 ‹expr-rel› → ‹expr-add›
    BEGIN_SEMANTIC_RULE
        return handle_numeric_variable_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //58 ‹op-rel› → <
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //59 ‹op-rel› → <=
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //60 ‹op-rel› → >
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //61 ‹op-rel› → >=
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //62 ‹expr-add› → ‹expr-add› ‹op-add› ‹expr-mul›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto
            expr_add = get_rule_type_at(0),
            op_add = get_rule_type_at(1),
            expr_mul = get_rule_type_at(2) 
        ;
        if(
            !is_numeric_variable_type(expr_add)
            ||
            is_not_var_entity_or_not_void(op_add)
            ||
            !is_numeric_variable_type(expr_mul)
        )
            return ans;
        ans->type_value = int_type;
        return ans;
    END_SEMANTIC_RULE,
    //63 ‹expr-add› → ‹expr-mul›
    BEGIN_SEMANTIC_RULE
        auto expr_mul = get_rule_type_at(0);
        return handle_numeric_variable_single(expr_mul,expr_mul->type_value);
    END_SEMANTIC_RULE,
    //64 ‹op-add› → +
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //65 ‹op-add› → −
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //66 ‹expr-mul› → ‹expr-mul› ‹op-mul› ‹expr-unary›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto
            expr_mul = get_rule_type_at(0),
            op_mul = get_rule_type_at(1),
            expr_unary = get_rule_type_at(2) 
        ;
        if(
            !is_numeric_variable_type(expr_mul)
            ||
            is_not_var_entity_or_not_void(op_mul)
            ||
            !is_numeric_variable_type(expr_unary)
        )
            return ans;
        ans->type_value = int_type;
        return ans;
    END_SEMANTIC_RULE,
    //67 ‹expr-mul› → ‹expr-unary›
    BEGIN_SEMANTIC_RULE
        auto expr_unary = get_rule_type_at(0);
        return handle_numeric_variable_single(expr_unary,expr_unary->type_value);
    END_SEMANTIC_RULE,
    //68 ‹op-mul› → *
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //69 ‹op-mul› → /
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //70 ‹op-mul› → %
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //71 ‹expr-unary› →	‹op-unary› ‹expr-unary›
    BEGIN_SEMANTIC_RULE
        auto ans = ERROR_SEMANTIC_ANS;
        auto 
            op_unary = get_rule_type_at(0),
            expr_unary = get_rule_type_at(1)
        ;
        if(
            is_not_var_entity_or_not_void(op_unary)
            ||
            !is_numeric_variable_type(expr_unary)
        )
            return ans;
        ans->type_value = int_type;
        return ans;
    END_SEMANTIC_RULE,
    //72 ‹expr-unary› →	‹expr-primary› ---> ‹expr-unary› is ONLY a bridge to ‹expr-primary›
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //73 ‹op-unary› → + 
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //74 ‹op-unary› → -
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //75 ‹op-unary› → not
    BEGIN_SEMANTIC_RULE
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //76 ‹expr-primary› → ‹id› ---> ‹expr-primary› is ONLY a bridge to ‹id›
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //77 ‹expr-primary› → ‹fun-call› ---> ‹expr-primary› is ONLY a bridge to ‹fun-call›
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //78 ‹expr-primary› → ‹array› ---> ‹expr-primary› is ONLY a bridge to ‹array›
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //79 ‹expr-primary› → ‹lit› --- > ‹expr-primary› is ONLY a bridge to ‹lit›
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(0));
    END_SEMANTIC_RULE,
    //80 ‹expr-primary› → ( ‹expr› ) --- > ‹expr-primary› is ONLY a bridge to ( ‹expr› )
    BEGIN_SEMANTIC_RULE
        return handle_bridge_variable_type(get_rule_type_at(1));
    END_SEMANTIC_RULE,
    //81 ‹array› →	[ ‹expr-list› ] ---> HARD !!!
    BEGIN_SEMANTIC_RULE
        get_rule_type_at(1);
        RETURN_VAR_ENTITY_AND_VOID_TYPE
    END_SEMANTIC_RULE,
    //82 ‹lit› → ‹lit-bool› --- > <lit> is ONLY a bridge to ‹lit-bool›
    BEGIN_SEMANTIC_RULE
        return handle_bool_variable_single(get_rule_type_at(0),bool_type);
    END_SEMANTIC_RULE,
    //83 ‹lit› → ‹lit-int› --- > <lit> is ONLY a bridge to ‹lit-int›
    BEGIN_SEMANTIC_RULE
        return handle_int_variable_single(get_rule_type_at(0),int_type);
    END_SEMANTIC_RULE,
    //84 ‹lit› → ‹lit-char› --- > <lit> is ONLY a bridge to ‹lit-char›
    BEGIN_SEMANTIC_RULE
        return handle_char_variable_single(get_rule_type_at(0),char_type);
    END_SEMANTIC_RULE,
    //85 ‹lit› → ‹lit-str› --- > <lit> is ONLY a bridge to ‹lit-str›
    BEGIN_SEMANTIC_RULE
        return handle_string_variable_single(get_rule_type_at(0),string_type);
    END_SEMANTIC_RULE
}; //handle later

#endif // SEMANTICAL_RULES_INIT_H