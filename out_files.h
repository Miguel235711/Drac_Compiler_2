#ifndef OUT_FILE_NAMES_H
#define OUT_FILE_NAMES_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

class OutFiles{
    public:
    
        OutFiles();
        virtual ~OutFiles();
        std::string& operator[](char flag);
        OutFiles& operator=(const OutFiles& out_files);
        bool is_flag_valid(char flag);
        std::function<void(std::string)> get_func(char flag);
        void open();
        void close();
    
    private:

        const std::unordered_set<char> flags = {
            'l'
            ,'t'
            ,'s'
        };
        std::unordered_map<char,std::string> out_type_to_file_name;
        std::unordered_map<char,std::function<void(const std::string)> > out_type_to_func;
        const std::function<void(std::string)> f_out_stdout = [&](const std::string x){
            std::cout << x;
        };
        std::ofstream out_l,out_t,out_s;
};

#endif // OUT_FILE_NAMES_H