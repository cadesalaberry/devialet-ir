#include <HTTPClient.h>
#include <devialet_api.hpp>

const char* volumeUrl = "http://192.168.1.32/ipcontrol/v1/systems/current/sources/current/soundControl/volume";
const char* muteUrl = "http://192.168.1.32/ipcontrol/v1/groups/current/sources/current/playback/mute";

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

/**
 * Mute the Devialet with a POST request to the mute URL with no parameters.
 * Does not seem to work yet.
 */
void muteDevialet() {
  HTTPClient http;
  http.begin(muteUrl);
  int httpCode = http.POST("");

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("[devialer_api] Muting reply " + String(httpCode));
  } else {
    Serial.println("[devialer_api] Could not mute: " + String(httpCode));
  }

  http.end();
}

void controlDevialetVolume(int command) {
  HTTPClient http;

  if (command == VOLUME_MUTE) {
    muteDevialet();
    return;
  }

  // GET request to get the current volume
  http.begin(volumeUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    // Parse the volume from the response
    int currentVolume = parseVolume(payload);
    int newVolume = currentVolume;

    if (command == VOLUME_UP) newVolume += 3;
    if (command == VOLUME_DOWN) newVolume -= 3;

    // Makes sure the volume does not go out of bound
    newVolume = clamp(0, newVolume, 100);

    Serial.print("[devialer_api] Requesting volume change: " + String(currentVolume));
    Serial.println(" => " + String(newVolume));

    // Prepare the JSON payload for the POST request
    String postPayload = "{\"volume\": " + String(newVolume) + "}";

    // POST request to set the new volume
    http.begin(volumeUrl);
    http.addHeader("Content-Type", "application/json");
    int postHttpCode = http.POST(postPayload);

    if (postHttpCode == 200) {
      String postResponse = http.getString();
      Serial.println("[devialer_api] Volume change OK " + String(postHttpCode));
    } else {
      Serial.println("[devialer_api] Could not change volume: " + String(postHttpCode));
    }
  } else {
    Serial.println("[devialer_api] Could not get volume: " + String(httpCode));
  }

  http.end();
}