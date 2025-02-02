#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

const int WIDTH = 800, HEIGHT = 800;

typedef struct {
    int x, y;   // Position on screen
    int state;  // 0 = Red, 1 = Green
} TrafficLight;


void renderTrafficLight(SDL_Renderer *renderer, TrafficLight light) {
    if (light.state) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    }

    SDL_FRect rect = {light.x, light.y, 30, 50}; // Traffic light size
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL Window
    SDL_Window *window = SDL_CreateWindow("trafficLight", 800, 800, 0);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create Renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TrafficLight lights[4] = {
    {270, 250, 0}, // Light for Road A
    {530, 250, 1}, // Light for Road B
    {270, 530, 0}, // Light for Road C
    {530, 530, 0}  // Light for Road D
    };

    Uint32 lastSwitchTime = SDL_GetTicks(); // Store time to switch lights
    int currentGreen = 1; // Start with road B green

    SDL_Event event;
    int running = 1;
 while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) { // Close button pressed
            running = 0;
        }
    }

    if (SDL_GetTicks() - lastSwitchTime > 5000) { // Change light every 5 sec
        lights[currentGreen].state = 0; // Turn off current green light
        currentGreen = (currentGreen + 1) % 4; // Move to next light
        lights[currentGreen].state = 1; // Turn it green
        lastSwitchTime = SDL_GetTicks(); // Reset timer
    }

    // Set background color (green for grass)
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);

    // Set road color (gray)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

    // Draw horizontal road
    SDL_FRect horizontalRoad = {0.0f, 300.0f, 800.0f, 200.0f};
    SDL_RenderFillRect(renderer, &horizontalRoad);

    // Draw vertical road
    SDL_FRect verticalRoad = {300.0f, 0.0f, 200.0f, 800.0f};
    SDL_RenderFillRect(renderer, &verticalRoad);

    // Set lane line color (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Horizontal road lanes
    for (int i = 1; i <= 2; i++) {
        float laneY = 300.0f + (i * 200.0f / 3);
        SDL_FRect leftLane = {0.0f, laneY, 300.0f, 2.0f};
        SDL_RenderFillRect(renderer, &leftLane);
        SDL_FRect rightLane = {500.0f, laneY, 300.0f, 2.0f};
        SDL_RenderFillRect(renderer, &rightLane);
    }

    // Vertical road lanes
    for (int i = 1; i <= 2; i++) {
        float laneX = 300.0f + (i * 200.0f / 3);
        SDL_FRect upperLane = {laneX, 0.0f, 2.0f, 300.0f};
        SDL_RenderFillRect(renderer, &upperLane);
        SDL_FRect lowerLane = {laneX, 500.0f, 2.0f, 300.0f};
        SDL_RenderFillRect(renderer, &lowerLane);
    }

    // Draw priority lane (dashed yellow line)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    float priorityLaneX = 366.67f + (33.33f); 
    for (float y = 0.0f; y < 300.0f; y += 40.0f) { // Dashed effect
        SDL_FRect dash = {priorityLaneX, y, 5.0f, 20.0f}; 
        SDL_RenderFillRect(renderer, &dash);
    }

    // ✅ Now draw traffic lights AFTER drawing the roads!
    for (int i = 0; i < 4; i++) {
        renderTrafficLight(renderer, lights[i]);
    }

    // Render everything
    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60 FPS
}

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
