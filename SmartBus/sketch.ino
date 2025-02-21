#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>  // Wi-Fi and MQTT settings

const char* ssid = "Wokwi-GUEST"; 
const char* password = ""; 
const char* mqtt_server = "broker.hivemq.com"; 
const int mqttPort = 1883; 
const char* mqtt_topic = "bus/tracking";  

// Fixed pickup point (latitude and longitude)
float pickup_lat = 12.971598;  // Latitude of the pickup point
float pickup_lon = 77.594566;  // Longitude of the pickup point

// Bus starting point (5 km away from pickup point)
float bus_lat = 12.927000;  // Starting latitude (5 km south of pickup)
float bus_lon = 77.594566;  // Starting longitude

// Movement simulation (simulating bus movement by changing coordinates)
float increment = 0.009; // Approx. 1 km of movement per update (latitude change)

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Wi-Fi and MQTT setup
WiFiClient espClient; 
PubSubClient client(espClient); 

// Function to set up MQTT connection
void setupMQTT() {
    client.setServer(mqtt_server, mqttPort);
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT!");
        } else {
            Serial.print("Failed: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

// Function to publish MQTT message
void publishMQTT(float distance) {
    String message = "Distance to pickup: " + String(distance, 1) + " km";
    client.publish(mqtt_topic, message.c_str());
    Serial.println("Published: " + message);
}

// Function to update the LCD
void updateDisplay(float distance) {
    lcd.clear();
    if (distance <= 1.0) {
        lcd.print("Arriving soon!");
    } else {
        lcd.print("Distance: ");
        lcd.setCursor(0, 1);
        lcd.print(String(distance, 1) + " km");
    }
}

// Function to calculate distance between two coordinates
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371.0; // Radius of Earth in km
    float dLat = radians(lat2 - lat1);
    float dLon = radians(lon2 - lon1);
    float a = sin(dLat / 2) * sin(dLat / 2) + 
              cos(radians(lat1)) * cos(radians(lat2)) * 
              sin(dLon / 2) * sin(dLon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void setup() {
    Serial.begin(115200);

    lcd.init();
    lcd.backlight();

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    setupMQTT();

    lcd.print("System Ready");
    delay(2000);
}

void loop() {
    if (!client.connected()) {
        setupMQTT();
    }
    client.loop();

    // Calculate distance to the pickup point
    float distance = calculateDistance(bus_lat, bus_lon, pickup_lat, pickup_lon);

    // Update display and log distance to debug
    updateDisplay(distance);
    Serial.println("Current distance: " + String(distance, 1) + " km");

    // Send MQTT message for debug
    publishMQTT(distance);

    // Simulate movement towards the pickup point only if the bus is farther than 1 km
    if (distance > 1.0) {
        // Moving towards the pickup point by decreasing latitude (moving north)
        bus_lat += increment; // Move bus closer to pickup point (north)
    } else {
        Serial.println("Bus is near the pickup point!");
    }

    delay(5000); // Simulate delay between updates
}
