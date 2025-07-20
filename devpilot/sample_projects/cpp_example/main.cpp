#include <iostream>
#include "calculator.hpp"

void demonstrateCalculator() {
    Calculator calc;
    
    try {
        // Test basic operations
        std::cout << "Testing basic operations:" << std::endl;
        double result1 = calc.add(10.0, 5.0);
        std::cout << "10 + 5 = " << result1 << std::endl;
        
        double result2 = calc.subtract(20.0, 8.0);
        std::cout << "20 - 8 = " << result2 << std::endl;
        
        double result3 = calc.multiply(4.0, 7.0);
        std::cout << "4 * 7 = " << result3 << std::endl;
        
        double result4 = calc.divide(15.0, 3.0);
        std::cout << "15 / 3 = " << result4 << std::endl;
        
        // Test advanced operations
        std::cout << "\nTesting advanced operations:" << std::endl;
        double result5 = calc.power(2.0, 8.0);
        std::cout << "2^8 = " << result5 << std::endl;
        
        double result6 = calc.squareRoot(64.0);
        std::cout << "sqrt(64) = " << result6 << std::endl;
        
        // Show last result
        std::cout << "\nLast result: " << calc.getLastResult() << std::endl;
        
        // Reset calculator
        calc.reset();
        std::cout << "After reset: " << calc.getLastResult() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int processUserInput() {
    Calculator calc;
    char operation;
    double a, b;
    
    std::cout << "Enter operation (+, -, *, /, p for power, s for sqrt, q to quit): ";
    while (std::cin >> operation && operation != 'q') {
        try {
            if (operation == 's') {
                std::cout << "Enter value: ";
                std::cin >> a;
                std::cout << "Result: " << calc.squareRoot(a) << std::endl;
            } else if (operation == 'p') {
                std::cout << "Enter base and exponent: ";
                std::cin >> a >> b;
                std::cout << "Result: " << calc.power(a, b) << std::endl;
            } else {
                std::cout << "Enter two numbers: ";
                std::cin >> a >> b;
                
                switch (operation) {
                    case '+':
                        std::cout << "Result: " << calc.add(a, b) << std::endl;
                        break;
                    case '-':
                        std::cout << "Result: " << calc.subtract(a, b) << std::endl;
                        break;
                    case '*':
                        std::cout << "Result: " << calc.multiply(a, b) << std::endl;
                        break;
                    case '/':
                        std::cout << "Result: " << calc.divide(a, b) << std::endl;
                        break;
                    default:
                        std::cout << "Unknown operation: " << operation << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        
        std::cout << "Enter operation (+, -, *, /, p for power, s for sqrt, q to quit): ";
    }
    
    return 0;
}

int main() {
    std::cout << "=== Calculator Demo ===" << std::endl;
    
    // Run demonstration
    demonstrateCalculator();
    
    std::cout << "\n=== Interactive Mode ===" << std::endl;
    
    // Interactive mode
    return processUserInput();
}
