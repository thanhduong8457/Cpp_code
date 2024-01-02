#include <iostream>
#include <map>
#include <string>

namespace test_1{
    void test(void)
    {
        std::cout<<"this is namespace 1"<<std::endl;
    }
}

namespace test_2{
    void test(void)
    {
        std::cout<<"this is namespace 2"<<std::endl;
    }
}

int main(void)
{
	test_1::test();
	test_2::test();
    
	return 0;
}
