#include "delta_robot.h"

delta_robot::delta_robot(void) {
    // data = new data_delta_t;
    initialize();
}

delta_robot::~delta_robot(void) {
    while (!m_data_delta.empty()) {
        delete m_data_delta.back();
        m_data_delta.pop_back();
    }
}

void delta_robot::initialize(void) {
    this->vmax = 1500;
    this->amax = 200000;
    this->num_point_1 = 120;
    this->num_point_2 = 120;
    while (!m_data_delta.empty()) {
        delete m_data_delta.back();
        m_data_delta.pop_back();
    }
}

/// @brief 
/// @param point0 
/// @param pointf 
/// @return 
Point delta_robot::unit_vector(Point point0, Point pointf) {
    Point return_point;
    double delta_x = pointf.x - point0.x;
    double delta_y = pointf.y - point0.y;
    double delta_z = pointf.z - point0.z;

    double modulo = sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

    return_point.x = delta_x / modulo;
    return_point.y = delta_y / modulo;
    return_point.z = delta_z / modulo;
    return return_point;
}

// void delta_robot::path_linear_speed(double xo,double yo,double zo,double xf,double yf,double zf)
// {
//    //######  international units change  ##########
//    double xoo = xo * mmtm;
//    double yoo = yo * mmtm;
//    double zoo = zo * mmtm;
//    double xff = xf * mmtm;
//    double yff = yf * mmtm;
//    double zff = zf * mmtm;

//    system_linear(xoo, yoo, zoo, xff, yff, zff);

// }

/// @brief 
/// @param mStartPoint 
/// @param mEndPoint 
/// @param dis 
/// @param rot_z 
/// @param rot_y 
/// @param theta_y 
/// @param theta_z 
/// @param rot_tras 
void delta_robot::system_linear(void) {
    initialize();

    double xoo = mStartPoint.x * mmtm;
    double yoo = mStartPoint.y * mmtm;
    double zoo = mStartPoint.z * mmtm;

    double xff = mEndPoint.x * mmtm;
    double yff = mEndPoint.y * mmtm;
    double zff = mEndPoint.z * mmtm;

    double pf[3] = {0, 0, 0};
    double pf_trans[4] = {0, 0, 0, 0};
    double x_trans[4] = {0, 0, 0, 0};

    // Traslacion(Xo, Yo, Zo)
    rot_tras[0][0] = 1;
    rot_tras[1][1] = 1;
    rot_tras[2][2] = 1;
    rot_tras[3][3] = 1;
    rot_tras[0][3] = -xoo;
    rot_tras[1][3] = -yoo;
    rot_tras[2][3] = -zoo;
    //[1  0  0 -xoo]
    //[0  1  0 -yoo]
    //[0  0  1 -zoo]
    //[0  0  0   1 ]

    // Traslacion(Xf, Yf, Zf)
    pf_trans[0] = xff;
    pf_trans[1] = yff;
    pf_trans[2] = zff;
    pf_trans[3] = 1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            x_trans[i] += rot_tras[i][j] * pf_trans[j];
        }
    }

    Point my_unit_vector;
    Point temp_point;
    Point temp_point1;
    temp_point.init();
    temp_point1.init();
    temp_point1.x = x_trans[0];
    temp_point1.y = x_trans[1];
    temp_point1.z = x_trans[2];
    my_unit_vector = unit_vector(temp_point, temp_point1);
    angle_rotation(my_unit_vector);

    double cos_axisz = cos(theta_y);
    double sin_axisz = sin(theta_y);

    double cos_axisy = cos(theta_z);
    double sin_axisy = sin(theta_z);

    pf[0] = x_trans[0];
    pf[1] = x_trans[1];
    pf[2] = x_trans[2];

    rot_z[0][0] = cos_axisz;
    rot_z[0][1] = sin_axisz;
    rot_z[1][0] = -1 * sin_axisz;
    rot_z[1][1] = cos_axisz;
    rot_z[2][2] = 1;

    double m_rot1[3] = {};
    double m_rot2[3] = {};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot1[i] += rot_z[i][j] * pf[j];
        }
    }

    rot_y[0][0] = cos_axisy;
    rot_y[0][2] = sin_axisy;
    rot_y[1][1] = 1;
    rot_y[2][0] = -sin_axisy;
    rot_y[2][2] = cos_axisy;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot2[i] += rot_y[i][j] * m_rot1[j];
        }
    }

    dis = m_rot2[0] * mtmm;
    theta_z = theta_z * rtd;
    theta_y = theta_y * rtd;
}

/// @brief 
/// @param nx 
/// @param ny 
/// @param nz 
/// @param theta_z 
/// @param theta_y 
void delta_robot::angle_rotation(Point unit_vector) {
    if (unit_vector.z < 0) {
        theta_z = (360 * dtr) + asin(unit_vector.z);
    } else {
        theta_z = asin(unit_vector.z);
    }

    if (unit_vector.x < 0) {
        theta_y = (180 * dtr) + atan(unit_vector.y / unit_vector.x);
    } else if (unit_vector.x == 0) {
        if (unit_vector.y < 0) {
            theta_y = 270 * dtr;
        } else if (unit_vector.y == 0) {
            theta_y = 0;
        } else {
            theta_y = 90 * dtr;
        }
    } else {
        if (unit_vector.y < 0) {
            theta_y = (360 * dtr) + atan(unit_vector.y / unit_vector.x);
        } else {
            theta_y = atan(unit_vector.y / unit_vector.x);
        }
    }
}

double delta_robot::delta_calcAngleYZ(double x0, double y0, double z0) {
    double y1 = -0.5 * 0.57735 * ff; // f/2 * tg 30
    y0 -= 0.5 * 0.57735 * ee;        // shift center to edge
                                     // z = a + b*y
    double a = (x0 * x0 + y0 * y0 + z0 * z0 + rf * rf - re * re - y1 * y1) / (2 * z0);
    double b = (y1 - y0) / z0;

    // discriminant
    double d = -(a + b * y1) * (a + b * y1) + rf * (b * b * rf + rf);
    if (d < 0) {
        return -1; // non-existing point
    }
    double yj = (y1 - a * b - sqrt(d)) / (b * b + 1); // choosing outer point
    double zj = a + b * yj;
    double theta = 180.0 * atan(-zj / (y1 - yj)) / pi + ((yj > y1) ? 180.0 : 0.0);
    return theta;
}

/// @brief 
/// @param  
void delta_robot::TrapezoidalVelocityProfile(void) {
    try {
        double q0 = 0; // start from zero
        q0 *= mmtm;
        dis *= mmtm;
        vmax *= mmtm;
        amax *= mmtm;

        if (num_point_1 <= 0) num_point_1 = 2;
        if (num_point_2 <= 0) num_point_2 = 2;

        double tau = vmax / amax;

        double T = 0;
        if ((this->dis - q0) > 0) {
            T = (this->dis - q0) / vmax + tau;
        }
        else if ((this->dis - q0) < 0) {
            T = (q0 - this->dis) / vmax + tau;
        }

        double paso1 = tau / num_point_1;
        double paso2 = (T - (2 * tau)) / num_point_2;
        int pas_total = num_point_1 + num_point_2 + num_point_1;

        double Tf = 2 * (sqrt((this->dis - q0) / (amax)));
        double vmax_acel = amax * (Tf / 2);

        if (vmax_acel <= vmax) {
            vmax = vmax_acel;
            tau = Tf / 2;
            T = Tf;
            paso1 = tau / num_point_1;
            paso2 = 0;
            num_point_2 = 0;
            pas_total = num_point_1 + num_point_2 + num_point_1;
        }

        for (int i = 0; i < pas_total; i++) {
            data = new data_delta_t;
            data->pos = 0;
            data->position_val.init();
            data->theta_val.init();
            data->acel = 0;
            data->vel = 0;
            data->time_point = 0;
            m_data_delta.push_back(data);
        }

        double ti = -1 * paso1;
        double q_actual, v_actual, a_actual;

        for (int i = 0; i < pas_total; i++) {
            if (i >= 0 && i < num_point_1) {
                ti += paso1;
            }
            else if (i >= num_point_1 && i < (num_point_1 + num_point_2)) {
                ti += paso2;
            }
            else if (i >= (num_point_1 + num_point_2) && i < (pas_total)) {
                ti += paso1;
            }
            else {
                ti = T;
            }

            ls_v_a_puntual(q0, dis, ti, q_actual, v_actual, a_actual);
            m_data_delta[i]->pos = q_actual;
            m_data_delta[i]->vel = v_actual;
            m_data_delta[i]->acel = a_actual;
            m_data_delta[i]->time_point = ti;
            cout << "dis=" << q_actual << ", v=" << v_actual << ", a_actual=" << a_actual << ", time_point=" << ti << endl;
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

/// @brief 
/// @param q0 
/// @param this->dis 
/// @param vmax 
/// @param amax 
/// @param tactual 
/// @param q_actual 
/// @param v_actual 
/// @param a_actual 
void delta_robot::ls_v_a_puntual(
    double q0, 
    double q1,
    double tactual, 
    double &q_actual, 
    double &v_actual, 
    double &a_actual
) {
    double tau = this->vmax / this->amax;

    double T;
    int s = 0;

    if (q1 - q0 > 0) {
        s = 1;
    }
    else if (q1 - q0 < 0) {
        s = -1;
    }

    T = s * (q1 - q0) / vmax + tau;

    double timer1 = tau;
    double timer2 = T - tau;
    double timer3 = T;

    if ((0 <= tactual) && (tactual <= timer1)) {
        q_actual = (q0) + ((s) * (amax / 2) * (tactual * tactual));
        v_actual = s * amax * tactual;
        a_actual = s * amax;
    }
    else if ((timer1 < tactual) && (tactual <= timer2)) {
        q_actual = (q0) - ((s) * ((vmax * vmax) / (2 * amax))) + (s * vmax * tactual);
        v_actual = s * vmax;
        a_actual = 0;
    }
    else if ((timer2 < tactual) && (tactual <= timer3)) {
        q_actual = (q1) + ((s) * ((-1 * ((amax * (T * T)) / (2))) + (amax * T * tactual) + (-1 * (amax / 2) * (tactual * tactual))));
        v_actual = s * ((amax * T) - (amax * tactual));
        a_actual = s * (-1 * amax);
    }
    else {
        q_actual = 0;
        v_actual = 0;
        a_actual = 0;
    }
}

/// @brief 
void delta_robot::system_linear_matrix(void) {
    theta_y = theta_y * dtr;
    theta_z = theta_z * dtr;
    double xyz_res[4] = {0, 0, 0, 0};
    
    for (int i = 0; i < m_data_delta.size(); i++) {
        system_linear_invese(m_data_delta[i]->pos, xyz_res);

        m_data_delta[i]->position_val.x = xyz_res[0];
        m_data_delta[i]->position_val.y = xyz_res[1];
        m_data_delta[i]->position_val.z = xyz_res[2];
        cout << "x=" << xyz_res[0] << ", y=" << xyz_res[1] << ", z=" << xyz_res[2] << endl;
        
        // // ######  velocidad xyz  ##########
        // system_linear_invese(m_data_delta[i]->vel, rot_z, rot_y, theta_y, theta_z, rot_tras, xyz_res);
        // m_data_delta[i]->vel_x = xyz_res[0][0];
        // m_data_delta[i]->vel_y = xyz_res[1][0];
        // m_data_delta[i]->vel_z = xyz_res[2][0];

        // // ######  aceleracion xyz  ##########
        // system_linear_invese(m_data_delta[i]->acel, rot_z, rot_y, theta_y, theta_z, rot_tras, xyz_res);
        // m_data_delta[i]->acel_x = xyz_res[0][0];
        // m_data_delta[i]->acel_y = xyz_res[1][0];
        // m_data_delta[i]->acel_z = xyz_res[2][0];
    }
}

/// @brief 
/// @param xprima 
/// @param rot_z 
/// @param rot_y 
/// @param theta_y 
/// @param theta_z 
/// @param m_trans 
/// @param xyz_res 
void delta_robot::system_linear_invese(double xprima, double (&xyz_res)[4]) {
    double rot_tras_invese[4][4] = {};
    double pf_invese[3] = {};
    double pf_trans_invese[4] = {};
    double m_trans_invese[4][4] = {};
    // ######   Rotacion Y  ######
    pf_invese[0] = xprima;
    pf_invese[1] = 0;
    pf_invese[2] = 0;

    double rot_y_tras[3][3] = {};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            rot_y_tras[j][i] = rot_y[i][j];
        }
    }

    double m_rot1_invese[3] = {};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot1_invese[i] += rot_y_tras[i][j] * pf_invese[j];
        }
    }
    // ######   Rotacion Z  ######
    double rot_z_tras[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            rot_z_tras[j][i] = rot_z[i][j];
        }
    }

    double m_rot2_invese[3] = {};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_rot2_invese[i] += rot_z_tras[i][j] * m_rot1_invese[j];
        }
    }

    pf_trans_invese[0] = m_rot2_invese[0];
    pf_trans_invese[1] = m_rot2_invese[1];
    pf_trans_invese[2] = m_rot2_invese[2];
    pf_trans_invese[3] = 1;

    m_trans_invese[0][3] = -1 * rot_tras[0][3];
    m_trans_invese[1][3] = -1 * rot_tras[1][3];
    m_trans_invese[2][3] = -1 * rot_tras[2][3];
    m_trans_invese[0][0] = 1;
    m_trans_invese[1][1] = 1;
    m_trans_invese[2][2] = 1;
    m_trans_invese[3][3] = 1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            xyz_res[i] += m_trans_invese[i][j] * pf_trans_invese[j];
        }
    }
}

/// @brief 
/// @param mStartPoint 
/// @return 
double delta_robot::CalculateAngleYZ(Point mStartPoint) {
    double y1 = -0.5 * 0.57735 * ff; // f/2 * tg 30
    mStartPoint.y -= 0.5 * 0.57735 * ee;        // shift center to edge
                                     // z = a + b*y
    double a = (mStartPoint.x * mStartPoint.x + mStartPoint.y * mStartPoint.y + mStartPoint.z * mStartPoint.z + rf * rf - re * re - y1 * y1) / (2 * mStartPoint.z);
    double b = (y1 - mStartPoint.y) / mStartPoint.z;

    // discriminant
    double d = -(a + b * y1) * (a + b * y1) + rf * (b * b * rf + rf);
    if (d < 0) {
        return 0; // non-existing point
    }
    double yj = (y1 - a * b - sqrt(d)) / (b * b + 1); // choosing outer point
    double zj = a + b * yj;
    double theta = 180.0 * atan(-zj / (y1 - yj)) / pi + ((yj > y1) ? 180.0 : 0.0);
    return theta;
}

/// @brief 
/// @param  
void delta_robot::InverseAllJointStateExist(void) {
    for (int i = 0; i < m_data_delta.size(); i++) {
        m_data_delta[i]->theta_val = inverse(m_data_delta[i]->position_val);
    }
}

/// @brief 
/// @param mStartPoint 
/// @return 
Theta delta_robot::inverse(Point point) {
    Theta theta;
    Point point_temp;

    // Rotacion(0, 120, 240)
    theta.angle2 = angle_yz(point);

    point_temp.x = point.x * cos120 + point.y * sin120;
    point_temp.y = point.y * cos120 - point.x * sin120;
    point_temp.z = point.z;
    theta.angle3 = angle_yz(point_temp); // rotate  to +119 deg

    point_temp.x = point.x * cos120 - point.y * sin120;
    point_temp.y = point.y * cos120 + point.x * sin120;
    point_temp.z = point.z;
    theta.angle1 = angle_yz(point_temp); // rotate to -120 deg

    cout << "with x=" << point_temp.x << ", y=" << point_temp.y << ", z=" << point_temp.z ;
    cout << " then thetal=" << theta.angle1 << ", theta2=" << theta.angle2 << ", theta3=" << theta.angle3 << endl;
    
    return theta;
}

/// @brief 
/// @param x0 
/// @param y0 
/// @param z0 
/// @param theta 
/// @return 
double delta_robot::angle_yz(Point point) {
    double theta = 0;

    double y1 = -0.5 * tan30 * ff * mmtm;
    point.y = point.y - 0.5 * tan30 * ee * mmtm;
    // z = a + b * y
    double a = (point.x * point.x + point.y * point.y + point.z * point.z + rf * mmtm * rf * mmtm - re * mmtm * re * mmtm - y1 * y1) / (2.0 * point.z);
    double b = (y1 - point.y) / point.z;

    // discriminante
    // d = -(a + b * y1) * (a + b * y1) + rf * (b * b * rf + rf)
    double d = ((-1) * (a + b * y1) * (a + b * y1)) + (rf * mmtm * rf * mmtm * (b * b + 1.0));

    if (d < 0) {
        cout << "discriminante - angle_yz" << endl;
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

    return theta;
}

/// @brief 
/// @param ti 
/// @param pointi 
/// @param theta 
/// @param gripper 
/// @param position 
void delta_robot::CreateJointStateList(
    Point pointi,
    Theta theta,
    int gripper, 
    double (&position)[13]
) {
    // Rviz interior angles in Radians
    double punto[3] = {-pointi.y, -pointi.x, -pointi.z};

    double c2[3] = {};
    double p2[3] = {};
    punto_codo(theta.angle2, c2);
    punto_ee(punto, 2, p2);

    double a2_a, a2_b;
    angulos_codo(c2, p2, 2, a2_a, a2_b);

    double c3[3] = {};
    double temp[3] = {};
    double p3[3] = {};
    punto_codo(theta.angle3, temp);
    rotation120(temp, c3);
    punto_ee(punto, 3, p3);

    double a3_a, a3_b;
    angulos_codo(c3, p3, 3, a3_a, a3_b);

    double c1[3] = {};
    double p1[3] = {};
    punto_codo(theta.angle1, c1);
    rotation120(c1, temp);
    rotation120(temp, c1);
    punto_ee(punto, 1, p1);

    double a1_a, a1_b;
    angulos_codo(c1, p1, 1, a1_a, a1_b);

    // Data to publish in Rviz
    position[0]  = theta.angle1;
    position[1]  = theta.angle2;
    position[2]  = theta.angle3;
    position[3]  = theta.angle1 + a1_a;
    position[4]  = a1_b;
    position[5]  = theta.angle2 + a2_a;
    position[6]  = a2_b;
    position[7]  = theta.angle3 + a3_a;
    position[8]  = a3_b;
    position[9]  = pointi.x;
    position[10] = pointi.y;
    position[11] = pointi.z;
    position[12] = gripper;
}
