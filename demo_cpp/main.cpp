#include <iostream>
#include <string>

std::string defangIPaddr(const std::string& address) {
    std::string defangedAddress{""};
    for (char c : address) {
        if (c == '.') {
            defangedAddress += "[.]";
        } else {
            defangedAddress += c;
        }
    }
    return defangedAddress;
}

int main() {
    const std::string ipAddress {"1.1.1.1"};
    const std::string defangedIP {defangIPaddr(ipAddress)};
    std::cout << "Defanged IP address: " << defangedIP << std::endl;
    return 0;
}
