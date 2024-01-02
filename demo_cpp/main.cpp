#include <iostream>
#include <string>
#include <vector>

using namespace std;

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

int main (void) 
{
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << "with i = " << i << " and j = " << j << " then in block "<<  atWhichInBlock(i, j) << endl;
		}
		cout << endl;
	}

   return 0;
}