#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate vehicles for a specific lane
void generateVehicles(const char *laneFile, int vehicleCount) {
    FILE *fp = fopen(laneFile, "w");
    if (fp == NULL) {
        printf("Error opening %s for writing.\n", laneFile);
        return;
    }

    // Randomly generate vehicle data
    for (int i = 0; i < vehicleCount; i++) {
        // Assign random x and y positions, speed, etc.
        float x = 100.0 + i * 10;  // Example X position
        float y = 350.0;            // Example Y position (same for all)
        float speed = 2.0 + rand() % 3;  // Random speed between 2 and 4
        fprintf(fp, "%d,%.2f,%.2f,%.2f\n", i, x, y, speed);
    }

    fclose(fp);
    printf("Generated %d vehicles in %s\n", vehicleCount, laneFile);
}

int main() {
    srand(time(NULL));  // Seed for random number generation

    // Generate vehicles for each lane
    generateVehicles("laneA.txt", 12);  // Generate 12 vehicles for lane A
    generateVehicles("laneB.txt", 10);  // Generate 10 vehicles for lane B
    generateVehicles("laneC.txt", 15);  // Generate 15 vehicles for lane C
    generateVehicles("laneD.txt", 8);   // Generate 8 vehicles for lane D

    return 0;
}
