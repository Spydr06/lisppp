#include "builtin.hpp"
#include "value.hpp"

namespace lisp {
    namespace builtin {
        std::shared_ptr<Value> print(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            for(uint64_t i = 1; i < args.size(); i++)
                args[i]->eval(context)->print(std::cout) << ' ';
            std::cout << std::endl;
            return context.get_const_val(ConstValue::Kind::Nil);
        }

        std::shared_ptr<Value> setq(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 3)
                return ERROR("expect 2 arguments for `setq`");
            auto first = args[1];
            if(first->kind() != ValueKind::Ident)
                return ERROR("expect first `setq` argument to be an identifier");

            auto ident = dynamic_cast<IdentValue&>(*first);
            
            auto second = args[2]->eval(context);
            context.add_symbol(ident.get_name(), second);

            return context.get_const_val(ConstValue::Kind::Nil);
        }

        std::shared_ptr<Value> defun(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 4)
                return ERROR("expect 4 arguments for `defun`");
            
            auto first = args[1];
            if(first->kind() != ValueKind::Ident)
                return ERROR("expect first `defun` argument to be an identifier");
            
            auto ident = dynamic_cast<IdentValue&>(*first);

            auto second = args[2];
            if(second->kind() != ValueKind::Compound)
                return ERROR("expect second `defun` argument to be a compound");

            std::vector<std::string> argument_names;
            
            auto argument_list = dynamic_cast<CompoundValue&>(*second);
            for(auto& argument : argument_list.get_contents()) {
                if(argument->kind() != ValueKind::Ident)
                    return ERROR("expect arguments to be identifiers");
                
                argument_names.push_back((dynamic_cast<IdentValue&>(*argument)).get_name());
            }

            auto body = args[3];

            context.add_symbol(ident.get_name(), std::make_shared<FunctionValue>(ident.get_name(), argument_names, body));
            
            return context.get_const_val(ConstValue::Kind::Nil);
        }

        std::shared_ptr<Value> eq(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 3)
                return ERROR("expect at least 2 arguments for `eq`");
            
            auto first = args[1]->eval(context);

            for(uint64_t i = 2; i < args.size(); i++) {
                if(!(args[i]->eval(context)->equals(first)))
                    return context.get_const_val(ConstValue::Kind::F);
            }
            return context.get_const_val(ConstValue::Kind::T);
        }

        std::shared_ptr<Value> cond(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 4 || args.size() % 2 == 1)
                return ERROR("expect uneven amount of four or more arguments for `cond`");
            
            for(uint64_t i = 1; i < args.size() - 1; i += 2) {
                auto cond = args[i]->eval(context);
                if(cond->kind() != ValueKind::Const)
                    return ERROR("expect `T`, `F` or `NIL` as condition values");
                
                if(dynamic_cast<ConstValue&>(*cond).is_truthy())
                    return args[i + 1]->eval(context);
            }

            return args.back()->eval(context);
        }

        std::shared_ptr<Value> if_(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 4)
                return ERROR("expect exactly 3 arguments for `if`");
            
            auto cond = args[1]->eval(context);
            if(cond->kind() != ValueKind::Const)
                return ERROR("expect `T`, `F` or `NIL` as condition values");
            
            if(dynamic_cast<ConstValue&>(*cond).is_truthy())
                return args[2]->eval(context);
            return args[3]->eval(context);
        }

        std::shared_ptr<Value> add(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 2)
                return ERROR("expect at least 1 argument for `+`");

            auto first = args[1]->eval(context);
            int64_t total = dynamic_cast<NumberValue&>(*first).value();

             for(uint64_t i = 2; i < args.size(); i++) {
                auto arg = args[i]->eval(context);
                if(arg->kind() != ValueKind::Number)
                    return ERROR("`+` exepects all arguments to be numbers");
                total += dynamic_cast<NumberValue&>(*arg).value();
            }

            return context.get_number(total);
        }

        std::shared_ptr<Value> subtract(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 2)
                return ERROR("expect at least 1 argument for `-`");

            auto first = args[1]->eval(context);
            int64_t total = dynamic_cast<NumberValue&>(*first).value();

             for(uint64_t i = 2; i < args.size(); i++) {
                auto arg = args[i]->eval(context);
                if(arg->kind() != ValueKind::Number)
                    return ERROR("`-` exepects all arguments to be numbers");
                total -= dynamic_cast<NumberValue&>(*arg).value();
            }

            return context.get_number(total);
        }

        std::shared_ptr<Value> multiply(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 2)
                return ERROR("expect at least 1 argument for `*`");

            auto first = args[1]->eval(context);
            int64_t total = dynamic_cast<NumberValue&>(*first).value();

            for(uint64_t i = 2; i < args.size(); i++) {
                auto arg = args[i]->eval(context);
                if(arg->kind() != ValueKind::Number)
                    return ERROR("`*` exepects all arguments to be numbers");
                total *= dynamic_cast<NumberValue&>(*arg).value();
            }

            return context.get_number(total);
        }

        std::shared_ptr<Value> divide(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() < 2)
                return ERROR("expect at least 1 argument for `/`");

            auto first = args[1]->eval(context);
            int64_t total = dynamic_cast<NumberValue&>(*first).value();

            for(uint64_t i = 2; i < args.size(); i++) {
                auto arg = args[i]->eval(context);
                if(arg->kind() != ValueKind::Number)
                    return ERROR("`/` exepects all arguments to be numbers");
                total /= dynamic_cast<NumberValue&>(*arg).value();
            }

            return context.get_number(total);
        }

        std::shared_ptr<Value> lt(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 3)
                return ERROR("expect exactly 2 arguments for `<`");
            
            auto first = args[1]->eval(context);
            if(first->kind() != ValueKind::Number)
                return ERROR("`<` only operates on numbers");
            
            auto second = args[2]->eval(context);
            if(second->kind() != ValueKind::Number)
                return ERROR("`<` only operates on numbers");
            
            if(dynamic_cast<NumberValue&>(*first).value() < dynamic_cast<NumberValue&>(*second).value())
                return context.get_const_val(ConstValue::Kind::T);
            return context.get_const_val(ConstValue::Kind::F);
        }

        std::shared_ptr<Value> gt(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 3)
                return ERROR("expect exactly 2 arguments for `>`");
            
            auto first = args[1]->eval(context);
            if(first->kind() != ValueKind::Number)
                return ERROR("`>` only operates on numbers");
            
            auto second = args[2]->eval(context);
            if(second->kind() != ValueKind::Number)
                return ERROR("`>` only operates on numbers");
            
            if(dynamic_cast<NumberValue&>(*first).value() > dynamic_cast<NumberValue&>(*second).value())
                return context.get_const_val(ConstValue::Kind::T);
            return context.get_const_val(ConstValue::Kind::F);
        }

        std::shared_ptr<Value> lt_eq(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 3)
                return ERROR("expect exactly 2 arguments for `<=`");
            
            auto first = args[1]->eval(context);
            if(first->kind() != ValueKind::Number)
                return ERROR("`<=` only operates on numbers");
            
            auto second = args[2]->eval(context);
            if(second->kind() != ValueKind::Number)
                return ERROR("`<=` only operates on numbers");
            
            if(dynamic_cast<NumberValue&>(*first).value() <= dynamic_cast<NumberValue&>(*second).value())
                return context.get_const_val(ConstValue::Kind::T);
            return context.get_const_val(ConstValue::Kind::F);
        }

        std::shared_ptr<Value> gt_eq(Context& context, std::vector<std::shared_ptr<Value>>& args) {
            if(args.size() != 3)
                return ERROR("expect exactly 2 arguments for `>=`");
            
            auto first = args[1]->eval(context);
            if(first->kind() != ValueKind::Number)
                return ERROR("`>=` only operates on numbers");
            
            auto second = args[2]->eval(context);
            if(second->kind() != ValueKind::Number)
                return ERROR("`>=` only operates on numbers");
            
            if(dynamic_cast<NumberValue&>(*first).value() >= dynamic_cast<NumberValue&>(*second).value())
                return context.get_const_val(ConstValue::Kind::T);
            return context.get_const_val(ConstValue::Kind::F);
        }
    }

    const std::unordered_map<std::string, Builtin> BUILTINS = {
        {"print", builtin::print},
        {"setq", builtin::setq},
        {"defun", builtin::defun},
        {"eq", builtin::eq},
        {"cond", builtin::cond},
        {"if", builtin::if_},
        {"+", builtin::add},
        {"-", builtin::subtract},
        {"*", builtin::multiply},
        {"/", builtin::divide},
        {"<", builtin::lt},
        {">", builtin::gt},
        {"<=", builtin::lt_eq},
        {">=", builtin::gt_eq}
    };
}