#include "syntactical_analyzer.h"
#include "lexical_analyzer.h"


SyntacticalAnalyzer::SyntacticalAnalyzer(const InFileNames & in_file_names){
    
    ///load rules
    std::ifstream in(in_file_names.rule_file_name);
    std::string line;
    int symbol, rule_count = 0;
    while(getline(in,line)){
        //std::cout << "line: " << line << "\n";
        std::stringstream line_stream(line);
        line_stream >> symbol;
        //std:: cout << "left non-terminal: " << symbol << "\n";
        ProductionRule * rule = new ProductionRule(rule_count++,symbol);
        while(line_stream >> symbol){
            //std::cout << "right symbol: " << symbol << "\n";
            rule->add_right_symbol(symbol);
        }
        //std::cout << "what: " << rule.get_first_right_symbol() << "\n";
        rules.push_back(rule);
    }

    //load sytacical table

    in.close();
    in.open(in_file_names.syntactical_file_name);
    int n,m,to_st_ind,op_type_int;
    std::pair<int,std::pair<OpType,int> > to_insert;
    
    in >> n;
    table.resize(n);
    //std::cout << "n: " << n << "\n";
    for(int i = 0 ; i < n ; i ++){
        table[i] = new State();
        in >> m;
        /*if(i==0)
            std::cout << "m: " << m << "\n";*/
        while(m--){
            in >> to_insert.first >> op_type_int >> to_insert.second.second;
            /*if(i==0)
                std::cout << "symbol: " << to_insert.first << "\n";*/
            to_insert.second.first = static_cast<OpType>(op_type_int);
            //std::cout << to_insert.first << " " << to_insert.second.first << " " << to_insert.second.second << "\n";
            table[i]->adjacent.insert(to_insert);
        }
    }
    in.close();
}
SyntacticalAnalyzer::~SyntacticalAnalyzer(){

}
bool SyntacticalAnalyzer::is_correct(std::vector<std::pair<int,std::string> > & tokens){
    std::stack<StackElement*> st;
    st.push(new StackElement(true,0));
    //std::cout << "what\n";
    for(auto token: tokens){
        //std::cout << "token: " << token.first << "\n";
        while(1){
            if(st.empty())
                return false;
            auto x = st.top();
            std::pair<OpType,int> mov;
            if(x->is_state){
                ///stack , token
                mov = get_mov(x->element,token.first);
            }else{
                ///stack, stack
                st.pop();
                if(st.empty())
                    return false;
                auto y = st.top();
                assert(y->is_state); /// check that it is a state
                mov = get_mov(y->element,x->element);
                st.push(x);
            }
            //std::cout << "mov type: " << mov.first << "\n";
            if(mov.first == Acc){
                //add -1 token
                auto root = new StackElement(false,rules[0]->get_left_non_terminal());
                root->node->adjacent.push_back(parent_stack_element->node);
                parent_stack_element = root;
                //std::cout << "root: " << parent_stack_element->element << " " << parent_stack_element->is_state << "\n";
                return true;
            }
            if(mov.first == None){
                //syntatical error
                return false;
            }
            if(mov.first == Reduction){
                //std::cout << "Reduction! " << st.size() << "\n";
                if(!handle_reduction(st,mov.second))
                    //syntatical error
                    return false;
            }else{
                auto last_to_push = new StackElement(true,mov.second);
                if(mov.first == Shift){
                    st.push(token.first == LexicalAnalyzer::id_symbol ?  new StackElement(false,token.first,token.second) : new StackElement(false,token.first)); ///could be terminal symbol
                    st.push(last_to_push);                   
                    break;
                }else{
                    //(mov.first == GoTo){
                    st.push(last_to_push);
                }
            }
        }
    }
    return false;
}
std::pair<OpType,int>  SyntacticalAnalyzer::get_mov(int state,int symbol){
    auto & adj = table[state]->adjacent;
    //std::cout << adj.size() << "\n";
    auto it = adj.find(symbol);
    if(it==adj.end())
        return {None,-1};
    return it->second;
}
bool SyntacticalAnalyzer::handle_reduction(std::stack<StackElement*> & st,int rule_number){
    //std::cout << "rule_number: " << rule_number << "\n";
    //std::cout << "rules: " << rules.size() << "\n";
    auto rule = rules[rule_number];
    parent_stack_element = new StackElement(false,rule->get_left_non_terminal());
    auto & parent_adjacent =  parent_stack_element->node->adjacent;
    for(int i = -1 ;rule->is_there_symbol_at(i);i--){
        auto r_symbol = rule->get_ith_right_symbol(i);
        /*if(r_symbol==empty_symbol&&!st.empty()){ ///especial case because we know that empty means only this symbol DANGEROUS!!!!!!!!!!
            ///pop state
            std::cout << "especial\n";
            st.pop();
            break;
        }*/
        //std::cout << "r_symbol: " << r_symbol << "i: " << i << "\n";
        if(!st.empty()&&st.top()->is_state){
            i++;
        }else{
            if(st.empty() || r_symbol!=st.top()->element)
                //could not reduce
                return false; 
            ///track nodes ....
            //element
            parent_adjacent.push_back(st.top()->node);
        }
        st.pop();
    }
    //std::cout << "rule_number: " << rule_number << "\n";
    std::reverse(parent_adjacent.begin(),parent_adjacent.end());
    st.push(parent_stack_element);
    return true;
}  


void SyntacticalAnalyzer::print_syntatical_tree(std::function<void(std::string)> & f_out){
    //f_out("# <program>\n");
    print_syntatical_tree(parent_stack_element->node,f_out,1);
}

void SyntacticalAnalyzer::print_syntatical_tree(SyntacticalNode * node,std::function<void(std::string)> & f_out,int level){
    f_out(std::string(level,'#')+" "+LexicalAnalyzer::symbol_lexval_to_name[node->symbol]+"\n");
    for(auto child : node->adjacent)
        print_syntatical_tree(child,f_out,level+1);
}

SyntacticalNode * SyntacticalAnalyzer::get_syntatical_tree_root(){
    return parent_stack_element->node;
}