#include <iostream>
#include <map>

using namespace std;

int main(void)
{
    try {
        int a, b;
        cin>>a>>b;
        if ( 0 == b ) {
            throw 69;
        }
    }
    catch(int e) {
        std::cout << "can not divide to zero" << '\n';
    }
    
    return 0;
}