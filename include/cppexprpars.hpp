#include "cppexprpars/parser.hpp"

namespace cppexprpars {

class ExprParser {
public:
    ExprParser() :
        context_(EvaluationContext::default_context()),
        registry_(FunctionRegistry::default_registry()) { }

    void set_expression(const std::string& expr);

    void set_variable(const std::string& name, ExprFloat value);

    ExprFloat get_variable(const std::string& name) const;

    void register_function(const std::string& name, Function fn);

    ExprFloat evaluate();

private:
    std::string expression_;
    EvaluationContext context_;
    FunctionRegistry registry_;
};

} // namespace cppexprpars
