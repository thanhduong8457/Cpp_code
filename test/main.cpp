#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main (void) 
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
   
      for (int j = 0; j < 82; j++) {
         if (j < 10) {
            cout << j << "  ";
         }
         else {
            cout << j << " ";
         }
      }

   return 0;
}