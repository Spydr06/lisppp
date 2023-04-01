#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <system_error>
#include <variant>
#include <cerrno>
#include <vector>
#include <memory>
#include <cstring>

#include "parser.hpp"

void panic(const char* msg) {
    std::cerr << "[Panic]: " << msg << std::endl;
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        panic("Expected <file>");
    }

    auto filename = argv[1];
    std::ifstream file(filename);
    if(file.fail()) {
        panic(strerror(errno));
    }
    
    std::vector<lisp::Value*> values;

    while(true) {
        auto result = lisp::parse(file);
        if(auto error = result->is_error()) {
            panic(error.value()->c_str());
        }
        result->print(std::cout);
        std::cout << std::endl;
        if(result->is_eof()) {
            break;
        }
        values.push_back(result.release());
    }

    for(auto* value : values)
        delete value;

    file.close();
    return 0;
}