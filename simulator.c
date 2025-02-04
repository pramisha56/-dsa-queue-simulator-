#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

const int WIDTH = 800, HEIGHT = 800;

typedef struct {
    int x, y;   // Position on screen
    int state;  // 0 = Red, 1 = Green
} TrafficLight;

typedef struct {
    float x, y;   // Position of vehicle
    float speed;  // Speed of vehicle
    int lane;     // Lane index (matches traffic light index)
} Vehicle;

// Queue node
typedef struct Node {
    Vehicle vehicle;
    struct Node* next;
} Node;

// Queue structure
typedef struct {
    Node* front;
    Node* rear;
} Queue;

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

void enqueue(Queue* q, Vehicle v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vehicle = v;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

void dequeue(Queue* q) {
    if (q->front == NULL) return;
    Node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

void renderTrafficLight(SDL_Renderer *renderer, TrafficLight light) {
    if (light.state) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    }
    SDL_FRect rect = {light.x, light.y, 30, 50}; // Traffic light size
    SDL_RenderFillRect(renderer, &rect);
}

void renderVehicle(SDL_Renderer *renderer, Vehicle vehicle) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for vehicles
    SDL_FRect rect = {vehicle.x, vehicle.y, 40, 20}; // Vehicle size
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Traffic Simulation", 800, 800, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) {
        printf("Initialization failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    TrafficLight lights[4] = {
        {270, 250, 0}, {530, 250, 1}, {270, 530, 0}, {530, 530, 0}
    };

    Uint32 lastSwitchTime = SDL_GetTicks();
    int currentGreen = 1;
    Queue vehicleQueue;
    initQueue(&vehicleQueue);

    enqueue(&vehicleQueue, (Vehicle){100, 350, 2.0f, 0});
    enqueue(&vehicleQueue, (Vehicle){550, 550, 1.8f, 1});
    enqueue(&vehicleQueue, (Vehicle){300, 100, 2.5f, 2});

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
        }
        if (SDL_GetTicks() - lastSwitchTime > 5000) {
            lights[currentGreen].state = 0;
            currentGreen = (currentGreen + 1) % 4;
            lights[currentGreen].state = 1;
            lastSwitchTime = SDL_GetTicks();
        }

        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_FRect horizontalRoad = {0.0f, 300.0f, 800.0f, 200.0f};
        SDL_RenderFillRect(renderer, &horizontalRoad);
        SDL_FRect verticalRoad = {300.0f, 0.0f, 200.0f, 800.0f};
        SDL_RenderFillRect(renderer, &verticalRoad);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 1; i <= 2; i++) {
            float laneY = 300.0f + (i * 200.0f / 3);
            SDL_FRect leftLane = {0.0f, laneY, 300.0f, 2.0f};
            SDL_RenderFillRect(renderer, &leftLane);
            SDL_FRect rightLane = {500.0f, laneY, 300.0f, 2.0f};
            SDL_RenderFillRect(renderer, &rightLane);
        }
        for (int i = 1; i <= 2; i++) {
            float laneX = 300.0f + (i * 200.0f / 3);
            SDL_FRect upperLane = {laneX, 0.0f, 2.0f, 300.0f};
            SDL_RenderFillRect(renderer, &upperLane);
            SDL_FRect lowerLane = {laneX, 500.0f, 2.0f, 300.0f};
            SDL_RenderFillRect(renderer, &lowerLane);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        float priorityLaneX = 366.67f + (33.33f);
        for (float y = 0.0f; y < 300.0f; y += 40.0f) {
            SDL_FRect dash = {priorityLaneX, y, 5.0f, 20.0f};
            SDL_RenderFillRect(renderer, &dash);
        }
        
        for (int i = 0; i < 4; i++) {
            renderTrafficLight(renderer, lights[i]);
        }

        Node* temp = vehicleQueue.front;
        while (temp) {
            if (lights[temp->vehicle.lane].state == 1) {
                temp->vehicle.x += temp->vehicle.speed;
                if (temp->vehicle.x > WIDTH) dequeue(&vehicleQueue);
            }
            renderVehicle(renderer, temp->vehicle);
            temp = temp->next;
        }

        SDL_Delay(16);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}