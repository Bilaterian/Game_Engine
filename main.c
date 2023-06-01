#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "data_types.h"
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

int main(int argc, char* argv[]) {

    if(!init()){
        printf("Failed to initialize!\n");
    }
    else{
        //Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event event;

		//Enable text input
		//SDL_StartTextInput();

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
                //draw here


            }
        }
    }
    close();
	return 0;
}
