#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Input: s = "pwwkew"
// Output: 3
// Explanation: The answer is "wke", with the length of 3.
// Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        bool is_insert = true;
        unsigned int max = 0;
        vector<char> list_string;
        vector<vector<char>> list_vector_string;
        if (1 == s.size()) {
            return 1;
        }
        for (int i = 0; i < s.size(); i++) {
            list_string.push_back(s[i]);
            for (int j = i + 1; j < s.size(); j++) {
                for (int k = 0; k < list_string.size(); k++) {
                    if (s[j] == list_string[k]) {
                        is_insert = false;
                    }
                }
                if (true == is_insert) {
                    list_string.push_back(s[j]);
                    if (j == s.size() - 1) {
                        list_vector_string.push_back(list_string);
                        list_string.erase(list_string.begin(), list_string.end());
                        break;
                    }
                } else {
                    list_vector_string.push_back(list_string);
                    list_string.erase(list_string.begin(), list_string.end());
                    is_insert = true;
                    break;
                }
            }
        }
        for (int i = 0; i < list_vector_string.size(); i++) {
            if (list_vector_string[i].size() > max) {
                max = list_vector_string[i].size();
            }
        }
        return max;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    } else {
        cout << "This check point is FAIL" << endl << endl;
    }
}

void check_point(string my_string, const unsigned int expected_val) {
    
    Solution *mSolution = NULL;
    mSolution = new Solution();

    unsigned int val = mSolution->lengthOfLongestSubstring(my_string);

    cout << "test run with string: \"" << my_string << "\"" << endl << "result is:   " << val << endl <<"expected is: " << expected_val << endl;

    if (expected_val == val) {
        judment(true);
    } else {
        judment(false);
    }
    
    delete mSolution;
}

int main(void)
{
    //string my_string = "au";
    check_point("au", 2);

    //string my_string = "abcdaef"; // -> 6
    check_point("abcdaef", 6);

    //string my_string = ""; // -> 0
    check_point("", 0);

    //string my_string = " "; // -> 1
    check_point(" ", 1);

       //string my_string = " "; // -> 1
    check_point("  ", 1);

    //string my_string = "abcabcbb"; // -> 3
    check_point("abcabcbb", 3);

    //string my_string = "abcabcbbabcabcbb"; // -> 3
    check_point("abcabcbbabcabcbb", 3);

    //string my_string = "aaaaaaaaa"; // -> 1
    check_point("aaaaaaaaa", 1);

    check_point("ohomm", 3);

    return 0;
} 
