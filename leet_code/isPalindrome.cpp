#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    bool isPalindrome(int x) {
        bool return_val = false;
        unsigned int val_bakup = x;
        vector<int> list_index;

        while (val_bakup != 0) {
            list_index.push_back(val_bakup%10);
            val_bakup = (val_bakup - val_bakup%10)/10;
        }

        for (int i = 0; i < list_index.size(); i++) {
            val_bakup = val_bakup*10 + list_index[i];
        }

        if (val_bakup == x) {
            return_val = true;
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

void check_point(int check_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "with val = " << check_val << endl;

    judment(mSolution->isPalindrome(check_val));

    delete mSolution;
}

int main(void)
{
    check_point(121);
    check_point(-121);
    check_point(10);
    check_point(101);
    check_point(1011);
    check_point(1234321);

    return 0;
}