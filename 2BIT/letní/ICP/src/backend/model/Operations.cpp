/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu operaci a funkci
 * @author HanakJiri, xhanak33
 * @file Operations.cpp
 */

#include "Operations.h"


Operations::Operations() {
    auto * op_add = new Op_add;
    auto * op_sub = new Op_sub();
    auto * op_mul = new Op_mul();
    auto * op_div = new Op_div();
    auto * op_and = new Op_and();
    auto * op_or = new Op_or();
    auto * op_not = new Op_not();
    auto * op_xor = new Op_xor();
    auto * op_sin = new Op_sin();
    auto * op_cos = new Op_cos();
    auto * op_tan = new Op_tan();

    Operations::operators.emplace_back(op_add);
    Operations::operators.emplace_back(op_sub);
    Operations::operators.emplace_back(op_mul);
    Operations::operators.emplace_back(op_div);
    Operations::operators.emplace_back(op_and);
    Operations::operators.emplace_back(op_or);
    Operations::operators.emplace_back(op_not);
    Operations::operators.emplace_back(op_xor);
    Operations::operators.emplace_back(op_sin);
    Operations::operators.emplace_back(op_cos);
    Operations::operators.emplace_back(op_tan);
}


std::vector<Ops *> Operations::get_operations() {
    return Operations::operators;
}


void Operations::clear() {
    for (auto const &oper : Operations::operators) {
        delete oper;
    }
}


double Op_add::eval(std::vector<double> values) {

    if (values.size() < 2) {
        throw "Operand is missing";
    }

    double result = 0;

    for (auto const &val : values) {
        result += val;
    }
    return result;
}


std::string Op_add::repr() {
    return "+";
}


double Op_sub::eval(std::vector<double> values) {
    if (values.size() != 2) {
        throw "Bad operands";
    }
    return values[0] - values[1];
}

std::string Op_sub::repr() {
    return "-";
}


double Op_mul::eval(std::vector<double> values) {
    if (values.size() < 2) {
        throw "Operand is missing";
    }

    double result = 1;

    for (auto const &val : values) {
        result *= val;
    }
    return result;
}

std::string Op_mul::repr() {
    return "*";
}


double Op_div::eval(std::vector<double> values) {
    if (values.size() != 2) {
        throw "Bad operands";
    }

    if (values[1] == 0) {
        throw "Division by zero";
    }

    return values[0] / values[1];
}

std::string Op_div::repr() {
    return "/";
}


double Op_and::eval(std::vector<double> values) {

    if (values.size() < 2) {
        throw "Operand is missing";
    }

    unsigned result = UINT_MAX;

    for (auto const &val : values) {
        if (!is_binary(val)) {
            throw "Not binary data";
        }
        result &= (unsigned)val;
    }
    return double(result);
}

std::string Op_and::repr() {
    return "&";
}

double Op_or::eval(std::vector<double> values) {
    if (values.size() < 2) {
        throw "Operand is missing";
    }

    unsigned result = 0;

    for (auto const &val : values) {
        if (!is_binary(val)) {
            throw "Not binary data";
        }
        result |= (unsigned)val;
    }
    return double(result);
}

std::string Op_or::repr() {
    return "|";
}

double Op_not::eval(std::vector<double> values) {
    if (values.size() != 1) {
        throw "Bad operands";
    }

    if (!is_binary(values[0])) {
        throw "Not binary data";
    }

    return double(!(bool)values[0]);
}

std::string Op_not::repr() {
    return "!";
}

double Op_xor::eval(std::vector<double> values) {

    if (values.size() < 2) {
        throw "Operand is missing";
    }

    unsigned result = 0;

    for (auto const &val : values) {
        if (!is_binary(val)) {
            throw "Not binary data";
        }
        result ^= (unsigned)val;
    }
    return double(result);
}

std::string Op_xor::repr() {
    return "^";
}

double Op_sin::eval(std::vector<double> values) {
    if (values.size() != 1) {
        throw "Operand is missing";
    }
    return std::sin(values[0]);
}

std::string Op_sin::repr() {
    return "sin()";
}

double Op_cos::eval(std::vector<double> values) {
    if (values.size() != 1) {
        throw "Operand is missing";
    }
    return std::cos(values[0]);
}

std::string Op_cos::repr() {
    return "cos()";
}

double Op_tan::eval(std::vector<double> values) {
    if (values.size() != 1) {
        throw "Operand is missing";
    }
    return std::tan(values[0]);
}

std::string Op_tan::repr() {
    return "tan()";
}

bool is_binary(double value) {
    return value == 0 || value == 1;
}