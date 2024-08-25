//
//  main.cpp
//  leetcode_debug
//
//  Created by Thanh Duong on 21/6/24.
//

#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int> > merge(vector<vector<int> >& intervals) {
        for (int i = 0; i < intervals.size(); i++) {
            for (int j = i+1; j < intervals.size(); j++) {
                if ((intervals[i][0] <= intervals[j][0])&&(intervals[j][0] <= intervals[i][1])&&(intervals[i][1] <= intervals[j][1])) {
                    intervals[i][1] = intervals[j][1];
                    intervals.erase(intervals.begin()+j);
                    return merge(intervals);
                } 
                else if ((intervals[j][0] <= intervals[i][0])&&(intervals[i][0] <= intervals[j][1])&&(intervals[j][1] <= intervals[i][1])) {
                    intervals[j][1] = intervals[i][1];
                    intervals.erase(intervals.begin()+i);
                    return merge(intervals);
                }
                else if ((intervals[i][0] >= intervals[j][0])&&(intervals[i][1] <= intervals[j][1])) {
                    intervals[i][1] = intervals[i][1];
                    intervals.erase(intervals.begin()+i);
                    return merge(intervals);
                }
            }
        }
        return intervals;
    }
};

int main(int argc, const char * argv[]) {
    Solution *my_soltion;
    my_soltion = new Solution();
    
    vector<vector<int> > my_intervals;
    vector<int> sub_intervals;
    
    //[[1,3],[2,6],[8,10],[15,18]]
    
    sub_intervals.push_back(1);
    sub_intervals.push_back(4);
    my_intervals.push_back(sub_intervals);
    sub_intervals[0] = 0;
    sub_intervals[1] = 4;
    my_intervals.push_back(sub_intervals);
    // sub_intervals[0] = 8;
    // sub_intervals[1] = 10;
    // my_intervals.push_back(sub_intervals);
    // sub_intervals[0] = 15;
    // sub_intervals[1] = 18;
    // my_intervals.push_back(sub_intervals);
    
    my_soltion->merge(my_intervals);
    
    return 0;
}
