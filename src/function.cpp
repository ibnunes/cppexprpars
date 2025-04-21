#include "cppexprpars/function.hpp"

using namespace cppexprpars;

FunctionRegistry FunctionRegistry::default_registry() {
    FunctionRegistry reg;

    reg.register_function("sin", [](const std::vector<double>& args) {
        if (args.size() != 1) throw std::runtime_error("sin expects 1 argument");
        return std::sin(args[0]);
    });

    reg.register_function("cos", [](const std::vector<double>& args) {
        if (args.size() != 1) throw std::runtime_error("cos expects 1 argument");
        return std::cos(args[0]);
    });

    reg.register_function("sqrt", [](const std::vector<double>& args) {
        if (args.size() != 1) throw std::runtime_error("sqrt expects 1 argument");
        return std::sqrt(args[0]);
    });

    reg.register_function("min", [](const std::vector<double>& args) {
        if (args.size() != 2) throw std::runtime_error("min expects 2 arguments");
        return std::min(args[0], args[1]);
    });

    reg.register_function("max", [](const std::vector<double>& args) {
        if (args.size() != 2) throw std::runtime_error("max expects 2 arguments");
        return std::max(args[0], args[1]);
    });

    // TODO: add more functions

    return reg;
}
