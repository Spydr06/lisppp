#include "value.hpp"
#include "builtin.hpp"

namespace lisp {
    std::shared_ptr<Value> ErrorValue::eval(Context&) {
        return ERROR(m_Reason);
    }

    std::shared_ptr<Value> EofValue::eval(Context&) {
        return std::make_shared<EofValue>();
    }

    std::shared_ptr<Value> StringValue::eval(Context& context) {
        return context.get_string(m_Value);
    }

    std::shared_ptr<Value> NumberValue::eval(Context& context) {
        return context.get_number(m_Value);
    }

    std::shared_ptr<Value> IdentValue::eval(Context& context) {
        if(auto value = context.get_symbol(m_Name))
            return value.value();
        return ERROR("unknown identifier");
    }

    std::shared_ptr<Value> QuoteValue::eval(Context&) {
        return m_Quoted;
    }

    std::shared_ptr<Value> CompoundValue::eval(Context& context) {
        if(!m_Contents.empty() && m_Contents[0]->kind() == ValueKind::Ident) {
            auto name = dynamic_cast<IdentValue&>(*m_Contents[0]);
            auto builtin = BUILTINS.find(name.get_name());
            if(builtin != BUILTINS.end())
                return builtin->second(context, m_Contents);

            if(auto symbol = context.get_symbol(name.get_name())) {
                if(symbol.value()->kind() != ValueKind::Function)
                    return ERROR("can only call functions");
                auto function = dynamic_cast<FunctionValue&>(*symbol.value());

                if(function.get_arguments().size() != m_Contents.size() - 1)
                    return ERROR("wrong number of arguments for function call");
                
                context.push();

                for(size_t i = 0; i < function.get_arguments().size(); i++) {
                    context.add_symbol(function.get_arguments()[i], m_Contents[i + 1]->eval(context));
                }

                auto result = function.eval(context);
                context.pop();
                return result;
            }
            return ERROR("could not find function");
        } 
        
        std::shared_ptr<Value> result = std::make_shared<CompoundValue>();
        for(auto& content : m_Contents) {
            result = content->eval(context);
            if(result && result->kind() == ValueKind::Error)
                return result;
        }
        return result;
    }

    std::shared_ptr<Value> FunctionValue::eval(Context& context) {
        return m_Body->eval(context);
    }

    std::shared_ptr<Value> ConstValue::eval(Context& context) {
        return context.get_const_val(m_Kind);
    }
}
