#include "functions.h"

inline float toDeg(float rad){
    return rad * (180.0 / M_PI);
}

inline float toRad(float deg){
    return deg * (M_PI / 180.0);
}

float angleLoop(float deg){
    if(deg >= 360.0){
        return deg - 360.0;
    }
    else if(deg <= 0.0){
        return deg + 360.0;
    }

    return deg;
}
