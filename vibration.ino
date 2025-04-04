#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

#define VIBRATION_PIN 2  // Define the digital pin for SW-180 sensor
#define SD_CS_PIN 10      // Define the chip select pin for the SD card module
#define LDR_PIN A0        // Define the analog pin for the LDR sensor

MPU6050 mpu;
RTC_DS3231 rtc;
File logFile;

void setup() {
    Serial.begin(9600);
    pinMode(VIBRATION_PIN, INPUT_PULLUP); // SW-180 as input with pull-up
    pinMode(LDR_PIN, INPUT);
    Wire.begin();
    mpu.initialize();
    
    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!");
        while (1);
    }
    
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed!");
        while (1);
    }
    
    if (!rtc.begin()) {
        Serial.println("RTC initialization failed!");
        while (1);
    }
}

void loop() {
    DateTime now = rtc.now();
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    float x = ax / 16384.0;
    float y = ay / 16384.0;
    float z = az / 16384.0;
    int ldrValue = analogRead(LDR_PIN);

    String logEntry = String(now.minute()) + ":" + String(now.second()) + " -> " +
                      "x: " + String(x, 2) + " y: " + String(y, 2) + " z: " + String(z, 2);
    
    if (digitalRead(VIBRATION_PIN) == LOW) {
        logEntry += " -> Vibration detected!";
    }
    
    if (ldrValue > 500) {
        logEntry += " -> Lid opened!";
    }
    
    Serial.println(logEntry);
    
    logFile = SD.open("log.txt", FILE_WRITE);
    if (logFile) {
        logFile.println(logEntry);
        logFile.close();
    } else {
        Serial.println("Error opening log file!");
    }
    
    delay(200);
}
