#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#define LEFT_TURN_THRESHOLD 310.0  // The threshold for making the left turn
#define TURN_DISTANCE 50.0        // Distance to move after making the left turn
#define TURN_ANGLE 90             // The angle of the left turn
#define PRIORITY_LANE_THRESHOLD 10 // Number of vehicles to activate priority lane
#define DISTANCE_BETWEEN_VEHICLES 40.0
#define MAX_NUMBER_OF_VEHICLES 7

const int WIDTH = 800, HEIGHT = 800;

typedef struct {
    int x, y;   // Position on screen
    int state;  // 0 = Red, 1 = Green
} TrafficLight;

typedef struct {
    float x, y;   // Position of vehicle
    float speed;
    int road;  
    int lane;     // Lane index (0 = AL1, 1 = AL2, 2 = AL3, etc.)
    int hasTurnedLeft;  // Flag to track if the vehicle has turned left
} Vehicle;

void renderVehicle(SDL_Renderer *renderer, Vehicle vehicle);
void renderTrafficLight(SDL_Renderer *renderer, TrafficLight light);

// Queue node
typedef struct Node {
    Vehicle vehicle;
    struct Node* next;
    struct Node* prev;
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

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

int isQueueFull(Queue* q) {
    return q->count == MAX_NUMBER_OF_VEHICLES;
}

void enqueue(Queue* q, Vehicle v) {
    if (!isQueueFull(q)) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->vehicle = v;
        newNode->next = NULL;
        if (q->rear == NULL) {
            q->front = q->rear = newNode;
        } else {
            q->rear->next = newNode;
            newNode->prev = q->rear;
            q->rear = newNode;
        }
        q->count++;  // Increment vehicle count
    } 
    else {
        return;
    }
}

void dequeue(Queue* q) {
    if (q->front == NULL) return;
    Node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->count--;  // Decrement vehicle count
}

// Function to read vehicles from .txt file and update queue
void updateVehicleQueueFromFile(Queue* q, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening %s for reading.\n", filename);
        return;
    }

    char line[1000000];
    while (fgets(line, sizeof(line), fp)) {
        Vehicle v;
        if (sscanf(line, "%d,%d,%f,%f,%f", &v.road, &v.lane, &v.x, &v.y, &v.speed) == 5) {
            if (v.lane == 2) {
                enqueue(&q[1], v);
            }
            else if (v.lane == 3) {
                enqueue(&q[2], v);
            }
        }
    }
    fclose(fp);

    // Clear the file after reading the vehicles
    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening %s for writing.\n", filename);
        return;
    }
    fclose(fp);
}

// / Rest of your code (queue functions, etc.)

void renderTrafficLight(SDL_Renderer *renderer, TrafficLight light) {
    // Draw the traffic light box
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark gray for the box
    SDL_FRect box = {(float)light.x, (float)light.y, 30.0f, 80.0f}; // Use SDL_FRect
    SDL_RenderFillRect(renderer, &box);

    // Draw the small stand below the traffic light box
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray for the stand
    SDL_FRect stand = {(float)light.x + 10.0f, (float)light.y + 80.0f, 10.0f, 20.0f}; // Stand dimensions
    SDL_RenderFillRect(renderer, &stand);

    // Draw the lights (circles)
    if (light.state) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green light
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red light
    }

    // Draw a circle for the light using SDL_RenderPoint
    int centerX = light.x + 15; // Center of the box
    int centerY = light.y + 20; // Position of the light
    int radius = 10; // Radius of the light

    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderPoint(renderer, centerX + w, centerY + h);
            }
        }
    }
}
void renderVehicle(SDL_Renderer *renderer, Vehicle vehicle) {
    // Main body of the vehicle (car)
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for vehicle body
    SDL_FRect rect = {vehicle.x, vehicle.y, 30.0f, 20.0f}; // Use SDL_FRect
    SDL_RenderFillRect(renderer, &rect);

    // Car roof (to make it look more like a car from above)
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255); // Darker blue for roof
    SDL_FRect roof = {vehicle.x + 5.0f, vehicle.y + 5.0f, 20.0f, 10.0f}; // Use SDL_FRect
    SDL_RenderFillRect(renderer, &roof);

    // Wheels (four small black circles/rectangles at the corners)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for wheels

    // Front-left wheel
    SDL_FRect wheel1 = {vehicle.x - 2.0f, vehicle.y - 2.0f, 6.0f, 6.0f};
    SDL_RenderFillRect(renderer, &wheel1);

    // Front-right wheel
    SDL_FRect wheel2 = {vehicle.x + 26.0f, vehicle.y - 2.0f, 6.0f, 6.0f};
    SDL_RenderFillRect(renderer, &wheel2);

    // Rear-left wheel
    SDL_FRect wheel3 = {vehicle.x - 2.0f, vehicle.y + 16.0f, 6.0f, 6.0f};
    SDL_RenderFillRect(renderer, &wheel3);

    // Rear-right wheel
    SDL_FRect wheel4 = {vehicle.x + 26.0f, vehicle.y + 16.0f, 6.0f, 6.0f};
    SDL_RenderFillRect(renderer, &wheel4);
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
        {265, 210, 0}, // Top-left
        {505, 210, 1}, // Top-right
        {265, 510, 0}, // Bottom-left
        {505, 510, 0}  // Bottom-right
    };

    Uint32 lastSwitchTime = SDL_GetTicks();
    int currentGreen = 1;
    Queue vehicleQueueA[3], vehicleQueueB[3], vehicleQueueC[3], vehicleQueueD[3];
    for (int i = 0; i < 3; i++) {
        initQueue(&vehicleQueueA[i]);
        initQueue(&vehicleQueueB[i]);
        initQueue(&vehicleQueueC[i]);
        initQueue(&vehicleQueueD[i]);
    }

    SDL_Event event;
    int running = 1;
    static Uint32 lastUpdateTime = 0;  // Add this to update the vehicle queue at intervals

    // Main loop (focus on vehicle movement and queue management)
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = 0;
        }

        // Switch the traffic light every 2 seconds
        // TODO: Need to change this logic later.
        if (SDL_GetTicks() - lastSwitchTime > 20000) {
            lights[currentGreen].state = 0;  // Set current green light to red
            currentGreen = (currentGreen + 1) % 4;  // Move to the next light
            lights[currentGreen].state = 1;  // Set new light to green
            lastSwitchTime = SDL_GetTicks();
        }

        updateVehicleQueueFromFile(vehicleQueueA, "RoadA.txt");
        updateVehicleQueueFromFile(vehicleQueueB, "RoadB.txt");
        updateVehicleQueueFromFile(vehicleQueueC, "RoadC.txt");
        updateVehicleQueueFromFile(vehicleQueueD, "RoadD.txt");
        lastUpdateTime = SDL_GetTicks();

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
        for (int i = 0; i < 3; i++) {
            temp = vehicleQueueA[i].front;
            while (temp) {
                if (temp->vehicle.lane == 2) {  // AL2 (second lane)
                    if (temp == vehicleQueueA[i].front) {
                        if (temp->vehicle.y < 290) {
                            temp->vehicle.y += temp->vehicle.speed;
                        }
                    }
                    else {
                        if (temp->vehicle.y < (temp->prev->vehicle.y) - DISTANCE_BETWEEN_VEHICLES) {
                            temp->vehicle.y += temp->vehicle.speed;
                        }
                    }
                    if (lights[0].state == 1) {  // Green Light for Road A
                        if (temp == vehicleQueueA[i].front) {
                            // Move upto the junction.
                            if (temp->vehicle.y <= 450) {
                                temp->vehicle.y += temp->vehicle.speed;
                            }
                            else {
                                if (temp->vehicle.x <= 450) {
                                    temp->vehicle.x += temp->vehicle.speed;
                                }
                                else {
                                    if (temp->vehicle.y <= 830) {
                                        temp->vehicle.y += temp->vehicle.speed;
                                    }
                                }
                            }
                        }
                        else {
                            if (temp->vehicle.y <= (temp->prev->vehicle.y) - DISTANCE_BETWEEN_VEHICLES) {
                                temp->vehicle.y += temp->vehicle.speed;
                                
                            }
                            temp->vehicle.x = temp->prev->vehicle.x;
                            // else {
                            //     if (temp->vehicle.x <= (temp->prev->vehicle.x) - DISTANCE_BETWEEN_VEHICLES) {
                            //         temp->vehicle.x += temp->vehicle.speed;
                            //     }
                            //     else {
                            //         if (temp->vehicle.y <= (temp->prev->vehicle.y) + DISTANCE_BETWEEN_VEHICLES) {
                            //             temp->vehicle.y += temp->vehicle.speed;
                            //         }
                            //     }
                            // }
                        }
                    }
                } 
                else if (temp->vehicle.lane == 3) {  // AL3 (third lane)
                    // Free lane, always allow left turn
                    if (temp->vehicle.y < LEFT_TURN_THRESHOLD) {
                        temp->vehicle.y += temp->vehicle.speed;  // Move straight
                    } else {
                        temp->vehicle.x += temp->vehicle.speed;  // Move right
                        // if (temp->vehicle.x >= 800) {
                        //     dequeue(&vehicleQueueA);
                        // }
                    }
                }
                renderVehicle(renderer, temp->vehicle);
                temp = temp->next;
            }
        }

        // Process vehicles from queueB (Road B)
        for (int i = 0; i < 3; i++) {
            temp = vehicleQueueB[i].front;
            while (temp) {
                if (temp->vehicle.lane == 2) {  // BL2 (second lane)
                    if (temp == vehicleQueueB[i].front) {
                        if (temp->vehicle.x > 480) {
                            temp->vehicle.x -= temp->vehicle.speed;
                        }
                    }
                    else {
                        if (temp->vehicle.x > (temp->prev->vehicle.x) + DISTANCE_BETWEEN_VEHICLES) {
                            temp->vehicle.x -= temp->vehicle.speed;
                        }
                    }
                    if (lights[1].state == 1) {  // Green Light for Road B
                        if (temp == vehicleQueueB[i].front) {
                            // Move upto the junction.
                            if (temp->vehicle.x >= 350) {
                                temp->vehicle.x -= temp->vehicle.speed;
                            }
                            else {
                                if (temp->vehicle.y <= 450) {
                                    temp->vehicle.y += temp->vehicle.speed;
                                }
                                else {
                                    if (temp->vehicle.x >= -30) {
                                        temp->vehicle.x -= temp->vehicle.speed;
                                    }
                                }
                            }
                        }
                        else {
                            if (temp->vehicle.x >= (temp->prev->vehicle.x) + DISTANCE_BETWEEN_VEHICLES) {
                                temp->vehicle.x -= temp->vehicle.speed;
                                
                            }
                            temp->vehicle.y = temp->prev->vehicle.y;
                            // else {
                            //     if (temp->vehicle.y <= (temp->prev->vehicle.y) - DISTANCE_BETWEEN_VEHICLES) {
                            //         temp->vehicle.y += temp->vehicle.speed;
                            //     }
                            //     else {
                            //         if (temp->vehicle.x >= (temp->prev->vehicle.x) - DISTANCE_BETWEEN_VEHICLES) {
                            //             temp->vehicle.x -= temp->vehicle.speed;
                            //         }
                            //     }
                            // }
                        }
                    }
                } else if (temp->vehicle.lane == 3) {  // BL3 (third lane)
                    // Free lane, always allow left turn
                    if (temp->vehicle.x > 460) {
                        temp->vehicle.x -= temp->vehicle.speed;  // Move straight
                    }
                    else {
                        temp->vehicle.y += temp->vehicle.speed;  // Move down
                        // if (temp->vehicle.y > 800) {
                        //     dequeue(&vehicleQueueB);
                        // }
                    }
                }

                renderVehicle(renderer, temp->vehicle);
                temp = temp->next;
            }
        }

        // Process vehicles from queueC (Road C)
        for (int i = 0; i < 3; i++) {
            temp = vehicleQueueC[i].front;
            while (temp) {
                if (temp->vehicle.lane == 2) {  // CL2 (second lane)
                    if (temp == vehicleQueueC[i].front) {
                        if (temp->vehicle.y > 500) {
                            temp->vehicle.y -= temp->vehicle.speed;
                        }
                    }
                    else {
                        if (temp->vehicle.y > (temp->prev->vehicle.y) + DISTANCE_BETWEEN_VEHICLES) {
                            temp->vehicle.y -= temp->vehicle.speed;
                        }
                    }
                    if (lights[2].state == 1) {  // Green Light for Road C
                        if (temp == vehicleQueueC[i].front) {
                            if (temp->vehicle.y >= 350) {
                                temp->vehicle.y -= temp->vehicle.speed;
                            }
                            else {
                                if (temp->vehicle.x >= 320) {
                                    temp->vehicle.x -= temp->vehicle.speed;
                                }
                                else {
                                    if (temp->vehicle.y >= -30) {
                                        temp->vehicle.y -= temp->vehicle.speed;
                                    }
                                }
                            }
                        }
                        else {
                            if (temp->vehicle.y >= (temp->prev->vehicle.y) + DISTANCE_BETWEEN_VEHICLES) {
                                temp->vehicle.y -= temp->vehicle.speed;
                                
                            }
                            temp->vehicle.x = temp->prev->vehicle.x;
                            // else {
                            //     if (temp->vehicle.x >= (temp->prev->vehicle.x) + DISTANCE_BETWEEN_VEHICLES) {
                            //         temp->vehicle.x -= temp->vehicle.speed;
                            //     }
                            //     else {
                            //         if (temp->vehicle.y >= (temp->prev->vehicle.y) - DISTANCE_BETWEEN_VEHICLES) {
                            //             temp->vehicle.y -= temp->vehicle.speed;
                            //         }
                            //     }
                            // }
                        }
                    }
                } else if (temp->vehicle.lane == 3) {  // CL3 (third lane)
                    // Free lane, always allow left turn
                    if (temp->vehicle.y > 460) {
                        temp->vehicle.y -= temp->vehicle.speed;  // Move straight
                    } else {
                        temp->vehicle.x -= temp->vehicle.speed;  // Move left
                        // if (temp->vehicle.x < -20) {
                        //     dequeue(&vehicleQueueC);
                        // }
                    }
                }

                renderVehicle(renderer, temp->vehicle);
                temp = temp->next;
            }
        }

        // Process vehicles from queueD (Road D)
        for (int i = 0; i < 3; i++) {
            temp = vehicleQueueD[i].front;
            while (temp) {
                if (temp->vehicle.lane == 2) {  // DL2 (second lane)
                    if (temp == vehicleQueueD[i].front) {
                        if (temp->vehicle.x < 290) {
                            temp->vehicle.x += temp->vehicle.speed;
                        }
                    }
                    else {
                        if (temp->vehicle.x < (temp->prev->vehicle.x) - DISTANCE_BETWEEN_VEHICLES) {
                            temp->vehicle.x += temp->vehicle.speed;
                        }
                    }
                    if (lights[3].state == 1) {  // Green Light for Road D
                        if (temp == vehicleQueueD[i].front) {
                            if (temp->vehicle.x <= 450) {
                                temp->vehicle.x += temp->vehicle.speed;
                            }
                            else {
                                if (temp->vehicle.y >= 350) {
                                    temp->vehicle.y -= temp->vehicle.speed;
                                }
                                else {
                                    if (temp->vehicle.x <= 830) {
                                        temp->vehicle.x += temp->vehicle.speed;
                                    }
                                }
                            }
                        }
                        else {
                            if (temp->vehicle.x <= (temp->prev->vehicle.x) - DISTANCE_BETWEEN_VEHICLES) {
                                temp->vehicle.x += temp->vehicle.speed;
                            }
                            temp->vehicle.y = temp->prev->vehicle.y;
                            // else {
                            //     if (temp->vehicle.y >= (temp->prev->vehicle.y) + DISTANCE_BETWEEN_VEHICLES) {
                            //         temp->vehicle.y -= temp->vehicle.speed;
                            //     }
                            //     else {
                            //         if (temp->vehicle.x <= (temp->prev->vehicle.x) + DISTANCE_BETWEEN_VEHICLES) {
                            //             temp->vehicle.x += temp->vehicle.speed;
                            //         }
                            //     }
                            // }
                        }
                    }
                } else if (temp->vehicle.lane == 3) {  // DL3 (third lane)
                    // Free lane, always allow left turn
                    if (temp->vehicle.x < 330) {
                        temp->vehicle.x += temp->vehicle.speed;  // Move straight
                    } else {
                        temp->vehicle.y -= temp->vehicle.speed;  // Move up
                        // if (temp->vehicle.y < -20) {
                        //     dequeue(&vehicleQueueD);
                        // }
                    }
                }

                renderVehicle(renderer, temp->vehicle);
                temp = temp->next;
            }
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