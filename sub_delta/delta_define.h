#ifndef __DELTA_DEFINE__
#define __DELTA_DEFINE__

#include <iostream>
#include <chrono>
#include <thread>
#include "vector"
#include "math.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define sqrt3   1.732050808
#define pi      3.141592654
#define sin120  0.8660254038
#define cos120  -0.5

#define sin240  -0.8660254038
#define cos240  -0.5

#define tan60   1.732050808
#define sin30   0.5
#define tan30   0.5773502692

#define ee      86.5       // endeffector 
#define ff      346.4      // base
#define re      465        // endeffector arm
#define rf      200        // Base arm

#define hf      299.99119987
#define he        74.911197427

#define mmtm    0.001
#define mtmm    1000
#define dtr     (pi/180)
#define rtd     (180/pi)

#endif
