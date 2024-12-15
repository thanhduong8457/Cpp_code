#ifndef __DELTA_ROBOT__
#define __DELTA_ROBOT__

#include "common.h"

class delta_robot{
private:
    double CalculateAngleYZ(Point);
    Point unit_vector(Point point0, Point pointf);
    void angle_rotation(Point unit_vector);
    double delta_calcAngleYZ(double x0, double y0, double z0);
    double angle_yz(Point point0);
    void system_linear_invese(double xprima, double(&xyz_res)[4]);

public:
    double vmax;
    double amax;

    unsigned int num_point_1;
    unsigned int num_point_2;

    double rot_z[3][3];
    double rot_y[3][3];
    double rot_tras[4][4];
    double dis, theta_y, theta_z;

    Point mStartPoint;
    Point mEndPoint;
    
    data_delta_t *data = NULL;
    std::vector<data_delta_t*> m_data_delta;

    delta_robot(void);
    ~delta_robot(void);

    void initialize(void);

    //void path_linear_speed(double xo, double yo, double zo, double xf, double yf, double zf);
    Theta inverse(Point point0);

    void TrapezoidalVelocityProfile(void);
    void ls_v_a_puntual(double q0, double q1, double tactual, double &q_actual, double &v_actual, double &a_actual);

    void system_linear(void);
    void system_linear_matrix(void);
    void CreateJointStateList(Point point, Theta theta, int gripper, double(&position)[13]);
    void InverseAllJointStateExist(void);
};
#endif
