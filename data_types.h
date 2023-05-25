#ifndef DATA_TYPES_H_INCLUDED
#define DATA_TYPES_H_INCLUDED



#endif // DATA_TYPES_H_INCLUDED
#include <SDL2/SDL.h>


typedef struct v2{
    float x;
    float y;
}vector2;

typedef struct v3{
    float x;
    float y;
    float z;
}vector3;

typedef struct w{
    vector2 a, b;
    int portal;
}wall;

typedef struct s{
    int id;
    int firstWall;
    int numWalls;
    float floor;
    float ceiling;
}sector;


