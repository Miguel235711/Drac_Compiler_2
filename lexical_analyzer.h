#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <utility>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <cwchar>

//testing libraries

#include <vector>
#include <cassert>

#include "lexical_automata.h"
#include "token.h"

class LexicalAnalyzer{
    public:
        LexicalAnalyzer();
        virtual ~LexicalAnalyzer();
        bool get_tokens(const std::string & in_file_name,std::vector<Token> & tokens);
        std::string get_last_token();
        int get_column();
        int get_line();
        bool valid_lit_int(const std::string & lit_int);
        static std::unordered_map<int,std::string> symbol_lexval_to_name;
        static int 
            id_symbol
            ,curly_open_symbol
            ,curly_close_symbol
            ,fun_def_symbol
            ,break_symbol
            ,stmt_do_while_symbol
            ,stmt_while_symbol
            ,param_list_symbol
        ;
    private:
        LexicalAutomata automata;
        std::string token;
        int line=1,l_col=0,col=0;
        bool handle_char(char c,std::vector<Token> & tokens,std::ifstream & in);
        const int64_t min_value = -2147483648, max_value = 2147483647; 
};

#endif // LEXICAL_ANALYZER_H