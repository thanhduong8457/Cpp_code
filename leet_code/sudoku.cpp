#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

//#define CASE_CHECK_VALID
#define ENABLE_DEBUG

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
    vector<vector<int>> transfromFromBlock(vector<vector<int>> vector_int) {
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
    void printBoardInt(vector<vector<int>>& board) {
        for (int row = 0; row < board.size(); row++) {
            for (int colum = 0; colum < board[row].size(); colum++) {
                cout << " " << board[row][colum] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    unsigned int atWhichInBlock(const unsigned int row, const unsigned int colum) {
        unsigned int return_val = 0;
        if (row < 3) {
            return_val = 0;
        }
        else if (row < 6) {
            return_val = 3;
        }
        else {
            return_val = 6;
        }
        if (colum < 3) {
            return_val += 0;
        }
        else if (colum < 6) {
            return_val += 1;
        }
        else {
            return_val += 2;
        }
        return return_val;
    }
public:
    bool isValidSudoku(vector<vector<char>>& board) {
        bool return_val = true;
        vector<vector<int>> boardConvertToInt;
        vector<vector<int>> boardTransposition;
        vector<vector<int>> boardTransfromFromBlock;

        boardConvertToInt = converToInt(board);
#ifdef ENABLE_DEBUG
        cout << "matrix after convert member to int" << endl;
        printBoardInt(boardConvertToInt);
#endif
        return_val = isNoRepeat(boardConvertToInt);
        if (false == return_val) {
            return return_val;
        }

        boardTransfromFromBlock = transfromFromBlock(boardConvertToInt);
#ifdef ENABLE_DEBUG
        cout << "matrix after group member from block" << endl;
        printBoardInt(boardTransfromFromBlock);
#endif
        return_val = isNoRepeat(boardTransfromFromBlock);
        if (false == return_val) {
            return return_val;
        }

        boardTransposition = converToInt(board);
        transposition(boardTransposition);
#ifdef ENABLE_DEBUG
        cout << "matrix after transposition" << endl;
        printBoardInt(boardTransposition);
#endif
        return_val = isNoRepeat(boardTransposition);
        if (false == return_val) {
            return return_val;
        }
#ifdef ENABLE_DEBUG
        cout << "######## this sudoku valid ########" << endl;
#endif
        return return_val;
    }
    void solveSudoku(vector<vector<char>>& board) {
        if (false == isValidSudoku(board)) {
            return;
        }
        vector<vector<int>> boardConvertToInt;
        vector<vector<int>> boardConvertToIntBackup;
        vector<vector<int>> boardTransfromFromBlock;

        map<int, vector<int>> allValueAvailable;
        map<int, vector<int>> twoValueAvailable;
        map<int, vector<int>>::iterator itValAva;

        map<int, bool> numArray;
        map<int, bool>::iterator itr;

        map<int, vector<int>> backupVal;

        boardConvertToInt = converToInt(board);
        boardTransfromFromBlock = transfromFromBlock(boardConvertToInt);

        bool isContinueCaculate = true;
        while (isContinueCaculate) {
            isContinueCaculate = false;
            for (int row = 0; row < boardConvertToInt.size(); row++) {
                for (int colum = 0; colum < boardConvertToInt[row].size(); colum++) {
                    if (0 != boardConvertToInt[row][colum]) {
                        continue;
                    }
                    else {
                        for (int i = 1; i < 10; i++) {
                            numArray[i] = false;
                        }
                        for (int i = 0; i < boardConvertToInt.size(); i++) {
                            if (0 == boardConvertToInt[row][i]) {
                                continue;
                            }
                            else {
                                numArray[boardConvertToInt[row][i]] = true;
                            }
                        }
                        for (int i = 0; i < boardConvertToInt.size(); i++) {
                            if (0 == boardConvertToInt[i][colum]) {
                                continue;
                            }
                            else {
                                numArray[boardConvertToInt[i][colum]] = true;
                            }
                        }
                        for (int i = 0; i < boardTransfromFromBlock.size(); i++) {
                            int fixed = atWhichInBlock(row, colum);
                            if (0 == boardTransfromFromBlock[fixed][i]) {
                                continue;
                            }
                            else {
                                numArray[boardTransfromFromBlock[fixed][i]] = true;
                            }
                        }
                        for (itr = numArray.begin(); itr != numArray.end(); itr++) {
                            if (false == itr->second) {
                                allValueAvailable[row + colum * 10].push_back(itr->first);
                            }
                        }
                    }
                }
            }

            for (itValAva = allValueAvailable.begin(); itValAva != allValueAvailable.end(); itValAva++) {
                int row = itValAva->first % 10;
                int colum = (itValAva->first - row) / 10;
#ifdef ENABLE_DEBUG
                cout << "with [row][colum] = [" << row << "][" << colum << "]" << " in block " << atWhichInBlock(row, colum) << " value available is: ";
                for (int i = 0; i < itValAva->second.size(); i++) {
                    cout << itValAva->second[i] << " ";
                }
                cout << endl;
#endif
                if (1 == itValAva->second.size()) {
                    boardConvertToInt[row][colum] = itValAva->second[0];
                    isContinueCaculate = true;
                    board = converToChar(boardConvertToInt);
#ifdef ENABLE_DEBUG
                    cout << "the value available in [row][colum] = [" << row << "][" << colum << "] is unique -> update it to board " << endl;
#endif
                    boardTransfromFromBlock = transfromFromBlock(boardConvertToInt);
                    twoValueAvailable.erase(twoValueAvailable.begin(), twoValueAvailable.end());
                }
                else if (2 == itValAva->second.size()) {
                    twoValueAvailable[itValAva->first] = itValAva->second;
                }
            }

            if (false == isContinueCaculate && twoValueAvailable.size() != 0) {
                // backup board 
                //boardConvertToIntBackup = boardConvertToInt;

                // calculate all cases available of two values for each position whose store in twoValueAvailable map variable

                // with one by one case -> continue to update sudoku table

                if (true == isContinueCaculate) {
                    break;
                }
            }
            allValueAvailable.erase(allValueAvailable.begin(), allValueAvailable.end());
#ifdef ENABLE_DEBUG
            cout << "board after update" << endl;
            printBoardChar(board);
            cout << endl;
#endif
        }

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
};

void judment(bool is_pass) {
    if (true == is_pass) {
        cout << "####################################" << endl;
        cout << "##### This check point is PASS #####" << endl;
        cout << "####################################" << endl << endl;
    }
    else {
        cout << "####################################" << endl;
        cout << "##### This check point is FAIL #####" << endl;
        cout << "####################################" << endl << endl;
        exit(0);
    }
}
void isValidSudoku(vector<vector<char>>& board, bool expected_val) {
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
void solveSudoku(vector<vector<char>>& board, vector<vector<char>>& expected_board) {
    Solution *mSolution = NULL;
    mSolution = new Solution();

    cout << "With sudoku below " << endl;
    mSolution->printBoardChar(board);

    mSolution->solveSudoku(board);

    if (expected_board == board) {
        judment(true);
    }
    else {
        cout << "current value is: " << endl;
        mSolution->printBoardChar(board);

        cout << "expected value is: " << endl;
        mSolution->printBoardChar(expected_board);
        judment(false);
    }

    delete mSolution;
}

int main(void)
{
    vector<vector<char>> board;
#ifdef CASE_CHECK_VALID
    board.push_back({ '5','3','.','.','7','.','.','.','.' });
    board.push_back({ '6','.','.','1','9','5','.','.','.' });
    board.push_back({ '.','9','8','.','.','.','.','6','.' });
    board.push_back({ '8','.','.','.','6','.','.','.','3' });
    board.push_back({ '4','.','.','8','.','3','.','.','1' });
    board.push_back({ '7','.','.','.','2','.','.','.','6' });
    board.push_back({ '.','6','.','.','.','.','2','8','.' });
    board.push_back({ '.','.','.','4','1','9','.','.','5' });
    board.push_back({ '.','.','.','.','8','.','.','7','9' });

    isValidSudoku(board, true);

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

    isValidSudoku(board, false);
#else
    vector<vector<char>> expected_board;

    //checkpoint 1

    board.erase(board.begin(), board.end());
    board.push_back({ '5','3','.','.','7','.','.','.','.' });
    board.push_back({ '6','.','.','1','9','5','.','.','.' });
    board.push_back({ '.','9','8','.','.','.','.','6','.' });
    board.push_back({ '8','.','.','.','6','.','.','.','3' });
    board.push_back({ '4','.','.','8','.','3','.','.','1' });
    board.push_back({ '7','.','.','.','2','.','.','.','6' });
    board.push_back({ '.','6','.','.','.','.','2','8','.' });
    board.push_back({ '.','.','.','4','1','9','.','.','5' });
    board.push_back({ '.','.','.','.','8','.','.','7','9' });

    expected_board.erase(expected_board.begin(), expected_board.end());
    expected_board.push_back({ '5','3','4','6','7','8','9','1','2' });
    expected_board.push_back({ '6','7','2','1','9','5','3','4','8' });
    expected_board.push_back({ '1','9','8','3','4','2','5','6','7' });
    expected_board.push_back({ '8','5','9','7','6','1','4','2','3' });
    expected_board.push_back({ '4','2','6','8','5','3','7','9','1' });
    expected_board.push_back({ '7','1','3','9','2','4','8','5','6' });
    expected_board.push_back({ '9','6','1','5','3','7','2','8','4' });
    expected_board.push_back({ '2','8','7','4','1','9','6','3','5' });
    expected_board.push_back({ '3','4','5','2','8','6','1','7','9' });

    solveSudoku(board, expected_board);

    //checkpoint 2

    board.erase(board.begin(), board.end());
    board.push_back({ '.','.','9','7','4','8','.','.','.' });
    board.push_back({ '7','.','.','.','.','.','.','.','.' });
    board.push_back({ '.','2','.','1','.','9','.','.','.' });
    board.push_back({ '.','.','7','.','.','.','2','4','.' });
    board.push_back({ '.','6','4','.','1','.','5','9','.' });
    board.push_back({ '.','9','8','.','.','.','3','.','.' });
    board.push_back({ '.','.','.','8','.','3','.','2','.' });
    board.push_back({ '.','.','.','.','.','.','.','.','6' });
    board.push_back({ '.','.','.','2','7','5','9','.','.' });

    expected_board.erase(expected_board.begin(), expected_board.end());
    expected_board.push_back({ '5','1','9','7','4','8','6','3','2' });
    expected_board.push_back({ '7','8','3','6','5','2','4','1','9' });
    expected_board.push_back({ '4','2','6','1','3','9','8','7','5' });
    expected_board.push_back({ '3','5','7','9','8','6','2','4','1' });
    expected_board.push_back({ '2','6','4','3','1','7','5','9','8' });
    expected_board.push_back({ '1','9','8','5','2','4','3','6','7' });
    expected_board.push_back({ '9','7','5','8','6','3','1','2','4' });
    expected_board.push_back({ '8','3','2','4','9','1','7','5','6' });
    expected_board.push_back({ '6','4','1','2','7','5','9','8','3' });

    solveSudoku(board, expected_board);

    //checkpoint 3

    board.erase(board.begin(), board.end());
    board.push_back({ '.','.','.','2','.','.','.','6','3' });
    board.push_back({ '3','.','.','.','.','5','4','.','1' });
    board.push_back({ '.','.','1','.','.','3','9','8','.' });
    board.push_back({ '.','.','.','.','.','.','.','9','.' });
    board.push_back({ '.','.','.','5','3','8','.','.','.' });
    board.push_back({ '.','3','.','.','.','.','.','.','.' });
    board.push_back({ '.','2','6','3','.','.','5','.','.' });
    board.push_back({ '5','.','3','7','.','.','.','.','8' });
    board.push_back({ '4','7','.','.','.','1','.','.','.' });

    expected_board.erase(expected_board.begin(), expected_board.end());
    expected_board.push_back({ '8','5','4','2','1','9','7','6','3' });
    expected_board.push_back({ '3','9','7','8','6','5','4','2','1' });
    expected_board.push_back({ '2','6','1','4','7','3','9','8','5' });
    expected_board.push_back({ '7','8','5','1','2','6','3','9','4' });
    expected_board.push_back({ '6','4','9','5','3','8','1','7','2' });
    expected_board.push_back({ '1','3','2','9','4','7','8','5','6' });
    expected_board.push_back({ '9','2','6','3','8','4','5','1','7' });
    expected_board.push_back({ '5','1','3','7','9','2','6','4','8' });
    expected_board.push_back({ '4','7','8','6','5','1','2','3','9' });

    solveSudoku(board, expected_board);

#endif
    return 0;
}
