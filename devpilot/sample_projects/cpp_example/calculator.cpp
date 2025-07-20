#include "calculator.hpp"
#include <iostream>
#include <cmath>
#include <stdexcept>

Calculator::Calculator() : lastResult(0.0) {
    std::cout << "Calculator initialized" << std::endl;
}

Calculator::~Calculator() {
    std::cout << "Calculator destroyed" << std::endl;
}

double Calculator::add(double a, double b) {
    if (!validateInput(a) || !validateInput(b)) {
        throw std::invalid_argument("Invalid input values");
    }
    
    lastResult = a + b;
    logOperation("add", lastResult);
    return lastResult;
}

double Calculator::subtract(double a, double b) {
    if (!validateInput(a) || !validateInput(b)) {
        throw std::invalid_argument("Invalid input values");
    }
    
    lastResult = a - b;
    logOperation("subtract", lastResult);
    return lastResult;
}

double Calculator::multiply(double a, double b) {
    if (!validateInput(a) || !validateInput(b)) {
        throw std::invalid_argument("Invalid input values");
    }
    
    lastResult = a * b;
    logOperation("multiply", lastResult);
    return lastResult;
}

double Calculator::divide(double a, double b) {
    if (!validateInput(a) || !validateInput(b)) {
        throw std::invalid_argument("Invalid input values");
    }
    
    if (b == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    
    lastResult = a / b;
    logOperation("divide", lastResult);
    return lastResult;
}

double Calculator::power(double base, double exponent) {
    if (!validateInput(base) || !validateInput(exponent)) {
        throw std::invalid_argument("Invalid input values");
    }
    
    lastResult = std::pow(base, exponent);
    logOperation("power", lastResult);
    return lastResult;
}

double Calculator::squareRoot(double value) {
    if (!validateInput(value)) {
        throw std::invalid_argument("Invalid input value");
    }
    
    if (value < 0.0) {
        throw std::invalid_argument("Cannot calculate square root of negative number");
    }
    
    lastResult = std::sqrt(value);
    logOperation("sqrt", lastResult);
    return lastResult;
}

void Calculator::reset() {
    lastResult = 0.0;
    std::cout << "Calculator reset" << std::endl;
}

double Calculator::getLastResult() const {
    return lastResult;
}

bool Calculator::validateInput(double value) {
    // Check for NaN and infinity
    return std::isfinite(value);
}

void Calculator::logOperation(const std::string& operation, double result) {
    std::cout << "Operation: " << operation << " = " << result << std::endl;
}
