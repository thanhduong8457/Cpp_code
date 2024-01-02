#include <iostream>

class A;  // Forward declaration

using my_func = void (A::*)();

class B {
public:
    my_func my_func_t;
    A *Aobj;

    B(A *temp_obj, my_func temp_ptr) : Aobj(temp_obj), my_func_t(temp_ptr) {}

    void callBackFunction() {
        (Aobj->*my_func_t)();
    }
};

class A {
public:
    B *demo1;
    B *demo2;

    void AFunction() {
        std::cout << "AFunction called" << std::endl;
    }

    void BFunction() {
        std::cout << "BFunction called" << std::endl;
    }

    A() {
        demo1 = new B(this, &A::AFunction);
        demo2 = new B(this, &A::BFunction);
    }

    void call_test() {
        this->demo1->callBackFunction();
        this->demo2->callBackFunction();
    }
};

int main() {
    A *a = new A();
    a->call_test();
    delete a;  // Don't forget to clean up memory
    return 0;
}