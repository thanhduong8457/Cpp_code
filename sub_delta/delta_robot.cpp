#include "delta_robot.h"

delta_robot::delta_robot(void) {

}

delta_robot::~delta_robot(void) {
    
}

void delta_robot::unit_vector(double xo,double yo,double zo,
                            double xf,double yf,double zf, 
                            double &nx, double &ny, double &nz) {
    double delta_x = xf - xo;
    double delta_y = yf - yo;
    double delta_z = zf - zo;

    double modulo = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);

    nx = delta_x / modulo;
    ny = delta_y / modulo;
    nz = delta_z / modulo;
}

//void delta_robot::path_linear_speed(double xo,double yo,double zo,double xf,double yf,double zf)
//{
//   //######  international units change  ##########
//   double xoo = xo * mmtm;
//   double yoo = yo * mmtm;
//   double zoo = zo * mmtm;
//   double xff = xf * mmtm;
//   double yff = yf * mmtm;
//   double zff = zf * mmtm;
//
//   system_linear(xoo, yoo, zoo, xff, yff, zff);
//
//}

void delta_robot::system_linear(double xo, double yo, double zo, double xf, double yf, double zf,
    double& dis, double(&rot_z)[3][3], double(&rot_y)[3][3], double& theta_y, double& theta_z, double(&rot_tras)[4][4])
 {
    double xoo = xo * mmtm;
    double yoo = yo * mmtm;
    double zoo = zo * mmtm;

    double xff = xf * mmtm;
    double yff = yf * mmtm;
    double zff = zf * mmtm;

    double pf[3][1] = {0, 0, 0};
    double pf_trans[4][1] = {0, 0, 0, 0};

    double x_trans[4][1] = {0, 0, 0, 0};

    // Traslacion(Xo, Yo, Zo)
    rot_tras[0][0] = 1;
    rot_tras[1][1] = 1;
    rot_tras[2][2] = 1;
    rot_tras[3][3] = 1;
    rot_tras[0][3] = -xoo;
    rot_tras[1][3] = -yoo;
    rot_tras[2][3] = -zoo;
    // Traslacion(Xf, Yf, Zf)
    pf_trans[0][0] = xff;
    pf_trans[1][0] = yff;
    pf_trans[2][0] = zff;
    pf_trans[3][0] = 1;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            x_trans[i][0] += rot_tras[i][j] * pf_trans[j][0];
        }
    }

    double nx, ny, nz;
    unit_vector(0, 0, 0, x_trans[0][0], x_trans[1][0], x_trans[2][0], nx, ny, nz);
    angulos_rot(nx, ny, nz, theta_z, theta_y);

    double cos_axisz = cos(theta_y);
    double sin_axisz = sin(theta_y);

    double cos_axisy = cos(theta_z);
    double sin_axisy = sin(theta_z);

    // plano xy ya trasladado
    pf[0][0] = x_trans[0][0];
    pf[1][0] = x_trans[1][0];
    pf[2][0] = x_trans[2][0];
    // rotacion respecto a eje z
    rot_z[0][0] = cos_axisz;
    rot_z[0][1] = sin_axisz;
    rot_z[1][0] = -1 * sin_axisz;
    rot_z[1][1] = cos_axisz;
    rot_z[2][2] = 1;

    double m_rot1[3][1] = {};
    double m_rot2[3][1] = {};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot1[i][0] += rot_z[i][j] * pf[j][0];
        }
    }

    // rotacion respecto a eje y
    rot_y[0][0] = cos_axisy;
    rot_y[0][2] = sin_axisy;
    rot_y[1][1] = 1;
    rot_y[2][0] = -sin_axisy;
    rot_y[2][2] = cos_axisy;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot2[i][0] += rot_y[i][j] * m_rot1[j][0];
        }
    }

    dis = m_rot2[0][0] * mtmm;
    theta_z = theta_z * rtd;
    theta_y = theta_y * rtd;
 }

void delta_robot::angulos_rot(double nx,double ny,double nz,double &theta_z,double &theta_y) {
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

void delta_robot::ls_v_a_total(double q0,double q1,double vmax,double amax,int pas_1,int pas_2) {
    q0 *= mmtm;
    q1 *= mmtm;
    vmax *= mmtm;
    amax *= mmtm;

    if (pas_1 <= 0)    pas_1 = 2;
    if (pas_2 <= 0)    pas_2 = 2;

    double tau = vmax/amax;

    double T = 0;
    if((q1-q0)>0)   T = (q1 - q0)/vmax + tau;
    else if((q1-q0)<0)    T = (q0 - q1)/vmax + tau;

    double paso1 = tau/pas_1;
    double paso2 = (T - (2 * tau)) / pas_2;

    int pas_total = pas_1 + pas_2 + pas_1;

    double Tf = 2 * (sqrt(((q1) - (q0)) / (amax)));
    double vmax_acel = amax * (Tf / 2);

    if (vmax_acel <= vmax) {
        cout<<"case vmax_acel <= vmax we have"<<endl;
        vmax = vmax_acel;
        tau = Tf/2;
        T = Tf;
        paso1 = tau/pas_1;
        paso2 = 0;
        pas_2 = 0;
        pas_total = pas_1 + pas_2 + pas_1;
    }

    for(int i=0;i<pas_total;i++) {
        data_delta_t *data = NULL;
        data = new data_delta_t;

        data->pos = 0;
        data->pos_x = 0;
        data->pos_y = 0;
        data->pos_z = 0;
        data->acel = 0;
        data->vel = 0;
        data->tiempo = 0;
        m_data_delta.push_back(data);
    }

    double ti = -1 * paso1;
    double q_actual, v_actual, a_actual;

    for (int i = 0; i < pas_total; i++)
    {
        if (i >= 0 && i < pas_1) ti += paso1;
        else if (i >= pas_1 && i < (pas_1 + pas_2)) ti += paso2;
        else if (i >= (pas_1 + pas_2) && i < (pas_total)) ti += paso1;
        else ti = T;

        ls_v_a_puntual(q0, q1, vmax, amax, ti, q_actual, v_actual, a_actual);
        m_data_delta[i]->pos = q_actual;
        m_data_delta[i]->vel = v_actual;
        m_data_delta[i]->acel = a_actual;
        m_data_delta[i]->tiempo = ti;
        cout << "dis=" << q_actual << ", v=" << v_actual << ", a_actual=" << a_actual << ", time_point=" << ti << endl;
    }
}

void delta_robot::ls_v_a_puntual(double q0, double q1, 
                                double vmax, double amax, 
                                double tactual, double &q_actual, double &v_actual, double &a_actual) {
    double tau = vmax/amax;

    double T;
    int s = 0;

    if(q1-q0>0) s = 1;
    else if(q1-q0<0) s = -1;

    T = s*(q1 - q0) / vmax + tau;
 
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

void delta_robot::system_linear_matrix(double tiempo, double rot_z[3][3], double rot_y[3][3], double theta_y, double theta_z, double m_trans[4][4]) {
    theta_y = theta_y * dtr;
    theta_z = theta_z * dtr;

    for (int i = 0; i<m_data_delta.size(); i++)
    {
        //   ######  posicion xyz  ##########
        //stem_linear_inv(m_data_delta[i]->pos, value_x, value_y, value_z);
        double xyz_res[4][1] = {0, 0, 0, 0};
        system_linear_inv(m_data_delta[i]->pos, rot_z, rot_y, theta_y, theta_z, m_trans, xyz_res);

        m_data_delta[i]->pos_x = xyz_res[0][0];
        m_data_delta[i]->pos_y = xyz_res[1][0];
        m_data_delta[i]->pos_z = xyz_res[2][0];
        cout << "x=" << xyz_res[0][0] << ", y=" << xyz_res[1][0] << ", z=" << xyz_res[2][0] << endl;
        /*
        // ######  velocidad xyz  ##########
        system_linear_inv(m_data_delta[i]->vel, rot_z, rot_y, theta_y, theta_z, m_trans, xyz_res);
        m_data_delta[i]->vel_x = xyz_res[0][0];
        m_data_delta[i]->vel_y = xyz_res[1][0];
        m_data_delta[i]->vel_z = xyz_res[2][0];

        // ######  aceleracion xyz  ##########
        system_linear_inv(m_data_delta[i]->acel, rot_z, rot_y, theta_y, theta_z, m_trans, xyz_res);
        m_data_delta[i]->acel_x = xyz_res[0][0];
        m_data_delta[i]->acel_y = xyz_res[1][0];
        m_data_delta[i]->acel_z = xyz_res[2][0];
        */
    }
}

void delta_robot::system_linear_inv(double xprima, double rot_z[3][3], double rot_y[3][3], double theta_y, double theta_z, double m_trans[4][4], double(&xyz_res)[4][1]) {
    double rot_tras_inv[4][4] = {};
    double pf_inv[3][1] = {};
    double pf_trans_inv[4][1] = {};
    double m_trans_inv[4][4] = {};
    //######   Creacion de Matrices  ######

    //######   Rotacion Y  ######
    pf_inv[0][0] = xprima;
    pf_inv[1][0] = 0;
    pf_inv[2][0] = 0;

    double rot_y_tras[3][3] = {};
    //rot_y_tras = rot_y.transpose()
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            rot_y_tras[j][i] = rot_y[i][j];
        }
    }
    double m_rot1_inv[3][1] = {};
    //m_rot1_inv = rot_y_tras.dot(pf_inv)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
                m_rot1_inv[i][0] += rot_y_tras[i][j] * pf_inv[j][0];
        }
    }
    //######   Rotacion Z  ######
    //rot_z_tras = rot_z.transpose()
    double rot_z_tras[3][3] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            rot_z_tras[j][i] = rot_z[i][j];
        }
    }

    //m_rot2_inv = rot_z_tras.dot(m_rot1_inv)
    double m_rot2_inv[3][1] = {};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            m_rot2_inv[i][0] += rot_z_tras[i][j] * m_rot1_inv[j][0];;
        }
    }

    //######   Traslacion ######
    pf_trans_inv[0][0] = m_rot2_inv[0][0];
    pf_trans_inv[1][0] = m_rot2_inv[1][0];
    pf_trans_inv[2][0] = m_rot2_inv[2][0];
    pf_trans_inv[3][0] = 1;

    m_trans_inv[0][3] = -1 * m_trans[0][3];
    m_trans_inv[1][3] = -1 * m_trans[1][3];
    m_trans_inv[2][3] = -1 * m_trans[2][3];
    m_trans_inv[0][0] = 1;
    m_trans_inv[1][1] = 1;
    m_trans_inv[2][2] = 1;
    m_trans_inv[3][3] = 1;

    //xyz_res = (m_trans_inv).dot(pf_trans_inv)
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            xyz_res[i][0] += m_trans_inv[i][j] * pf_trans_inv[j][0];;
        }
    }
}

double delta_robot::delta_calcAngleYZ(double x0, double y0, double z0) {
    double y1 = -0.5 * 0.57735 * ff; // f/2 * tg 30
    y0 -= 0.5 * 0.57735 * ee;    // shift center to edge
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
void delta_robot::delta_calcInverse(double x0, double y0, double z0, double& theta1, double& theta2, double& theta3) {
    theta1 = delta_calcAngleYZ(x0, y0, z0);
    theta2 = delta_calcAngleYZ(x0 * cos120 + y0 * sin120, y0 * cos120 - x0 * sin120, z0);  // rotate  to +119 deg
    theta3 = delta_calcAngleYZ(x0 * cos120 - y0 * sin120, y0 * cos120 + x0 * sin120, z0);  // rotate to -120 deg
}

void delta_robot::delta_calcForward(double theta1, double theta2, double theta3, double& x0, double& y0, double& z0) {
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
    if (d < 0) {
        return; // non-existing point
    }

    z0 = -0.5 * (b + sqrt(d)) / a;
    x0 = (a1 * z0 + b1) / dnm;
    y0 = (a2 * z0 + b2) / dnm;
}

void delta_robot::inverse_m(void) {
    double theta_m1, theta_m2, theta_m3;
    theta_m1 = theta_m2 = theta_m3 = 0;

    for (int i = 0; i < m_data_delta.size(); i++) {
        inverse(m_data_delta[i]->pos_x, m_data_delta[i]->pos_y, m_data_delta[i]->pos_z, theta_m1, theta_m2, theta_m3);

        m_data_delta[i]->theta_1 = theta_m1;
        m_data_delta[i]->theta_2 = theta_m2;
        m_data_delta[i]->theta_3 = theta_m3;
    }
}

int delta_robot::inverse(double x0, double y0, double z0, double &theta1, double& theta2, double& theta3) {
    theta1 = theta2 = theta3 = 0;

    //Rotacion(0, 120, 240)
    int status = angle_yz(x0, y0, z0, theta2);
    if (status == 0) status = angle_yz(x0 * cos120 + y0 * sin120, y0 * cos120 - x0 * sin120, z0, theta3);  // rotate  to +119 deg
    if (status == 0) status = angle_yz(x0 * cos120 - y0 * sin120, y0 * cos120 + x0 * sin120, z0, theta1);  // rotate to -120 deg

    cout << "with x=" << x0 << ", y=" << y0 << ", z=" << z0 ;
    cout << " then thetal=" << theta1 << ", theta2=" << theta2 << ", theta3=" << theta3 << endl;

    return status;
}

int delta_robot::angle_yz(double x0, double y0, double z0, double &theta) {
    double y1 = -0.5 * tan30 * ff * mmtm;
    y0 = y0 - 0.5 * tan30 * ee * mmtm;
    // z = a + b * y
    double a = (x0 * x0 + y0 * y0 + z0 * z0 + rf * mmtm * rf * mmtm - re * mmtm * re * mmtm - y1 * y1) / (2.0 * z0);
    double b = (y1 - y0) / z0;

    // discriminante
    //d = -(a + b * y1) * (a + b * y1) + rf * (b * b * rf + rf)
    double d = ((-1) * (a + b * y1) * (a + b * y1)) + (rf * mmtm * rf * mmtm * (b * b + 1.0));

    if (d < 0) {
        cout<<"discriminante - angle_yz"<<endl;
        return 1; // error inversa
    }

    double yj = ((y1 - a * b) - sqrt(d)) / (b * b + 1.0);
    double zj = a + b * yj;

    if ((y1 - yj) != 0.0) {
        theta = atan(((-1) * (zj)) / (abs(y1 - yj)));
        if (yj < y1) {
            theta = theta * (rtd);
        }
        else {
            if (zj < 0) {
                theta = theta * (180.0 / pi);
                theta = 180.0 - theta;
            }
            else {
                theta = theta * (180.0 / pi);
                theta = (-1.0 * 180.0) + theta;
            }
        }
    }
    else {
        if (zj < 0) {
            theta = 90.0;
        }
        else {
            theta = -90.0;
        }
    }
    return 0;
}

void delta_robot::angulos_eulerianos(double ti, double xi, double yi, double zi, 
    double th1, double th2, double th3, int gripper, double (&position)[13]) {
    // Rviz interior angles in Radians
    double punto[3] = { -yi , -xi , -zi };

    double c2[3] = {};
    double p2[3] = {};
    punto_codo(th2, c2);
    punto_ee(punto, 2, p2);

    double a2_a, a2_b;
    angulos_codo(c2, p2, 2, a2_a, a2_b);

    double c3[3] = {};
    double temp[3] = {};
    double p3[3] = {};
    punto_codo(th3, temp);
    rotacion120(temp, c3);
    punto_ee(punto, 3, p3);

    double a3_a, a3_b;
    angulos_codo(c3, p3, 3, a3_a, a3_b);

    double c1[3] = {};
    double p1[3] = {};
    punto_codo(th1, c1);
    rotacion120(c1, temp);
    rotacion120(temp, c1);
    punto_ee(punto, 1, p1);

    double a1_a, a1_b;
    angulos_codo(c1, p1, 1, a1_a, a1_b);

    // Data to publish in Rviz
    position[0] = th1 * dtr;
    position[1] = th2 * dtr;
    position[2] = th3 * dtr;
    position[3] = th1 * dtr + a1_a;
    position[4] = a1_b;
    position[5] = th2 * dtr + a2_a;
    position[6] = a2_b;
    position[7] = th3 * dtr + a3_a;
    position[8] = a3_b;
    position[9] = xi;
    position[10] = yi;
    position[11] = zi;
    position[12] = gripper;
}
