#include "lexical_analyzer.h"

LexicalAnalyzer::LexicalAnalyzer(){

}
LexicalAnalyzer::~LexicalAnalyzer(){

}
int LexicalAnalyzer::get_line(){
    return line;
}
int LexicalAnalyzer::get_column(){
    return col;
}
bool LexicalAnalyzer::valid_lit_int(const std::string & lit_int){
    // bool is_neg = lit_int[0]=='-';
    // int i = is_neg;
    // for(;i<lit_int.size() && lit_int[i]=='0';i++);
    // auto lit_int_value = lit_int.substr(i,lit_int.size()-i);
    // return lit_int_value <= (is_neg ? max_neg_abs_value : max_pos_abs_value);
    if(lit_int.size()>11)
        return false;
    char* pEnd;
    int64_t x = strtoll(lit_int.c_str(),&pEnd,10);
    return min_value<=x&&x<=max_value;
}
bool LexicalAnalyzer::handle_char(char c,std::vector<Token> & tokens,std::ifstream & in){
    if(c=='\n'||c=='\r'){
        line++,l_col=col,col=0;
        //std::cout << "line++ " << " line= " << line << "\n" ;
    }else{ 
        col++;
        //std::cout << "c: *" << c  << "* col=" << col << "\n";
    }
    if(automata.next(c))
        token.push_back(c);
    else{
        in.unget();
        if(c=='\n'||c=='\r'){
            //std::cout << "line--\n";
            line--,col=l_col;
        }else{
            col--;
        }
        if(automata.in_token()){
            //token found
            int label = automata.get_token_label();
            if(label){
                auto token_ = Token(label,token,{line,col});
                if(token_.label==2){
                    if(!valid_lit_int(token_.content))
                        std::cout << line << ":" << col << ":Warnning:overflow: not valid <lit-int> -> " << token_.content << ", it should be in the range [-2^31,2^31-1]\n";
                    //     std::cout << "Warnning: overflow:" << line << ":" << col <<  " not valid <lit-int> -> " << token_.content << ", it should be 10 or less digits\n";
                    // else{
                    //     char* pEnd;
                    //     int64_t x = strtoll(token_.content.c_str(),&pEnd,10);
                    //     std::cout << "x: " << x << "\n";
                    //     if(x<-2147483648||x>2147483647)

                    // }
                }
                tokens.push_back(token_);
            }
            automata.restart();
        }else{
            if(!isspace(c))
                token.push_back(c);
            automata.restart();
            return false; // error
        }
        token="";
    }
    return true;
}
bool LexicalAnalyzer::get_tokens(const std::string & in_file_name,std::vector<Token> & tokens){
    std::ifstream in(in_file_name);
    line=1;
    char c;
    tokens.clear();
    while(in.get(c)){
        //std::cout << "c: " << c << "\n";
        if(!handle_char(c,tokens,in))
            return false;
    }
    //for last token
    if(!handle_char('\n',tokens,in))
        return false;
    return true;
}
std::string LexicalAnalyzer::get_last_token(){
    return token;
}