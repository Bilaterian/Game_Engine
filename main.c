#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "data_types.h"

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

typedef struct {
    vector3 position;
    vector3 velocity;
} Particle;


int main(int argc, char* argv[]) {

    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raycaster",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_GLContext Context = SDL_GL_CreateContext(window);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    time_t t;
    srand((unsigned) time(&t));

    wall *walls = (wall *)malloc(sizeof(wall) * 5);

    for(int i = 0; i < 5; i++){
        walls[i].a.x = (float)(rand() % 256) - 128.0;
        walls[i].a.y = (float)(rand() % 256) - 128.0;
        walls[i].b.x = (float)(rand() % 256) - 128.0;
        walls[i].b.y = (float)(rand() % 256) - 128.0;

        walls[i].portal = 0;
        walls[i].id = i;
    }


    Uint32 previousTicks = SDL_GetTicks();
    bool quit = false;
    // Game loop will go here
    while (quit == false) {
        vector3 camera;
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

            Uint32 currentTicks = SDL_GetTicks();
            float deltaTime = (currentTicks - previousTicks) / 1000.0f; // Convert to seconds
            previousTicks = currentTicks;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            //draw here

            SDL_RenderPresent(renderer);
        }

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
