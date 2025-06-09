#include <SPI.h>
#include <WiFi101.h>

// WiFi credentials
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Server details
char server[] = "192.168.1.X"; // Replace X with your computer's local IP address
int port = 5000;

// Ultrasonic sensor pins
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

// Constants for pull-up detection
const float MIN_DISTANCE = 100.0; // Minimum distance to detect a pull-up (in cm) - when you're at the top
const float MAX_DISTANCE = 200.0; // Maximum distance to detect a pull-up (in cm) - when you're at the bottom
const int REP_COOLDOWN = 1000;    // Minimum time between reps (in ms)

// Variables
int repCount = 0;
bool inPullupPosition = false;
unsigned long lastRepTime = 0;
WiFiClient client;

void setup()
{
    Serial.begin(9600);

    // Initialize ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Connect to WiFi
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        WiFi.begin(ssid, pass);
        delay(5000);
    }

    Serial.println("Connected to WiFi");
    printWifiStatus();
}

void loop()
{
    float distance = measureDistance();

    // Debug print
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Detect pull-up
    if (distance < MIN_DISTANCE && !inPullupPosition)
    {
        inPullupPosition = true;
        Serial.println("Pull-up started");
    }
    else if (distance > MAX_DISTANCE && inPullupPosition)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastRepTime > REP_COOLDOWN)
        {
            repCount++;
            lastRepTime = currentTime;
            Serial.print("Rep count: ");
            Serial.println(repCount);

            // Send data to server
            sendDataToServer();
        }
        inPullupPosition = false;
        Serial.println("Pull-up completed");
    }

    delay(100);
}

float measureDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;

    return distance;
}

void sendDataToServer()
{
    if (client.connect(server, port))
    {
        Serial.println("Connected to server");

        // Create JSON payload
        String jsonData = "{\"reps\":" + String(repCount) + "}";

        // Send HTTP POST request
        client.println("POST /api/reps HTTP/1.1");
        client.println("Host: " + String(server));
        client.println("Content-Type: application/json");
        client.println("Content-Length: " + String(jsonData.length()));
        client.println();
        client.println(jsonData);

        // Wait for response
        while (client.available())
        {
            char c = client.read();
            Serial.write(c);
        }

        client.stop();
    }
    else
    {
        Serial.println("Connection to server failed");
    }
}

void printWifiStatus()
{
    // Print the SSID
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // Print your board's IP address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // Print the received signal strength
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}