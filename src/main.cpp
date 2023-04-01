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
#include "context.hpp"
#include "value.hpp"

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
    
    auto root = std::make_unique<lisp::CompoundValue>();

    while(true) {
        auto result = lisp::parse(file);
        if(auto error = result->is_error())
            panic(error.value()->c_str());

        if(result->is_eof())
            break;
        root->add_value(result);
    }

    auto context = lisp::Context();
    root->eval(context);

    file.close();
    return 0;
}