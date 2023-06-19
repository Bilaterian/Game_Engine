#ifndef DATA_TYPES_H_INCLUDED
#define DATA_TYPES_H_INCLUDED

#endif // DATA_TYPES_H_INCLUDED

const int MAX_RAY_DIST = 128;

typedef struct{
 int fr1;
 int fr2;           //frame 1 frame 2, to create constant frame rate
}time;

typedef struct{         //save sin cos in values 0-360 degrees;
    float cos[360];
    float sin[360];
}math;

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
    color wallColor;
}wall;

typedef struct {
    wall *walls;
    int numWalls;
    float floor;
    float ceiling;
    int dist;
    color floorColor;
    color ceilingColor;
    int surface;
}sector;

typedef struct{
    vector3 pos;        //player position. Z is up
    int a;              //player angle of rotation left right
    int l;              //variable to look up and down
}player;

typedef struct{
 int w,s,a,d;           //move up, down, left, right
 int sl,sr;             //strafe left, right
 int m;                 //move up, down, look up, down
}keys;

