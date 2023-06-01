#ifndef DATA_TYPES_H_INCLUDED
#define DATA_TYPES_H_INCLUDED



#endif // DATA_TYPES_H_INCLUDED
#include <SDL2/SDL.h>



const int MAX_RAY_DIST = 128;


typedef struct {
    float x;
    float y;
}vector2;

typedef struct {
    float x;
    float y;
    float z;
}vector3;

typedef struct {
    int r;
    int g;
    int b;
    int a;
}color;

typedef struct {
    vector2 a, b;
    int portal;
    int id;
    color wallColor;
}wall;

typedef struct {
    int id;
    wall *walls;
    int numWalls;
    float floor;
    float ceiling;
}sector;

typedef struct {
    vector3 camera;

} player;

