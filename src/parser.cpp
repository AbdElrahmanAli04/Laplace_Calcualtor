#include "../include/parser.h"
#include <cctype>               // For isdigit ,   isalpha , isspace, isalnum
#include <cmath>                
#include <iostream>             
#include <algorithm>            // For std::find_if


// --- Token Constructors Definition ---
Token::Token(TokenType t, std::string txt) : type(t), text(std::move(txt)), value(0.0) {
    if (type == TokenType::NUMBER && !text.empty()) {
        try {
            value = std::stod(text);
        } catch (const std::out_of_range& oor) {
            throw std::runtime_error("Number out of range: " + text);
        } catch (const std::invalid_argument& ia) {
            // This can happen if text is not a valid double, though tokenizer should ensure it is.
            throw std::runtime_error("Invalid number format: " + text);
        }
    }
}

// Constructor for tokens that are not numbers or have pre-defined text
Token::Token(TokenType t, const char* txt_char) : type(t), text(txt_char), value(0.0) {}

// --- Tokenizer Function Definition ---
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < input.length()) {
        char current_char = input[pos];

        if (std::isspace(current_char)) {
            pos++;
            continue;
        }

        // Handle numbers: allows for leading '.', e.g., ".5"
        if (std::isdigit(current_char) || ( (current_char == '.'  )  && pos + 1 < input.length() && std::isdigit(input[pos+1]))) { // Check for leading digit or decimal point
            std::string num_str;
            bool decimal_found = false;
            while (pos < input.length() && (std::isdigit(input[pos]) || input[pos] == '.')) {
                if (input[pos] == '.') {
                    if (decimal_found) {
                        // Allow only one decimal point
                        throw std::runtime_error("Multiple decimal points in number: " + num_str + input[pos]);
                    }
                    decimal_found = true;
                }
                num_str += input[pos];
                pos++;
            }
            tokens.emplace_back(TokenType::NUMBER, num_str);
            continue;
        }

        // Handle identifiers (function names, 't', 'PI', 'e', 'exp')
        if (std::isalpha(current_char)) {
            std::string ident_str;
            ident_str += current_char;
            pos++;
            while (pos < input.length() && (std::isalnum(input[pos]))) { // allows letters and numbers
                ident_str += input[pos];
                pos++;
            }
            // All identifiers are parsed as IDENTIFIER type, their specific meaning (PI, sin, t)
            // will be interpreted by the parser based on their text.
            tokens.emplace_back(TokenType::IDENTIFIER, ident_str);
            continue;
        }

        // Handle operators and parentheses
        switch (current_char) {
            case '+': tokens.emplace_back(TokenType::PLUS, "+"); pos++; break;
            case '-': tokens.emplace_back(TokenType::MINUS, "-"); pos++; break;
            case '*': tokens.emplace_back(TokenType::MULTIPLY, "*"); pos++; break;
            case '/': tokens.emplace_back(TokenType::DIVIDE, "/"); pos++; break;
            case '^': tokens.emplace_back(TokenType::POWER, "^"); pos++; break;
            case '(': tokens.emplace_back(TokenType::LPAREN, "("); pos++; break;
            case ')': tokens.emplace_back(TokenType::RPAREN, ")"); pos++; break;
            default:
                throw std::runtime_error("Unknown character in input: " + std::string(1, current_char));
        }
    }
    tokens.emplace_back(TokenType::END_OF_INPUT, ""); // Mark the end of input
    return tokens;
}

// --- Parser Class Method Definitions ---

Token& Parser::current_token() {
    if (token_idx_ >= tokens_.size()) {
        throw std::runtime_error("Unexpected end of input.");
    }
    return tokens_[token_idx_];
}

Token& Parser::peek_token(size_t offset) {
    if (token_idx_ + offset >= tokens_.size()) {
        throw std::runtime_error("Unexpected end of input on peek.");
    }
    return tokens_[token_idx_ + offset];
}

void Parser::consume_token() {
    if (token_idx_ < tokens_.size()) {
        token_idx_++;
    }
}


// Helper to parse arguments like "a*t", "omega*t", "a", "omega"
// Returns the constant factor 'a' or 'omega'. Assumes 't' is the variable.
double Parser::evaluate_simple_parameter_argument() {
    double val = 1.0; // Default coefficient if only 't' is present, e.g., sin(t) -> omega=1
    double sign = 1.0;
    bool t_seen = false;

    // Handle leading sign (e.g., exp(-3*t))
    if (current_token().type == TokenType::MINUS) {
        sign = -1.0;
        consume_token();
    } else if (current_token().type == TokenType::PLUS) {
        consume_token(); // consume optional '+'
    }

    // Parse the first component (number, PI, or t)
    if (current_token().type == TokenType::NUMBER) {
        val = current_token().value;
        consume_token();
    } else if (current_token().text == "PI") {
        val = M_PI;
        consume_token();
    } else if (current_token().text == "t") {
        val = 1.0; // Implies 1*t
        t_seen = true;
        consume_token();
    } else {
        throw std::runtime_error("Invalid function argument. Expected number, PI, or 't', got: " + current_token().text);
    }

    // Check for multiplication (e.g., 2*t, PI*t, t*2, t*PI)
    if (current_token().type == TokenType::MULTIPLY) {
        consume_token(); // Consume '*'
        if (current_token().type == TokenType::NUMBER) {
            if (t_seen) { // If 't*NUMBER', update coefficient
                val *= current_token().value;
            } else { // If 'NUMBER*NUMBER' or 'PI*NUMBER', update coefficient
                val *= current_token().value;
            }
            consume_token();
        } else if (current_token().text == "PI") {
            if (t_seen) { // If 't*PI', update coefficient
                val *= M_PI;
            } else { // If 'NUMBER*PI' or 'PI*PI', update coefficient
                val *= M_PI;
            }
            consume_token();
        } else if (current_token().text == "t") {
            if (t_seen) { // Already saw 't', like 't*t' (unsupported in arguments like sin(t*t))
                throw std::runtime_error("Unexpected 't' in function argument (e.g., 't*t' is not supported).");
            }
            t_seen = true;
            consume_token();
        } else {
            throw std::runtime_error("Expected number, PI, or 't' after '*' in argument, got: " + current_token().text);
        }
    }
    return val * sign;
}



// Parses the smallest unit: a number, a variable 't', a function call, or a parenthesized expression.
ParsedTerm Parser::parse_factor() {
    ParsedTerm term;
    term.coefficient = 1.0; // Factors initially have coeff 1.0

    // Handle numbers or constants like 'PI'
    if (current_token().type == TokenType::NUMBER) {
        term.type = FunctionType::CONSTANT;
        term.coefficient *= current_token().value; // Apply explicit coefficient
        term.original_term_str += current_token().text;
        consume_token();
        return term;
    } else if (current_token().type == TokenType::IDENTIFIER && current_token().text == "PI") {
        term.type = FunctionType::CONSTANT;
        term.coefficient *= M_PI;
        term.original_term_str += current_token().text;
        consume_token();
        return term;
    } else if (current_token().type == TokenType::IDENTIFIER && current_token().text == "t") {
        term.type = FunctionType::T_POW_N;
        term.parameters.push_back(1.0); // Default to t^1
        term.original_term_str += current_token().text;
        consume_token();
        if (current_token().type == TokenType::POWER) { // Handle t^n
            term.original_term_str += current_token().text; // Add '^'
            consume_token(); // Consume '^'
            if (current_token().type != TokenType::NUMBER) {
                throw std::runtime_error("Expected number for exponent after 't^', got: " + current_token().text);
            }
            term.parameters[0] = current_token().value; // Update power 'n'
            term.original_term_str += current_token().text; // Add 'n'
            consume_token();
        }
        return term;
    } else if (current_token().type == TokenType::IDENTIFIER) {
        // It's a function name: sin, cos, exp, sinh, cosh
        std::string func_name = current_token().text;
        term.original_term_str += current_token().text;
        consume_token(); // Consume function name

        if (func_name == "e") { // Special handling for 'e' followed by '^' for exp(at)
            if (current_token().type == TokenType::POWER) {
                term.original_term_str += current_token().text; // Add '^'
                consume_token(); // Consume '^'
            } else {
                throw std::runtime_error("Identifier 'e' must be followed by '^' for exponentiation or '(' for exp() function: " + current_token().text);
            }
        }

        if (current_token().type != TokenType::LPAREN) {
            throw std::runtime_error("Expected '(' after function name " + func_name + ", got: " + current_token().text);
        }
        term.original_term_str += current_token().text; // Add '('
        consume_token(); // Consume '('

        // Parse argument (e.g., 2*t, -3*t, t, PI*t)
        double param_val = evaluate_simple_parameter_argument();
        term.parameters.push_back(param_val);
        // original_term_str for parameter part is built inside evaluate_simple_parameter_argument if needed

        if (current_token().type != TokenType::RPAREN) {
            throw std::runtime_error("Expected ')' after function arguments, got: " + current_token().text);
        }
        term.original_term_str += current_token().text; // Add ')'
        consume_token(); // Consume ')'

        if (func_name == "sin") {
            term.type = FunctionType::SIN;
        } else if (func_name == "cos") {
            term.type = FunctionType::COS;
        } else if (func_name == "exp" || func_name == "e") { // 'e' handled as exp
            term.type = FunctionType::EXP;
        } else if (func_name == "sinh") {
            term.type = FunctionType::SINH;
        } else if (func_name == "cosh") {
            term.type = FunctionType::COSH;
        } else {
            throw std::runtime_error("Unrecognized function name: " + func_name);
        }
        return term;
    } else if (current_token().type == TokenType::LPAREN) {
        term.original_term_str += current_token().text; // Add '('
        consume_token(); // Consume '('
        
        std::vector<ParsedTerm> sub_terms = parse_expression_in_parentheses_helper(); // Call helper for recursive parsing
        if (sub_terms.size() != 1) {
            throw std::runtime_error("Parenthesized expressions currently only support a single combined term for Laplace transform purposes.");
        }
        term = sub_terms[0]; // Take the result of the sub-expression as the factor

        if (current_token().type != TokenType::RPAREN) {
            throw std::runtime_error("Expected ')' after parenthesized expression, got: " + current_token().text);
        }
        term.original_term_str += current_token().text; // Add ')'
        consume_token(); // Consume ')'
        return term;
    } else {
        throw std::runtime_error("Unexpected token while parsing factor: " + current_token().text);
    }
}


// Parses terms with multiplication and division precedence
ParsedTerm Parser::parse_multiplication() {
    std::vector<ParsedTerm> factors;
    factors.push_back(parse_factor()); // Get the first factor

    while (current_token().type == TokenType::MULTIPLY) {
        consume_token(); // Consume '*'
        factors.push_back(parse_factor()); // Get the next factor
    }

    if (factors.empty()) {
        throw std::runtime_error("No factors found in multiplication.");
    }

    ParsedTerm combined_term;
    combined_term.coefficient = 1.0;
    std::string original_str_parts;

    // Accumulate overall coefficient and build original string representation
    for (const auto& factor : factors) {
        combined_term.coefficient *= factor.coefficient;
        if (!original_str_parts.empty()) {
            original_str_parts += "*";
        }
        original_str_parts += factor.original_term_str;
    }
    combined_term.original_term_str = original_str_parts;

    // If only one factor, return it directly (after applying its coefficient)
    if (factors.size() == 1) {
        combined_term.type = factors[0].type;
        combined_term.parameters = factors[0].parameters;
        return combined_term;
    }

    // New logic for combining multiple factors
    double t_exponent = 0.0;
    bool has_t_term = false;
    double exp_a = 0.0;
    bool has_exp_term = false;
    double trig_omega = 0.0;
    FunctionType trig_hyper_type = FunctionType::UNRECOGNIZED;

    for (const auto& factor : factors) {
        if (factor.type == FunctionType::CONSTANT) {
            // Coefficient already handled
        } else if (factor.type == FunctionType::T_POW_N) {
            has_t_term = true;
            t_exponent += factor.parameters[0]; // Sum exponents for t
        } else if (factor.type == FunctionType::EXP) {
            if (has_exp_term) {
                throw std::runtime_error("Multiple exponential functions in multiplication are not supported.");
            }
            has_exp_term = true;
            exp_a = factor.parameters[0];
        } else if (factor.type == FunctionType::SIN || factor.type == FunctionType::COS ||
                   factor.type == FunctionType::SINH || factor.type == FunctionType::COSH) {
            if (trig_hyper_type != FunctionType::UNRECOGNIZED) {
                throw std::runtime_error("Multiple trigonometric/hyperbolic functions in multiplication are not supported.");
            }
            trig_hyper_type = factor.type;
            trig_omega = factor.parameters[0];
        } else {
            throw std::runtime_error("Unsupported function type in multiplication: " + factor.text_representation());
        }
    }

    // Determine the final combined type
    if (has_t_term && t_exponent > 1.0) {
        // If t has a power greater than 1, and there are other functional terms (exp, trig/hyper),
        // it's an unsupported combination for now.
        if (has_exp_term || trig_hyper_type != FunctionType::UNRECOGNIZED) {
            throw std::runtime_error("Unsupported complex multiplication: t^n (n>1) with other functions.");
        }
        // If only t^n (n>1) and constants, it's just T_POW_N
        combined_term.type = FunctionType::T_POW_N;
        combined_term.parameters.push_back(t_exponent);
    } else if (has_t_term && t_exponent == 1.0) { // t^1
        if (has_exp_term && trig_hyper_type != FunctionType::UNRECOGNIZED) {
            // t * exp(at) * trig(omega*t)
            if (trig_hyper_type == FunctionType::SIN) combined_term.type = FunctionType::T_EXP_SIN;
            else if (trig_hyper_type == FunctionType::COS) combined_term.type = FunctionType::T_EXP_COS;
            else if (trig_hyper_type == FunctionType::SINH) combined_term.type = FunctionType::T_EXP_SINH;
            else if (trig_hyper_type == FunctionType::COSH) combined_term.type = FunctionType::T_EXP_COSH;
            combined_term.parameters.push_back(exp_a);
            combined_term.parameters.push_back(trig_omega);
        } else if (has_exp_term) {
            // t * exp(at)
            combined_term.type = FunctionType::T_EXP;
            combined_term.parameters.push_back(exp_a);
        } else if (trig_hyper_type != FunctionType::UNRECOGNIZED) {
            // t * trig(omega*t)
            if (trig_hyper_type == FunctionType::SIN) combined_term.type = FunctionType::T_SIN;
            else if (trig_hyper_type == FunctionType::COS) combined_term.type = FunctionType::T_COS;
            else if (trig_hyper_type == FunctionType::SINH) combined_term.type = FunctionType::T_SINH;
            else if (trig_hyper_type == FunctionType::COSH) combined_term.type = FunctionType::T_COSH;
            combined_term.parameters.push_back(trig_omega);
        } else {
            // Just t^1 and constants
            combined_term.type = FunctionType::T_POW_N;
            combined_term.parameters.push_back(1.0);
        }
    } else if (has_exp_term && trig_hyper_type != FunctionType::UNRECOGNIZED) {
        // exp(at) * trig(omega*t)
        if (trig_hyper_type == FunctionType::SIN) combined_term.type = FunctionType::EXP_SIN;
        else if (trig_hyper_type == FunctionType::COS) combined_term.type = FunctionType::EXP_COS;
        else if (trig_hyper_type == FunctionType::SINH) combined_term.type = FunctionType::EXP_SINH;
        else if (trig_hyper_type == FunctionType::COSH) combined_term.type = FunctionType::EXP_COSH;
        combined_term.parameters.push_back(exp_a);
        combined_term.parameters.push_back(trig_omega);
    } else if (has_exp_term) {
        // Just exp(at) and constants
        combined_term.type = FunctionType::EXP;
        combined_term.parameters.push_back(exp_a);
    } else if (trig_hyper_type != FunctionType::UNRECOGNIZED) {
        // Just trig/hyperbolic and constants
        combined_term.type = trig_hyper_type;
        combined_term.parameters.push_back(trig_omega);
    } else {
        // Only constants were multiplied
        combined_term.type = FunctionType::CONSTANT;
    }

    return combined_term;
}


// Helper function to parse an expression that is expected to be within parentheses.
// It's like a mini-parse_expression, but it expects to find ')' at the end.
std::vector<ParsedTerm> Parser::parse_expression_in_parentheses_helper() {
    std::vector<ParsedTerm> terms_in_paren;
    double overall_sign_for_term = 1.0;

    // Handle initial sign inside parentheses, e.g. (-sin(t))
    if (current_token().type == TokenType::MINUS) {
        overall_sign_for_term = -1.0;
        consume_token();
    } else if (current_token().type == TokenType::PLUS) {
        consume_token();
    }

    // Parse the first term within parentheses
    ParsedTerm current_parsed_term = parse_multiplication();
    current_parsed_term.coefficient *= overall_sign_for_term;
    terms_in_paren.push_back(current_parsed_term);

    // Continue parsing subsequent terms connected by + or - until RPAREN or END_OF_INPUT
    while (current_token().type == TokenType::PLUS || current_token().type == TokenType::MINUS) {
        overall_sign_for_term = (current_token().type == TokenType::PLUS) ? 1.0 : -1.0;
        consume_token(); // Consume '+' or '-'

        // Parse the next additive term
        current_parsed_term = parse_multiplication();
        current_parsed_term.coefficient *= overall_sign_for_term;
        terms_in_paren.push_back(current_parsed_term);
    }
    // The calling `parse_factor` will consume the RPAREN.
    return terms_in_paren;
}

std::vector<ParsedTerm> Parser::parse_expression(const std::string& input) {
    tokens_ = tokenize(input); // Call the external tokenize function
    token_idx_ = 0; // Reset token index

    std::vector<ParsedTerm> result_terms; // Local vector to hold results

    if (tokens_.empty() || current_token().type == TokenType::END_OF_INPUT) {
        return result_terms; // Empty input, return empty list
    }

    // Handle leading sign for the first term
    double overall_sign_for_term = 1.0;
    if (current_token().type == TokenType::MINUS) {
        overall_sign_for_term = -1.0;
        consume_token();
    } else if (current_token().type == TokenType::PLUS) {
        consume_token();
    }

    // Parse the first additive term (which can contain multiplications)
    ParsedTerm current_parsed_term = parse_multiplication(); // Use the new function
    current_parsed_term.coefficient *= overall_sign_for_term;
    result_terms.push_back(current_parsed_term);

    // Continue parsing subsequent additive terms connected by + or -
    while (current_token().type == TokenType::PLUS || current_token().type == TokenType::MINUS) {
        overall_sign_for_term = (current_token().type == TokenType::PLUS) ? 1.0 : -1.0;
        consume_token(); // Consume '+' or '-'

        // Parse the next additive term
        current_parsed_term = parse_multiplication(); // Use the new function
        current_parsed_term.coefficient *= overall_sign_for_term;
        result_terms.push_back(current_parsed_term);
    }

    if (current_token().type != TokenType::END_OF_INPUT) {
        throw std::runtime_error("Unexpected token at end of expression: " + current_token().text);
    }
    return result_terms;
}
