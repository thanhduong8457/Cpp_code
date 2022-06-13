#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

int main()
{
	string temp;
	getline(cin, temp);
	int n = stoi(temp);
	int array[n];

	for(int i=0; i<n+1; i++)
	{
		array[i] = 0;
	}

	string my_str;
	getline(cin, my_str);

	int m = my_str.length();

    char str[m+1];

	strcpy(str, my_str.c_str());
 
    // Returns first token
    char *token = strtok(str, " ");
   
    // Keep printing tokens while one of the
    // delimiters present in str[].
	int i = 0;
    while (token != NULL)
    {
        //printf("%s\n", token);
		array[i++] = stoi(token);
        token = strtok(NULL, " ");
    }

	for (int i = n - 1; i >= 0; i--)
	{
		cout << array[i];
		if (i>0) cout << " ";
		else cout << endl;
	}
 
    return 0;
}