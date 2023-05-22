#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_PARTICLES = 255;

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    Vector3 position;
    Vector3 velocity;
} Particle;

typedef struct {
    Particle particles[1];
    int count;
} Object;

void renderObject(Object* object, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for objects
    int size = 100;
    for (int i = 0; i < object->count; i++) {
        int x = (int)object->particles[i].position.x;
        int y = (int)object->particles[i].position.y;
        int size = 10; // Adjust the size of the rendered object
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
    object->count = 1;
    Uint32 previousTicks = SDL_GetTicks();
    bool quit = false;
    // Game loop will go here
    while (quit == false) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                    object->particles[0].velocity.y -= 0.1f; // Adjust the velocity change as needed
                    break;
                    case SDLK_DOWN:
                    object->particles[0].velocity.y += 0.1f;
                    break;
                    case SDLK_LEFT:
                    object->particles[0].velocity.x -= 0.1f;
                    break;
                    case SDLK_RIGHT:
                    object->particles[0].velocity.x += 0.1f;
                    break;
                }
            }

            Uint32 currentTicks = SDL_GetTicks();
            float deltaTime = (currentTicks - previousTicks) / 1000.0f; // Convert to seconds
            previousTicks = currentTicks;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            // Update the physics simulation
            //updateVelocity(object, deltaTime);
            updatePosition(object, deltaTime);

            renderObject(object, renderer);

            SDL_RenderPresent(renderer);
        }

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
