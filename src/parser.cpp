#include "parser.hpp"
#include <cctype>
#include <fstream>
#include <memory>
#include <sstream>
#include <variant>
#include <iostream>
#include <vector>

namespace lisp {
    inline bool is_ident_char(char c) {
        return std::isalnum(c) || c == '?' || c == '_' || c == '-' || c == '!';
    }

    std::unique_ptr<Value> parse_number(char c, std::ifstream& input) {
        int64_t value = (c - '0');
        while(std::isdigit(input.peek())) {
            value *= 10;
            value += input.get() - '0';
        }
        c = input.peek();
        if(std::isspace(c) || c == ')' || input.eof())
            return std::make_unique<NumberValue>(value);
        return std::make_unique<ErrorValue>("unexpected character after number literal");
    }

    std::unique_ptr<Value> parse_string(std::ifstream& input) {
        auto string = std::ostringstream();

        char c = input.get();
        while(c != '"' && !input.eof()) {
            string << c;
            c = input.get();
        }
        if(input.eof())
            return std::make_unique<ErrorValue>("unclosed string literal");
        return std::make_unique<StringValue>(string.str());
    }

    std::unique_ptr<Value> parse_ident(char c, std::ifstream& input) {
        auto name = std::ostringstream();
        name << c;

        while(is_ident_char(input.peek())) {
            name << (char) input.get();
        }
        return std::make_unique<IdentValue>(name.str());
    }

    std::unique_ptr<Value> parse_compound(std::ifstream& input) {
        std::vector<Value*> values;

        while(input.peek() != ')' && !input.eof())
        {
            values.push_back(parse(input).release());
            while(std::isspace(input.peek())) {
                input.get();
            }
        }        
        if(input.get() == ')')
            return std::make_unique<CompoundValue>(values);
        return std::make_unique<ErrorValue>("unclosed compound literal");
    }

    std::unique_ptr<Value> parse(std::ifstream& input) {
        char c = 0;
        input >> c;
        switch(c) {
        case '\0':
            return std::make_unique<EofValue>();
        case '(':
            return parse_compound(input);
        case '"':
            return parse_string(input);
        case '\'':
            return std::make_unique<QuoteValue>(parse(input).release());
        case '0'...'9':
            return parse_number(c, input);
        default:
            if(is_ident_char(c)) {
                return parse_ident(c, input);
            }
            if(input.eof()) {
                return std::make_unique<EofValue>();
            }
        }
        std::cout << c << std::endl;
        return std::make_unique<ErrorValue>("unexpected character");
    }
}