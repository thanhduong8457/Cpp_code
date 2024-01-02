#ifndef __PERSON_H__
#define __PERSON_H__
#include <string>

class Person
{
    std::string first_name;
    std::string last_name;
    std::string gender;
    int age;
public:
    Person();
    Person(std::string);
    Person(std::string,std::string, int);
    std::string get_fistname();
    std::string get_fullname();
    int get_age();
    void set_firstname(std::string);
    void set_lastname(std::string);
    void set_gender(std::string);
    void set_age(int);
};
#endif// __PERSON_H__