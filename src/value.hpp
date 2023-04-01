#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <memory>

#define ERROR(reason) std::make_shared<ErrorValue>((reason))
#define NIL() std::make_shared<ConstValue>(ConstValue::Kind::Nil)
#define T() std::make_shared<ConstValue>(ConstValue::Kind::T)
#define F() std::make_shared<ConstValue>(ConstValue::Kind::F)
#define NUMBER(num) std::make_shared<NumberValue>((num))

#ifndef __LISP_VALUES
#define __LISP_VALUES

namespace lisp {
    class Context;
    enum ValueKind {
        Error,
        Eof,
        String,
        Number,
        Ident,
        Quote,
        Compound,
        Function,
        Const
    };

    class Value {
    public:
        virtual ~Value() {};
        
        virtual ValueKind kind() const = 0;
        
        virtual std::ostream& print(std::ostream& stream) const = 0;
        virtual std::shared_ptr<Value> eval(Context& context) = 0;
        virtual bool equals(std::shared_ptr<Value> other) = 0;

        virtual std::optional<std::shared_ptr<std::string>> is_error() const { return std::nullopt; };
        virtual bool is_eof() const { return false; };
    private:
    };

    // errors
    class ErrorValue : public Value {
    public:
        ErrorValue(std::string reason) : m_Reason(std::make_shared<std::string>(reason)) {}
        ErrorValue(std::shared_ptr<std::string> reason) : m_Reason(reason) {}
        ~ErrorValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << "Error: " << m_Reason->c_str();
            return stream;
        };

        virtual std::optional<std::shared_ptr<std::string>> is_error() const override {
            return std::optional<std::shared_ptr<std::string>>(m_Reason);
        }

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Error; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Error)
                return false;
            return m_Reason == dynamic_cast<ErrorValue&>(*other).m_Reason;
        }

    private:
        std::shared_ptr<std::string> m_Reason;
    };

    // end of file
    class EofValue : public Value {
    public:
        EofValue() {}
        ~EofValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << "End of file";
            return stream;
        };

        virtual bool is_eof() const override {
            return true;
        }

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Eof; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            return other->kind() == ValueKind::Eof;
        }
    };

    // "foo"
    class StringValue : public Value {
    public:
        StringValue(std::string value) : m_Value(value) {}
        ~StringValue() {}

        virtual std::ostream& print(std::ostream& stream) const override { return stream << m_Value; };
        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::String; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::String)
                return false;
            return m_Value == dynamic_cast<StringValue&>(*other).m_Value;
        }

    private:
        std::string m_Value;
    };

    // 1234
    class NumberValue : public Value {
    public:
        NumberValue(int64_t value) : m_Value(value) {}
        ~NumberValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << m_Value;
            return stream;
        };

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Number; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Number)
                return false;
            return m_Value == dynamic_cast<NumberValue&>(*other).m_Value;
        }

        int64_t value() const { return m_Value; }

    private:
        int64_t m_Value;
    };

    // foo
    class IdentValue : public Value {
    public:
        IdentValue(std::string name) : m_Name(name) {}
        ~IdentValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << m_Name;
            return stream;
        };

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Ident; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Ident)
                return false;
            return m_Name == dynamic_cast<IdentValue&>(*other).m_Name;
        }

        std::string& get_name() { return this->m_Name; }

    private:
        std::string m_Name;
    };

    class QuoteValue : public Value {
    public:
        QuoteValue(std::shared_ptr<Value> quoted) : m_Quoted(quoted) {}
        ~QuoteValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            m_Quoted->print(stream);
            return stream;
        };

        virtual std::optional<std::shared_ptr<std::string>> is_error() const override { 
            return m_Quoted->is_error();
        }        
        
        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Quote; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Quote)
                return false;
            return m_Quoted->equals(dynamic_cast<QuoteValue&>(*other).m_Quoted);
        }

    private:
        std::shared_ptr<Value> m_Quoted;
    };

    // (foo "bar" 123)
    class CompoundValue : public Value {
    public:
        CompoundValue() {}
        CompoundValue(std::vector<std::shared_ptr<Value>> contents) : m_Contents(contents) {}
        ~CompoundValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << "(";
            for(auto& value : m_Contents) {
                value->print(stream);
                stream << " ";
            }
            stream << ")";
            return stream;
        };

        virtual std::optional<std::shared_ptr<std::string>> is_error() const override {
            for(auto& value : m_Contents) {
                auto error = value->is_error();
                if(error.has_value()) {
                    return error.value();
                }
            }
            return std::nullopt;
        }
        
        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Compound; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Compound)
                return false;
            return m_Contents == dynamic_cast<CompoundValue&>(*other).m_Contents;
        }

        void add_value(std::shared_ptr<Value> value) {
            m_Contents.push_back(value);
        }

        std::vector<std::shared_ptr<Value>>& get_contents() {
            return m_Contents;
        }

    private:
        std::vector<std::shared_ptr<Value>> m_Contents;
    };

    class FunctionValue : public Value {
    public:
        FunctionValue(std::string name, std::vector<std::string> arguments, std::shared_ptr<Value> body)
            : m_Name(name), m_Arguments(arguments), m_Body(body) {}
        ~FunctionValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            stream << "function " << m_Name; 
            return stream;
        };

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Function; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Function)
                return false;
            return m_Name == dynamic_cast<FunctionValue&>(*other).m_Name;
        }

        std::vector<std::string> get_arguments() const { return m_Arguments; }

    private:
        std::string m_Name;

        std::vector<std::string> m_Arguments;
        std::shared_ptr<Value> m_Body;
    };

    // t, f, nil
    class ConstValue : public Value {
    public:
        enum Kind {
            T,
            F,
            Nil
        };

        ConstValue(Kind kind) : m_Kind(kind) {}
        ConstValue(ConstValue* value) : m_Kind(value->m_Kind) {}
        ~ConstValue() {}

        virtual std::ostream& print(std::ostream& stream) const override {
            switch(m_Kind) {
            case T:
                stream << 'T';
                break;
            case F:
                stream << 'F';
                break;
            case Nil:
                stream << "NIL";
                break;
            }
            return stream;
        };

        virtual std::shared_ptr<Value> eval(Context& context) override;
        virtual ValueKind kind() const override { return ValueKind::Const; }

        Kind get_value_kind() const { return m_Kind; }

        virtual bool equals(std::shared_ptr<Value> other) override {
            if(other->kind() != ValueKind::Const)
                return false;
            return m_Kind == dynamic_cast<ConstValue&>(*other).m_Kind;
        }

        bool is_truthy() { return m_Kind == T; }

    private:
        Kind m_Kind;
    };

    static const std::unordered_map<std::string, ConstValue::Kind> CONSTVALUE_IDENTS = {
        {"t", ConstValue::Kind::T},
        {"T", ConstValue::Kind::T},
        {"f", ConstValue::Kind::F},
        {"F", ConstValue::Kind::F},
        {"nil", ConstValue::Kind::Nil},
        {"NIL", ConstValue::Kind::Nil},
    };
}

#endif // __LISP_VALUES

#include "context.hpp"
