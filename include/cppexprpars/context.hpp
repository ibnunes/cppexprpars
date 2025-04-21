#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

namespace cppexprpars {

class EvaluationContext {
public:
    void set_variable(const std::string& name, double value) {
        variables_[name] = value;
    }

    double get_variable(const std::string& name) const {
        auto it = variables_.find(name);
        if (it == variables_.end()) {
            throw std::runtime_error("Unknown variable: " + name);
        }
        return it->second;
    }

    static const EvaluationContext& default_context();

private:
    std::unordered_map<std::string, double> variables_;
};

} // namespace cppexprpars
