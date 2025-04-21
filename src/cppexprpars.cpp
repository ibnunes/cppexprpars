#include "cppexprpars.hpp"

namespace cppexprpars {

void ExprParser::set_expression(const std::string& expr) {
    expression_ = expr;
}

void ExprParser::set_variable(const std::string& name, ExprFloat value) {
    context_.set_variable(name, value);
}

ExprFloat ExprParser::get_variable(const std::string& name) const {
    return context_.get_variable(name);
}

void ExprParser::register_function(const std::string& name, Function fn) {
    registry_.register_function(name, fn);
}

ExprFloat ExprParser::evaluate() {
    Tokenizer tokenizer(expression_);
    Parser parser(
        std::move(tokenizer),
        &context_,
        &registry_
    );

    auto node = parser.parse();
    return node->evaluate();
}

}; // namespace cppexprpars
