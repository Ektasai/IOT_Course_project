#include <stdio.h>
#include <math.h>
#include <unistd.h>

// GPS coordinates (starting location 5 km away from pickup point)
double bus_lat = 12.927000;
double bus_lon = 77.594566;
double pickup_lat = 12.971598;
double pickup_lon = 77.594566;
double increment = 0.009; // Approx. 1 km of movement per update (latitude change)

// Function to calculate distance between two coordinates
double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth's radius in km
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void simulateGpsData() {
    char nmea_sentence[100];

    // Calculate distance to pickup point
    double distance = calculate_distance(bus_lat, bus_lon, pickup_lat, pickup_lon);

    // Format a GPGGA NMEA sentence
    snprintf(nmea_sentence, sizeof(nmea_sentence),
             "$GPGGA,123519,%.6f,N,%.6f,E,1,08,0.9,545.4,M,46.9,M,,*47\n",
             bus_lat, bus_lon);

    printf("%s", nmea_sentence);
    fflush(stdout); // Ensure data is sent immediately

    // Simulate movement towards pickup point
    if (distance > 1.0) {
        bus_lat += increment;  // Move bus closer to the pickup point (north)
    } else {
        printf("Bus is within 1 km of the pickup point!\n");
    }
}

int main() {
    while (1) {
        simulateGpsData();
        sleep(5); // Simulate real-time updates every 5 seconds
    }
}
