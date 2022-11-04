#include <iostream>
#include<string>
#include <map>

#include <chrono>
#include <thread>

#include "cJSON/cJSON.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define step_per_degree 1600/360

double theta_1, theta_2, theta_3;
int step_1, step_2, step_3;

int gripper = 0;

float theta_1_parse, theta_2_parse, theta_3_parse;

cJSON* my_data_json;

void json_data_process(char* data);

double xx;
double yy;
double zz;

double x;
double y;
double z;

int main()
{
	char data_arr[200];
    for (int i = 0; i < 200; i++)
    {
        data_arr[i] = 0;
    }
    string string_data;

     for(int i = 0; i<1; i++)
     {
         theta_1 += 0.2;
         theta_2 += 0.2;
         theta_3 += 0.2;

         string_data = "{\"theta1\":\"" + to_string(theta_1) +
            "\",\"theta2\":\"" + to_string(theta_2) +
            "\",\"theta3\":\"" + to_string(theta_3) + "\",\"gripper\":\"" + to_string(gripper) + "\"}\n";

         int temp = 0;
         while(string_data[temp]!=NULL)
         {
            data_arr[temp] = string_data[temp];
            temp++;
         }

         sleep_for(microseconds(1));

         json_data_process(data_arr);
         
         cout<<string_data<<endl;
     }

    return 0;
}

void json_data_process(char* data)
{
    my_data_json = cJSON_Parse(data);

    if (cJSON_GetObjectItem(my_data_json, "theta1"))
    {
        theta_1_parse = atof(cJSON_GetObjectItem(my_data_json, "theta1")->valuestring);
        //cout<<"theta 1 is: "<<theta_1_parse<<endl;
        step_1 = theta_1_parse * step_per_degree;

    }

    if (cJSON_GetObjectItem(my_data_json, "theta2"))
    {
        theta_2_parse = atof(cJSON_GetObjectItem(my_data_json, "theta2")->valuestring);
        //cout<<"theta 2 is: "<<theta_2_parse<<endl;
        step_2 = theta_2_parse * step_per_degree;
    }

    if (cJSON_GetObjectItem(my_data_json, "theta3"))
    {
        theta_3_parse = atof(cJSON_GetObjectItem(my_data_json, "theta3")->valuestring);
        //cout<<"theta 3 is: "<<theta_3_parse<<endl;
        step_3 = theta_3_parse * step_per_degree;
    }

    
    if (cJSON_GetObjectItem(my_data_json, "gripper"))
    {
        if(atoi(cJSON_GetObjectItem(my_data_json, "gripper")->valuestring))
        {
            cout<<"gripper is: True"<<endl;
        }
        else
        {
            cout<<"gripper is: False"<<endl;
        }
    }

    cout<<"step 1 is: "<<step_1<<" step 2 is: "<<step_2<<" step 3 is: "<<step_3<<endl;
    //cout << "theta 1 is: " << theta_1_parse << " theta 2 is: " << theta_2_parse << " theta 3 is: " << theta_3_parse << endl;

    cJSON_Delete(my_data_json);
}
