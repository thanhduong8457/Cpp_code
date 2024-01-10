#include "design.h"

void my_func(void) {
    std::cout << "my_func is called" << std::endl;
}

std::uint32_t factorial(std::uint32_t number) {
    return number <= 1 ? number : factorial(number - 1) * number;
}
