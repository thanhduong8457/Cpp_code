#include <iostream>
#include <vector>
#include <set>

using namespace std;

// Example 1:
// Input: nums = [1,1,2]
//     Output: 2, nums = [1,2,_]
//     Explanation: Your function should return k = 2, with the first two elements of nums being 1 and 2 respectively.
//     It does not matter what you leave beyond the returned k (hence they are underscores).

// Example 2:
//     Input: nums = [0,0,1,1,1,2,2,3,3,4]
//     Output: 5, nums = [0,1,2,3,4,_,_,_,_,_]
//     Explanation: Your function should return k = 5, with the first five elements of nums being 0, 1, 2, 3, and 4 respectively.
//     It does not matter what you leave beyond the returned k (hence they are underscores).

class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int return_val = 0;

        set<int> set_nums;
        set<int>::iterator it;

        for (int i = 0; i < nums.size(); i++) {
            set_nums.insert(nums[i]);
        }
        
        return_val = set_nums.size();

        int temp = 0;
        for (it = set_nums.begin(); it != set_nums.end(); it++) {
            nums[temp] = *it;
            temp++;
        } 

        return return_val;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    } else {
        cout << "This check point is FAIL" << endl << endl;
        exit(0);
    }
}

void check_point(vector<int> nums, vector<int> expectedNums, const unsigned int expected_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "with array [";

    for (int i = 0; i < nums.size(); i++) {
        cout << nums[i];
        if (i !=  nums.size() - 1) {
            cout << ", ";
        }
    }

    cout << "]" << endl;

    if (expected_val != mSolution->removeDuplicates(nums)) {
        cout << "expected val is: " << expected_val << endl;
        cout << "current val is: " << mSolution->removeDuplicates(nums) << endl;
        judment(false);
    }

    for (int i = 0; i < expected_val; i++) {
        if (nums[i] != expectedNums[i]) {
            judment(false);
        }
    }

    judment(true);

    delete mSolution;
}

int main(void)
{
    vector<int> my_vector;
    vector<int> nums;
    vector<int> expectedNums;
    unsigned int expected_val;

    nums = {1,1,2};
    expectedNums = {1,2,0};
    expected_val = 2;
    check_point(nums, expectedNums, expected_val);

    nums = {0,0,1,1,1,2,2,3,3,4};
    expectedNums = {0,1,2,3,4,0,0,0,0,0};
    expected_val = 5;
    check_point(nums, expectedNums, expected_val);

    cout << "######################################" << endl;
    cout << "############# TM is PASS #############" << endl;
    cout << "######################################" << endl;

    return 0;
}