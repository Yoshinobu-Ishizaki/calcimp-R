#include <stdio.h>
#include <stdlib.h>

extern double struve(double v, double x);

int main() {
    double x = 0.059978;
    double result;
    
    printf("Testing struve function:\n");
    printf("x = %.6f\n", x);
    
    // Call struve(1, x) - order 1, argument x
    result = struve(1.0, x);
    printf("struve(1, %.6f) = %.10f\n", x, result);
    
    // Expected value from Python: 0.000763201
    printf("Expected: 0.000763201\n");
    printf("Difference: %.10f\n", result - 0.000763201);
    
    return 0;
}
