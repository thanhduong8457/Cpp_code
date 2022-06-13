#include <iostream>
#include <vector>
using namespace std;

typedef struct my_data
{
	int red;
	int green;
	
	void increase_red()
	{
		red++;
	}
	
	void increase_green()
	{
		red++;
	}
}my_data_t;

std::vector<my_data_t *> m_my_data;

void create_data(int red, int green)
{
	my_data_t *data = NULL;
	data = new my_data_t;
	data->red = red;
	data->green = green;
	m_my_data.push_back(data);
}

int main(void)
{
	create_data(1,2);
<<<<<<< HEAD
	create_data(5,5);
	create_data(100,100);
	
	m_my_data[1]->increase_red();

	for(int i = 0;i<m_my_data.size();i++)
	{
		cout<<"index: "<<i<<endl;
		cout<<"red: "<<m_my_data[i]->red<<endl;
		cout<<"green: "<<m_my_data[i]->green<<endl<<endl;
	}

	delete [] m_my_data[1];
	m_my_data.erase(m_my_data.begin()+1);

	for(int i = 0;i<m_my_data.size();i++)
	{
		cout<<"index: "<<i<<endl;
		cout<<"red: "<<m_my_data[i]->red<<endl;
		cout<<"green: "<<m_my_data[i]->green<<endl<<endl;
	}
=======
	// create_data(5,5);
	// create_data(100,100);

	// for(int i = 0;i<m_my_data.size();i++)
	// {
	// 	cout<<"index: "<<i<<endl;
	// 	cout<<"red: "<<m_my_data[i]->red<<endl;
	// 	cout<<"green: "<<m_my_data[i]->green<<endl<<endl;
	// }

	// delete [] m_my_data[1];
	// m_my_data.erase(m_my_data.begin()+1);

	// for(int i = 0;i<m_my_data.size();i++)
	// {
	// 	cout<<"index: "<<i<<endl;
	// 	cout<<"red: "<<m_my_data[i]->red<<endl;
	// 	cout<<"green: "<<m_my_data[i]->green<<endl<<endl;
	// }
	// cout<<m_my_data.size()<<endl;
	
	cout<<m_my_data.size()<<endl;

	delete [] m_my_data[0];
	m_my_data.erase(m_my_data.begin());

>>>>>>> 4c959ecfb11eba07a060dffc432d25be6c9f68c0
	cout<<m_my_data.size()<<endl;

	return 0;
}