#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include "data_types.h"

const int MAX_PARTICLES = 255;


typedef struct {
    vector3 position;
    vector3 velocity;
} Particle;

typedef struct {
    Particle particles[1];
    int count;
} Object;

void renderObject(Object* object, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for objects
    int size = 10;
    for (int i = 0; i < object->count; i++) {
        int x = (int)object->particles[i].position.x;
        int y = (int)object->particles[i].position.y;
        SDL_Rect rect = { x, y, size, size };
        SDL_RenderFillRect(renderer, &rect);
    }
}


void updatePosition(Object* object, float deltaTime) {
    for (int i = 0; i < object->count; i++) {
        object->particles[i].position.x += object->particles[i].velocity.x * deltaTime;
        object->particles[i].position.y += object->particles[i].velocity.y * deltaTime;
        object->particles[i].position.z += object->particles[i].velocity.z * deltaTime;
    }
}

void updateVelocity(Object* object, float deltaTime) {
    // Update velocity based on acceleration, collisions, etc.
    for (int i = 0; i < object->count; i++) {
        object->particles[i].velocity.x *= 0.3f * deltaTime;
        object->particles[i].velocity.y *= 0.3f * deltaTime;
        object->particles[i].velocity.z *= 0.3f * deltaTime;
    }
}

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Physics Engine",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Object *object = (Object *)malloc(sizeof(Object));

    {
        object->count = 1;
        object->particles[0].position.x = 0;
        object->particles[0].position.y = 0;
    }

    time_t t;
    srand((unsigned) time(&t));

    wall *walls = (wall *)malloc(sizeof(wall) * 5);

    for(int i = 0; i < 5; i++){
        walls[i].a.x = (float)(rand() % 256);
        walls[i].a.y = (float)(rand() % 256);
        walls[i].b.x = (float)(rand() % 256);
        walls[i].b.y = (float)(rand() % 256);

        walls[i].portal = 0;
        walls[i].id = i;
    }

    for(int i = 0; i < 5; i++){
        printf("(%f, %f), (%f, %f)\n", walls[i].a.x, walls[i].a.y, walls[i].b.x, walls[i].b.y);
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
                        camera.y += 1.0;
                    break;
                    case SDLK_DOWN:
                        camera.y -= 1.0;
                    break;
                    case SDLK_LEFT:
                        camera.x -= 1.0;
                    break;
                    case SDLK_RIGHT:
                        camera.x += 1.0;
                    break;
                }
            }

            Uint32 currentTicks = SDL_GetTicks();
            float deltaTime = (currentTicks - previousTicks) / 1000.0f; // Convert to seconds
            previousTicks = currentTicks;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            //Update the physics simulation
            //updateVelocity(object, deltaTime);
            //updatePosition(object, deltaTime);

            renderObject(object, renderer);

            SDL_RenderPresent(renderer);
        }

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
