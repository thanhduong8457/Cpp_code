#include "delta_define.h"

void punto_codo(double theta, double(&b1)[3]);
void rotacion120(double ent[3], double(&sal)[3]);
void rotacion240(double ent[3], double(&sal)[3]);
void punto_ee(double eee[3], int brazo, double(&sal)[3]);
void sumav(double v1[3], double v2[3], double(&s)[3]);
void angulos_codo(double codo[3], double eee[3], int brazo, double& ang_a, double& ang_b);
void rotacion_y(double ent[3], double ang, double(&sal)[3]);
