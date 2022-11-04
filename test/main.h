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

typedef struct data_delta
{
    double pos;     //
    double pos_x;
    double pos_y;
    double pos_z;
    double vel;     //
    double vel_x;
    double vel_y;
    double vel_z;
    double acel;    //
    double acel_x;
    double acel_y;
    double acel_z;
    double tiempo;  // 
}data_delta_t;

std::vector<data_delta_t *> m_data_delta;

void unit_vector(double xo,double yo,double zo,double xf,double yf,double zf, double &nx, double &ny, double &nz)
{
    double delta_x = xf - xo;
    double delta_y = yf - yo;
    double delta_z = zf - zo;

    double modulo = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);

    nx = delta_x / modulo;
    ny = delta_y / modulo;
    nz = delta_z / modulo;
}

void angulos_rot(double nx,double ny,double nz,double &theta_y,double &theta_z){
    if (nz < 0) theta_z = (360 * dtr) + asin(nz);
    else    theta_z = asin(nz);

    if (nx < 0)   theta_y = (180 * dtr) + (atan(ny / nx));
    else if (nx == 0)
    {
        if (ny < 0) theta_y = (270 * dtr);
        else if (ny == 0)   theta_y = 0;
        else    theta_y = (90 * dtr);
    }
    else
    {
        if (ny < 0) theta_y = (360 * dtr) + (atan(ny / nx));
        else    theta_y = (atan(ny / nx));
    }
}

void system_linear(double xo, double yo, double zo, double xf, double yf, double zf)  // ,theta_z,theta_y) :
{
    // // ######  Angles theta-y theta-z  ######
    // double nx,ny,nz;
    // unit_vector(0, 0, 0, xf-xo, yf-yo, zf-zo, nx, ny, nz);

    // double theta_y, theta_z;
    // angulos_rot(nx, ny, nz, theta_y, theta_z);

    // double cos_axisz = cos(theta_y);
    // double sin_axisz = sin(theta_y);

    // double cos_axisy = cos(theta_z);
    // double sin_axisy = sin(theta_z);

    // // ######  Rotacion Z  ######
    // // # Plano xy ya trasladado
    // pf[0][0] = x_trans[0][0];
    // pf[1][0] = x_trans[1][0];
    // pf[2][0] = x_trans[2][0];
    // // # Rotacion respecto a eje Z
    // rot_z[0][0] = cos_axisz;
    // rot_z[0][1] = sin_axisz;
    // rot_z[1][0] = -1 * sin_axisz;
    // rot_z[1][1] = cos_axisz;
    // rot_z[2][2] = 1;

    // m_rot1 = rot_z.dot(pf);

    // // ######  Rotacion Y  ######
    // // # Rotacion respecto a eje Y
    // rot_y[0][0] = cos_axisy;
    // rot_y[0][2] = sin_axisy;
    // rot_y[1][1] = 1;
    // rot_y[2][0] = -sin_axisy;
    // rot_y[2][2] = cos_axisy;

    // m_rot2 = rot_y.dot(m_rot1);

    // //return [(m_rot2[0][0]) * mtmm, rot_z, rot_y, theta_y * rtd, theta_z * rtd, rot_tras]
}

void path_linear_speed(double xo,double yo,double zo,double xf,double yf,double zf)
{
//     //######  International units change  ##########
//   double xoo = xo * mmtm;
//   double yoo = yo * mmtm;
//   double zoo = zo * mmtm;
//   double xff = xf * mmtm;
//   double yff = yf * mmtm;
//   double zff = zf * mmtm;

//   system_linear(xoo, yoo, zoo, xff, yff, zff);

}

void ls_v_a_puntual(double q0, double q1, int vmax, int amax, int tactual, 
                    double &q_actual, double &v_actual, double &a_actual,
                    double &tramo1, double &tramo2, double &tramo3)
{
    double tau = vmax/amax;

    int s;
    if(q1-q0>0)   s = 1;
    else if(q1-q0<0)    s = -1;

    double T = (s * (((q1) - (q0)) / (vmax))) + (tau);
 
    double tramo1 = tau;
    double tramo2 = T - tau;
    double tramo3 = T;

    if ((0 <= tactual) && (tactual <= tramo1))
    {
        q_actual = (q0) + ((s) * (amax / 2) * (tactual*tactual));
        v_actual = s * amax * tactual;
        a_actual = s * amax;
    }
    else if ((tramo1 < tactual) && (tactual <= tramo2))
    {
        
        q_actual = (q0) - ((s) * ((vmax*vmax) / (2 * amax))) + (s * vmax * tactual);
        v_actual = s * vmax;
        a_actual = 0;
    }
    else if ((tramo2 < tactual) && (tactual <= tramo3))
    {
        q_actual = (q1) + ((s) * ((-1 * ((amax * (T*T)) / (2))) + (amax * T * tactual) + (-1 * (amax / 2) * (tactual*tactual))));
        v_actual = s * ((amax * T) - (amax * tactual));
        a_actual = s * (-1 * amax);
    }
    else
    {
        q_actual = 0;
        v_actual = 0;
        a_actual = 0;
    }
}

void ls_v_a_total(double q0,double q1,double vmax,double amax,int pas_1,int pas_2)
{
    q0 *= mmtm;
    q1 *= mmtm;
    vmax *= mmtm;
    amax *= mmtm;

    if (pas_1 <= 0)    pas_1 = 2;
    if (pas_2 <= 0)    pas_2 = 2;

    double tau = vmax/amax;

    double T;
    if(q1-q0>0)   T = (q1 - q0)/vmax + tau;
    else if(q1-q0<0)    T = (q0 - q1)/vmax + tau;


    double paso1 = tau/pas_1;
    double paso2 = (T - (2 * tau)) / pas_2;

    int pas_total = pas_1 + pas_2 + pas_1 + 1;

    double Tf = 2 * (sqrt(((q1) - (q0)) / (amax)));
    double vmax_acel = amax * (Tf / 2);

    if (vmax_acel <= vmax){
        vmax = vmax_acel;
        tau = Tf/2;
        T = Tf;
        paso1 = tau/pas_1;
        paso2 = 0;
        pas_2 = 0;
        pas_total = pas_1 + pas_2 + pas_1 + 1;
    }

    data_delta_t *data = NULL;
    data = new data_delta_t;

    for(int i=0;i<pas_total;i++)
    { 
	    data->acel = 0;
	    data->pos = 0;
        data->vel = 0;
        data->tiempo = 0;
	    m_data_delta.push_back(data);
    }

    double ti = -1 * paso1;

    double q_actual,v_actual,a_actual,tramo1,tramo2,tramo3;

    for(int i = 0; i<=pas_1 + 1; i++){
        ti = ti + paso1;
        ls_v_a_puntual(q0,q1,vmax,amax,ti,q_actual,v_actual,a_actual,tramo1,tramo2,tramo3);
        m_data_delta[i]->pos = q_actual;
        m_data_delta[i]->vel = v_actual;
        m_data_delta[i]->acel = a_actual;
        m_data_delta[i]->tiempo = ti;
    }
  
    for(int i = pas_1 + 1; i<=pas_1 + pas_2 + 1; i++){
        ti = ti + paso2;
        ls_v_a_puntual(q0,q1,vmax,amax,ti,q_actual,v_actual,a_actual,tramo1,tramo2,tramo3);
        m_data_delta[i]->pos = q_actual;
        m_data_delta[i]->vel = v_actual;
        m_data_delta[i]->acel = a_actual;
        m_data_delta[i]->tiempo = ti;
    }

    for(int i = pas_1 + pas_2 + 1; i<=pas_1 + pas_2 + pas_1; i++){
        ti = ti + paso1;
        ls_v_a_puntual(q0,q1,vmax,amax,ti,q_actual,v_actual,a_actual,tramo1,tramo2,tramo3);
        m_data_delta[i]->pos = q_actual;
        m_data_delta[i]->vel = v_actual;
        m_data_delta[i]->acel = a_actual;
        m_data_delta[i]->tiempo = ti;
    }

    for(int i = pas_1 + pas_2 + pas_1; i<=pas_1 + pas_2 + pas_1 + 1; i++){
        ti = T;
        ls_v_a_puntual(q0,q1,vmax,amax,ti,q_actual,v_actual,a_actual,tramo1,tramo2,tramo3);
        m_data_delta[i]->pos = q_actual;
        m_data_delta[i]->vel = v_actual;
        m_data_delta[i]->acel = a_actual;
        m_data_delta[i]->tiempo = ti;
    }

    // return [tiempo, mtmm * pos, mtmm * vel, mtmm * acel];
}

// void path_linear_speed_inv(rot_z, rot_y, double theta_y, double theta_z, m_trans)
// {
//     double m_tiempo = m_data_delta[0]->tiempo;
//     double m_pos = mmtm * m_data_delta[0]->pos;
//     double m_vel = mmtm * m_data_delta[0]->vel;
//     double m_acel = mmtm * m_data_delta[0]->acel;

//     theta_y = theta_y * dtr;
//     theta_z = theta_z * dtr;

//     system_linear_matrix(rot_z, rot_y, theta_y, theta_z, m_trans);

//   //return res_final_2
// }

// void system_linear_matrix(rot_z, rot_y, theta_y, theta_z, m_trans)
// {
//  for(int i=0;i<size(m_data_delta);i++)
//  {
//     //   ######  Posicion xyz  ##########
//   xyz_ls = system_linear_inv(m_data_delta[i]->pos, rot_z, rot_y, theta_y, theta_z, m_trans);

//   m_data_delta[i]->pos_x = xyz_ls[0][0];
//   m_data_delta[i]->pos_y = xyz_ls[1][0];
//   m_data_delta[i]->pos_z = xyz_ls[2][0];

// //   ######  Velocidad xyz  ##########
//   xyz_ls_vel = system_linear_inv(m_data_delta[i]->vel, rot_z, rot_y, theta_y, theta_z, m_trans);
//   m_data_delta[i]->vel_x = xyz_ls_vel[0][0];
//   m_data_delta[i]->vel_y = xyz_ls_vel[1][0];
//   m_data_delta[i]->vel_z = xyz_ls_vel[2][0];

// //   ######  Aceleracion xyz  ##########
//   xyz_ls_acel = system_linear_inv(m_data_delta[i]->acel, rot_z, rot_y, theta_y, theta_z, m_trans);
//   m_data_delta[i]->acel_x = xyz_ls_acel[0][0];
//   m_data_delta[i]->acel_y = xyz_ls_acel[1][0];
//   m_data_delta[i]->acel_z = xyz_ls_acel[2][0];
//  }

// //  return [m_tiempo,
// // 		 mtmm * m_pos_x[0], mtmm * m_pos_y[0], mtmm * m_pos_z[0],
// // 		 mtmm * m_vel_x[0], mtmm * m_vel_y[0], mtmm * m_vel_z[0],
// // 		 mtmm * m_acel_x[0], mtmm * m_acel_y[0], mtmm * m_acel_z[0]]
// }

// void system_linear_inv(double xprima, rot_z, rot_y, theta_y, theta_z, m_trans)
// {
//     //  ######   Creacion de Matrices  ######
//     double rot_tras_inv[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
//     double pf_inv[3] = {0,0,0};
//     double pf_trans_inv[4] = {0,0,0,0};
//     double m_trans_inv[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

//     // ######   Rotacion Y  ######
//     pf_inv[0] = xprima;
//     pf_inv[1] = 0;
//     pf_inv[2] = 0;

//     rot_y_tras = rot_y.transpose();

//     m_rot1_inv = rot_y_tras.dot(pf_inv);

//     // ######   Rotacion Z  ######
//     rot_z_tras = rot_z.transpose();
//     m_rot2_inv = rot_z_tras.dot(m_rot1_inv);

//     // ######   Traslacion ######
//     pf_trans_inv[0] = m_rot2_inv[0][0];
//     pf_trans_inv[1] = m_rot2_inv[1][0];
//     pf_trans_inv[2] = m_rot2_inv[2][0];
//     pf_trans_inv[3] = 1;

//     m_trans_inv[0][3] = -1 * m_trans[0][3];
//     m_trans_inv[1][3] = -1 * m_trans[1][3];
//     m_trans_inv[2][3] = -1 * m_trans[2][3];
//     m_trans_inv[0][0] = 1;
//     m_trans_inv[1][1] = 1;
//     m_trans_inv[2][2] = 1;
//     m_trans_inv[3][3] = 1;

//     xyz_res = (m_trans_inv).dot(pf_trans_inv);

//     //return xyz_res
// }
