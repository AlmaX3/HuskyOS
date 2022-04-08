#include <Math.h>

int pow(int base, int exponent) {
    int returned_value = base;
    for (int i = 0; i < exponent; i++)
    {
        returned_value *= base;
        
    }
    return returned_value;
    
}