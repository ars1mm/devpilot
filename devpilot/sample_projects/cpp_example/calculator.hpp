#pragma once

class Calculator {
public:
    Calculator();
    ~Calculator();
    
    // Basic arithmetic operations
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
    
    // Advanced operations
    double power(double base, double exponent);
    double squareRoot(double value);
    
    // Utility methods
    void reset();
    double getLastResult() const;
    
private:
    double lastResult;
    bool validateInput(double value);
    void logOperation(const std::string& operation, double result);
};
