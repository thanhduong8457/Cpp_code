#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> return_val;
        for (int i = 0; i < nums.size() - 1; i++) {
            for (int j = i + 1; j < nums.size(); j++) {
                if ((nums[i] + nums[j]) == target) {
                    return_val.push_back(i);
                    return_val.push_back(j);
                    break;
                }
            }
        }
        return return_val;
    }
};

int main(void)
{
    Solution *mSolution = NULL;
    mSolution = new Solution();

    vector<int> check_val;
    vector<int> result_val;
    int array_temp[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        check_val.push_back(array_temp[i]);
    }

    result_val = mSolution->twoSum(check_val, 5);

    if (result_val.size() != 0) {
        cout << "[" << result_val[0] << ", " << result_val[1] << "]" << endl;
    } 
    else {
        cout << "there are no result" << endl;
    }

    delete mSolution;

    return 0;
}
