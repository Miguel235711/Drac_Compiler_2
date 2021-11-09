#include "lexical_automata.h"

LexicalAutomata::LexicalAutomata(){    

    for(auto & adhoc_node : adhoc_nodes)
        adhoc_node = new Node();
    cur = adhoc_nodes[0];

    add_range('a','f',letter_chars);
    add_range('A','F',letter_chars);

    hex_chars = letter_chars;

    add_range('g','z',letter_chars);
    add_range('G','Z',letter_chars);

    add_range('0','9',number_chars);

    copy(number_chars.begin(),number_chars.end(),back_inserter(hex_chars));

    identifer_chars = letter_chars;
    copy(number_chars.begin(),number_chars.end(),back_inserter(identifer_chars));
   
    
    identifer_chars.push_back('_');

    add_adhoc_nodes();
    
    std::ifstream in("reserved.in");
    int label;
    std::string line,token;
    while(std::getline(in,line)){
        std::stringstream line_stream(line);
        line_stream >> label >> token;
        add_to_trie(token,label);
    }
    add_to_identifier_edges(adhoc_nodes[0]);
}
void LexicalAutomata::add_adhoc_nodes(){
    //0
    auto & _0_ch = adhoc_nodes[0]->children;
    _0_ch['"']=adhoc_nodes[10];
    _0_ch['\'']=adhoc_nodes[8];
    add_specific(_0_ch,adhoc_nodes[12],ignore_chars);
    _0_ch['-']=adhoc_nodes[1];
    _0_ch['(']=adhoc_nodes[3];
    //add_specific(_0_ch,adhoc_nodes[6],letter_chars);
    add_specific(_0_ch,adhoc_nodes[7],number_chars);
    //1
    auto & _1_ch = adhoc_nodes[1]->children;
    _1_ch['-']=adhoc_nodes[2];
    //2
    adhoc_nodes[2]->label=0;
    auto & _2_ch = adhoc_nodes[2]->children;
    set_all_but(adhoc_nodes[2],adhoc_nodes[2],{'\r','\n'});
    //3
    auto & _3_ch = adhoc_nodes[3]->children;
    _3_ch['*']=adhoc_nodes[4];
    //4
    auto & _4_ch = adhoc_nodes[4]->children;
    _4_ch['*']=adhoc_nodes[5];
    set_all_but(adhoc_nodes[4],adhoc_nodes[4],{'*'});
    //5
    auto & _5_ch = adhoc_nodes[5]->children;
    _5_ch[')']=adhoc_nodes[12];
    set_all_but(adhoc_nodes[5],adhoc_nodes[4],{')'});
    //6
    adhoc_nodes[6]->label=1;
    auto & _6_ch = adhoc_nodes[6]->children;
    add_specific(_6_ch,adhoc_nodes[6],identifer_chars);
    //7
    adhoc_nodes[7]->label=2;
    auto & _7_ch = adhoc_nodes[7]->children;
    add_specific(_7_ch,adhoc_nodes[7],number_chars);
    //8
    auto & _8_ch = adhoc_nodes[8]->children;
    _8_ch['\\']=adhoc_nodes[13];
    set_all_but(adhoc_nodes[8],adhoc_nodes[20],{'\'','\n','\r','\t','"'});
    //9
    adhoc_nodes[9]->label=3;
    //10
    auto & _10_ch = adhoc_nodes[10]->children;
    _10_ch['"']=adhoc_nodes[11];
    _10_ch['\\']=adhoc_nodes[21];
    set_all_but(adhoc_nodes[10],adhoc_nodes[10],{'\'','\n','\r','\t','"'});
    //11
    adhoc_nodes[11]->label=4;
    //12
    adhoc_nodes[12]->label=0;
    //13
    auto & _13_ch = adhoc_nodes[13]->children;
    _13_ch['u']=adhoc_nodes[14];
    add_specific(_13_ch,adhoc_nodes[20],{'n','r','t','\\','\'','"'});
    //14
    auto & _14_ch = adhoc_nodes[14]->children;
    add_specific(_14_ch,adhoc_nodes[15],hex_chars);
    //15
    auto & _15_ch = adhoc_nodes[15]->children;
    add_specific(_15_ch,adhoc_nodes[16],hex_chars);
    //16
    auto & _16_ch = adhoc_nodes[16]->children;
    add_specific(_16_ch,adhoc_nodes[17],hex_chars);
    //17
    auto & _17_ch = adhoc_nodes[17]->children;
    add_specific(_17_ch,adhoc_nodes[18],hex_chars);
    //18
    auto & _18_ch = adhoc_nodes[18]->children;
    add_specific(_18_ch,adhoc_nodes[19],hex_chars);
    //19
    auto & _19_ch = adhoc_nodes[19]->children;
    add_specific(_19_ch,adhoc_nodes[20],hex_chars);
    //20
    auto & _20_ch = adhoc_nodes[20]->children;
    _20_ch['\'']=adhoc_nodes[9];
    //21
    auto & _21_ch = adhoc_nodes[21]->children;
    _21_ch['u'] = adhoc_nodes[22];
    add_specific(_21_ch,adhoc_nodes[10],{'n','r','t','\\','\'','"'});
    //22
    auto & _22_ch = adhoc_nodes[22]->children;
    add_specific(_22_ch,adhoc_nodes[23],hex_chars);
    //23
    auto & _23_ch = adhoc_nodes[23]->children;
    add_specific(_23_ch,adhoc_nodes[24],hex_chars);
    //24
    auto & _24_ch = adhoc_nodes[24]->children;
    add_specific(_24_ch,adhoc_nodes[25],hex_chars);
    //25
    auto & _25_ch = adhoc_nodes[25]->children;
    add_specific(_25_ch,adhoc_nodes[26],hex_chars);
    //26
    auto & _26_ch = adhoc_nodes[26]->children;
    add_specific(_26_ch,adhoc_nodes[27],hex_chars);
    //27
    auto & _27_ch = adhoc_nodes[27]->children;
    add_specific(_27_ch,adhoc_nodes[10],hex_chars);
    
}
void LexicalAutomata::set_all_but(Node * from,Node * to, std::unordered_set<char> but){
    //dangerous!!
    from->children['\0']=to;
    from->is_all_but=true;
    from->but=but;
}
void LexicalAutomata::add_specific(std::unordered_map<char,Node *> & children,Node * to, std::vector<char> which){
    for(auto c : which)
        children[c]=to;
}
void LexicalAutomata::add_range(char begin,char end,std::vector<char> & v){
    if(begin<=end)
        for(;;begin++){
            v.push_back(begin);
            if(begin==end)
                break;
        }
}
LexicalAutomata::~LexicalAutomata(){

}
bool LexicalAutomata::next(char x){
    auto & children = cur->children;
    auto & but = cur->but;
    auto next = children.find(x);
    if(next!=children.end()){
        cur = next->second;
        return true;
    }
    if(cur->is_all_but&&but.find(x)==but.end()){
        //x is accepted
        cur = children['\0'];
        return true;
    }
    return false;
}
bool LexicalAutomata::in_token(){
    return cur->label>-1;
}
int LexicalAutomata::get_token_label(){
    return cur->label;
}
void LexicalAutomata::add_to_trie(std::string token, int label){
    Node * cur = adhoc_nodes[0];
    for(char c: token){
        auto & children = cur->children;
        auto next = children.find(c);
        if(next==children.end())
            next = children.insert({c,new Node()}).first;
        cur = next->second;
    }
    cur->label = label;
    //cur-> = ;
}
void LexicalAutomata::restart(){
    cur = adhoc_nodes[0];
    openQuotation=0;
}
void LexicalAutomata::add_to_identifier_edges(Node * node){
    auto & children =  node->children;
    for(char c:identifer_chars){
        if((!isdigit(c)&&c!='_')||node!=adhoc_nodes[0]){
            auto it = children.find(c);
            if(it==children.end())
                //add edge from node to identifier through c
                children[c]=adhoc_nodes[6];
            else{
                //add that this node is identifier if not already something else
                if(it->second->label<0)
                    it->second->label=1;
                add_to_identifier_edges(it->second);
            }
        }
    }
}