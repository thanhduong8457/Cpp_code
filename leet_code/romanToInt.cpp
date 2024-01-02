#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Roman numerals are represented by seven different symbols: I, V, X, L, C, D and M.
//
//------------------------
//| Symbol     |  Value  |
//------------------------
//| I          |   1     |
//| V          |   5     |
//| X          |   10    |
//| L          |   50    |
//| C          |   100   |
//| D          |   500   |
//| M          |   1000  |
//------------------------
//
//For example, 2 is written as II in Roman numeral, just two ones added together. 
//12 is written as XII, which is simply X + II. 
//The number 27 is written as XXVII, which is XX + V + II.
//
//Roman numerals are usually written largest to smallest from left to right. 
//However, the numeral for four is not IIII. Instead, the number four is written as IV. 
//Because the one is before the five we subtract it making four. 
//The same principle applies to the number nine, which is written as IX. 
//There are six instances where subtraction is used:
//
//I can be placed before V (5) and X (10) to make 4 and 9. 
//X can be placed before L (50) and C (100) to make 40 and 90. 
//C can be placed before D (500) and M (1000) to make 400 and 900.
//
//Example 1:
//    Input: s = "III"
//    Output: 3
//    Explanation: III = 3.
//
//Example 2:
//    Input: s = "LVIII"
//    Output: 58
//    Explanation: L = 50, V= 5, III = 3.
//
//Example 3:
//    Input: s = "MCMXCIV"
//    Output: 1994
//    Explanation: M = 1000, CM = 900, XC = 90 and IV = 4.

enum singleRomanToInt {
    emI = 1,
    emV = 5,
    emX = 10,
    emL = 50,
    emC = 100,
    emD = 500,
    emM = 1000
};

class Solution {
private:
    int singleRomanToInt(const char s) {
        switch (s) {
            case 'I': return 1;
            case 'V': return 5;
            case 'X': return 10;
            case 'L': return 50;
            case 'C': return 100;
            case 'D': return 500;
            case 'M': return 1000;
            default: return 0;
        }  
    }
public:
    int romanToInt(string s) {
        int return_val = 0;
        int temp = 0;
        while (temp < s.size()) {
            if (singleRomanToInt(s[temp]) < singleRomanToInt(s[temp+1])) {
                return_val += singleRomanToInt(s[temp+1]) - singleRomanToInt(s[temp]);
                temp += 2;
            }
            else {
                return_val += singleRomanToInt(s[temp]);
                temp += 1;
            }
        }
        return return_val;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    } else {
        cout << "This check point is FAIL" << endl << endl;
    }
}

void check_point(const string my_string, const int check_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "with Roman Numeral = " << my_string << endl;

    if (check_val == mSolution->romanToInt(my_string)) {
        judment(true);
    }
    else {
        cout << "current val is: " << mSolution->romanToInt(my_string) << endl;
        cout << "expected val is: " << check_val << endl;
        judment(false);
    }

    delete mSolution;
}

int main(void)
{
    check_point("III", 3);
    check_point("LVIII", 58);
    check_point("MCMXCIV", 1994);

    return 0;
}