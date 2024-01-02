#include "Student.h"
using namespace std;

Student::Student():Person()
{
    id_student = "";
    major = "";
    gpa = 0;
}

Student::Student(string id_student, int gpa)
{
    major = "";
    this->id_student = id_student;
    this->gpa = gpa;
}

Student::Student(string name, string gender, int age, string id_student, string major, float gpa):Person(name,gender,age)
{
    this->id_student = id_student;
    this->major = major;
    this->gpa = gpa;
}

string Student::show_info()
{
    cout << "full name: " << get_fullname() << endl;
    cout << "age: " << get_age() << endl;
    cout << "id student: " << id_student << endl;
    cout << "major: " << major << endl;
    cout << "gpa: " << gpa << endl;
}