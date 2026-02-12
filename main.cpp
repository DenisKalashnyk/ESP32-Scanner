
//#include <artifact_part.hpp>/**/


#include <Arduino.h>
#include "WiFi.h"
#include <math.h>
#include <lcd1602.hpp>
#include <HTTPClient.h>

struct WiFiScan {
  String ssid;
  long rssi;
};

HTTPClient httpClient;

// List of WiFi networks found
WiFiScan wifiNetworks[20];
int wifiCount = 0;
String artefact_prefix = "START_";
//          address, cols, rows, rs, rw, enable, d4, d5, d6, d7
LCD1602 lcd(0x27,    16,   2,    13, -1,  12,    14, 27, 26, 25);

const int LED_RED_PIN  = 2; // On-board LED pin
const int LED_GREEN_PIN = 4; // External LED pin

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan for artefact");

  ledcAttachPin(LED_RED_PIN, 0);   // Attach the red LED pin to channel 0
  ledcAttachPin(LED_GREEN_PIN, 1); // Attach the green LED pin to channel 1

  ledcWrite(0, 0);                  // Set red LED to off
  ledcWrite(1, 0);                  // Set green LED to off

}

bool SCAN_MODE = true; // Set to true to enable scanning mode
String nearestSSID = "";

void loop() {
  if(SCAN_MODE){
    Serial.println("Scan start...");
    int n = WiFi.scanNetworks();
    Serial.println("Scan done...");
    if (n == 0) {
      Serial.println("Artefact not found");
    } else {
      wifiCount = 0;
      for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        String wifi_name = WiFi.SSID(i).c_str();
        long int rssi = WiFi.RSSI(i);
        Serial.printf("%2d: %s (%ld dBm)\n", i + 1, wifi_name.c_str(), rssi);
        delay(10);
        if(wifiCount < 20) {
          if ( (wifi_name.length() > artefact_prefix.length()) && 
              (wifi_name.startsWith(artefact_prefix) ) ) 
          {
            // Store the WiFi network information in the array
            wifiNetworks[wifiCount].ssid = wifi_name;
            wifiNetworks[wifiCount].rssi = rssi;
            wifiCount++;
          }
        }
      }
    }
    Serial.println("");
    if (wifiCount > 0) {
      int nearestIndex = 0;
      long nearestRssi = wifiNetworks[0].rssi;

      for (int i = 1; i < wifiCount; ++i) {
        if (wifiNetworks[i].rssi > nearestRssi) {
          nearestRssi = wifiNetworks[i].rssi;
          nearestIndex = i;
        }
      }
      nearestSSID = wifiNetworks[nearestIndex].ssid;
      SCAN_MODE = false; // Disable scanning mode after finding the artefact
      WiFi.begin(nearestSSID.c_str());
      int count = 0;
      while(WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
        count++;
        if(count > 50) { // Timeout after 5 seconds
          Serial.println("Connection timed out");
          WiFi.disconnect();
          SCAN_MODE = true; // Re-enable scanning mode if connection fails
          return;
        }
      }
      Serial.println("Connecting to " + nearestSSID);
      httpClient.begin("http://192.168.4.1/name");
      int httpCode = httpClient.GET();
      if(httpCode > 0) {
        String response = httpClient.getString();
        Serial.println("Artefact found: " + response);
        lcd.setCursor(0, 1);
        lcd.print(response.c_str());
      } else {
        Serial.println("HTTP GET failed: " + String(httpCode));
      }
      httpClient.end();
    }else{
      Serial.println("Artefact not found");
      lcd.setCursor(0, 0);
      lcd.print("9999999999999999");
    }
    WiFi.scanDelete();
  }else{
    if( WiFi.status() == WL_CONNECTED ) {
      // Serial.println("Artefact found: " + nearestSSID);
      long nearestRssi = WiFi.RSSI();
      // Print the nearest artefact found in meters -90 dBm is 100 meters away, -15 dBm is 1 meter away
      float distance = pow(10, (- nearestRssi) / 45.0) / 5.0; // Convert RSSI to distance in meters
      char buff[32];
      snprintf(buff, sizeof(buff), "%i", int(distance));
      String distanceStr(buff);
      Serial.println(distanceStr);
      lcd.setCursor(0, 0);
      //Update distance string to fit the display
      while (distanceStr.length() < 16) {
        distanceStr = "0" + distanceStr;
      }
      lcd.print(distanceStr);
    } else {
      WiFi.disconnect();
      SCAN_MODE = true; // Re-enable scanning mode if not connected
    }
    delay(250);
  }
   
}
/**/