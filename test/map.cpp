#include <iostream>
#include <map>

using namespace std;

//(key, value)
//insert
//count
//find
//erase

int main()
{
	map<int, int> mp;

	mp[1] = 2;
	mp[2] = 3;
	mp[5] = 2;
	mp[6] = 3;

	for(auto it : mp)
	{
		cout<<it.first<<" "<<it.second<<endl;
	}

	return 0;
}