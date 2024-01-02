#include <iostream>
#include <string>
#include "Person.h"
#include "Student.h"
#include "func.h"

using namespace std;
int a,b,c;
int main()  
{
    Student A("Duong", "male", 19, "1811809", "electrical", 7.5);
    A.set_firstname("Thanh");
    A.set_age(21);

    A.show_info();

    a = 1;
    b = 3;

    c = sum(a, b);

    cout<<"value of c: "<<c<<endl;


    return 0;
}