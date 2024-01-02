#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Given a string s containing just the characters '(', ')', '{', '}', '[' and ']', determine if the input string is valid.

// An input string is valid if:

// Open brackets must be closed by the same type of brackets.
// Open brackets must be closed in the correct order.
// Every close bracket has a corresponding open bracket of the same type.
 

// Example 1:
//     Input: s = "()"
//     Output: true

// Example 2:
//     Input: s = "()[]{}"
//     Output: true

// Example 3:
//     Input: s = "(]"
//     Output: false

class Solution {
private:
    enum emvalidParentheses {
        emRoundBracketOpen = 0x28,
        emRoundBracketClose = 0x29,
        emBracketOpen = 0x5b,
        emBracketClose = 0x5d,
        emCurlyBracketOpen = 0x7b,
        emCurlyBracketClose = 0x7d
    };
public:
    bool isValid(string s) {
        vector<unsigned int> list_bracket;
        for (int i = 0; i< s.size(); i++) {
            switch ((int)s[i]) {
                case emRoundBracketOpen:
                    list_bracket.push_back(emRoundBracketOpen);
                    break;
                case emRoundBracketClose:
                    if ((list_bracket.size() == 0) || (emRoundBracketOpen != list_bracket[list_bracket.size()-1])) {
                        return false;
                    }
                    list_bracket.pop_back();
                    break;

                case emBracketOpen:
                    list_bracket.push_back(emBracketOpen);
                    break;
                case emBracketClose:
                    if ((list_bracket.size() == 0) || (emBracketOpen != list_bracket[list_bracket.size()-1])) {
                        return false;
                    }
                    list_bracket.pop_back();
                    break;

                case emCurlyBracketOpen:
                    list_bracket.push_back(emCurlyBracketOpen);
                    break;
                case emCurlyBracketClose:
                    if ((list_bracket.size() == 0) || (emCurlyBracketOpen != list_bracket[list_bracket.size()-1])) {
                        return false;
                    }
                    list_bracket.pop_back();
                    break; 
                default:
                    break;
            }
        }

        if (list_bracket.size() != 0) {
            return false;
        }
        
        return true;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    } else {
        cout << "This check point is FAIL" << endl << endl;
    }
}

void check_point(const string test_string, bool check_val){
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "with string input is: " << test_string << endl;

    if (check_val == mSolution->isValid(test_string)) {
        judment(true);
    }
    else {
        cout << "current output is: " << mSolution->isValid(test_string) << endl;
        cout << "expected output is: " << check_val << endl;
        judment(false);
    }

    delete mSolution;
}

int main(void)
{
    check_point("[", false);
    check_point("((", false);
    check_point("){", false);
    check_point("()", true);
    check_point("()[]{}", true);
    check_point("(]", false);
    check_point("{[}]", false);
    check_point("{[]}", true);

    return 0;
}