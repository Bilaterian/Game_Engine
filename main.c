#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
#include <ctype.h>

#include "data_types.h"
#include "functions.h"

#define res        1                        //0=160x120 1=360x240 4=640x480
#define SW         160*res                  //screen width
#define SH         120*res                  //screen height
#define SW2        (SW/2)                   //half of screen width
#define SH2        (SH/2)                   //half of screen height
#define pixelScale 4/res                    //OpenGL pixel scale
#define GLSW       (SW*pixelScale)          //OpenGL window width
#define GLSH       (SH*pixelScale)          //OpenGL window height
#define numSect    4
#define numWall    16

SDL_Window* window = NULL;
SDL_GLContext context;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

math M;
player P;
sector S[4];
time T;

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

void pixel(int x,int y, int c){
    color colour = getColor(int c);
    //alter how pixels are drawn here
    glColor3ub(colour.r, colour.g, colour.b);
    glBegin(GL_POINTS);
    glVertex2i((x * pixelScale) + 2, (y * pixelScale) + 2);
    glEnd();
}

void clearBackground(){     //clear background color
    int x, y;
    for(y=0;y<SH;y++){
        for(x=0;x<SW;x++){
            pixel(x,y,8);
        }
    }
}

void display(){
    int x,y;
    if(T.fr1 - T.fr2 >= 50){                        //only draw 20 frames/second
        clearBackground();
        movePlayer();
        draw3D();

        T.fr2 = T.fr1;
        glutSwapBuffers();
        glutReshapeWindow(GLSW, GLSH);             //prevent window scaling
    }

    T.fr1 = glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
    glutPostRedisplay();
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

    P.xyz.x = 70.0f; P.xyz.y = 110.0f; P.xyz.z = 20.0f; P.a = 0; P.l = 0;

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
        window = SDL_CreateWindow( "bsp renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
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

            glClear(GL_COLOR_BUFFER_BIT);
            //draw here
            display();
            glFlush();

            SDL_GL_SwapWindow( window );
        }
    }
    close();
	return 0;
}
