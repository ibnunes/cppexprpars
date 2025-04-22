//  cppexprpars.cpp - Lightweight C++ Expression Parser (Implementation)
//
//  This file implements the parser, tokenizer, evaluation context, and function
//  registry used in cppexprpars. Include "cppexprpars.hpp" to access the public API.
//
//  https://github.com/ibnunes/cppexprpars
//
//  -------------------------------------------------------------------------
//
//  MIT License
//
//  Copyright (c) 2025 Igor Nunes
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.



#include "cppexprpars.hpp"


namespace cppexprpars {

static FunctionRegistry default_registry_ = FunctionRegistry::default_registry();

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



FunctionRegistry FunctionRegistry::default_registry() {
    FunctionRegistry reg;

    reg.register_function("sin", [](const std::vector<double>& args) {
        return std::sin(args[0]);
    }, 1);

    reg.register_function("cos", [](const std::vector<double>& args) {
        return std::cos(args[0]);
    }, 1);

    reg.register_function("sqrt", [](const std::vector<double>& args) {
        return std::sqrt(args[0]);
    }, 1);

    reg.register_function("min", [](const std::vector<double>& args) {
        return std::min(args[0], args[1]);
    }, 2);

    reg.register_function("max", [](const std::vector<double>& args) {
        return std::max(args[0], args[1]);
    }, 2);

    // TODO: Add more functions

    return reg;
}

void FunctionRegistry::register_function(
    const std::string& name,
    Function fn,
    size_t nargs,
    ArityMismatchHandler on_invalid_args
) {
    functions_[name] = [fn = std::move(fn), nargs, name, on_invalid_args = std::move(on_invalid_args)]
                       (const std::vector<ExprFloat>& args) -> ExprFloat
    {
        if (args.size() == nargs)
            return fn(args);
        if (!on_invalid_args)
            throw std::runtime_error(name + " expects " + std::to_string(nargs) + " arguments");
        on_invalid_args(name, nargs, args.size());
        return std::numeric_limits<ExprFloat>::quiet_NaN();     // THINK: What is better here?
    };
}

Function FunctionRegistry::get_function(const std::string& name) const {
    auto it = functions_.find(name);
    if (it == functions_.end())
        throw std::runtime_error("Unknown function: " + name);
    return it->second;
}



void EvaluationContext::set_variable(const std::string& name, double value) {
    variables_[name] = value;
}

double EvaluationContext::get_variable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw std::runtime_error("Unknown variable: " + name);
    }
    return it->second;
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



BinaryOp BinaryExprNode::charToBinaryOp(char op_char) {
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



UnaryOp UnaryExprNode::charToUnaryOp(char op_char) {
    switch (op_char) {
        case '+':
            return UnaryOp::Plus;
        case '-':
            return UnaryOp::Minus;
        default:
            throw std::invalid_argument("Unsupported binary operator");
    }
}



ExprFloat VariableExprNode::evaluate() const {
    return resolver_(name_);
}

ExprFloat FuncExprNode::evaluate() const {
    std::vector<ExprFloat> evaluated_args;
    for (const auto& arg : args_) {
        evaluated_args.push_back(arg->evaluate());
    }

    auto fn = registry_->get_function(name_);
    return fn(evaluated_args);
}

ExprFloat ConstantExprNode::evaluate() const {
    return value_;
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



void Tokenizer::next_token() {
    skip_whitespace();
    if (pos_ >= input_.size()) {
        current_token_ = {TokenType::End, ""};
        return;
    }

    char ch = input_[pos_];
    if (std::isdigit(ch) || ch == '.') {
        parse_number();
    } else if (std::isalpha(ch) || ch == '_') {
        parse_identifier();
    } else {
        switch (ch) {
            case '+': make_token(TokenType::Plus); break;
            case '-': make_token(TokenType::Minus); break;
            case '*': make_token(TokenType::Star); break;
            case '/': make_token(TokenType::Slash); break;
            case '^': make_token(TokenType::Caret); break;
            case '(': make_token(TokenType::LeftParen); break;
            case ')': make_token(TokenType::RightParen); break;
            case ',': make_token(TokenType::Comma); break;
            default:
                current_token_ = {TokenType::Invalid, std::string(1, ch)};
                ++pos_;
                break;
        }
    }
}

void Tokenizer::skip_whitespace() {
    while (pos_ < input_.size() && std::isspace(input_[pos_]))
        ++pos_;
}

void Tokenizer::parse_number() {
    size_t start = pos_;
    bool has_dot = false;

    // Integer and fractional part
    while (pos_ < input_.size() && (std::isdigit(input_[pos_]) || input_[pos_] == '.')) {
        if (input_[pos_] == '.') {
            if (has_dot) break; // Only one dot allowed
            has_dot = true;
        }
        ++pos_;
    }

    // Scientific notation (e.g. 1.2e-3)
    if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
        ++pos_;
        if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) {
            ++pos_;
        }
        while (pos_ < input_.size() && std::isdigit(input_[pos_])) {
            ++pos_;
        }
    }

    std::string number_text = input_.substr(start, pos_ - start);
    try {
        double value = std::stod(number_text);
        current_token_ = {TokenType::Number, number_text, value};
    } catch (...) {
        current_token_ = {TokenType::Invalid, number_text};
    }
}

void Tokenizer::parse_identifier() {
    size_t start = pos_;
    while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_'))
        ++pos_;
    current_token_ = {TokenType::Identifier, input_.substr(start, pos_ - start)};
}

void Tokenizer::make_token(TokenType type) {
    current_token_ = {type, std::string(1, input_[pos_])};
    ++pos_;
}



std::unique_ptr<ExprNode> Parser::parse() {
    auto expr = parse_expression();
    if (tokenizer_.current().type != TokenType::End) {
        throw std::runtime_error("Unexpected token after expression: '" + tokenizer_.current().text + "'");
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_expression(int precedence) {
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

std::unique_ptr<ExprNode> Parser::parse_primary() {
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

int Parser::get_precedence(TokenType type) const {
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus: return 1;
        case TokenType::Star:
        case TokenType::Slash: return 2;
        case TokenType::Caret: return 3;
        default: return -1;
    }
}

bool Parser::is_right_associative(TokenType type) const {
    return type == TokenType::Caret;
}



void ExprParser::set_expression(const std::string& expr) {
    expression_ = expr;
}

void ExprParser::set_variable(const std::string& name, ExprFloat value) {
    context_.set_variable(name, value);
}

ExprFloat ExprParser::get_variable(const std::string& name) const {
    return context_.get_variable(name);
}

void ExprParser::register_function(
    const std::string& name,
    Function fn,
    size_t nargs,
    ArityMismatchHandler on_invalid_args
) {
    registry_.register_function(name, fn, nargs, on_invalid_args);
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

}   // namespace cppexprpars
