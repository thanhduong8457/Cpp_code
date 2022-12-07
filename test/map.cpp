#include <iostream>
#include <iterator>
#include <map>

using namespace std;

//(key, value)
//insert
//count
//find
//erase

int main(void)
{
	map<int, int> mp;
	map<int, int>::iterator itr;

	mp[1] = 2;
	mp[2] = 3;
	mp[5] = 2;
	mp[6] = 3;

	for(itr = mp.begin(); itr != mp.end(); itr++) {
		cout<<itr.first<<" "<<itr.second<<endl;
	}

	return 0;
}