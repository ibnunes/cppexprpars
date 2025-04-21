#pragma once

#include "cppexprpars/operators.hpp"
#include "cppexprpars/context.hpp"
#include "cppexprpars/function.hpp"
#include <memory>
#include <functional>
#include <string>
#include <stdint.h>

namespace cppexprpars {

using ExprFloat = double;
using ExprInt = uint64_t;

enum class ExprNodeType {
    Constant,
    Variable,
    Binary,
    Unary,
    Function
};

class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual ExprFloat evaluate() const = 0;
};

using ExprNodePtr = std::unique_ptr<ExprNode>;

class BinaryExprNode : public ExprNode {
public:
    BinaryExprNode(BinaryOp op, ExprNodePtr left, ExprNodePtr right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    BinaryExprNode(char op, ExprNodePtr left, ExprNodePtr right)
        : op_(charToBinaryOp(op)), left_(std::move(left)), right_(std::move(right)) {}

    ExprFloat evaluate() const override;

protected:
    static BinaryOp charToBinaryOp(char op_char) {
        switch (op_char) {
            case '+':
                return BinaryOp::Add;
            case '-':
                return BinaryOp::Subtract;
            case '*':
                return BinaryOp::Multiply;
            case '/':
                return BinaryOp::Divide;
            case '%':
                return BinaryOp::Modulo;
            case '^':
                return BinaryOp::Power;
            default:
                throw std::invalid_argument("Unsupported binary operator");
        }
    }

private:
    BinaryOp op_;
    ExprNodePtr left_;
    ExprNodePtr right_;
};

class UnaryExprNode : public ExprNode {
public:
    UnaryExprNode(UnaryOp op, ExprNodePtr operand)
        : op_(op), operand_(std::move(operand)) {}

    UnaryExprNode(char op, ExprNodePtr operand)
        : op_(charToUnaryOp(op)), operand_(std::move(operand)) {}

    ExprFloat evaluate() const override;

protected:
    static UnaryOp charToUnaryOp(char op_char) {
        switch (op_char) {
            case '+':
                return UnaryOp::Plus;
            case '-':
                return UnaryOp::Minus;
            default:
                throw std::invalid_argument("Unsupported binary operator");
        }
    }

private:
    UnaryOp op_;
    ExprNodePtr operand_;
};

using VariableResolver = std::function<ExprFloat(const std::string&)>;

class VariableExprNode : public ExprNode {
public:
    // Use predefined context
    VariableExprNode(std::string name, const EvaluationContext* context = &default_context_)
        : name_(std::move(name)),
            resolver_([context](const std::string& varName) {
                return context->get_variable(varName);
            }) {}

    // Advanced use case: custom resolver
    VariableExprNode(std::string name, VariableResolver resolver)
        : name_(std::move(name)), resolver_(std::move(resolver)) {}

    ExprFloat evaluate() const override {
        return resolver_(name_);
    }

    static void set_default_context(const EvaluationContext* context);

private:
    std::string name_;
    VariableResolver resolver_;

    static EvaluationContext default_context_;
};

void set_default_context(const EvaluationContext* context);

class FuncExprNode : public ExprNode {
public:
    FuncExprNode(std::string name,
                    std::vector<ExprNodePtr> args,
                    const FunctionRegistry* registry = &default_registry_)
        : name_(std::move(name)),
            args_(std::move(args)),
            registry_(registry) {}

    ExprFloat evaluate() const override {
        std::vector<ExprFloat> evaluated_args;
        for (const auto& arg : args_) {
            evaluated_args.push_back(arg->evaluate());
        }

        auto fn = registry_->get_function(name_);
        return fn(evaluated_args);
    }

    static void set_default_registry(const FunctionRegistry* registry);

private:
    std::string name_;
    std::vector<ExprNodePtr> args_;
    const FunctionRegistry* registry_;

    static FunctionRegistry default_registry_;
};

class ConstantExprNode : public ExprNode {
public:
    explicit ConstantExprNode(ExprFloat value) : value_(value) {}

    ExprFloat evaluate() const override {
        return value_;
    }

private:
    ExprFloat value_;
};

} // namespace cppexprpars
