#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

// Determine if a 9 x 9 Sudoku board is valid. Only the filled cells need to be validated according to the following rules:

// Each row must contain the digits 1-9 without repetition.
// Each column must contain the digits 1-9 without repetition.
// Each of the nine 3 x 3 sub-boxes of the grid must contain the digits 1-9 without repetition.
// Note:

// A Sudoku board (partially filled) could be valid but is not necessarily solvable.
// Only the filled cells need to be validated according to the mentioned rules.

// Input: board = 
//     [["5","3",".",".","7",".",".",".","."]
//     ,["6",".",".","1","9","5",".",".","."]
//     ,[".","9","8",".",".",".",".","6","."]
//     ,["8",".",".",".","6",".",".",".","3"]
//     ,["4",".",".","8",".","3",".",".","1"]
//     ,["7",".",".",".","2",".",".",".","6"]
//     ,[".","6",".",".",".",".","2","8","."]
//     ,[".",".",".","4","1","9",".",".","5"]
//     ,[".",".",".",".","8",".",".","7","9"]]
//     Output: true

// Example 2:
//     Input: board = 
//     [["8","3",".",".","7",".",".",".","."]
//     ,["6",".",".","1","9","5",".",".","."]
//     ,[".","9","8",".",".",".",".","6","."]
//     ,["8",".",".",".","6",".",".",".","3"]
//     ,["4",".",".","8",".","3",".",".","1"]
//     ,["7",".",".",".","2",".",".",".","6"]
//     ,[".","6",".",".",".",".","2","8","."]
//     ,[".",".",".","4","1","9",".",".","5"]
//     ,[".",".",".",".","8",".",".","7","9"]]
//     Output: false
//     Explanation: Same as Example 1, except with the 5 in the top left corner being modified to 8. Since there are two 8's in the top left 3x3 sub-box, it is invalid.

class Solution {
private:
    bool isNoRepeat(vector<char> my_vector){
        bool return_val = true;

        set<unsigned int> list_num;
        set<unsigned int>::iterator it;

        for (int i = 0; i < 10; i++) {
            list_num.insert(i);
        }

        int temp = 0;
        for (int i = 0; i < my_vector.size(); i++) {

            temp = ((int)my_vector[i] - 48);

            if (temp < 0 || temp > 9) {
                continue;
            }

            std::set<unsigned int>::iterator it_s = list_num.find(temp);
            if (it_s == list_num.end()) {
                return false;
            }
        }


        return return_val;
    }
    int charToInt(char temp) {
        return ((int)temp - 48);
    }
public:
    void print_board(vector<vector<char>>& board) {
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                cout << " " << board[row][colum] << " ";
            }
        cout << endl;
        }
    }
    void transposition(vector<vector<char>>& board) {
        vector<vector<char>> board_temp;
        vector<char> board_temp_temp;

        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board.size(); j++) {
                if (i == j) {
                    board_temp_temp.push_back('1');
                }
                else {
                    board_temp_temp.push_back('0');
                }
            }
            board_temp.push_back(board_temp_temp);
            board_temp_temp.erase(board_temp_temp.begin(), board_temp_temp.end());
        }

        print_board(board_temp);

        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board.size(); i++) {
                if (!(charToInt(board[i][j]) < 0 || charToInt(board[i][j]) > 9)) {
                    board[i][j] += board[j][i] * board_temp[i][j];
                } 
            }
        }

    }
    bool isValidSudoku(vector<vector<char>>& board) {
        bool return_val = true;

        for (int i = 0; i < board.size(); i++) {
            if (false == isNoRepeat(board[i])) {
                return false;
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
        exit(0);
    }
}

void check_point(vector<vector<char>>& board, bool expected_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "With sudoku below " << endl;

    mSolution->print_board(board);
    cout << endl;
    mSolution->transposition(board);
    cout << endl;
    mSolution->print_board(board);
    cout << endl;

    if (expected_val == mSolution->isValidSudoku(board)) {
        judment(true);
    } else {
        cout << "current value is: " << mSolution->isValidSudoku(board) << endl;
        cout << "expected value is: " << expected_val << endl;
        judment(false);
    }
    
    delete mSolution;
}

int main (void)
{
    vector<vector<char>> board;
    board.push_back({'5','3','.','.','7','.','.','.','.'});
    board.push_back({'6','.','.','1','9','5','.','.','.'});
    board.push_back({'.','9','8','.','.','.','.','6','.'});
    board.push_back({'8','.','.','.','6','.','.','.','3'});
    board.push_back({'4','.','.','8','.','3','.','.','1'});
    board.push_back({'7','.','.','.','2','.','.','.','6'});
    board.push_back({'.','6','.','.','.','.','2','8','.'});
    board.push_back({'.','.','.','4','1','9','.','.','5'});
    board.push_back({'.','.','.','.','8','.','.','7','9'});

    check_point(board, true);

    board.erase(board.begin(), board.end());
    board.push_back({'8','3','.','.','7','.','.','.','.'});
    board.push_back({'6','.','.','1','9','5','.','.','.'});
    board.push_back({'.','9','8','.','.','.','.','6','.'});
    board.push_back({'8','.','.','.','6','.','.','.','3'});
    board.push_back({'4','.','.','8','.','3','.','.','1'});
    board.push_back({'7','.','.','.','2','.','.','.','6'});
    board.push_back({'.','6','.','.','.','.','2','8','.'});
    board.push_back({'.','.','.','4','1','9','.','.','5'});
    board.push_back({'.','.','.','.','8','.','.','7','9'});
    
    check_point(board, false);

    return 0;
}