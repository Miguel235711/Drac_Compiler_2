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
bool LexicalAnalyzer::handle_char(char c,std::vector<std::pair<int,std::string> > & tokens,std::ifstream & in){
    if(c=='\n'||c=='\r')
        line++,l_col=col,col=0;
    else 
        col++;
    if(automata.next(c))
        token.push_back(c);
    else{
        in.unget();
        if(c=='\n'||c=='\r')
            line--,col=l_col;
        if(automata.in_token()){
            //token found
            int label = automata.get_token_label();
            if(label)
                tokens.push_back({label,token});
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
bool LexicalAnalyzer::get_tokens(const std::string & in_file_name,std::vector<std::pair<int,std::string> > & tokens){
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