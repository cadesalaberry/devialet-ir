#include <HTTPClient.h>

const char* volumeUrl = "http://192.168.1.32/ipcontrol/v1/systems/current/sources/current/soundControl/volume";

int parseVolume(String json) {
  int number;
  sscanf(json.c_str(), "{\"volume\":%d}", &number);
  return number;
}

// Function to clamp a number between a minimum and a maximum value
int clamp(int minValue, int value, int maxValue) {
    if (value < minValue) {
        return minValue;
    } else if (value > maxValue) {
        return maxValue;
    } else {
        return value;
    }
}

void controlDevialetVolume(int command) {
  HTTPClient http;

  // GET request to get the current volume
  http.begin(volumeUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("GET response: " + payload);

    // Parse the volume from the response
    int currentVolume = parseVolume(payload);
    int newVolume = currentVolume;

    if (command == 0) newVolume = 0;
    if (command == 1) newVolume += 5;
    if (command == -1) newVolume -= 5;

    // Makes sure the volume does not go out of bound
    newVolume = clamp(0, newVolume, 100);

    // Prepare the JSON payload for the POST request
    String postPayload = "{\"volume\": " + String(newVolume) + "}";

    // POST request to set the new volume
    http.begin(volumeUrl);
    http.addHeader("Content-Type", "application/json");
    int postHttpCode = http.POST(postPayload);

    if (postHttpCode > 0) {
      String postResponse = http.getString();
      Serial.println("POST response: " + postResponse);
    } else {
      Serial.println("Error on POST request: " + String(postHttpCode));
    }
  } else {
    Serial.println("Error on GET request: " + String(httpCode));
  }

  http.end();
}