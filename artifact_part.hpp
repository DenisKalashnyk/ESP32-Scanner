#ifndef     ARTIFACT_PART_HPP
#define     ARTIFACT_PART_HPP

#include <Arduino.h>
#include "WiFi.h"
#include <WebServer.h>

char const* SSID = "START_000001";
char const* PASSWORD = nullptr;

WebServer server(80); // Create a web server on port 80

#define LED_COUNT 9
int led_pins[LED_COUNT] = {15, 2, 4, 16, 17, 5, 18, 19, 21};
int led_counts[LED_COUNT] = {200, 250, 300, 350, 400, 450, 500, 550, 600};

bool http_server_start() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/plain", "Artefact for game STALKER");
    });
    server.on("/name", HTTP_GET, []() {
        server.send(200, "text/plain", "MEDUZA");
    });
    server.begin();
    Serial.println("HTTP server started");
    return true;
}

void leds_setup(){
    int i = 0;
    for(i=0; i < LED_COUNT; i++){
        pinMode(led_pins[i], OUTPUT);
        digitalWrite(led_pins[i], LOW);
    }
}

void setup(){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
    Serial.begin(115200);
    Serial.println("WiFi AP started");
    // Set the AP IP address
    IPAddress local_ip(192, 168, 4, 1);
    WiFi.softAPConfig(local_ip, local_ip, IPAddress(255, 255, 255, 0));

        // Event on WiFi connection
    WiFi.onEvent([](arduino_event_t *event) {
        if (event->event_id == ARDUINO_EVENT_WIFI_STA_START) {
            Serial.println("WiFi STA started");
        } else if (event->event_id == ARDUINO_EVENT_WIFI_STA_CONNECTED) {
            Serial.println("WiFi STA connected");
        } else if (event->event_id == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            Serial.println("WiFi STA disconnected");
        }
    });
    // Start the HTTP server
    if (!http_server_start()) {
        Serial.println("Failed to start HTTP server");
    } else {
        Serial.println("HTTP server started successfully");
    }

    leds_setup();

}

void leds_process(){
    long int counter = millis();
    int i = 0;
    for(i=0; i < LED_COUNT; i++){
        long int phasa = counter % led_counts[i];
        if( phasa > led_counts[i]/2 ){
            digitalWrite(led_pins[i], HIGH);
        }else{
            digitalWrite(led_pins[i], LOW);
        }
    }
}

void loop(){
    // Nothing to do here
    server.handleClient(); // Handle incoming HTTP requests
    delay(50); // Add a small delay to avoid busy-waiting
    leds_process();
}

#endif // ARTIFACT_PART_HPP