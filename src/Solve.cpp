#include <iostream>
#include <string>
#include <vector>
#include <limits> // Required for std::numeric_limits
#include <stdexcept> // Required for std::runtime_error
#include "laplace_transforms.h" 
#include "parser.h"  
#include <locale>
#include <codecvt> 

class Solve {
    private : 
    bool continue_solving = true;
    Parser parser;


    public : 

    Solve(std::wstring &inputString) {

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        std::string input_function = converter.to_bytes(inputString);


        parser = Parser(); // Re-initialize the parser for a fresh state

        try {
            std::vector<ParsedTerm> terms = parser.parse_expression(input_function);
            std::string total_laplace_transform = "";

            for (size_t i = 0; i < terms.size(); ++i) {
                ParsedTerm term = terms[i];
                std::string term_laplace_str;

                switch (term.type) {
                    case FunctionType::CONSTANT:
                        term_laplace_str = Laplace::transform_constant(term.coefficient);
                        break;
                    case FunctionType::T_POW_N:
                        if (term.parameters.empty()) throw std::runtime_error("Missing exponent for t");
                        term_laplace_str = Laplace::transform_t_pow_n(static_cast<int>(term.parameters[0]), term.coefficient);
                        break;
                    case FunctionType::SIN:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for sin");
                        term_laplace_str = Laplace::transform_sin(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::COS:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for cos");
                        term_laplace_str = Laplace::transform_cos(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::EXP:
                        if (term.parameters.empty()) throw std::runtime_error("Missing 'a' for exp");
                        term_laplace_str = Laplace::transform_exp(term.parameters[0], term.coefficient);
                        break;
                    // Basic Hyperbolic Functions
                    case FunctionType::SINH:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for sinh");
                        term_laplace_str = Laplace::transform_sinh(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::COSH:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for cosh");
                        term_laplace_str = Laplace::transform_cosh(term.parameters[0], term.coefficient);
                        break;
                    // Compound Function Types
                    case FunctionType::T_EXP:
                        if (term.parameters.empty()) throw std::runtime_error("Missing 'a' for t*exp");
                        term_laplace_str = Laplace::transform_t_exp(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::T_SIN:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for t*sin");
                        term_laplace_str = Laplace::transform_t_sin(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::T_COS:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for t*cos");
                        term_laplace_str = Laplace::transform_t_cos(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::EXP_SIN:
                        if (term.parameters.size() < 2) throw std::runtime_error("Missing 'a' or 'omega' for exp*sin");
                        term_laplace_str = Laplace::transform_exp_sin(term.parameters[0], term.parameters[2], term.coefficient);
                        break;
                    case FunctionType::EXP_COS:
                        if (term.parameters.size() < 2) throw std::runtime_error("Missing 'a' or 'omega' for exp*cos");
                        term_laplace_str = Laplace::transform_exp_cos(term.parameters[0], term.parameters[2], term.coefficient);
                        break;
                    // Compound Hyperbolic Functions
                    case FunctionType::T_SINH:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for t*sinh");
                        term_laplace_str = Laplace::transform_t_sinh(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::T_COSH:
                        if (term.parameters.empty()) throw std::runtime_error("Missing omega for t*cosh");
                        term_laplace_str = Laplace::transform_t_cosh(term.parameters[0], term.coefficient);
                        break;
                    case FunctionType::EXP_SINH:
                        if (term.parameters.size() < 2) throw std::runtime_error("Missing 'a' or 'omega' for exp*sinh");
                        term_laplace_str = Laplace::transform_exp_sinh(term.parameters[0], term.parameters[2], term.coefficient);
                        break;
                    case FunctionType::EXP_COSH:
                        if (term.parameters.size() < 2) throw std::runtime_error("Missing 'a' or 'omega' for exp*cosh");
                        term_laplace_str = Laplace::transform_exp_cosh(term.parameters[0], term.parameters[2], term.coefficient);
                        break;

                    case FunctionType::UNRECOGNIZED:
                        term_laplace_str = "[ERROR: Unrecognized Term]";
                        break;
                    default:
                        term_laplace_str = "[ERROR: Unknown Function Type]";
                        break;
                }

                if (i == 0) {
                    total_laplace_transform = term_laplace_str;
                } else {
                    // Check if the current term's coefficient is negative to determine if we should add '-' or '+'
                    if (term.coefficient < 0) {
                        // If it's negative, the sign is already part of term_laplace_str, so just append
                        total_laplace_transform += " " + term_laplace_str;
                    } else {
                        // If it's positive, add a '+'
                        total_laplace_transform += " + " + term_laplace_str;
                    }
                }
            }
            // std::cout << "L{" << input_function << "} = " << total_laplace_transform << std::endl;

            
            inputString = converter.from_bytes(total_laplace_transform);

        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        }

}


} ;