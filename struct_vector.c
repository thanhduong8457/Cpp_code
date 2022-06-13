#include <stdio.h>


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
};

int main(void)
{
	my_data data;
	
	data.red = 1;
	data.green = 2;
	
	printf("%d\r\n",data.red);
	printf("%d\r\n",data.green);
	
	data.increase_red();
	
	printf("%d\r\n",data.red);
	printf("%d\r\n",data.green);
	

	return 0;
}