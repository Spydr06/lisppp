#include <cerrno>
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
    
    auto context = lisp::Context();
    auto root = std::make_unique<lisp::CompoundValue>();

    while(true) {
        auto result = lisp::parse(file, context);
        if(auto error = result->is_error())
            panic(error.value()->c_str());

        if(result->is_eof())
            break;
        root->add_value(result);
    }

    root->eval(context);

    file.close();
    return 0;
}