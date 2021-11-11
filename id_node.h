#ifndef ID_NODE_H
#define ID_NODE_H

#include <string>

    enum Mode{
        var_def,fun_def,ref,none
    };

    struct IdNode{
        int scope,index;
        Mode id_type;
        std::string name;
        static int index_counter;
        IdNode(int scope,Mode id_type,std::string name):scope(scope),id_type(id_type),name(name){
            index=index_counter++;
        }
    };
#endif // ID_NODE_H 