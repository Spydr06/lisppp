#pragma once

#include <optional>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

#include "value.hpp"

namespace lisp {
    class Context {
    public:
        Context() : m_SymbolStack() {
            this->push();
        }
        ~Context() {}

        std::optional<std::shared_ptr<Value>> get_symbol(std::string& name) {
            for(int64_t i = m_SymbolStack.size() - 1; i >= 0; i--) {
                auto value = m_SymbolStack[i].find(name);
                if(value != m_SymbolStack[i].end())
                    return value->second;
            }
            return std::nullopt;
        }

        void add_symbol(std::string name, std::shared_ptr<Value> value) {
            m_SymbolStack.back().insert_or_assign(name, value);
        }

        void push() {
            m_SymbolStack.push_back({});
        }

        void pop() {
            m_SymbolStack.pop_back();
        }

    private:
        std::vector<std::unordered_map<std::string, std::shared_ptr<Value>>> m_SymbolStack;
    };
}
