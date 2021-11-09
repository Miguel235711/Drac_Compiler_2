#include <vector>
#include <utility>
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <functional>

#include "lexical_analyzer.h"
#include "syntactical_analyzer.h"
#include "production_rule.h"

bool testing = false;
auto cases_path_c_str = std::string("./cases/").c_str();
int SyntacticalAnalyzer::end_symbol = 40, SyntacticalAnalyzer::empty_symbol = 41;
int ProductionRule::min_symbol_value = 44, ProductionRule::max_symbol_value = 41;

int main(int argc,char ** argv){
    LexicalAnalyzer lexical_analyzer;
    std::vector<std::string> filenames; 

    std::string out_file_name;

    for(int i = 1 ; i < argc; i ++){
        auto str_n = strlen(argv[i]);
        if(out_file_name.empty() && str_n>3 && argv[i][0]=='-'&&argv[i][1]=='f'&&argv[i][2]=='='){
            out_file_name = &argv[i][3];
        }else{
            //filename
            filenames.push_back(argv[i]);
        }
    }
    DIR *dir;
    dirent * ent;

    auto f_out_stdout = [&](const std::string & x){
        std::cout << x;
    };
    std::ofstream out;
    if(!out_file_name.empty())
        out.open(out_file_name);
    auto f_out_custom_file = [&](const std::string & x){
        out << x;
    };

    std::function<void(std::string)> f_out;
    if(out_file_name.empty())
        f_out = f_out_stdout;
    else
        f_out = f_out_custom_file;
    

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
            f_out("Error getting testing directory\n");
            return EXIT_FAILURE;
        }
    }
    //init syntatical analyzer
    SyntacticalAnalyzer syntactical_analyzer("./syntactical_table.txt","./grammar.txt");
    for(auto file_name: filenames){
        std::vector<std::pair<int,std::string> > tokens;
        if(!lexical_analyzer.get_tokens(file_name,tokens)){
            f_out(std::to_string(lexical_analyzer.get_line())+":"+std::to_string(lexical_analyzer.get_column())+" Token "+lexical_analyzer.get_last_token()+" is not valid\n");
            return EXIT_FAILURE;
        }
        f_out(file_name+"\n");
        for(auto token: tokens)
            f_out("label: "+std::to_string(token.first)+" content: "+token.second+"\n");
        ///
        //add 
        tokens.push_back({SyntacticalAnalyzer::end_symbol,""}); // special symbol for syntactical analysis
        f_out(syntactical_analyzer.is_correct(tokens) ? "Correct syntaxis" : "Incorrect syntaxis");
    }
    out.close();
    return 0;
}