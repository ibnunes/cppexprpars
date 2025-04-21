#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <cmath>

namespace cppexprpars {

using Function = std::function<double(const std::vector<double>&)>;

class FunctionRegistry {
public:
    void register_function(const std::string& name, Function fn) {
        functions_[name] = std::move(fn);
    }

    Function get_function(const std::string& name) const {
        auto it = functions_.find(name);
        if (it == functions_.end()) {
            throw std::runtime_error("Unknown function: " + name);
        }
        return it->second;
    }

    static FunctionRegistry default_registry();

private:
    std::unordered_map<std::string, Function> functions_;
};

} // namespace cppexprpars
