#pragma once

#include <unordered_map>
#include "value.hpp"

namespace lisp {
    using Builtin = std::shared_ptr<Value> (*)(Context& context, std::vector<std::shared_ptr<Value>>& args);

    extern const std::unordered_map<std::string, Builtin> BUILTINS;
}
