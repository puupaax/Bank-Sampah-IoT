
#include <FirebaseESP32.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include <WiFi.h>


#include "HX711.h"

// Firebase credentials
#define FIREBASE_HOST "https://iotgas-cd9a3-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyAIMiCIl--HjP77RuRpSkO7y3nuOzIKYLE"

// WiFi credentials
#define WIFI_SSID "poco"
#define WIFI_PASSWORD "pocophone"

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

FirebaseData firebaseData;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  /* Assign the api key (required) */
  config.api_key = FIREBASE_AUTH;

  /* Assign the RTDB URL (required) */
  config.database_url = FIREBASE_HOST;

  // Initialize Firebase
  Firebase.signUp(&config, &auth, "", "");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {
  if (scale.is_ready()) {
    scale.set_scale();
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);

    // Write data to Firebase
    if (Firebase.pushFloat(firebaseData, "/condition", reading)) {
      Serial.println("Data sent to Firebase");
    } else {
      Serial.println("Failed to send data to Firebase");
      Serial.println("Error: " + firebaseData.errorReason());
    }
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}