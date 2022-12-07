#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Write a function to find the longest common prefix string amongst an array of strings.
// If there is no common prefix, return an empty string "".

// Example 1:
//     Input: strs = ["flower","flow","flight"]
//     Output: "fl"

// Example 2:
//     Input: strs = ["dog","racecar","car"]
//     Output: ""
//     Explanation: There is no common prefix among the input strings.

class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        string my_string = "";
        unsigned int max = 0;
        char temp;
        bool is_insert = true;
        
        for (int i= 0; i <  strs.size(); i++) {
            if (max < strs[i].size()) {
                max = strs[i].size();
            }
        }

        for (int i = 0; i < max; i++) {
            temp = strs[0][i];
            for (int j = 1; j < strs.size(); j++) {
                if (temp != strs[j][i]) {
                    is_insert = false;
                    break;
                }
            }
            if (true == is_insert) {
                my_string += temp;
            }
        }
        
        return my_string;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    } else {
        cout << "This check point is FAIL" << endl << endl;
    }
}

void check_point(vector<string>& my_string, const string check_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    if (check_val == mSolution->longestCommonPrefix(my_string)) {
        judment(true);
    }
    else {
        cout << "current output is: " << mSolution->longestCommonPrefix(my_string) << endl;
        cout << "expected output is: " << check_val << endl;
        judment(false);
    }

    delete mSolution;
}

int main(void)
{
    string string_result;
    vector<string> string_test;

    string_test.push_back("flower");
    string_test.push_back("flow");
    string_test.push_back("flight");
    string_result = "fl";

    check_point(string_test, string_result);

    string_test.erase(string_test.begin(), string_test.end());
    string_test.push_back("dog");
    string_test.push_back("racecar");
    string_test.push_back("car");
    string_result = "";

    check_point(string_test, string_result);


    return 0;
}