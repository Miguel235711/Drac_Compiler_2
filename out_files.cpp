#include "out_files.h"

OutFiles::OutFiles(){
    for(auto flag : flags)
        out_type_to_func[flag]=f_out_stdout;
}

OutFiles::~OutFiles(){
    
}

std::string& OutFiles::operator[](char flag){
    return out_type_to_file_name[flag];
}
bool OutFiles::is_flag_valid(char flag){
    return flags.find(flag)!=flags.end();
}
void OutFiles::open(){
    //std::cout << out_type_to_file_name.size() << "\n";
    for(auto out_type_and_file_name: out_type_to_file_name){
        auto file_name =  out_type_and_file_name.second;
        const auto flag = out_type_and_file_name.first;
        file_name.insert(file_name.find_last_of('.'),"."+std::string(1,flag));
        auto & out_stream = flag == 'l' ? out_l : flag == 't' ? out_t : out_s;
        out_stream.open(file_name);
        out_type_to_func[flag] = [&](std::string content){
            out_stream << content;
            //std::cout << "content: " << content << "\n";
        };
    }
}
void OutFiles::close(){
    out_l.close();
    out_t.close();
    out_s.close();
}
std::function<void(std::string)> OutFiles::get_func(char flag){
    assert(is_flag_valid(flag));
    return out_type_to_func[flag];
}