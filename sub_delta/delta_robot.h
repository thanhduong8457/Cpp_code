#ifndef __DELTA_ROBOT__
#define __DELTA_ROBOT__

#include "delta_define.h"
#include "codos.h"

typedef struct data_delta {
    double pos;     //
    double pos_x;
    double pos_y;
    double pos_z;

    double vel;     //
    //double vel_x;
    //double vel_y;
    //double vel_z;

    double acel;    //
    //double acel_x;
    //double acel_y;
    //double acel_z;

    double theta_1;
    double theta_2;
    double theta_3;

    double tiempo;  //
}data_delta_t;

class delta_robot{
private:
    double delta_calcAngleYZ(double x0, double y0, double z0);
    void unit_vector(double xo, double yo, double zo, double xf, double yf, double zf, double& nx, double& ny, double& nz);
    void angulos_rot(double nx, double ny, double nz, double& theta_y, double& theta_z);
    int inverse(double x0, double y0, double z0, double& theta1, double& theta2, double& theta3);
    int angle_yz(double x0, double y0, double z0, double& theta);
    void system_linear_inv(double xprima, double rot_z[3][3], 
    double rot_y[3][3], double theta_y, double theta_z, double m_trans[4][4], double(&xyz_res)[4][1]);

public:
    std::vector<data_delta_t*> m_data_delta;

    delta_robot(void);
    ~delta_robot(void);

    //void path_linear_speed(double xo, double yo, double zo, double xf, double yf, double zf);
    //void system_linear(double xo, double yo, double zo, double xf, double yf, double zf);
    void ls_v_a_total(double q0, double q1, double vmax, double amax, int pas_1, int pas_2);
    void ls_v_a_puntual(double q0, double q1, double vmax, double amax, double tactual,
        double &q_actual, double &v_actual, double &a_actual);

    void system_linear(double xo, double yo, double zo, double xf, double yf, double zf,
        double& dis, double(&rot_z)[3][3], double(&rot_y)[3][3], double& theta_y, double& theta_z, double(&rot_tras)[4][4]);

    void system_linear_matrix(double tiempo, double rot_z[3][3], double rot_y[3][3], 
        double theta_y, double theta_z, double m_trans[4][4]);
    void delta_calcInverse(double x0, double y0, double z0, double& theta1, double& theta2, double& theta3);
    void delta_calcForward(double theta1, double theta2, double theta3, double& x0, double& y0, double& z0);
    void angulos_eulerianos(double ti, double xi, double yi, double zi,
        double th1, double th2, double th3, int gripper, double(&position)[13]);

    void inverse_m(void);
};
#endif
