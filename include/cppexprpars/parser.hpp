#pragma once

#include "tokenizer.hpp"
#include "expr_node.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

namespace cppexprpars {

class Parser {
public:
    explicit Parser(Tokenizer tokenizer) :
        tokenizer_(std::move(tokenizer)),
        context_(cppexprpars::get_default_context()),
        registry_(cppexprpars::get_default_registry())
        {}

    explicit Parser(Tokenizer tokenizer, EvaluationContext* context, FunctionRegistry* registry) :
        tokenizer_(std::move(tokenizer)),
        context_(context),
        registry_(registry)
        {}

    std::unique_ptr<ExprNode> parse() {
        auto expr = parse_expression();
        if (tokenizer_.current().type != TokenType::End) {
            throw std::runtime_error("Unexpected token after expression: '" + tokenizer_.current().text + "'");
        }
        return expr;
    }

    inline void set_context(EvaluationContext* context) {
        this->context_ = context;
    }

    inline void set_registry(FunctionRegistry* registry) {
        this->registry_ = registry;
    }

private:
    Tokenizer tokenizer_;

    EvaluationContext* context_;
    FunctionRegistry* registry_;

    std::unique_ptr<ExprNode> parse_expression(int precedence = 0) {
        auto lhs = parse_primary();

        while (true) {
            TokenType type = tokenizer_.current().type;
            int token_prec = get_precedence(type);
            if (token_prec < precedence) break;

            Token op = tokenizer_.current();
            tokenizer_.next_token();

            int next_prec = token_prec + (is_right_associative(op.type) ? 0 : 1);
            auto rhs = parse_expression(next_prec);

            lhs = std::make_unique<BinaryExprNode>(op.text[0], std::move(lhs), std::move(rhs));
        }

        return lhs;
    }

    std::unique_ptr<ExprNode> parse_primary() {
        Token token = tokenizer_.current();
        tokenizer_.next_token();

        switch (token.type) {
            case TokenType::Number:
                return std::make_unique<ConstantExprNode>(token.number_value);

            case TokenType::Identifier: {
                if (tokenizer_.current().type == TokenType::LeftParen) {
                    // Function call
                    tokenizer_.next_token(); // consume '('
                    std::vector<std::unique_ptr<ExprNode>> args;

                    if (tokenizer_.current().type != TokenType::RightParen) {
                        while (true) {
                            args.push_back(parse_expression());
                            if (tokenizer_.current().type == TokenType::Comma) {
                                tokenizer_.next_token();
                            } else {
                                break;
                            }
                        }
                    }

                    if (tokenizer_.current().type != TokenType::RightParen) {
                        throw std::runtime_error("Expected ')' after function arguments");
                    }
                    tokenizer_.next_token();

                    return std::make_unique<FuncExprNode>(token.text, std::move(args), registry_);
                }

                // Just a variable
                return std::make_unique<VariableExprNode>(token.text, context_);
            }

            case TokenType::LeftParen: {
                auto expr = parse_expression();
                if (tokenizer_.current().type != TokenType::RightParen) {
                    throw std::runtime_error("Expected ')' after expression");
                }
                tokenizer_.next_token();
                return expr;
            }

            case TokenType::Minus: {
                auto inner = parse_expression(3); // high precedence for unary minus
                return std::make_unique<UnaryExprNode>('-', std::move(inner));
            }

            default:
                throw std::runtime_error("Unexpected token: '" + token.text + "'");
        }
    }

    int get_precedence(TokenType type) const {
        switch (type) {
            case TokenType::Plus:
            case TokenType::Minus: return 1;
            case TokenType::Star:
            case TokenType::Slash: return 2;
            case TokenType::Caret: return 3;
            default: return -1;
        }
    }

    bool is_right_associative(TokenType type) const {
        return type == TokenType::Caret;
    }
};

/*
class ExprParser {
public:
    ExprParser() :
        context_(EvaluationContext::default_context()),
        registry_(FunctionRegistry::default_registry()) { }

    void set_expression(const std::string& expr) {
        expression_ = expr;
    }

    inline void set_variable(const std::string& name, ExprFloat value) {
        context_.set_variable(name, value);
    }

    inline ExprFloat get_variable(const std::string& name) const {
        return context_.get_variable(name);
    }

    inline void register_function(const std::string& name, Function fn) {
        registry_.register_function(name, fn);
    }

    ExprFloat evaluate() {
        Tokenizer tokenizer(expression_);
        Parser parser(
            std::move(tokenizer),
            &context_,
            &registry_
        );

        auto node = parser.parse();
        return node->evaluate();
    }

private:
    std::string expression_;
    EvaluationContext context_;
    FunctionRegistry registry_;
};
*/

} // namespace cppexprpars
