#pragma once

#include <fstream>
#include <memory>

#include "value.hpp"

namespace lisp {
    std::unique_ptr<Value> parse(std::ifstream& input);
}
