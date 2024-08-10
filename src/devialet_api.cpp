#include <HTTPClient.h>
#include <ESPmDNS.h>

#include <devialet_api.hpp>

// The full URL looks like: http://192.168.1.32/ipcontrol/v1/systems/current/sources/current/soundControl/volume
const char* volumeUrlPath = "/ipcontrol/v1/systems/current/sources/current/soundControl/volume";
const char* muteUrlPath = "/ipcontrol/v1/groups/current/sources/current/playback/mute";

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

String getDevialetIP() {
  // Query for the Devialet service, assuming the service type is "_http._tcp.local"
  int n = MDNS.queryService("http", "tcp");
  if (n == 0) {
    Serial.println("[devialet_api] No services found");
  } else {
    for (int i = 0; i < n; ++i) {
      // Serial.println("[devialet_api] Service " + String(i) + ": " + MDNS.hostname(i) + " (" + MDNS.address(i).toString() + ")");
      if (MDNS.hostname(i).indexOf("Phantom") >= 0) { // Assuming the hostname contains "devialet"
        return MDNS.address(i).toString();
      }
    }
  }

  Serial.println("[devialet_api] No Phantom speaker was found");
  return "";
}

/**
 * Mute the Devialet with a POST request to the mute URL with no parameters.
 * Does not seem to work yet.
 */
void muteDevialet(String devialetIP) {
  HTTPClient http;
  String fullUrl = "http://" + devialetIP + muteUrlPath;
  http.begin(fullUrl);
  int httpCode = http.POST("");

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("[devialer_api] Muting reply " + String(httpCode));
  } else {
    Serial.println("[devialer_api] Could not mute: " + String(httpCode));
  }

  http.end();
}

void controlDevialetVolume(String devialetIP, int command) {
  HTTPClient http;

  if (command == VOLUME_MUTE) {
    muteDevialet(devialetIP);
    return;
  }

  String fullUrl = "http://" + devialetIP + volumeUrlPath;
  // GET request to get the current volume
  http.begin(fullUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    // Parse the volume from the response
    int currentVolume = parseVolume(payload);
    int newVolume = currentVolume;

    if (command == VOLUME_UP) newVolume += 2;
    if (command == VOLUME_DOWN) newVolume -= 2;

    // Makes sure the volume does not go out of bound
    newVolume = clamp(0, newVolume, 100);

    Serial.print("[devialer_api] Requesting volume change: " + String(currentVolume));
    Serial.println(" => " + String(newVolume));

    // Prepare the JSON payload for the POST request
    String postPayload = "{\"volume\": " + String(newVolume) + "}";

    // POST request to set the new volume
    http.begin(volumeUrlPath);
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