#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

class Solution {
private:
    enum ASCII_VALUE {
        ZERO_VALUE = 48
    };
    bool isNoRepeat(vector<vector<int>> my_vector) {
        bool return_val = true;

        map<unsigned int, bool> list_num;
        map<unsigned int, bool>::iterator it;

        for (int row = 0; row < my_vector.size(); row++) {
            for (int i = 1; i < 10; i++) {
                list_num[i] = false;
            }
            for (int colum = 0; colum < my_vector[row].size(); colum++) {
                if (0 == my_vector[row][colum]) {
                    continue;
                }
                for (it = list_num.begin(); it != list_num.end(); it++) {
                    if (my_vector[row][colum] == it->first) {
                        if (false == it->second) {
                            it->second = true;
                        }
                        else {
                            return false;
                        }
                    }
                }

            }
        }
        return return_val;
    }
    vector<vector<int>> converToInt(vector<vector<char>>& board) {
        int temp = 0;
        vector<vector<int>> return_vector;
        vector<int> return_vector_temp;
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                temp = (int)board[row][colum] - 48;
                if (temp < 0 || temp > 9) {
                    return_vector_temp.push_back(0);
                }
                else {
                    return_vector_temp.push_back(temp);
                }
            }
            return_vector.push_back(return_vector_temp);
            return_vector_temp.erase(return_vector_temp.begin(), return_vector_temp.end());
        }

        return return_vector;
    }
    vector<vector<char>> converToChar(vector<vector<int>>& board) {
        char temp = '.';
        vector<vector<char>> return_vector;
        vector<char> return_vector_temp;
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                temp = (char)((char)board[row][colum] + 48);
                if (ZERO_VALUE == temp) {
                    return_vector_temp.push_back('.');
                }
                else {
                    return_vector_temp.push_back(temp);
                }
            }
            return_vector.push_back(return_vector_temp);
            return_vector_temp.erase(return_vector_temp.begin(), return_vector_temp.end());
        }

        return return_vector;
    }
    vector<vector<int>> unitMatrix(unsigned int size) {
        vector<vector<int>> board_temp;
        vector<int> board_temp_temp;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    board_temp_temp.push_back(1);
                }
                else {
                    board_temp_temp.push_back(0);
                }
            }
            board_temp.push_back(board_temp_temp);
            board_temp_temp.erase(board_temp_temp.begin(), board_temp_temp.end());
        }
        return board_temp;
    }
    vector<vector<int>> matrixMultiplication(vector<vector<int>> matrixA, vector<vector<int>> matrixB) {
        vector<vector<int>> board_int_return;
        vector<int> board_int_return_temp;
        int temp2 = 0;

        for (int row = 0; row < matrixA.size(); row++) {
            for (int colum = 0; colum < matrixA.size(); colum++) {
                for (int i = 0; i < matrixA.size(); i++) {
                    temp2 += matrixA[i][row] * matrixB[colum][i];
                }
                board_int_return_temp.push_back(temp2);
                temp2 = 0;
            }
            board_int_return.push_back(board_int_return_temp);
            board_int_return_temp.erase(board_int_return_temp.begin(), board_int_return_temp.end());
        }
        return board_int_return;
    }
    vector<vector<int>> createInBlock(vector<vector<int>> vector_int) {
        vector<int> block0;
        vector<int> block1;
        vector<int> block2;
        vector<int> block3;
        vector<int> block4;
        vector<int> block5;
        vector<int> block6;
        vector<int> block7;
        vector<int> block8;
        vector<vector<int>> vector_return;

        for (int i = 0; i < 9; i++) {
            if (i < 3) {
                for (int j = 0; j < 3; j++) {
                    block0.push_back(vector_int[i][j]);
                    block1.push_back(vector_int[i][j + 3]);
                    block2.push_back(vector_int[i][j + 6]);
                }
            }
            else if (i < 6) {
                for (int j = 0; j < 3; j++) {
                    block3.push_back(vector_int[i][j]);
                    block4.push_back(vector_int[i][j + 3]);
                    block5.push_back(vector_int[i][j + 6]);
                }
            }
            else {
                for (int j = 0; j < 3; j++) {
                    block6.push_back(vector_int[i][j]);
                    block7.push_back(vector_int[i][j + 3]);
                    block8.push_back(vector_int[i][j + 6]);
                }
            }
        }

        vector_return.push_back(block0);
        vector_return.push_back(block1);
        vector_return.push_back(block2);
        vector_return.push_back(block3);
        vector_return.push_back(block4);
        vector_return.push_back(block5);
        vector_return.push_back(block6);
        vector_return.push_back(block7);
        vector_return.push_back(block8);

        return vector_return;
    }
    void transposition(vector<vector<int>>& board) {
        vector<vector<int>> board_int_return;
        vector<vector<int>> mUnitMatrix = unitMatrix(board.size());

        board = matrixMultiplication(board, mUnitMatrix);
    }
public:
    bool isValidSudoku(vector<vector<char>>& board) {
        bool return_val = true;
        vector<vector<int>> vector_int;
        vector<vector<int>> vector_in_block;

        vector_int = converToInt(board);
        //printBoardInt(vector_int);

        return_val = isNoRepeat(vector_int);
        if (false == return_val) {
            return false;
        }

        vector_in_block = createInBlock(vector_int);
        //printBoardInt(vector_in_block);

        return_val = isNoRepeat(vector_in_block);
        if (false == return_val) {
            return false;
        }

        transposition(vector_int);
        //printBoardInt(vector_int);

        return_val = isNoRepeat(vector_int);
        if (false == return_val) {
            return false;
        }

        return return_val;
    }
    void printBoardChar(vector<vector<char>>& board) {
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                cout << " " << board[row][colum] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    void printBoardInt(vector<vector<int>>& board) {
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                cout << " " << board[row][colum] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "This check point is PASS" << endl << endl;
    }
    else {
        cout << "This check point is FAIL" << endl << endl;
        exit(0);
    }
}

void check_point(vector<vector<char>>& board, bool expected_val) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "With sudoku below " << endl;

    mSolution->printBoardChar(board);

    bool current_val = mSolution->isValidSudoku(board);

    if (expected_val == current_val) {
        judment(true);
    }
    else {
        cout << "current value is: " << current_val << endl;
        cout << "expected value is: " << expected_val << endl;
        judment(false);
    }

    delete mSolution;
}

int main(void)
{
    vector<vector<char>> board;
    board.push_back({ '5','3','.','.','7','.','.','.','.' });
    board.push_back({ '6','.','.','1','9','5','.','.','.' });
    board.push_back({ '.','9','8','.','.','.','.','6','.' });
    board.push_back({ '8','.','.','.','6','.','.','.','3' });
    board.push_back({ '4','.','.','8','.','3','.','.','1' });
    board.push_back({ '7','.','.','.','2','.','.','.','6' });
    board.push_back({ '.','6','.','.','.','.','2','8','.' });
    board.push_back({ '.','.','.','4','1','9','.','.','5' });
    board.push_back({ '.','.','.','.','8','.','.','7','9' });

    check_point(board, true);

    board.erase(board.begin(), board.end());
    board.push_back({ '8','3','.','.','7','.','.','.','.' });
    board.push_back({ '6','.','.','1','9','5','.','.','.' });
    board.push_back({ '.','9','8','.','.','.','.','6','.' });
    board.push_back({ '8','.','.','.','6','.','.','.','3' });
    board.push_back({ '4','.','.','8','.','3','.','.','1' });
    board.push_back({ '7','.','.','.','2','.','.','.','6' });
    board.push_back({ '.','6','.','.','.','.','2','8','.' });
    board.push_back({ '.','.','.','4','1','9','.','.','5' });
    board.push_back({ '.','.','.','.','8','.','.','7','9' });

    check_point(board, false);

    return 0;
}
