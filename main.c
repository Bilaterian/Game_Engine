#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
#include <ctype.h>

#include "data_types.h"
#include "functions.h"

bool init();
bool initGL();
void close();

SDL_Window* window = NULL;
SDL_GLContext context;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
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
                        case SDLK_UP:

                        break;
                        case SDLK_DOWN:

                        break;
                        case SDLK_LEFT:

                        break;
                        case SDLK_RIGHT:

                        break;
                    }
                }
            }

            glClear(GL_COLOR_BUFFER_BIT);
            //draw here
            glFlush();

            SDL_GL_SwapWindow( window );
        }
    }
    close();
	return 0;
}
