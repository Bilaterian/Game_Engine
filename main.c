#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
#include <ctype.h>

#include "data_types.h"
#include "functions.h"

SDL_Window* window = NULL;
SDL_GLContext context;

math M;
player P;
sector S[4];
time T;
keys K;

color getColor(int c){
    color returnColor;
    if(c==0){ returnColor.r=255; returnColor.g=255; returnColor.b=0;   returnColor.a = 255;} //Yellow
    if(c==1){ returnColor.r=160; returnColor.g=160; returnColor.b=0;   returnColor.a = 255;} //Yellow darker
    if(c==2){ returnColor.r=0;   returnColor.g=255; returnColor.b=0;   returnColor.a = 255;} //Green
    if(c==3){ returnColor.r=0;   returnColor.g=160; returnColor.b=0;   returnColor.a = 255;} //Green darker
    if(c==4){ returnColor.r=0;   returnColor.g=255; returnColor.b=255; returnColor.a = 255;} //Cyan
    if(c==5){ returnColor.r=0;   returnColor.g=160; returnColor.b=160; returnColor.a = 255;} //Cyan darker
    if(c==6){ returnColor.r=160; returnColor.g=100; returnColor.b=  0; returnColor.a = 255;} //brown
    if(c==7){ returnColor.r=215; returnColor.g=50;  returnColor.b=  0; returnColor.a = 255;} //brown darker
    if(c==8){ returnColor.r=0;   returnColor.g=60;  returnColor.b=130; returnColor.a = 255;}

    return returnColor;
}

void pixel(int x,int y, color c){
    //alter how pixels are drawn here
    glColor3ub(c.r, c.g, c.b);
    float x1 = (float)(x - (GLSW / 2)) / (float)GLSW * 2;
    float y1 = (float)(y - (GLSH / 2)) / (float)GLSH * 2;
    //printf("%d %d", x1, y1);
    glBegin(GL_QUADS);
        glVertex2f( x1, y1 );
        glVertex2f( x1 + pixLengthx, y1 );
        glVertex2f( x1 + pixLengthx, y1 + pixLengthy);
        glVertex2f( x1, y1 + pixLengthy );
    glEnd();
}

void movePlayer(){
 //move up, down, left, right
 if(K.a == 1 && K.m == 0){ P.a -= 4; if(P.a < 0){ P.a += 360;} printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.d == 1 && K.m == 0){ P.a += 4; if(P.a > 359){ P.a -= 360;} printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 int dx = M.sin[P.a] * 10.0;
 int dy = M.cos[P.a] * 10.0;
 if(K.w == 1 && K.m == 0){ P.pos.x += dx; P.pos.y += dy; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.s == 1 && K.m == 0){ P.pos.x -= dx; P.pos.y -= dy; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 //strafe left, right
 if(K.sr == 1){ P.pos.x -= dy; P.pos.y += dx; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.sl == 1){ P.pos.x += dy; P.pos.y -= dx; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 //move up, down, look up, look down
 if(K.a == 1 && K.m == 1){ P.l -= 1; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.d == 1 && K.m == 1){ P.l += 1; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.w == 1 && K.m == 1){ P.pos.z += 4; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
 if(K.s == 1 && K.m == 1){ P.pos.z -= 4; printf("%lf %lf %lf %d\n", P.pos.x, P.pos.y, P.pos.z, P.a);}
}

void clearBackground(){     //clear background color
    int x, y;
    for(y=0;y<GLSH;y += 8){
        for(x=0;x<GLSW;x += 8){
             pixel(x,y,getColor(8));
        }
    }
}

void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2){ //clip line
    float da = *y1;                 //distance plane -> point a
    float db = y2;                  //distance plane -> point b
    float d = da - db;
    if(d == 0){                     //intersection factor (between 0 and 1)
        d = 1;
    }
    float s = da / (da - db);
    *x1 = *x1 + s * (x2 - (*x1));
    *y1 = *y1 + s * (y2 - (*y1));
    if(*y1 == 0){                   //prevent divide by 0
        *y1 = 1;
    }
    *z1 = *z1 + s * (z2 - (*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, color c, int s){
    int x, y;
    //hold difference in distance
    int dyb = b2 - b1;              //y distance of bottom line
    int dyt = t2 - t1;              //y distance of top line
    int dx  = x2 - x1;              //x distance
    if(dx == 0){
        dx = 1;
    }
    int xs = x1;                    //hold initial x1 starting position
    //CLIP X
    if(x1 < 1){//clip left
        x1 = 1;
    }
    if(x2 < 1){//clip left
        x2 = 1;
    }
    if(x1 > SW - 1){//clip right
        x1 = SW - 1;
    }
    if(x2 > SW - 1){//clip right
        x2 = SW-1;
    }
    //draw vertical lines
    for(x = x1; x < x2; x++){       //the Y start and end point
        int y1 = dyb * (x - xs + 0.5) / dx + b1;//y bottom point
        int y2 = dyt * (x - xs + 0.5) / dx + t1;//y top point
        //CLIP Y
        if(y1 < 1){
            y1 = 1;
        }
        if(y2 < 1){
            y2 = 1;
        }
        if(y1 > SH - 1){
            y1 = SH - 1;
        }
        if(y2 > SH - 1){
            y2 = SH - 1;
        }
        if(S[s].surface == 1){  //save bottom points
            S[s].surf[x] = y1;
            continue;
        }
        if(S[s].surface == 2){  //save top points
            S[s].surf[x] = y2;
            continue;
        }
        if(S[s].surface == -1){ //bottom
            for(y = S[s].surf[x]; y < y1; y++){
                pixel(x, y, S[s].floorColor);
            }
        }
        if(S[s].surface == -2){ //top
            for(y = y1; y < S[s].surf[x]; y++){
                pixel(x, y, S[s].ceilingColor);
            }
        }
        for(y = y1; y < y2; y++){
            pixel(x, y, c);
        }
    }
}

void draw3D(){
    int s, w;
    int loop;
    int wx[4], wy[4], wz[4];                //local xyz variables
    float CS = M.cos[P.a], SN = M.sin[P.a]; //sin/cos values based on player rotation
    //order sectors by distance
    for(s = 0; s < numSect - 1; s++){
        for(w = 0; w < numSect - s - 1; w++){
            if(S[w].dist < S[w + 1].dist){
                sector st = S[w];
                S[w] = S[w + 1];
                S[w + 1] = st;
            }
        }
    }
    //draw sectors
    for(s = 0; s < numSect; s++){
        S[s].dist = 0;                      //clears distance
        if(P.pos.z < S[s].floor){
            S[s].surface = 1;               //bottom surface
        }
        else if(P.pos.z > S[s].ceiling){
            S[s].surface = 2;               //top surface
        }
        else{
            S[s].surface = 0;               //no surface
        }
        for(loop = 0; loop < 2; loop++){
            for(w = 0; w < S[s].numWalls; w++){
                //offset bottom 2 points by player
                int x1 = S[s].walls[w].a.x - P.pos.x, y1 = S[s].walls[w].a.y - P.pos.y;
                int x2 = S[s].walls[w].b.x - P.pos.x, y2 = S[s].walls[w].b.y - P.pos.y;
                if(loop == 0){
                    int swp = x1;
                        x1 = x2;
                        x2 = swp;

                        swp = y1;
                        y1 = y2;
                        y2 = swp;
                }
                //world X position
                wx[0] = (x1 * CS) - (y1 * SN);
                wx[1] = (x2 * CS) - (y2 * SN);
                wx[2] = wx[0];                          //top line has same x
                wx[3] = wx[1];
                //world Y position
                wy[0] = (y1 * CS) + (x1 * SN);
                wy[1] = (y2 * CS) + (x2 * SN);
                wy[2] = wy[0];                          //top line has same y
                wy[3] = wy[1];
                S[s].dist += dist(0, 0, (wx[0] + wx[1]) / 2, wy[0] + wy[1] / 2); //store this wall distance
                //world Z height
                wz[0] = S[s].floor - P.pos.z + ((P.l * wy[0]) / 32.0);
                wz[1] = S[s].floor - P.pos.z + ((P.l * wy[1]) / 32.0);
                wz[2] = wz[0] + S[s].ceiling;                     //top line has new z
                wz[3] = wz[1] + S[s].ceiling;
                //dont draw if behind player
                if(wy[0] < 1 && wy[1] < 1){
                    continue;                             //wall behind player, don't draw
                }
                //point 1 behind player, clip
                if(wy[0] < 1){
                    clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]);  //bottom line
                    clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]);  //top line
                }
                //point 2 behind player, clip
                if(wy[1] < 1){
                    clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]);  //bottom line
                    clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]);  //top line
                }

                //screen x, screen y position
                wx[0] = wx[0] * 200 / wy[0] + SW2;
                wy[0] = wz[0] * 200 / wy[0] + SH2;
                wx[1] = wx[1] * 200 / wy[1] + SW2;
                wy[1] = wz[1] * 200 / wy[1] + SH2;
                wx[2] = wx[2] * 200 / wy[2] + SW2;
                wy[2] = wz[2] * 200 / wy[2] + SH2;
                wx[3] = wx[3] * 200 / wy[3] + SW2;
                wy[3] = wz[3] * 200 / wy[3] + SH2;
                //draw points
                drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], S[s].walls[w].wallColor, s);
            }
            if(S[s].dist != 0){
                S[s].dist = S[s].dist / (0 - S[s].numWalls);    //find average sector distance
            }
            S[s].surface *= -1;             //flip to negative to draw surface
        }
    }
}

void display(){
    int x, y;
    if(T.fr1 - T.fr2 >= 100){                        //only draw 20 frames/second
        clearBackground();
        movePlayer();
        draw3D();

        T.fr2 = T.fr1;
        //glutSwapBuffers();
        //glutReshapeWindow(GLSW, GLSH);             //prevent window scaling
    }

    T.fr1 = SDL_GetTicks();
    //T.fr1 = glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
    //glutPostRedisplay();
}

void KeysDown(unsigned char key,int x,int y)
{
 if(key=='w'==1){ K.w =1;}
 if(key=='s'==1){ K.s =1;}
 if(key=='a'==1){ K.a =1;}
 if(key=='d'==1){ K.d =1;}
 if(key=='m'==1){ K.m =1;}
 if(key==','==1){ K.sr=1;}
 if(key=='.'==1){ K.sl=1;}
}
void KeysUp(unsigned char key,int x,int y)
{
 if(key=='w'==1){ K.w =0;}
 if(key=='s'==1){ K.s =0;}
 if(key=='a'==1){ K.a =0;}
 if(key=='d'==1){ K.d =0;}
 if(key=='m'==1){ K.m =0;}
 if(key==','==1){ K.sr=0;}
 if(key=='.'==1){ K.sl=0;}
}

int loadSectors[] = {
//wall start, wall end, z1 height, z2 height, floor color, ceiling color
0,  4,  0,  40, 2,  3,  //sector 1
4,  8,  0,  40, 4,  5,  //sector 2
8,  12, 0,  40, 6,  7,  //sector 3
12, 16, 0,  40, 0,  1,  //sector 4
};

int loadWalls[] = {
//x1, y1, x2, y2, color
0,  0,  32, 0,  0,
32, 0,  32, 32, 1,
32, 32, 0,  32, 0,
0,  32, 0,  0,  1,

64, 0,  96, 0,  2,
96, 0,  96, 32, 3,
96, 32, 64, 32, 2,
64, 32, 64, 0,  3,

64, 64, 96, 64, 4,
96, 64, 96, 96, 5,
96, 96, 64, 96, 4,
64, 96, 64, 64, 5,

0,  64, 32, 64, 6,
32, 64, 32, 96, 7,
32, 96, 0,  96, 6,
0,  96, 0,  64, 7,
};

bool gameInit(){
    bool success = true;
    int x;

    for(x = 0; x < 360; x++){               //precalculate sin cos in degrees
        M.cos[x] = cos(x/180.0 * M_PI);
        M.sin[x] = sin(x/180.0 * M_PI);
    }

    P.pos.x = 70.0f; P.pos.y = 110.0f; P.pos.z = 20.0f; P.a = 0; P.l = 0;

    int v1 = 0, v2 = 0;
    for(int s = 0; s < numSect; s++){
        S[s].numWalls = loadSectors[v1 + 1] - loadSectors[v1];
        S[s].walls = (wall *)malloc(sizeof(wall) * S[s].numWalls);
        S[s].floor = loadSectors[v1 + 2];                       //sector bottom height
        S[s].ceiling = loadSectors[v1 + 3];                     //sector top height
        S[s].floorColor = getColor(loadSectors[v1 + 4]);        //sector top color
        S[s].ceilingColor = getColor(loadSectors[v1 + 5]);      //sector bottom color
        v1 += 6;
        for(int w = 0; w < S[s].numWalls; w++){
            S[s].walls[w].a.x = loadWalls[v2 + 0];                       //bottom x1
            S[s].walls[w].a.y = loadWalls[v2 + 1];                       //bottom y1
            S[s].walls[w].b.x = loadWalls[v2 + 2];                       //top x2
            S[s].walls[w].b.y = loadWalls[v2 + 3];                       //top y2
            S[s].walls[w].wallColor = getColor(loadWalls[v2 + 4]);       //wall color
            v2 += 5;
        }
    }

    return success;
}

bool initGL(){
    bool success = true;
    GLenum error = GL_NO_ERROR;

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR ){
        //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        success = false;
    }

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR ){
        //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        success = false;
    }
	 //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR ){
        //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        success = false;
    }

    return success;
}

bool init(){
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else{
        //Use OpenGL 2.1
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

        //Create window
        window = SDL_CreateWindow( "bsp renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    GLSW, GLSH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( window == NULL ){
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
		else{
            //Create context
            context = SDL_GL_CreateContext( window );
            if( context == NULL ){
                printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else{
                //Use Vsync
                if( SDL_GL_SetSwapInterval( 1 ) < 0 ){
                    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
                }

                //Initialize OpenGL
                if( !initGL() ){
                    printf( "Unable to initialize OpenGL!\n" );
                    success = false;
                }
            }
        }
    }

    return success;
}

void close(){
	//Destroy window
	SDL_DestroyWindow( window );
	window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(!init()){
        printf("Failed to initialize!\n");
    }
    else{
        //Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event event;

        while(!quit){ //game loop
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {//will be used for camera movement
                        //int w,s,a,d;           //SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT
                        //int sl,sr;             //strafe left, right
                        //int m;                 //move up, down, look up, down
                        case SDLK_UP:

                        break;
                        case SDLK_DOWN:

                        break;
                        case SDLK_LEFT:

                        break;
                        case SDLK_RIGHT:

                        break;

                        case SDLK_LESS:

                        break;
                        case SDLK_GREATER:

                        break;
                    }
                }
            }

            //glClear(GL_COLOR_BUFFER_BIT);
            //draw here
            display();

            SDL_GL_SwapWindow( window );
        }
    }

    close();
	return 0;
}
