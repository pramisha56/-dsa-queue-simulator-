#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define road_min 1
#define road_max 4
#define vehicle_min 1
#define vehicle_max 3

// Function to generate vehicles for a specific lane
void generateVehicles(const char *laneFile, int vehicleCount, int road) {
    FILE *fp = fopen(laneFile, "a+");
    if (fp == NULL) {
        printf("Error opening %s for writing.\n", laneFile);
        return;
    }

    for (int i = 0; i < vehicleCount; i++) {
        int lane = (rand() % 2) + 2; // Randomly choose Lane 2 or Lane 3
        float x, y;

        // Define starting positions based on the road and lane
        switch (road) {
            case 1: // Road A (top to bottom)
                if (lane == 3) {
                    x = 450.0f;
                    y = 0.0f;
                }
                else if (lane == 2) {
                    x = 385.0f;
                    y = 0.0f;
                }
                break;
            case 2: // Road B (right to left)
               if(lane == 2){
                   x = 750.0f;
                   y = 385.0f;
                }
                else if(lane == 3){
                    x = 750.0f;
                    y = 450.0f;
                }
                break;
            case 3: // Road C (bottom to top)
                if(lane == 2){
                    x = 385.0f;
                    y = 750.0f;
                }
                else if(lane == 3){
                    x = 320.0f;
                    y = 750.0f;
                }
                break;
            case 4: // Road D (right to left)
               if(lane == 2){
                   x = 25.0f;
                   y = 385.0f;
                }
                else if(lane == 3){
                    x = 25.0f;
                    y = 320.0f;
                }
                break;
            default:
                x = 0.0f;
                y = 0.0f;
                break;
        }

        float speed = 0.8 ;
        fprintf(fp, "%d,%d,%.2f,%.2f,%.2f\n", road, lane, x, y, speed);
    }

    fclose(fp);
    printf("Generated %d vehicles in %s\n", vehicleCount, laneFile);
}

int main() {
    srand(time(NULL));  // Seed for random number generation

    int vehicleCount = 0;
    while (1) {
        int random_lane_index = rand() % (road_max - road_min + 1) + road_min; // Random road index (1 to 4)
       
        // Generate vehicles for the selected lane
        if (random_lane_index == 1) {
            generateVehicles("RoadA.txt", rand() % (vehicle_max - vehicle_min + 1) + vehicle_min, 1);
        } else if (random_lane_index == 2) {
            generateVehicles("RoadB.txt", rand() % (vehicle_max - vehicle_min + 1) + vehicle_min, 2);
        } else if (random_lane_index == 3) {
            generateVehicles("RoadC.txt", rand() % (vehicle_max - vehicle_min + 1) + vehicle_min, 3);
        } else if (random_lane_index == 4) {
            generateVehicles("RoadD.txt", rand() % (vehicle_max - vehicle_min + 1) + vehicle_min, 4);
        }

        vehicleCount++;
        Sleep(3000); // Sleep for 3 seconds
    }

    return 0;
}