#ifndef ID_NODE_H
#define ID_NODE_H

#include <string>

    enum Mode{
        var_def,fun_def_symbol,var_ref,fun_ref,none
    };

    struct IdNode{
        int scope,index;
        Mode id_type;
        std::string name;
        static int index_counter;
        int arity=-1; /// only useful for <id> that has <fun-def> as immediate parent
        IdNode(int scope,Mode id_type,std::string name):scope(scope),id_type(id_type),name(name){
            index=index_counter++;
        }
    };
#endif // ID_NODE_H 