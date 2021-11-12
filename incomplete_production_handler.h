#ifndef INCOMPLETE_PRODUCTION_HANDLER_H
#define INCOMPLETE_PRODUCTION_HANDLER_H

#include<string>
#include <vector>

#include "production_rule.h"
#include "unordered_map"
#include "lexical_analyzer.h"

class IncompleteProductionHandler{

    public:

        IncompleteProductionHandler(std::vector<ProductionRule*>rules);
        virtual ~IncompleteProductionHandler();

        void restart();
        std::vector<std::string> closest_rule_expected_next_token();
        bool next(int label);
    private:

        std::vector<ProductionRule*> rules;
        std::unordered_map<ProductionRule*,std::pair<int,int> > candidates; // ind,missing_label 
        bool just_restarted=true;
        std::vector<int>last_symbols;
};

#endif //INCOMPLETE_PRODUCTION_HANDLER_H