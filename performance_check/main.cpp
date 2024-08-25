// main.cpp
#include <iostream>
#include <vector>

void dummy_work() {
    std::vector<int> v(1000000);
    for (int i = 0; i < 1000000; ++i) {
        v[i] = i;
    }
}

int main() {
    for (int i = 0; i < 100; ++i) {
        dummy_work();
    }
    return 0;
}
