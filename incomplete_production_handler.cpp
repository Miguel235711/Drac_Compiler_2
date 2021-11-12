#include "incomplete_production_handler.h"


IncompleteProductionHandler::IncompleteProductionHandler(std::vector<ProductionRule*>rules):rules(rules){
    restart();
}
IncompleteProductionHandler::~IncompleteProductionHandler(){
}

void IncompleteProductionHandler::restart(){
    just_restarted=true;
    candidates.clear();
}
std::vector<std::string> IncompleteProductionHandler::closest_rule_expected_next_token(){
    //std::cout << "size: " << candidates.size() << "\n";
    //std::cout << "closest_rule_expected_next_token\n";
    if(candidates.empty() && last_symbols.empty())
        return {"<Uknown"};
    std::vector<std::string> ans;
    auto symbols = last_symbols;
    if(!candidates.empty())
        symbols.clear();
    for(auto  & candidate : candidates){
        symbols.push_back(candidate.second.second);
        //std::cout << "**left non terminal: " << candidate.first->get_left_non_terminal() << " " << candidate.second.first << "\n";
        /*if(!candidate.first->is_there_symbol_at(candidate.second.first)){
            symbol = candidate.second.second;
        }*/
    }
    //std::cout << candidates.begin()->first->get_left_non_terminal() << "\n";
    for(auto symbol: symbols){
        auto it = LexicalAnalyzer::symbol_lexval_to_name.find(symbol);
            if(it==LexicalAnalyzer::symbol_lexval_to_name.end())    
                ans.push_back("<Empty>");
            else    
                ans.push_back(it->second);
    }
    return ans;
}
bool IncompleteProductionHandler::next(int label){
    if(just_restarted){
        just_restarted = false;
        //calculate candidates
        for(auto candidate: rules){
            int i = -1;
            for(;candidate->is_there_symbol_at(i)&&candidate->get_ith_right_symbol(i)!=label;i--);
            if(candidate->is_there_symbol_at(i)){
                //std::cout << "left non terminal: " << candidate->get_left_non_terminal() << " " << i << "\n";
                ///from here
                candidates[candidate] = {i-1,candidate->is_there_symbol_at(i+1)?candidate->get_ith_right_symbol(i+1):41}; ///41 should not be possible
            }
        }
    }else{
        std::vector<ProductionRule*> to_erase;
        last_symbols.clear();
        for(auto candidate: candidates){
            auto prd_rule = candidate.first;
            auto & i = candidate.second.first;
            if(!prd_rule->is_there_symbol_at(i)|| prd_rule->get_ith_right_symbol(i)!=label){
                last_symbols.push_back(candidate.second.second);
                to_erase.push_back(prd_rule);
            }
            else
                i--;
        }
        ///erase
        for(auto to_er : to_erase)
            candidates.erase(to_er);
    }
    return candidates.size() > 1 ; // more than one candidate
}