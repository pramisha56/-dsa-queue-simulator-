#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

const int WIDTH = 800, HEIGHT = 800;

int main(int argc, char *argv[])
{
    // Initialize SDL
  if (!SDL_Init( SDL_INIT_VIDEO)) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL Window
    SDL_Window *window = SDL_CreateWindow("trafficLight",800,800,0);

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

    // Game Loop
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) { // Close button pressed
                running = 0;
            }
        }

         // Set background color (green for grass)
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Green for grass
        SDL_RenderClear(renderer);

        // Set road color (gray)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark Gray for roads

        // Horizontal road
        SDL_FRect horizontalRoad = {0.0f, 300.0f , 800.0f,200.0f }; // Make it wider
        SDL_RenderFillRect(renderer, &horizontalRoad); // Pass SDL_FRect

        // Vertical road
        SDL_FRect verticalRoad = {300.0f,0.0f, 200.0f,800.0f}; // Full height road
        SDL_RenderFillRect(renderer, &verticalRoad); // Pass SDL_FRect

        // Render the shapes
        SDL_RenderPresent(renderer);

        // Add a small delay to see the result
        SDL_Delay(16); // Delay to control frame rate (~60 FPS)
    }


    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
