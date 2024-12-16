#include <iostream>
#include <map>

#include "math.h"
#include <chrono>
#include <thread>
#include "vector"

#include "delta_define.h"
#include "delta_robot.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;      // nanoseconds, system_clock, seconds

#define sqrt3 1.732050808
#define pi 3.141592654
#define sin120 0.8660254038
#define cos120 -0.5
#define tan60 1.732050808
#define sin30 0.5
#define tan30 0.5773502692

#define ee 86.5  // endeffector
#define ff 346.4 // base
#define re 465   // endeffector arm
#define rf 200   // Base arm

#define mmtm 0.001
#define dtr (pi / 180)

double theta_1, theta_2, theta_3;
double x, y, z;
delta_robot *m_delta_robot;
double position_value[13];
double delta;


int gripper;
bool status;

int main() {
    double call_xo_2 = 0;
    double call_yo_2 = 0;
    double call_zo_2 = -375;
    double call_xf_2 = 50;
    double call_yf_2 = 50;
    double call_zf_2 = -400;

    double call_vmax_2 = 1500;
    double call_amax_2 = 200000;
    unsigned int num_point_1 = 120;
    unsigned int num_point_2 = 120;

    double dis, theta_y, theta_z;

    double rot_z[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    double rot_y[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    double rot_tras[4][4] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double position_value[13] = {};


    delta_robot *m_delta_robot = NULL;
    m_delta_robot = new delta_robot;

    //path_linear_speed(xx, yy, zz, x, y, z);
    m_delta_robot->system_linear(   call_xo_2, call_yo_2, call_zo_2, 
                                    call_xf_2, call_yf_2, call_zf_2, 
                                    dis, 
                                    rot_z, rot_y, 
                                    theta_y, theta_z, 
                                    rot_tras);

    // Trapezoidal velocity profile
    m_delta_robot->ls_v_a_total(0, dis, call_vmax_2, call_amax_2, num_point_1, num_point_2);

    // Reverse rotation end point, start point and trajectory #######
    m_delta_robot->system_linear_matrix(m_delta_robot->m_data_delta.size(), rot_z, rot_y, theta_y, theta_z, rot_tras);

    // // Inverse kinematics
    m_delta_robot->inverse_m();

    m_delta_robot->angulos_eulerianos(1, 
        m_delta_robot->m_data_delta[0]->pos_x, m_delta_robot->m_data_delta[0]->pos_y, m_delta_robot->m_data_delta[0]->pos_z,
        m_delta_robot->m_data_delta[0]->theta_1, m_delta_robot->m_data_delta[0]->theta_2, m_delta_robot->m_data_delta[0]->theta_3, 
        gripper, position_value);

    // for (unsigned int i = 0; i < m_delta_robot->m_data_delta.size(); i++) {
    //     m_delta_robot->m_data_delta[i]->theta_val = m_delta_robot->inverse(m_delta_robot->m_data_delta[i]->position_val);
    //     m_delta_robot->CreateJointStateList(m_delta_robot->m_data_delta[i]->position_val, m_delta_robot->m_data_delta[i]->theta_val, false, position_value);
    // }

    return 0;
}

