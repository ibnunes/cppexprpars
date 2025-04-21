#include "cppexprpars/expr_node.hpp"

namespace cppexprpars {

static FunctionRegistry /*FuncExprNode::*/default_registry_ = FunctionRegistry::default_registry();

void set_default_registry(const FunctionRegistry* registry) {
    default_registry_ = *registry;
}

FunctionRegistry* get_default_registry() {
    return &default_registry_;
}

void FuncExprNode::set_registry(const FunctionRegistry* registry) {
    this->registry_ = registry;
}

void FuncExprNode::set_registry_as_default() {
    set_registry(&default_registry_);
}

static EvaluationContext default_context_ = EvaluationContext::default_context();

void set_default_context(const EvaluationContext* context) {
    default_context_ = *context;
}

EvaluationContext* get_default_context() {
    return &default_context_;
}

void VariableExprNode::set_context(const EvaluationContext* context) {
    this->resolver_ = [context](const std::string& varName) {
        return context->get_variable(varName);
    };
}

void VariableExprNode::set_context_as_default() {
    set_context(&default_context_);
}

const EvaluationContext& EvaluationContext::default_context() {
    static EvaluationContext context = [] {
        EvaluationContext ctx;

        // Lowercase 'a' to 'z'
        for (char c = 'a'; c <= 'z'; ++c)
            ctx.set_variable(std::string(1, c), static_cast<ExprFloat>(c));

        // Uppercase 'A' to 'Z'
        for (char c = 'A'; c <= 'Z'; ++c)
            ctx.set_variable(std::string(1, c), static_cast<ExprFloat>(c));

        return ctx;
    }();

    return context;
}

ExprFloat UnaryExprNode::evaluate() const {
    const ExprFloat val = operand_->evaluate();

    switch (op_) {
        case UnaryOp::Plus: return val;
        case UnaryOp::Minus: return -val;
        default:
            throw std::runtime_error("Unknown unary operation");
    }
}

ExprFloat BinaryExprNode::evaluate() const {
    const ExprFloat lhs = left_->evaluate();
    const ExprFloat rhs = right_->evaluate();

    switch (op_) {
        case BinaryOp::Add: return lhs + rhs;
        case BinaryOp::Subtract: return lhs - rhs;
        case BinaryOp::Multiply: return lhs * rhs;
        case BinaryOp::Divide:
            if (rhs == 0.0) throw std::runtime_error("Division by zero");
            return lhs / rhs;
        case BinaryOp::Modulo:
            if (rhs == 0.0) throw std::runtime_error("Division by zero");
            return (ExprFloat)((ExprInt)lhs % (ExprInt)rhs);
        case BinaryOp::Power: return std::pow(lhs, rhs);
        default:
            throw std::runtime_error("Unknown binary operation");
    }
}

} // namespace cppexprpars
