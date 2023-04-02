#pragma once

#include <iostream>
#include <memory>

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

        const std::shared_ptr<ConstValue> get_const_val(ConstValue::Kind kind) const {
            return m_ConstVals[kind];
        }

        const std::shared_ptr<StringValue> get_string(std::string string) {
            auto value = m_Strings.find(string);
            if(value != m_Strings.end())
                return value->second;

            auto new_ptr = std::make_shared<StringValue>(string);
            m_Strings.insert_or_assign(string, new_ptr);
            return new_ptr;
        }

        const std::shared_ptr<NumberValue> get_number(int64_t number) {
            auto value = m_Numbers.find(number);
            if(value != m_Numbers.end())
                return value->second;
            
            auto new_ptr = std::make_shared<NumberValue>(number);
            m_Numbers.insert_or_assign(number, new_ptr);
            return new_ptr;
        }

        const std::shared_ptr<IdentValue> get_ident(std::string name) {
            auto value = m_Idents.find(name);
            if(value != m_Idents.end())
                return value->second;

            auto new_ptr = std::make_shared<IdentValue>(name);
            m_Idents.insert_or_assign(name, new_ptr);
            return new_ptr;
        }

    private:
        std::vector<std::unordered_map<std::string, std::shared_ptr<Value>>> m_SymbolStack;
        std::unordered_map<std::string, std::shared_ptr<StringValue>> m_Strings;
        std::unordered_map<int64_t, std::shared_ptr<NumberValue>> m_Numbers;
        std::unordered_map<std::string, std::shared_ptr<IdentValue>> m_Idents;

        const std::shared_ptr<ConstValue> m_ConstVals[3] = {
            std::make_shared<ConstValue>(ConstValue::Kind::T),
            std::make_shared<ConstValue>(ConstValue::Kind::F),
            std::make_shared<ConstValue>(ConstValue::Kind::Nil)
        };
    };
}
