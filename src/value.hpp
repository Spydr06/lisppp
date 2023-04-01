#pragma once
#include <functional>
#include <ostream>
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace lisp {
    class Value {
    public:
        virtual ~Value() {};
        virtual void print(std::ostream& stream) const = 0;
        virtual std::optional<std::shared_ptr<std::string>> is_error() const { return std::nullopt; };
        virtual bool is_eof() const { return false; };
    private:
    };

    // errors
    class ErrorValue : public Value {
    public:
        ErrorValue(std::string reason) : m_Reason(std::make_shared<std::string>(reason)) {}
        ~ErrorValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "ErrorValue(\"" << m_Reason << "\")";
        };

        virtual std::optional<std::shared_ptr<std::string>> is_error() const override {
            return std::optional<std::shared_ptr<std::string>>(m_Reason);
        }

    private:
        std::shared_ptr<std::string> m_Reason;
    };

    // end of file
    class EofValue : public Value {
    public:
        EofValue() {}
        ~EofValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "EofValue()";
        };

        virtual bool is_eof() const override {
            return true;
        }
    };

    // "foo"
    class StringValue : public Value {
    public:
        StringValue(std::string value) : m_Value(value) {}
        ~StringValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "StringValue(\"" << m_Value << "\")";
        };

    private:
        std::string m_Value;
    };

    // 1234
    class NumberValue : public Value {
    public:
        NumberValue(int64_t value) : m_Value(value) {}
        ~NumberValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "NumberValue(" << m_Value << ")";
        };

    private:
        int64_t m_Value;
    };

    // foo
    class IdentValue : public Value {
    public:
        IdentValue(std::string name) : m_Name(name) {}
        ~IdentValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "IdentValue(" << m_Name << ")";
        };

    private:
        std::string m_Name;
    };

    class QuoteValue : public Value {
    public:
        QuoteValue(Value* quoted) : m_Quoted(quoted) {}
        ~QuoteValue() {}

        virtual void print(std::ostream& stream) const override {
            stream << "QuoteValue(";
            m_Quoted->print(stream);
            stream << ")";
        };

        virtual std::optional<std::shared_ptr<std::string>> is_error() const override { 
            return m_Quoted->is_error();
        }

    private:
        std::unique_ptr<Value> m_Quoted;
    };

    // (foo "bar" 123)
    class CompoundValue : public Value {
    public:
        CompoundValue(std::vector<Value*> contents) : m_Contents(contents) {}
        ~CompoundValue() {
            for(auto* value : m_Contents)
                delete value;
        }

        virtual void print(std::ostream& stream) const override {
            stream << "CompoundValue(";
            for(auto& value : m_Contents) {
                value->print(stream);
                stream << ", ";
            }
            stream << ")";
        };

        virtual  std::optional<std::shared_ptr<std::string>> is_error() const override {
            for(auto& value : m_Contents) {
                auto error = value->is_error();
                if(error.has_value()) {
                    return error.value();
                }
            }
            return std::nullopt;
        }

    private:
        std::vector<Value*> m_Contents;
    };
}
