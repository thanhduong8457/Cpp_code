#ifndef __STUDENT_H__
#define __STUDENT_H__
#include <iostream>
#include <string>
#include "Person.h"
using namespace std;

class Student: public Person
{
    string id_student;
    string major;
    float gpa;
public:
    Student();
    Student(string, int);
    Student(string , string , int, string, string, float);  
    string show_info();
};

#endif//__STUDENT_H__