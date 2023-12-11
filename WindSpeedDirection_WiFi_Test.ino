// Include required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "dhtnew.h"
#define DHTPIN 4
#define DHTTYPE DHT11
const char* ssid = "Your network name";
const char* password = "Password for network";
const char* serverURL = "server url innit"; // URL to your PHP file
DHT dht(4, 11);

int northpin = 4; // Arbitrary digital pin for monitoring the output
int eastpin = 5;
int southpin = 13;
int westpin = 18;
String currentState = "Current state: ";
int north = 0;
int south = 0;
int east = 0;
int west = 0;

// Current state of the sensor
int lastState = 0;  // Place to save our previous state

// Define constants
const int sensorPin = 14;  // GPIO pin connected to the sensor
const int interruptPin = digitalPinToInterrupt(sensorPin);

// Variables
volatile unsigned int pulseCount = 0;
unsigned long lastMillis;

// Conversion factors
const float secondsPerInterval = 5.0;
const float mPerPulse = 0.65; // Adjust this based on your sensor specifications
const float kmPerHourConversion = 3.6;

// Function prototypes
void IRAM_ATTR handleInterrupt();

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(northpin, INPUT_PULLUP); // Enable pull-up as sensor output is open collector
  pinMode(eastpin, INPUT_PULLUP);  // Enable pull-up as sensor output is open collector
  pinMode(southpin, INPUT_PULLUP); // Enable pull-up as sensor output is open collector
  pinMode(westpin, INPUT_PULLUP);  // Enable pull-up as sensor output is open collector

  // Attach the interrupt handler
  attachInterrupt(interruptPin, handleInterrupt, FALLING);

  lastMillis = millis();

   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");

    dht.begin();
}

void loop() {
String deviceName = "Team 69"; // Replace with your team name
String postData = "currentState" + String(currentState) + "&device=" + deviceName;

HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(postData);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.println("Error sending request");
  }

  http.end();
  delay(10000); // Send data every tem seconds (adjust as needed)
  
  // Calculate wind speed every 5 seconds
  if (millis() - lastMillis >= (secondsPerInterval * 1000)) {
    detachInterrupt(interruptPin);  // Disable interrupts while calculating
    float windSpeedMetersPerSecond = (float)pulseCount * mPerPulse / secondsPerInterval;
    float windSpeedKmPerHour = windSpeedMetersPerSecond * kmPerHourConversion;

    Serial.print("Wind Speed: ");
    Serial.print(windSpeedMetersPerSecond, 2);
    Serial.print(" m/s, ");
    Serial.print(windSpeedKmPerHour, 2);
    Serial.println(" km/h");

    pulseCount = 0;  // Reset pulse count
    attachInterrupt(interruptPin, handleInterrupt, FALLING);  // Re-enable interrupts
    lastMillis = millis();  // Update last measurement time
  }

  // Additional code for direction sensing
  north = digitalRead(northpin);    // Read the current state of the sensor
  east = digitalRead(eastpin);      // Read the current state of the sensor
  south = digitalRead(southpin);    // Read the current state of the sensor
  west = digitalRead(westpin);      // Read the current state of the sensor

  if (north + east == 0) {
    Serial.println(currentState + "North-east");
  } else if (east + south == 0) {
    Serial.println(currentState + "South-east");
  } else if (west + south == 0) {
    Serial.println(currentState + "South-west");
  } else if (north + west == 0) {
    Serial.println(currentState + "North-west");
  } else if (north == 0) {
    Serial.println(currentState + "North");
  } else if (east == 0) {
    Serial.println(currentState + "East");
  } else if (south == 0) {
    Serial.println(currentState + "South");
  } else if (west == 0) {
    Serial.println(currentState + "West");
  }
  delay (1000);
  // Your main loop code goes here
}

// Interrupt service routine for handling sensor pulses
void IRAM_ATTR handleInterrupt() {
  pulseCount++;
}
