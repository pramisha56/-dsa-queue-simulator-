#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#define LEFT_TURN_THRESHOLD 500.0  // The threshold for making the left turn
#define TURN_DISTANCE 50.0        // Distance to move after making the left turn
#define TURN_ANGLE 90             // The angle of the left turn
#define PRIORITY_LANE_THRESHOLD 10 // Number of vehicles to activate priority lane

const int WIDTH = 800, HEIGHT = 800;

typedef struct {
    int x, y;   // Position on screen
    int state;  // 0 = Red, 1 = Green
} TrafficLight;

typedef struct {
    float x, y;   // Position of vehicle
    float speed;  // Speed of vehicle
    int lane;     // Lane index (0 = AL1, 1 = AL2, 2 = AL3, etc.)
    int hasTurnedLeft;  // Flag to track if the vehicle has turned left
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
    int count; // To track the number of vehicles in the queue
} Queue;

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    q->count = 0;
}

void enqueue(Queue* q, Vehicle v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vehicle = v;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;  // Increment vehicle count
}

void dequeue(Queue* q) {
    if (q->front == NULL) return;
    Node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->count--;  // Decrement vehicle count
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

// Function to read vehicles from laneA.txt and update queue
void updateVehicleQueueFromFile(Queue* q, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening %s for reading.\n", filename);
        return;
    }

    // Clear the queue before adding new vehicles
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }

    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        Vehicle v;
        int lane;
        if (sscanf(line, "%d,%f,%f,%f", &lane, &v.x, &v.y, &v.speed) == 4) {
            v.lane = lane;  // Assign lane from file data
            v.hasTurnedLeft = 0;  // Initialize the left turn flag
            enqueue(q, v);
        }
    }
    fclose(fp);
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
    SDL_FRect rect = {vehicle.x, vehicle.y, 20, 10}; // Adjusted vehicle size (smaller)
    SDL_RenderFillRect(renderer, &rect);
}

void moveVehicleLeftTurn(Vehicle* vehicle) {
    // Gradually change the vehicle's x and y position to simulate left turn
    vehicle->x -= TURN_DISTANCE * 0.5;  // Move left
    vehicle->y += TURN_DISTANCE * 0.5;  // Move downwards after left turn
    if (vehicle->x <= 300.0f && vehicle->y >= 350.0f) {
        vehicle->hasTurnedLeft = 1;  // Mark vehicle as having turned left
    }
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

    // Initialize traffic lights for 4 directions
    TrafficLight lights[4] = {
        {270, 250, 0}, {530, 250, 1}, {270, 530, 0}, {530, 530, 0}
    };

    Uint32 lastSwitchTime = SDL_GetTicks();
    int currentGreen = 1;
    Queue vehicleQueueA, vehicleQueueB, vehicleQueueC, vehicleQueueD;
    initQueue(&vehicleQueueA);
    initQueue(&vehicleQueueB);
    initQueue(&vehicleQueueC);
    initQueue(&vehicleQueueD);

    SDL_Event event;
    int running = 1;
    static Uint32 lastUpdateTime = 0;  // Add this to update the vehicle queue at intervals

    // Main loop (focus on vehicle movement and queue management)
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
        }

        // Switch the traffic light every 2 seconds
        if (SDL_GetTicks() - lastSwitchTime > 2000) {
            lights[currentGreen].state = 0;  // Set current green light to red
            currentGreen = (currentGreen + 1) % 4;  // Move to the next light
            lights[currentGreen].state = 1;  // Set new light to green
            lastSwitchTime = SDL_GetTicks();
        }

        // Read laneA.txt to update vehicles in the queue every 2 seconds
        if (SDL_GetTicks() - lastUpdateTime > 5000) {
            updateVehicleQueueFromFile(&vehicleQueueA, "laneA.txt");
            updateVehicleQueueFromFile(&vehicleQueueB, "laneB.txt");
            updateVehicleQueueFromFile(&vehicleQueueC, "laneC.txt");
            updateVehicleQueueFromFile(&vehicleQueueD, "laneD.txt");
            lastUpdateTime = SDL_GetTicks();
        }

        // Rendering section
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Grass background
        SDL_RenderClear(renderer);

        // Draw roads and lane markings 
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Gray for roads
        SDL_FRect horizontalRoad = {0.0f, 300.0f, 800.0f, 200.0f};
        SDL_RenderFillRect(renderer, &horizontalRoad);
        SDL_FRect verticalRoad = {300.0f, 0.0f, 200.0f, 800.0f};
        SDL_RenderFillRect(renderer, &verticalRoad);

        // --- Draw Lane Markings ---
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for lane markings
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

        // --- Draw the Priority Lane (Dashed Yellow Line) ---
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color for dashed line
        float priorityLaneX = 366.67f + 33.33f;
        for (float y = 0.0f; y < 300.0f; y += 40.0f) {
            SDL_FRect dash = {priorityLaneX, y, 5.0f, 20.0f};
            SDL_RenderFillRect(renderer, &dash);
        }

        // --- Vehicle Queue Processing and Rendering ---
        Node* temp;

        // Process vehicles from queueA (Road A)
        temp = vehicleQueueA.front;
        while (temp) {
            if (temp->vehicle.lane == 1) {  // AL2 (second lane)
                if (lights[0].state == 1) {  // Green Light for Road A
                    temp->vehicle.x += temp->vehicle.speed;  // Move straight
                }
            } else if (temp->vehicle.lane == 2) {  // AL3 (third lane)
                // Free lane, always allow left turn
                if (!temp->vehicle.hasTurnedLeft) {
                    moveVehicleLeftTurn(&temp->vehicle);
                }
            }

            if (temp->vehicle.hasTurnedLeft) {
                dequeue(&vehicleQueueA);
            }

            renderVehicle(renderer, temp->vehicle);
            temp = temp->next;
        }

        // Process vehicles from queueB (Road B)
        temp = vehicleQueueB.front;
        while (temp) {
            if (temp->vehicle.lane == 1) {  // BL2 (second lane)
                if (lights[1].state == 1) {  // Green Light for Road B
                    temp->vehicle.x -= temp->vehicle.speed;  // Move straight
                }
            } else if (temp->vehicle.lane == 2) {  // BL3 (third lane)
                // Free lane, always allow left turn
                if (!temp->vehicle.hasTurnedLeft) {
                    moveVehicleLeftTurn(&temp->vehicle);
                }
            }

            if (temp->vehicle.hasTurnedLeft) {
                dequeue(&vehicleQueueB);
            }

            renderVehicle(renderer, temp->vehicle);
            temp = temp->next;
        }

        // Process vehicles from queueC (Road C)
        temp = vehicleQueueC.front;
        while (temp) {
            if (temp->vehicle.lane == 1) {  // CL2 (second lane)
                if (lights[2].state == 1) {  // Green Light for Road C
                    temp->vehicle.x -= temp->vehicle.speed;  // Move straight
                }
            } else if (temp->vehicle.lane == 2) {  // CL3 (third lane)
                // Free lane, always allow left turn
                if (!temp->vehicle.hasTurnedLeft) {
                    moveVehicleLeftTurn(&temp->vehicle);
                }
            }

            if (temp->vehicle.hasTurnedLeft) {
                dequeue(&vehicleQueueC);
            }

            renderVehicle(renderer, temp->vehicle);
            temp = temp->next;
        }

        // Process vehicles from queueD (Road D)
        temp = vehicleQueueD.front;
        while (temp) {
            if (temp->vehicle.lane == 1) {  // DL2 (second lane)
                if (lights[3].state == 1) {  // Green Light for Road D
                    temp->vehicle.x += temp->vehicle.speed;  // Move straight
                }
            } else if (temp->vehicle.lane == 2) {  // DL3 (third lane)
                // Free lane, always allow left turn
                if (!temp->vehicle.hasTurnedLeft) {
                    moveVehicleLeftTurn(&temp->vehicle);
                }
            }

            if (temp->vehicle.hasTurnedLeft) {
                dequeue(&vehicleQueueD);
            }

            renderVehicle(renderer, temp->vehicle);
            temp = temp->next;
        }

        // Render all traffic lights
        for (int i = 0; i < 4; i++) {
            renderTrafficLight(renderer, lights[i]);
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}