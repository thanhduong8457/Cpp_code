#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main () 
{
   // string str1 = "Hello";
   // string str2 = "World";
   // string str3;
   // int  len ;

   // // copy str1 into str3
   // str3 = str1;
   // cout << "str3 : " << str3 << endl;

   // // concatenates str1 and str2
   // str3 = str1 + str2;
   // cout << "str1 + str2 : " << str3 << endl;

   // // total length of str3 after concatenation
   // len = str3.size();
   // cout << "str3.size() :  " << len << endl;
   
   vector<int> test_vector;

   test_vector.push_back(1);
   test_vector.push_back(2);
   test_vector.push_back(3);
   test_vector.push_back(4);

   cout << test_vector.size() << endl;

   int temp;

   test_vector.pop_back(temp);

   cout << temp  << endl;

   cout << test_vector.size() << endl;

   return 0;
}