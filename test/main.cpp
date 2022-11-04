#include <iostream>
#include <map>
#include <string>

using namespace std;

class A
{
	int a, b;
public:
	virtual int add(int a, int b)
	{
		cout<<"class A:"<<a+b<<endl;
		return a+b;
	}
};

class B: public A
{
	public:
	int add(int a, int b)
	{
		cout<<"class B:"<<a+b<<endl;
		return a+b;
	}
};

int main()
{
	B *a;
	a = new B;

	delete a;
	delete a;
	
	return 0;
}