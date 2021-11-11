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

bool testing = false;
auto cases_path_c_str = std::string("./cases/").c_str();
int SyntacticalAnalyzer::end_symbol = 40;
char subbuff[10000];
std::unordered_map<int,std::string> LexicalAnalyzer::symbol_lexval_to_name;
int LexicalAnalyzer::id_symbol = 1;
//int ProductionRule::min_symbol_value = 44, ProductionRule::max_symbol_value = 40;
auto in_file_names = InFileNames("./syntactical_table.txt","./grammar.txt","./symbols.txt");

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
    SemanticalAnalyzer semantical_analyzer(syntactical_analyzer,{-6,-9},{-12},-6,-9);
    out_files.open();
    auto lexical_f_out = out_files.get_func('l'), tree_f_out = out_files.get_func('t'), symbol_f_out = out_files.get_func('s');
    for(auto file_name: filenames){
        std::vector<std::pair<int,std::string> > tokens;
        if(!lexical_analyzer.get_tokens(file_name,tokens)){
            (std::to_string(lexical_analyzer.get_line())+":"+std::to_string(lexical_analyzer.get_column())+" Token "+lexical_analyzer.get_last_token()+" is not valid\n");
            return EXIT_FAILURE;
        }
        lexical_f_out(file_name+"\n");
        for(auto token: tokens)
            lexical_f_out("label: "+LexicalAnalyzer::symbol_lexval_to_name[token.first]+" content: "+token.second+"\n");
        ///
        //add 
        tokens.push_back({SyntacticalAnalyzer::end_symbol,""}); // special symbol for syntactical analysis
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
            syntactical_analyzer.print_syntatical_tree(tree_f_out);
            semantical_analyzer.print_symbol_table(symbol_f_out);
        }
    }
    out_files.close();
    return 0;
}