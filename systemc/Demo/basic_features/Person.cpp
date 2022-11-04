#include "Person.h"

Person::Person()
{
    this->first_name = "";
    this->last_name = "";
    this->gender = "";
    this->age = 0;
}
Person::Person(std::string name)
{
    Person();
    this->last_name = name;
}

Person::Person(std::string name,std::string gender, int age)
{
    this->first_name = "";
    this->last_name = name;
    this->gender = gender;
    this->age = age;
}

std::string Person::get_fistname()
{
    return this->first_name;
}

std::string Person::get_fullname()
{
    return (this->first_name + " " + this->last_name);
}

int Person::get_age()
{
    return this->age;
}

void Person::set_firstname(std::string first_name)
{
    this->first_name = first_name;
}

void Person::set_lastname(std::string last_name)
{
    this->last_name = last_name;
}

void Person::set_gender(std::string gender)
{
    this->gender = gender;
}

void Person::set_age(int age)
{
    this->age = age;
}