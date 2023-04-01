#pragma once

#include <fstream>
#include "value.hpp"

namespace lisp {
    std::shared_ptr<Value> parse(std::ifstream& input);
}
