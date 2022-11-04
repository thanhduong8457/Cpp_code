#include <iostream>
#include <map>

#include "math.h"
#include <chrono>
#include <thread>
#include "vector"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define sqrt3   1.732050808
#define pi      3.141592654
#define sin120  0.8660254038
#define cos120  -0.5
#define tan60   1.732050808
#define sin30   0.5
#define tan30   0.5773502692

#define ee      86.5       // endeffector 
#define ff      346.4      // base
#define re      465        // endeffector arm
#define rf      200        // Base arm

#define mmtm    0.001
#define dtr     (pi/180)

double theta_1, theta_2, theta_3;
double x, y, z;

double delta_calcAngleYZ(double x0, double y0, double z0);
void delta_calcInverse(double x0, double y0, double z0, double &theta1, double &theta2, double &theta3);
void delta_calcForward(double theta1, double theta2, double theta3, double &x0, double &y0, double &z0);
void test_direct();
void test_inverse();

int main()
{
	test_inverse();

	// test_direct();

	// theta_1 = 18.0275;
    // theta_2 = -4.72167;
    // theta_3 = 12.1526;

	// delta_calcForward(theta_1, theta_2, theta_3, x, y, z);
	// cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;

    x = 0;
    y = 0;
    z = -375;

    delta_calcInverse(x, y, z, theta_1, theta_2, theta_3);
	cout<<"theta_1: "<<theta_1<<" theta_2: "<<theta_2<<" theta_3: "<<theta_3<<endl;

	return 0;
}

void test_inverse()
{
	int point = 1000;

	double x0 = 0;
    double y0 = 0;
    double z0 = -375;
    
    double x1 = 50;
    double y1 = 50;
    double z1 = -400;

	double denta_x = (x1 - x0)/point;
    double denta_y = (y1 - y0)/point;
    double denta_z = (z1 - z0)/point;

	for(int i = 0; i <= point; i++)
	{
		x0 = x0 + denta_x;
        y0 = y0 + denta_y;
        z0 = z0 + denta_z;

		delta_calcInverse(x0, y0, z0, theta_1, theta_2, theta_3);

		cout<<"theta_1: "<<theta_1<<" theta_2: "<<theta_2<<" theta_3: "<<theta_3<<endl;
		sleep_for(milliseconds(10));
	}
}

void test_direct()
{
	int point = 1000;

	double theta1 = 0;
    double theta2 = 0;
    double theta3 = 0;
    
    double theta1_2 = 30;
    double theta2_2 = 40;
    double theta3_2 = 50;

	double denta_x = (theta1_2 - theta1)/point;
    double denta_y = (theta2_2 - theta2)/point;
    double denta_z = (theta3_2 - theta3)/point;

	for(int i = 0; i <= point; i++)
	{
		theta1 = theta1 + denta_x;
        theta2 = theta2 + denta_y;
        theta3 = theta3 + denta_z;

		delta_calcForward(theta1, theta2, theta3, x, y, z);

		cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
		sleep_for(milliseconds(10));
	}
}

double delta_calcAngleYZ(double x0, double y0, double z0)
{
    double y1 = -0.5 * tan30 * ff; // f/2 * tg 30
    y0 -= 0.5 * tan30 * ee;    // shift center to edge
                                 // z = a + b*y
    double a = (x0 * x0 + y0 * y0 + z0 * z0 + rf * rf - re * re - y1 * y1) / (2 * z0);
    double b = (y1 - y0) / z0;

    // discriminant
    double d = -(a + b * y1) * (a + b * y1) + rf * (b * b * rf + rf);
    if (d < 0) return -1; // non-existing point
    double yj = (y1 - a * b - sqrt(d)) / (b * b + 1); // choosing outer point
    double zj = a + b * yj;
    double theta = 180.0 * atan(-zj / (y1 - yj)) / pi + ((yj > y1) ? 180.0 : 0.0);
    return theta;
}

// inverse kinematics: (x0, y0, z0) -> (theta1, theta2, theta3)
// returned status: 0=OK, -1=non-existing position
void delta_calcInverse(double x0, double y0, double z0, double &theta1, double &theta2, double &theta3)
{
    theta1 = delta_calcAngleYZ(x0, y0, z0);
    theta2 = delta_calcAngleYZ(x0 * cos120 + y0 * sin120, y0 * cos120 - x0 * sin120, z0);  // rotate  to +119 deg
    theta3 = delta_calcAngleYZ(x0 * cos120 - y0 * sin120, y0 * cos120 + x0 * sin120, z0);  // rotate to -120 deg
}

void delta_calcForward(double theta1, double theta2, double theta3, double &x0, double &y0, double &z0)
{
    double t = (ff - ee) * tan30 / 2;

    theta1 *= dtr;
    theta2 *= dtr;
    theta3 *= dtr;

    double y1 = -(t + rf * cos(theta1));
    double z1 = -rf * sin(theta1);

    double y2 = (t + rf * cos(theta2)) * sin30;
    double x2 = y2 * tan60;
    double z2 = -rf * sin(theta2);

    double y3 = (t + rf * cos(theta3)) * sin30;
    double x3 = -y3 * tan60;
    double z3 = -rf * sin(theta3);

    double dnm = (y2 - y1) * x3 - (y3 - y1) * x2;

    double w1 = y1 * y1 + z1 * z1;
    double w2 = x2 * x2 + y2 * y2 + z2 * z2;
    double w3 = x3 * x3 + y3 * y3 + z3 * z3;

    // x = (a1*z + b1)/dnm
    double a1 = (z2 - z1) * (y3 - y1) - (z3 - z1) * (y2 - y1);
    double b1 = -((w2 - w1) * (y3 - y1) - (w3 - w1) * (y2 - y1)) / 2.0;

    // y = (a2*z + b2)/dnm;
    double a2 = -(z2 - z1) * x3 + (z3 - z1) * x2;
    double b2 = ((w2 - w1) * x3 - (w3 - w1) * x2) / 2.0;

    // a*z^2 + b*z + c = 0
    double a = a1 * a1 + a2 * a2 + dnm * dnm;
    double b = 2 * (a1 * b1 + a2 * (b2 - y1 * dnm) - z1 * dnm * dnm);
    double c = (b2 - y1 * dnm) * (b2 - y1 * dnm) + b1 * b1 + dnm * dnm * (z1 * z1 - re * re);

    // discriminant
    double d = b * b - (double)4.0 * a * c;
    if (d < 0)
    {
		return; // non-existing point
    }

    z0 = -0.5 * (b + sqrt(d)) / a;
    x0 = (a1 * z0 + b1) / dnm;
    y0 = (a2 * z0 + b2) / dnm;
}
