#ifndef LEXICAL_AUTOMATA_H
#define LEXICAL_AUTOMATA_H

#include <string>
#include <utility>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <iterator>

//testing libraries

#include <vector>
#include <cassert>

class LexicalAutomata{

    struct Node{
        Node(){
            is_all_but=false;
            label=-1;
        };
        std::unordered_map<char,Node*> children; //use '\0' for all_except
        std::unordered_set<char> but;
        int label;
        bool is_all_but;
    };

    public:
        LexicalAutomata();
        virtual ~LexicalAutomata();
        void restart();
        bool next(char x); //True if successfull , False if failure
        bool in_token();//True if in reserved word
        int get_token_label();
    private:
        Node * cur;
        int openQuotation = 0;
        void add_to_trie(std::string token,int label);
        std::vector<Node*> adhoc_nodes = std::vector<Node*>(28);
        void add_adhoc_nodes();
        void set_all_but(Node * from ,Node * to, std::unordered_set<char> but);
        void add_specific(std::unordered_map<char,Node *> & children,Node * to, std::vector<char> which);
        void add_range(char begin,char end,std::vector<char> & v);
        void add_to_identifier_edges(Node * node);
        std::vector<char> 
            identifer_chars
            ,number_chars
            ,letter_chars
            ,escaped_identifier_chars={'t','b','r','n','\'','"','\\','$'}
            ,ignore_chars={' ','\t','\n'}
            ,hex_chars;
};

#endif // LEXICAL_AUTOMATA_H