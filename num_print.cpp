#include <bits/stdc++.h>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
void printstring(int i);

int main()
{
    string n_temp;
    string m_temp;

	string temp;

    getline(cin, temp);

	size_t pos = 0;

	for(int i=0;i<temp.length()+1;i++)
	{
		if(temp[i]==' ')
		{
			pos = i;
			break;
		}
	}

	if(pos==0)
	{
		n_temp = temp;
		getline(cin, m_temp);
	}
	else
	{
		n_temp = temp.substr(0, pos);
    	m_temp = temp.erase(0, pos + 1);
	}

    int n = stoi(ltrim(rtrim(n_temp)));
    int m = stoi(ltrim(rtrim(m_temp)));

    for(int i = n; i<= m; i++)
    {
        if(i>9)
        {
            if(i%2)
            {
                cout<<"odd"<<endl;
            }
            else
            {
                cout<<"even"<<endl;
            }
        }
        else
        {
			printstring(i);
        }
        
    }


    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}

void printstring(int i)
{
	        switch (i)
            {
        case 0:
            cout<<"zero"<<endl;
            break;
        case 1:
            cout<<"one"<<endl;
            break;
        case 2:
            cout<<"two"<<endl;
            break;
        case 3:
            cout<<"three"<<endl;
            break;
        case 4:
            cout<<"four"<<endl;
            break;
        case 5:
            cout<<"five"<<endl;
            break;
        case 6:
            cout<<"six"<<endl;
            break;
        case 7:
            cout<<"seven"<<endl;
            break;
        case 8:
            cout<<"eight"<<endl;
            break;
        case 9:
            cout<<"nine"<<endl;
            break;
        default:
            break;
            }
}
