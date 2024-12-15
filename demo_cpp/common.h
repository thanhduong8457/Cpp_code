#include "delta_define.h"

class Point {
public:
    double x;
    double y;
    double z;

    Point() {
        init();
    }

    bool operator!=(const Point& other) const {
        return this->x != other.x || this->y != other.y || this->z != other.z;
    }

    void operator=(const Point& other) {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }

    void init() {
        x = 0;
        y = 0;
        z = 0;
    }
};

class Theta {
public:
    double angle1;
    double angle2;
    double angle3;

    Theta() {
        init();
    }

    void operator=(const Theta& other) {
        this->angle1 = other.angle1;
        this->angle2 = other.angle2;
        this->angle3 = other.angle3;
    }

    void init(void) {
        angle1 = 0;
        angle2 = 0;
        angle3 = 0;
    }
};

typedef struct data_delta {
    double pos;     //
    Point position_val;

    double vel;     //
    double vel_x;
    double vel_y;
    double vel_z;

    double acel;    //
    double acel_x;
    double acel_y;
    double acel_z;

    Theta theta_val;

    double time_point;  //
}data_delta_t;

void punto_codo(double theta, double(&b1)[3]);
void rotation120(double ent[3], double(&sal)[3]);
void rotation240(double ent[3], double(&sal)[3]);
void punto_ee(double eee[3], int brazo, double(&sal)[3]);
void sumav(double v1[3], double v2[3], double(&s)[3]);
void angulos_codo(double codo[3], double eee[3], int brazo, double& ang_a, double& ang_b);
void rotation_y(double ent[3], double ang, double(&sal)[3]);
